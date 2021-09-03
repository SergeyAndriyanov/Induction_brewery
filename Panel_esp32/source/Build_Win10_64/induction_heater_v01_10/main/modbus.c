
/*
 *  
 *
 *   Created on: 14 авг. 2021 г.
 *   Author: Sergey Andriyanov
 *   emai:asvlabpost@gmail.com
 *  
 */
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include <string.h>
#include <stdio.h>
#include "lcd.h"
#include "hw_init.h"
#include "MAX31865.h"
#include "dac_aud.h"
#include "driver/timer.h"
#include "driver/dac.h"
#include <stdlib.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "freertos/queue.h"
#include "key.h"
#include "datablock.h"
#include "inductionboard.h"

#include "nvs.h"
#include "menu.h"
#include "wchar.h"
#include "wifi.h"
#include "modbus.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "mdns.h"
#include "esp_netif.h"
#include "mbcontroller.h"
#include "process_work.h"
//-----------------------------------------------------------------------------//
Tparametrmodbus parametrmodbus;
const char *hostname = {"mb_slave_tcp_01"};
const char *hostnamedef = {"esp32_mb_slave_tcp"};
char mdsnval1[33] = {0};
char mdsnval2[33] = {0};
mdns_txt_item_t serviceTxtData[] = {{"board", "esp32"}};
Ttablepointdatareg tablereg = {0};
static portMUX_TYPE param_lock = portMUX_INITIALIZER_UNLOCKED;
//-----------------------------------------------------------------------------//
void start_mdns_service()
{

    uint8_t ap_mac[6] = {0};
    ESP_ERROR_CHECK(esp_read_mac(ap_mac, ESP_MAC_WIFI_SOFTAP));

    //initialize mDNS
    ESP_ERROR_CHECK(mdns_init());
    //set mDNS hostname (required if you want to advertise services)
    ESP_ERROR_CHECK(mdns_hostname_set(hostname));
    ESP_LOGI(SLAVE_TAG, "mdns hostname set to: [%s]", hostname);
    //set default mDNS instance name
    ESP_ERROR_CHECK(mdns_instance_name_set(hostnamedef));
    ESP_LOGI(SLAVE_TAG, "mdns hostname default set to: [%s]", hostnamedef);

    //initialize service
    ESP_ERROR_CHECK(mdns_service_add(hostname, "_modbus", "_tcp", MB_MDNS_PORT, serviceTxtData, 1));
    //add mac key string text item

    memset(mdsnval1, 0, sizeof(mdsnval1));
    sprintf(mdsnval1, "%s%02X%02X%02X%02X%02X%02X", "\0", MAC2STR(ap_mac));
    ESP_ERROR_CHECK(mdns_service_txt_item_set("_modbus", "_tcp", "mac", mdsnval1));
    //add slave id key txt item

    memset(mdsnval2, 0, sizeof(mdsnval2));
    sprintf(mdsnval2, "%s%02X%02X%02X%02X", "\0", MB_ID2STR(MB_DEVICE_ID));
    ESP_ERROR_CHECK(mdns_service_txt_item_set("_modbus", "_tcp", "mb_id", mdsnval2));
}

