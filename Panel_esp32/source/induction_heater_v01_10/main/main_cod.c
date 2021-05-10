
/*
 *  
 *
 *   Created on: 01 апр. 2021 г.
 *   Author: Sergey Andriyanov
 *   emai:asvlabpost@gmail.com
 *  
 */
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "lcd.h"
#include "hw_init.h"
#include "driver/gpio.h"
#include "wchar.h"
#include "MAX31865.h"
#include "dac_aud.h"
#include "driver/timer.h"
#include "driver/dac.h"
#include "inductionboard.h"
#include "key.h"
#include "datablock.h"
#include "menu.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "process_work.h"

void app_main(void)
{    
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_deinit();
        nvs_flash_erase();
        err = nvs_flash_init();
    }
    if (err != ESP_OK)
    {
        esp_restart();
    }
    load_dataproc();
    gpio_conf();

    dac_init_aud();
    runtaskaudio();    
    selectplayaudio(LOAD_AUDIO);

    lcdinit4bit();
    lcdsendcmd(LCD_FUNCTION_SET_COM | LCD_P_FLAF);
    lcdsendcmd(LCD_DYSPLAY_ONOFF_COM);
    lcdsendcmd(LCD_CLEAR_COM);
    lcdsendcmd(LCD_ENTRY_MODE_SET_COM | LCD_ID_FLAF);
    lcdsendcmd(LCD_RETURN_HOME_COM);
    lcdsendcmd(LCD_DYSPLAY_ONOFF_COM | LCD_CRS_NO_BLINK_NO_FLAF | LCD_D_FLAF);
    lcdsendcmd(LCD_CLEAR_COM);
    lcdbufclean();

    lcdmessage(1, 0, 1, 20);
    lcdmessage(2, 0, 2, 20);
    lcdmessage(3, 0, 3, 20);
    lcdmessage(4, 0, 4, 20);

    init_spi();
    max31865setrtdthresholds(MAX31865_MIN_PT100, MAX31865_MAX_PT100);
    run_task_getstatus_pt100();

    uart_2_induc_pcb_init();
    runtaskuart2();

    ControlFanRtlTask_run();
    runtaskkey();

    // init

    vTaskDelay(3000 / portTICK_PERIOD_MS);
    runtaskmenu();
    run_task_process_work();
    for (;;)
    {

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    fflush(stdout);
    esp_restart();
}
