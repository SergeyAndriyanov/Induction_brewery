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
#include "wifi.h"
#include "inductionboard.h"
#include "wchar.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "modbus.h"

/*
 *  
 *
 *   Created on: 12 авг. 2021 г.
 *   Author: Sergey Andriyanov
 *   emai:asvlabpost@gmail.com
 *  
 */
//-----------------------------------------------------------------------------//
TUparamWifi paramWifi;
uint8_t wifionoffold = 0;
uint8_t wifioneinit = 0;
uint8_t wifionedeinit = 0;
//-----------------------------------------------------------------------------//
static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;
static esp_netif_t *ap_netif = NULL;

//-----------------------------------------------------------------------------//
void runtaskwifi()
{
    xTaskCreate(task_wifi, "task_wifi", STACK_SIZE_WIFI_TASK, (void *)&paramWifi, (configMAX_PRIORITIES - 4), NULL);
}
//-----------------------------------------------------------------------------//
void task_wifi(void *arg)
{

    TUparamWifi *parg = (TUparamWifi *)arg;
    wifionoffold = parg->paramWifi_STR.wifionoff;

    while (1)
    {
        if (wifionoffold != parg->paramWifi_STR.wifionoff)
        {
            wifionoffold = parg->paramWifi_STR.wifionoff;
            if (parg->paramWifi_STR.wifionoff == 1)
            {

                load_default_paramwifi();
                save_param_wifi();
            }
        }

        if (parg->paramWifi_STR.wifionoff == 1)
        {
            wifionedeinit = 0;
            if (wifioneinit == 0)
            {
                wifi_init_softap();
            }
        }
        else
        {
            wifioneinit = 0;
            if (wifionedeinit == 0)
            {
                wifionedeinit = 1;
                wifi_deinit_softap();
            }
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

//-----------------------------------------------------------------------------//
void initparamwifi()
{
    if (read_param_wifi() == 0)
    {
        load_default_paramwifi();
        save_param_wifi();

        if (read_param_wifi() == 0)
        {
            esp_restart();
        }
    }
}
//-----------------------------------------------------------------------------//
uint8_t read_param_wifi()
{
    uint8_t ret = 0;
    nvs_handle_t paramwifi_handle;
    nvs_open(STORAGE_NAMESPACE_P_WIFI, NVS_READWRITE, &paramwifi_handle);
    size_t required_size = sizeof(TUparamWifi);
    nvs_get_blob(paramwifi_handle, KEY_WIFI, paramWifi.buf, &required_size);
    uint16_t crc = DataCrc(paramWifi.buf, (sizeof(TUparamWifi) - 2));
    nvs_commit(paramwifi_handle);
    nvs_close(paramwifi_handle);
    if (crc != paramWifi.paramWifi_STR.crc)
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
esp_err_t save_param_wifi()
{
    nvs_handle_t paramwifi_handle;
    esp_err_t err;
    err = nvs_open(STORAGE_NAMESPACE_P_WIFI, NVS_READWRITE, &paramwifi_handle);
    if (err != ESP_OK)
        return err;
    size_t required_size = sizeof(TUparamWifi);
    paramWifi.paramWifi_STR.crc = DataCrc(paramWifi.buf, (sizeof(TUparamWifi) - 2));
    err = nvs_erase_all(paramwifi_handle);
    if (err != ESP_OK)
        return err;

    err = nvs_set_blob(paramwifi_handle, KEY_WIFI, paramWifi.buf, required_size);

    if (err != ESP_OK)
        return err;
    err = nvs_commit(paramwifi_handle);
    if (err != ESP_OK)
        return err;
    nvs_close(paramwifi_handle);

    return ESP_OK;
}
//-----------------------------------------------------------------------------//
void load_default_paramwifi()
{

    uint8_t random_buff[MAXPASSNUM];
    char strkey[] = {"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!S"};
    memset(random_buff, 0, sizeof(random_buff));
    esp_fill_random(random_buff, MAXPASSNUM);

    char passs[MAXPASSNUM];
    memset(passs, 0, MAXPASSNUM);

    for (int i = 0; i < MAXPASSNUM; i++)
    {

        if (i < 12)
        {
            passs[i] = strkey[(random_buff[i] & 0x3f)];
        }
        else
        {
            passs[i] = 0;
        }
    }

    memset(paramWifi.paramWifi_STR.wifiSSID, 0, MAXSSIDNUM);
    memset(paramWifi.paramWifi_STR.wifipasswd, 0, MAXPASSNUM);

    sprintf(paramWifi.paramWifi_STR.wifiSSID, "Induct_Brew%03i", random_buff[0]);
    sprintf(paramWifi.paramWifi_STR.wifipasswd, "%s", passs);

    paramWifi.paramWifi_STR.wifionoff = 1;
    paramWifi.paramWifi_STR.crc = DataCrc(paramWifi.buf, (sizeof(TUparamWifi) - 2));
}
//-----------------------------------------------------------------------------//
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        //ESP_LOGI(TAG, "station " MACSTR " join, AID=%d", MAC2STR(event->mac), event->aid);
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        selectplayaudio(CONNECTWIFI_AUDIO); //process done
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        //ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d", MAC2STR(event->mac), event->aid);
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        selectplayaudio(DISCONNECTWIFI_AUDIO); //process done
    }
}
//-----------------------------------------------------------------------------//
void wifi_init_softap()
{

    esp_log_level_set("wifi", ESP_LOG_VERBOSE);

    if (wifioneinit == 1)
    {
        return;
    }

    ESP_ERROR_CHECK(esp_netif_init());
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ap_netif = esp_netif_create_default_wifi_ap();
    assert(ap_netif);

    esp_netif_ip_info_t ipInfo = {0};
    esp_netif_dns_info_t dns_info = {0};
    esp_netif_set_ip4_addr(&ipInfo.netmask, 255, 255, 255, 0);
    esp_netif_set_ip4_addr(&ipInfo.ip, 192, 168, 1, 1);
    esp_netif_set_ip4_addr(&ipInfo.gw, 192, 168, 1, 1);
    esp_netif_set_ip4_addr(&dns_info.ip.u_addr.ip4, 8, 8, 8, 8);

    //  IP4_ADDR(&ipInfo.ip, 192,168,1,1);
    // IP4_ADDR(&ipInfo.gw, 192,168,1,1);
    // IP4_ADDR(&ipInfo.netmask, 255,255,255,0);
    // IP_ADDR4(&dns_info.ip, 8, 8, 8, 8);
    ESP_ERROR_CHECK(esp_netif_dhcps_stop(ap_netif));
    ESP_ERROR_CHECK(esp_netif_set_ip_info(ap_netif, &ipInfo));
    ESP_ERROR_CHECK(esp_netif_set_dns_info(ap_netif, ESP_NETIF_DNS_MAIN, &dns_info));
    ESP_ERROR_CHECK(esp_netif_dhcps_start(ap_netif));

    ESP_ERROR_CHECK(esp_wifi_set_default_wifi_ap_handlers());
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_country_t config_country = {.cc = "US", .schan = 1, .nchan = 11, WIFI_COUNTRY_POLICY_MANUAL};
    ESP_ERROR_CHECK(esp_wifi_set_country(&config_country));

    wifi_config_t wifi_config = {0};
    memcpy(wifi_config.ap.ssid, paramWifi.paramWifi_STR.wifiSSID, strlen(paramWifi.paramWifi_STR.wifiSSID));
    wifi_config.ap.ssid_len = strlen(paramWifi.paramWifi_STR.wifiSSID);
    memcpy(wifi_config.ap.password, paramWifi.paramWifi_STR.wifipasswd, strlen(paramWifi.paramWifi_STR.wifipasswd));
    wifi_config.ap.channel = CONFIG_ESP_WIFI_CHANNEL;
    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    wifi_config.ap.ssid_hidden = 0;
    wifi_config.ap.max_connection = CONFIG_ESP_MAX_STA_CONN;
    wifi_config.ap.beacon_interval = 30000;

    ESP_LOGE("ttt", "ssid (%s) pasw (%s) auy%i", wifi_config.ap.ssid, wifi_config.ap.password, wifi_config.ap.authmode);
    //ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    wifioneinit = 1;
}
//-----------------------------------------------------------------------------//
void wifi_deinit_softap()
{
    //ESP_ERROR_CHECK(esp_wifi_disconnect());
    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_deinit());
    ESP_ERROR_CHECK(esp_event_loop_delete_default());
    vEventGroupDelete(wifi_event_group);
    ESP_ERROR_CHECK(esp_wifi_clear_default_wifi_driver_and_handlers(ap_netif));
    esp_netif_destroy(ap_netif);
    ap_netif = NULL;
   
}
//-----------------------------------------------------------------------------//
esp_netif_t *get_apwifi_netif()
{
    return ap_netif;
}
//-----------------------------------------------------------------------------//
