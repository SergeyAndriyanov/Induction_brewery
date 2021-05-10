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

/*
 *  
 *
 *   Created on: 01 апр. 2021 г.
 *   Author: Sergey Andriyanov
 *   emai:asvlabpost@gmail.com
 *  
 */
#include "driver/uart.h"
#include "freertos/queue.h"
#include "key.h"
#include "datablock.h"
#include "inductionboard.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "soc/rtc_wdt.h"

//-----------------------------------------------------------------------------//
TUDataProc dataprocess;
uint8_t mesh_num = 0;
uint8_t hops_num = 0;
uint32_t mesh_act_time = 0;
uint32_t hops_act_time = 0;
Tpidstr pidparametr={2.2,1.5,0.4,1.0,0.0,0.0,0.0};
TPump pumponof={0,0,0};
//-----------------------------------------------------------------------------//
void load_dataproc()
{

    if (read_data_process() == 0)
    {
        loaddefault();
        save_data_process();

        if (read_data_process() == 0)
        {
            esp_restart();
        }
    }
}
//-----------------------------------------------------------------------------//
void loaddefault()
{
    for (int i = 0; i < MAXPAUSMASHING; i++)
    {
        dataprocess.dataproc.mash[i].temperature_mesh = TEMPMESHDEF;
        dataprocess.dataproc.mash[i].temperature_pan1 = MESHTEMPPAN1;
        dataprocess.dataproc.mash[i].temperature_pan2 = MESHTEMPPAN1;
        dataprocess.dataproc.mash[i].temperature_mesh_delta = TEMPMESHDELTA;
        dataprocess.dataproc.mash[i].temperature_pan_delta = TEMPPANMESHDELTA;
        dataprocess.dataproc.mash[i].induct1_use = MESHINDUCTOR1_USE;
        dataprocess.dataproc.mash[i].induct2_use = MESHINDUCTOR2_USE;
        dataprocess.dataproc.mash[i].maxpwm = MESHMAXPWM;
        dataprocess.dataproc.mash[i].time_ms = TIME_MASH;
        dataprocess.dataproc.mash[i].numbermesh = MAXPAUSMASHING;
    }

    for (int i = 0; i < MAXXOPS; i++)
    {
        dataprocess.dataproc.hops[i].temperature_hops = TEMPHOPSDEF;
        dataprocess.dataproc.hops[i].temperature_pan1 = HOPSTEMPPAN1;
        dataprocess.dataproc.hops[i].temperature_pan2 = HOPSTEMPPAN2;
        dataprocess.dataproc.hops[i].temperature_hops_delta = TEMPHOPSDELTA;
        dataprocess.dataproc.hops[i].temperature_pan_delta = TEMPPANHOPSDELTA;
        dataprocess.dataproc.hops[i].induct1_use = HOPSINDUCTOR1_USE;
        dataprocess.dataproc.hops[i].induct2_use = HOPSINDUCTOR2_USE;
        dataprocess.dataproc.hops[i].maxpwm = HOPSMAXPWM;
        dataprocess.dataproc.hops[i].time_ms = TIME_HOPS;
        dataprocess.dataproc.hops[i].numberhops = MAXXOPS;
    }
    dataprocess.dataproc.mode_work = MODE_OFF;

    dataprocess.dataproc.crc = DataCrc(dataprocess.buf, (sizeof(TUDataProc) - 2));
}
//-----------------------------------------------------------------------------//
esp_err_t save_data_process()
{
    nvs_handle_t dataproc_handle;
    esp_err_t err;
    err = nvs_open(STORAGE_NAMESPACE_P, NVS_READWRITE, &dataproc_handle);
    if (err != ESP_OK)
        return err;
    size_t required_size = sizeof(TUDataProc);
    dataprocess.dataproc.crc = DataCrc(dataprocess.buf, (sizeof(TUDataProc) - 2));
    err = nvs_erase_all(dataproc_handle);
    if (err != ESP_OK)
        return err;

    err = nvs_set_blob(dataproc_handle, KEY_DATAPROCESS, dataprocess.buf, required_size);

    if (err != ESP_OK)
        return err;
    err = nvs_commit(dataproc_handle);
    if (err != ESP_OK)
        return err;
    nvs_close(dataproc_handle);

    return ESP_OK;
}
//-----------------------------------------------------------------------------//
uint8_t read_data_process()
{
    uint8_t ret = 0;
    nvs_handle_t dataproc_handle;
    nvs_open(STORAGE_NAMESPACE_P, NVS_READWRITE, &dataproc_handle);
    size_t required_size = sizeof(TUDataProc);
    nvs_get_blob(dataproc_handle, KEY_DATAPROCESS, dataprocess.buf, &required_size);
    uint16_t crc = DataCrc(dataprocess.buf, (sizeof(TUDataProc) - 2));
    nvs_commit(dataproc_handle);
    nvs_close(dataproc_handle);
    if (crc != dataprocess.dataproc.crc)
    {
        ret = 0;
    }
    else
    {
        ret = 1;
    }

    return ret;
}
//-----------------------------------------------------------------------------//