//-----------------------------------------------------------------------------//
void modbus_init_sl(void *mbhandl)
{

    parametrmodbus.modbustaskout = 0;
    mb_communication_info_t comm_info = {0};
    // Set UART log level
    ESP_ERROR_CHECK(mbc_slave_init_tcp(&mbhandl)); // Initialization of Modbus controller
    comm_info.ip_port = MB_TCP_PORT_NUMBER;
#if !CONFIG_EXAMPLE_CONNECT_IPV6
    comm_info.ip_addr_type = MB_IPV4;
#else
    comm_info.ip_addr_type = MB_IPV6;
#endif
    comm_info.ip_mode = MB_MODE_TCP;
    comm_info.ip_addr = NULL;
    comm_info.ip_netif_ptr = (void *)get_apwifi_netif();
    // Setup communication parameters and start stack
    ESP_ERROR_CHECK(mbc_slave_setup((void *)&comm_info));

    ESP_ERROR_CHECK(mbc_slave_set_descriptor_v1(0));
    // Initialization of Input Registers area0

    ESP_ERROR_CHECK(mbc_slave_set_descriptor_v1(1));
    // Initialization of Input Registers area1

    ESP_ERROR_CHECK(mbc_slave_set_descriptor_v1(2));
    // Initialization of Coils register area

    ESP_ERROR_CHECK(mbc_slave_set_descriptor_v1(3));
    // Initialization of Discrete Inputs register area

    ESP_ERROR_CHECK(mbc_slave_set_descriptor_v1(4));

    setup_reg_data();

    filltablereg();

    // Starts of modbus controller and stack
    ESP_ERROR_CHECK(mbc_slave_start());
    ESP_LOGI(SLAVE_TAG, "Modbus slave stack initialized.");
    ESP_LOGI(SLAVE_TAG, "Start modbus induction brewry...");

    // eMBRegHoldingCB
}
//-----------------------------------------------------------------------------//
void runtaskmodbus()
{
    xTaskCreate(task_modbus, "task_modbus", STACK_SIZE_MODBUS_TASK, (void *)&parametrmodbus, (configMAX_PRIORITIES - 4), NULL);
    xTaskCreate(task_updatereg, "task_updatereg", STACK_SIZE_MODBUS_UPDATEREGTASK, NULL, (configMAX_PRIORITIES - 4), NULL);
}

