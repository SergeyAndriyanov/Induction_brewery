//---------------------------------------------------------------------------------------------------
#ifndef WIFI_H_
#define WIFI_H_
//-----------------------------------------------------------------------------//
#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_wifi.h"
//-----------------------------------------------------------------------------//

#define STACK_SIZE_WIFI_TASK 20000
#define MAXPASSNUM (sizeof(char) * 20)
#define MAXSSIDNUM (sizeof(char) * 20)
#define KEY_WIFI (const char *)"paramwifi"
#define STORAGE_NAMESPACE_P_WIFI (const char *)"storparamwifi"
#define CONFIG_ESP_WIFI_CHANNEL 1
#define CONFIG_ESP_MAX_STA_CONN 1
//-----------------------------------------------------------------------------//

typedef struct
{
    uint8_t wifionoff;
    char wifiSSID[MAXSSIDNUM + sizeof(char)];
    char wifipasswd[MAXPASSNUM + sizeof(char)];
    uint16_t crc;
} TparamWifi;
typedef union
{
    TparamWifi paramWifi_STR;
    uint8_t buf[sizeof(TparamWifi)];

} TUparamWifi;

extern TUparamWifi paramWifi;
extern uint8_t wifioneinit;
//-----------------------------------------------------------------------------//

void runtaskwifi();
void task_wifi(void *arg);
void initparamwifi();
uint8_t read_param_wifi();
esp_err_t save_param_wifi();
void load_default_paramwifi();
void wifi_init_softap();
void wifi_deinit_softap();
esp_netif_t *get_apwifi_netif();
//-----------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------------------------------