//-----------------------------------------------------------------------------//
void modbus_deinit_mb()
{
    parametrmodbus.modbustaskout = 1;
    ESP_ERROR_CHECK(mbc_slave_destroy());
}
//-----------------------------------------------------------------------------//
void task_modbus(void *arg)
{
    Tparametrmodbus *parmod = (Tparametrmodbus *)arg;
    mb_param_info_t reg_info;
    ESP_LOGI(SLAVE_TAG, "Modbus start task get event");

    for (; (parmod->modbustaskout != 1);)
    {
        // Check for read/write events of Modbus master for certain events
        mb_event_group_t event = mbc_slave_check_event(MB_READ_WRITE_MASK);
        const char *rw_str = (event & MB_READ_MASK) ? "READ" : "WRITE";
        // Filter events and process them accordingly
        if (event & (MB_EVENT_HOLDING_REG_WR | MB_EVENT_HOLDING_REG_RD))
        {
            // Get parameter information from parameter queue
            ESP_ERROR_CHECK(mbc_slave_get_param_info(&reg_info, MB_PAR_INFO_GET_TOUT));
#if MB_EVENT_READ_WRITE_ESP_LOGI_ON == 1
            ESP_LOGI(SLAVE_TAG, "HOLDING %s (%u us), ADDR:%u, TYPE:%u, INST_ADDR:0x%.4x, SIZE:%u",
                     rw_str,
                     (uint32_t)reg_info.time_stamp,
                     (uint32_t)reg_info.mb_offset,
                     (uint32_t)reg_info.type,
                     (uint32_t)reg_info.address,
                     (uint32_t)reg_info.size);
#endif
            if (event & MB_EVENT_HOLDING_REG_WR)
            {
                parmod->eventwrite = 1;
            }
        }
        else if (event & MB_EVENT_INPUT_REG_RD)
        {
            ESP_ERROR_CHECK(mbc_slave_get_param_info(&reg_info, MB_PAR_INFO_GET_TOUT));
#if MB_EVENT_READ_WRITE_ESP_LOGI_ON == 1
            ESP_LOGI(SLAVE_TAG, "INPUT READ (%u us), ADDR:%u, TYPE:%u, INST_ADDR:0x%.4x, SIZE:%u",
                     (uint32_t)reg_info.time_stamp,
                     (uint32_t)reg_info.mb_offset,
                     (uint32_t)reg_info.type,
                     (uint32_t)reg_info.address,
                     (uint32_t)reg_info.size);
#endif
        }

        else if (event & MB_EVENT_DISCRETE_RD)
        {
            ESP_ERROR_CHECK(mbc_slave_get_param_info(&reg_info, MB_PAR_INFO_GET_TOUT));
#if MB_EVENT_READ_WRITE_ESP_LOGI_ON == 1
            ESP_LOGI(SLAVE_TAG, "DISCRETE READ (%u us): ADDR:%u, TYPE:%u, INST_ADDR:0x%.4x, SIZE:%u",
                     (uint32_t)reg_info.time_stamp,
                     (uint32_t)reg_info.mb_offset,
                     (uint32_t)reg_info.type,
                     (uint32_t)reg_info.address,
                     (uint32_t)reg_info.size);
#endif
        }
        else if (event & (MB_EVENT_COILS_RD | MB_EVENT_COILS_WR))
        {
            ESP_ERROR_CHECK(mbc_slave_get_param_info(&reg_info, MB_PAR_INFO_GET_TOUT));
#if MB_EVENT_READ_WRITE_ESP_LOGI_ON == 1
            ESP_LOGI(SLAVE_TAG, "COILS %s (%u us), ADDR:%u, TYPE:%u, INST_ADDR:0x%.4x, SIZE:%u",
                     rw_str,
                     (uint32_t)reg_info.time_stamp,
                     (uint32_t)reg_info.mb_offset,
                     (uint32_t)reg_info.type,
                     (uint32_t)reg_info.address,
                     (uint32_t)reg_info.size);
#endif
        }
    }
    parmod->modbustaskout = 0;
    // Destroy of Modbus controller on alarm
    ESP_LOGI(SLAVE_TAG, "Modbus controller destroyed.");
    vTaskDelete(NULL);
}
//-----------------------------------------------------------------------------//
void filltablereg()
{
    tablereg.datafloat[0] = &dataprocess.dataproc.mash[0].temperature_mesh;
    tablereg.datafloat[1] = &dataprocess.dataproc.mash[0].temperature_pan1;
    tablereg.datafloat[2] = &dataprocess.dataproc.mash[0].temperature_pan2;
    tablereg.datafloat[3] = &dataprocess.dataproc.mash[0].temperature_mesh_delta;
    tablereg.datafloat[4] = &dataprocess.dataproc.mash[0].temperature_pan_delta;
    tablereg.datafloat[5] = &dataprocess.dataproc.mash[1].temperature_mesh;
    tablereg.datafloat[6] = &dataprocess.dataproc.mash[1].temperature_pan1;
    tablereg.datafloat[7] = &dataprocess.dataproc.mash[1].temperature_pan2;
    tablereg.datafloat[8] = &dataprocess.dataproc.mash[1].temperature_mesh_delta;
    tablereg.datafloat[9] = &dataprocess.dataproc.mash[1].temperature_pan_delta;
    tablereg.datafloat[10] = &dataprocess.dataproc.mash[2].temperature_mesh;
    tablereg.datafloat[11] = &dataprocess.dataproc.mash[2].temperature_pan1;
    tablereg.datafloat[12] = &dataprocess.dataproc.mash[2].temperature_pan2;
    tablereg.datafloat[13] = &dataprocess.dataproc.mash[2].temperature_mesh_delta;
    tablereg.datafloat[14] = &dataprocess.dataproc.mash[2].temperature_pan_delta;
    tablereg.datafloat[15] = &dataprocess.dataproc.mash[3].temperature_mesh;
    tablereg.datafloat[16] = &dataprocess.dataproc.mash[3].temperature_pan1;
    tablereg.datafloat[17] = &dataprocess.dataproc.mash[3].temperature_pan2;
    tablereg.datafloat[18] = &dataprocess.dataproc.mash[3].temperature_mesh_delta;
    tablereg.datafloat[19] = &dataprocess.dataproc.mash[3].temperature_pan_delta;
    tablereg.datafloat[20] = &dataprocess.dataproc.mash[4].temperature_mesh;
    tablereg.datafloat[21] = &dataprocess.dataproc.mash[4].temperature_pan1;
    tablereg.datafloat[22] = &dataprocess.dataproc.mash[4].temperature_pan2;
    tablereg.datafloat[23] = &dataprocess.dataproc.mash[4].temperature_mesh_delta;
    tablereg.datafloat[24] = &dataprocess.dataproc.mash[4].temperature_pan_delta;
    tablereg.datafloat[25] = &dataprocess.dataproc.mash[5].temperature_mesh;
    tablereg.datafloat[26] = &dataprocess.dataproc.mash[5].temperature_pan1;
    tablereg.datafloat[27] = &dataprocess.dataproc.mash[5].temperature_pan2;
    tablereg.datafloat[28] = &dataprocess.dataproc.mash[5].temperature_mesh_delta;
    tablereg.datafloat[29] = &dataprocess.dataproc.mash[5].temperature_pan_delta;
    tablereg.datafloat[30] = &dataprocess.dataproc.mash[6].temperature_mesh;
    tablereg.datafloat[31] = &dataprocess.dataproc.mash[6].temperature_pan1;
    tablereg.datafloat[32] = &dataprocess.dataproc.mash[6].temperature_pan2;
    tablereg.datafloat[33] = &dataprocess.dataproc.mash[6].temperature_mesh_delta;
    tablereg.datafloat[34] = &dataprocess.dataproc.mash[6].temperature_pan_delta;

    //------

    tablereg.datafloat[35] = &dataprocess.dataproc.hops[0].temperature_hops;
    tablereg.datafloat[36] = &dataprocess.dataproc.hops[0].temperature_pan1;
    tablereg.datafloat[37] = &dataprocess.dataproc.hops[0].temperature_pan2;
    tablereg.datafloat[38] = &dataprocess.dataproc.hops[0].temperature_hops_delta;
    tablereg.datafloat[39] = &dataprocess.dataproc.hops[0].temperature_pan_delta;
    tablereg.datafloat[40] = &dataprocess.dataproc.hops[1].temperature_hops;
    tablereg.datafloat[41] = &dataprocess.dataproc.hops[1].temperature_pan1;
    tablereg.datafloat[42] = &dataprocess.dataproc.hops[1].temperature_pan2;
    tablereg.datafloat[43] = &dataprocess.dataproc.hops[1].temperature_hops_delta;
    tablereg.datafloat[44] = &dataprocess.dataproc.hops[1].temperature_pan_delta;
    tablereg.datafloat[45] = &dataprocess.dataproc.hops[2].temperature_hops;
    tablereg.datafloat[46] = &dataprocess.dataproc.hops[2].temperature_pan1;
    tablereg.datafloat[47] = &dataprocess.dataproc.hops[2].temperature_pan2;
    tablereg.datafloat[48] = &dataprocess.dataproc.hops[2].temperature_hops_delta;
    tablereg.datafloat[49] = &dataprocess.dataproc.hops[2].temperature_pan_delta;
    tablereg.datafloat[50] = &dataprocess.dataproc.hops[3].temperature_hops;
    tablereg.datafloat[51] = &dataprocess.dataproc.hops[3].temperature_pan1;
    tablereg.datafloat[52] = &dataprocess.dataproc.hops[3].temperature_pan2;
    tablereg.datafloat[53] = &dataprocess.dataproc.hops[3].temperature_hops_delta;
    tablereg.datafloat[54] = &dataprocess.dataproc.hops[3].temperature_pan_delta;
    tablereg.datafloat[55] = &dataprocess.dataproc.hops[4].temperature_hops;
    tablereg.datafloat[56] = &dataprocess.dataproc.hops[4].temperature_pan1;
    tablereg.datafloat[57] = &dataprocess.dataproc.hops[4].temperature_pan2;
    tablereg.datafloat[58] = &dataprocess.dataproc.hops[4].temperature_hops_delta;
    tablereg.datafloat[59] = &dataprocess.dataproc.hops[4].temperature_pan_delta;
    tablereg.datafloat[60] = &dataprocess.dataproc.hops[5].temperature_hops;
    tablereg.datafloat[61] = &dataprocess.dataproc.hops[5].temperature_pan1;
    tablereg.datafloat[62] = &dataprocess.dataproc.hops[5].temperature_pan2;
    tablereg.datafloat[63] = &dataprocess.dataproc.hops[5].temperature_hops_delta;
    tablereg.datafloat[64] = &dataprocess.dataproc.hops[5].temperature_pan_delta;
    tablereg.datafloat[65] = &dataprocess.dataproc.hops[6].temperature_hops;
    tablereg.datafloat[66] = &dataprocess.dataproc.hops[6].temperature_pan1;
    tablereg.datafloat[67] = &dataprocess.dataproc.hops[6].temperature_pan2;
    tablereg.datafloat[68] = &dataprocess.dataproc.hops[6].temperature_hops_delta;
    tablereg.datafloat[69] = &dataprocess.dataproc.hops[6].temperature_pan_delta;

    //------
    tablereg.datauint[0] = (uint32_t *)&dataprocess.dataproc.mash[0].induct1_use;
    tablereg.datauint[1] = (uint32_t *)&dataprocess.dataproc.mash[0].induct2_use;
    tablereg.datauint[2] = (uint32_t *)&dataprocess.dataproc.mash[0].maxpwm;
    tablereg.datauint[3] = (uint32_t *)&dataprocess.dataproc.mash[0].time_ms;
    tablereg.datauint[4] = (uint32_t *)&dataprocess.dataproc.mash[0].numbermesh;
    tablereg.datauint[5] = (uint32_t *)&dataprocess.dataproc.mash[1].induct1_use;
    tablereg.datauint[6] = (uint32_t *)&dataprocess.dataproc.mash[1].induct2_use;
    tablereg.datauint[7] = (uint32_t *)&dataprocess.dataproc.mash[1].maxpwm;
    tablereg.datauint[8] = (uint32_t *)&dataprocess.dataproc.mash[1].time_ms;
    tablereg.datauint[9] = (uint32_t *)&dataprocess.dataproc.mash[1].numbermesh;
    tablereg.datauint[10] = (uint32_t *)&dataprocess.dataproc.mash[2].induct1_use;
    tablereg.datauint[11] = (uint32_t *)&dataprocess.dataproc.mash[2].induct2_use;
    tablereg.datauint[12] = (uint32_t *)&dataprocess.dataproc.mash[2].maxpwm;
    tablereg.datauint[13] = (uint32_t *)&dataprocess.dataproc.mash[2].time_ms;
    tablereg.datauint[14] = (uint32_t *)&dataprocess.dataproc.mash[2].numbermesh;
    tablereg.datauint[15] = (uint32_t *)&dataprocess.dataproc.mash[3].induct1_use;
    tablereg.datauint[16] = (uint32_t *)&dataprocess.dataproc.mash[3].induct2_use;
    tablereg.datauint[17] = (uint32_t *)&dataprocess.dataproc.mash[3].maxpwm;
    tablereg.datauint[18] = (uint32_t *)&dataprocess.dataproc.mash[3].time_ms;
    tablereg.datauint[19] = (uint32_t *)&dataprocess.dataproc.mash[3].numbermesh;
    tablereg.datauint[20] = (uint32_t *)&dataprocess.dataproc.mash[4].induct1_use;
    tablereg.datauint[21] = (uint32_t *)&dataprocess.dataproc.mash[4].induct2_use;
    tablereg.datauint[22] = (uint32_t *)&dataprocess.dataproc.mash[4].maxpwm;
    tablereg.datauint[23] = (uint32_t *)&dataprocess.dataproc.mash[4].time_ms;
    tablereg.datauint[24] = (uint32_t *)&dataprocess.dataproc.mash[4].numbermesh;
    tablereg.datauint[25] = (uint32_t *)&dataprocess.dataproc.mash[5].induct1_use;
    tablereg.datauint[26] = (uint32_t *)&dataprocess.dataproc.mash[5].induct2_use;
    tablereg.datauint[27] = (uint32_t *)&dataprocess.dataproc.mash[5].maxpwm;
    tablereg.datauint[28] = (uint32_t *)&dataprocess.dataproc.mash[5].time_ms;
    tablereg.datauint[29] = (uint32_t *)&dataprocess.dataproc.mash[5].numbermesh;
    tablereg.datauint[30] = (uint32_t *)&dataprocess.dataproc.mash[6].induct1_use;
    tablereg.datauint[31] = (uint32_t *)&dataprocess.dataproc.mash[6].induct2_use;
    tablereg.datauint[32] = (uint32_t *)&dataprocess.dataproc.mash[6].maxpwm;
    tablereg.datauint[33] = (uint32_t *)&dataprocess.dataproc.mash[6].time_ms;
    tablereg.datauint[34] = (uint32_t *)&dataprocess.dataproc.mash[6].numbermesh;
    //-
    tablereg.datauint[35] = (uint32_t *)&dataprocess.dataproc.hops[0].induct1_use;
    tablereg.datauint[36] = (uint32_t *)&dataprocess.dataproc.hops[0].induct2_use;
    tablereg.datauint[37] = (uint32_t *)&dataprocess.dataproc.hops[0].maxpwm;
    tablereg.datauint[38] = (uint32_t *)&dataprocess.dataproc.hops[0].time_ms;
    tablereg.datauint[39] = (uint32_t *)&dataprocess.dataproc.hops[0].numberhops;
    tablereg.datauint[40] = (uint32_t *)&dataprocess.dataproc.hops[1].induct1_use;
    tablereg.datauint[41] = (uint32_t *)&dataprocess.dataproc.hops[1].induct2_use;
    tablereg.datauint[42] = (uint32_t *)&dataprocess.dataproc.hops[1].maxpwm;
    tablereg.datauint[43] = (uint32_t *)&dataprocess.dataproc.hops[1].time_ms;
    tablereg.datauint[44] = (uint32_t *)&dataprocess.dataproc.hops[1].numberhops;
    tablereg.datauint[45] = (uint32_t *)&dataprocess.dataproc.hops[2].induct1_use;
    tablereg.datauint[46] = (uint32_t *)&dataprocess.dataproc.hops[2].induct2_use;
    tablereg.datauint[47] = (uint32_t *)&dataprocess.dataproc.hops[2].maxpwm;
    tablereg.datauint[48] = (uint32_t *)&dataprocess.dataproc.hops[2].time_ms;
    tablereg.datauint[49] = (uint32_t *)&dataprocess.dataproc.hops[2].numberhops;
    tablereg.datauint[50] = (uint32_t *)&dataprocess.dataproc.hops[3].induct1_use;
    tablereg.datauint[51] = (uint32_t *)&dataprocess.dataproc.hops[3].induct2_use;
    tablereg.datauint[52] = (uint32_t *)&dataprocess.dataproc.hops[3].maxpwm;
    tablereg.datauint[53] = (uint32_t *)&dataprocess.dataproc.hops[3].time_ms;
    tablereg.datauint[54] = (uint32_t *)&dataprocess.dataproc.hops[3].numberhops;
    tablereg.datauint[55] = (uint32_t *)&dataprocess.dataproc.hops[4].induct1_use;
    tablereg.datauint[56] = (uint32_t *)&dataprocess.dataproc.hops[4].induct2_use;
    tablereg.datauint[57] = (uint32_t *)&dataprocess.dataproc.hops[4].maxpwm;
    tablereg.datauint[58] = (uint32_t *)&dataprocess.dataproc.hops[4].time_ms;
    tablereg.datauint[59] = (uint32_t *)&dataprocess.dataproc.hops[4].numberhops;
    tablereg.datauint[60] = (uint32_t *)&dataprocess.dataproc.hops[5].induct1_use;
    tablereg.datauint[61] = (uint32_t *)&dataprocess.dataproc.hops[5].induct2_use;
    tablereg.datauint[62] = (uint32_t *)&dataprocess.dataproc.hops[5].maxpwm;
    tablereg.datauint[63] = (uint32_t *)&dataprocess.dataproc.hops[5].time_ms;
    tablereg.datauint[64] = (uint32_t *)&dataprocess.dataproc.hops[5].numberhops;
    tablereg.datauint[65] = (uint32_t *)&dataprocess.dataproc.hops[6].induct1_use;
    tablereg.datauint[66] = (uint32_t *)&dataprocess.dataproc.hops[6].induct2_use;
    tablereg.datauint[67] = (uint32_t *)&dataprocess.dataproc.hops[6].maxpwm;
    tablereg.datauint[68] = (uint32_t *)&dataprocess.dataproc.hops[6].time_ms;
    tablereg.datauint[69] = (uint32_t *)&dataprocess.dataproc.hops[6].numberhops;
    tablereg.datauint[70] = (uint32_t *)&dataprocess.dataproc.mode_work;
    tablereg.datauint[71] = (uint32_t *)&pumponof.pump1;
    tablereg.datauint[72] = (uint32_t *)&pumponof.pump2;
    tablereg.datauint[73] = (uint32_t *)&pumponof.pump3;
}
//-----------------------------------------------------------------------------//
void task_updatereg()
{

    for (;;)
    {

        if (parametrmodbus.eventwrite == 1)
        {
            for (int i = 0; i < 70; i++)
            {
                portENTER_CRITICAL(&param_lock);
                *(tablereg.datafloat[i]) = holding_reg_params.data_fl[i];
                portEXIT_CRITICAL(&param_lock);
            }

            for (uint32_t i = 0, scaltime = 3; i < 74; i++)
            {
                portENTER_CRITICAL(&param_lock);
                if ((i == scaltime) && (i < 70))
                {
                    *(tablereg.datauint[i]) = holding_reg_params.data_ui[i] * 60000;
                    scaltime += 5;
                }
                else
                {
                    *(tablereg.datauint[i]) = holding_reg_params.data_ui[i];
                }
                portEXIT_CRITICAL(&param_lock);
            }
            parametrmodbus.eventwrite = 0;
            save_data_process();
        }
        else
        {

            for (int i = 0; i < 70; i++)
            {
                portENTER_CRITICAL(&param_lock);
                holding_reg_params.data_fl[i] = *(tablereg.datafloat[i]);
                portEXIT_CRITICAL(&param_lock);
            }

            for (uint32_t i = 0, scaltime = 3; i < 74; i++)
            {
                portENTER_CRITICAL(&param_lock);
                if ((i == scaltime) && (i < 70))
                {
                    holding_reg_params.data_ui[i] = *(tablereg.datauint[i]) / 60000;
                    scaltime += 5;
                }
                else
                {
                    holding_reg_params.data_ui[i] = *(tablereg.datauint[i]);
                }
                portEXIT_CRITICAL(&param_lock);
            }
            vTaskDelay(100 / portTICK_PERIOD_MS);

            portENTER_CRITICAL(&param_lock);
            input_reg_params.data_fl[0] = temperature; // 0 adr
            input_reg_params.data_fl[1] = TEM_IND1;
            input_reg_params.data_fl[2] = TEM_IND2;
            input_reg_params.data_fl[3] = TEM_IGBT1;
            input_reg_params.data_fl[4] = TEM_IGBT2;
            input_reg_params.data_fl[5] = (float)DataFromIndHeater.structurdata.i_inductor_1 / 10;
            input_reg_params.data_fl[6] = (float)DataFromIndHeater.structurdata.i_inductor_2 / 10;
            input_reg_params.data_fl[7] = DataFromIndHeater.structurdata.u_rectifler;
            input_reg_params.data_fl[8] = tempreqmesh_hops;

            input_reg_params.data_ui[0] = donetimemash; // 128 adr
            input_reg_params.data_ui[1] = donetimehops;
            input_reg_params.data_ui[2] = actualmashpause;
            input_reg_params.data_ui[3] = actualhopspause;
            input_reg_params.data_ui[4] = DataFromIndHeater.structurdata.statusinductor;
            input_reg_params.data_ui[5] = DataFromIndHeater.structurdata.factpwm1;
            input_reg_params.data_ui[6] = DataFromIndHeater.structurdata.factpwm2;
            input_reg_params.data_ui[7] = errormesage & 0x000000ff;
            input_reg_params.data_ui[8] = 25;

            portEXIT_CRITICAL(&param_lock);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
//-----------------------------------------------------------------------------//