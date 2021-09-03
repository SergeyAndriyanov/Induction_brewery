
/*
 *  
 *
 *   Created on: 01 апр. 2021 г.
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

//-----------------------------------------------------------------------------//
Tparametrkey parametrkey;
//-----------------------------------------------------------------------------//

uint8_t waiter_speedup = 0;
uint8_t waiter_speeddown = 0;
//-----------------------------------------------------------------------------//
void runtaskkey()
{
    xTaskCreate(task_key, "task_key", STACK_SIZE_KEY_TASK, (void *)&parametrkey, (configMAX_PRIORITIES - 4), NULL);
}
//-----------------------------------------------------------------------------//
void task_key(void *arg)
{
    Tparametrkey *tem = (Tparametrkey *)arg;
    while (1)
    {

        if ((gpio_get_level(KEY1) == 0) && (gpio_get_level(KEY2) == 1) && (gpio_get_level(KEY3) == 1) && (gpio_get_level(KEY4) == 1))
        {
            if (tem->countfilterkey1 < KEY_FILTER)
            {
                tem->countfilterkey1++;
            }
            else
            {
                if (tem->key1 == 0)
                {
                    selectplayaudio(KEYPRESS_AUDIO);
                }
                tem->key1 = 1;
            }
        }
        else
        {
            if (tem->countfilterkey1 > 0)
            {
                tem->countfilterkey1--;
            }
            else
            {
                tem->key1 = 0;
            }
        }

        if ((gpio_get_level(KEY1) == 1) && (gpio_get_level(KEY2) == 0) && (gpio_get_level(KEY3) == 1) && (gpio_get_level(KEY4) == 1))
        {
            if (tem->countfilterkey2 < KEY_FILTER)
            {
                tem->countfilterkey2++;
            }
            else
            {
                if (tem->key2 == 0)
                {
                    selectplayaudio(KEYPRESS_AUDIO);
                }
                tem->key2 = 1;
            }
        }
        else
        {
            if (tem->countfilterkey2 > 0)
            {
                tem->countfilterkey2--;
            }
            else
            {
                tem->key2 = 0;
            }
        }

        if ((gpio_get_level(KEY1) == 1) && (gpio_get_level(KEY2) == 1) && (gpio_get_level(KEY3) == 0) && (gpio_get_level(KEY4) == 1))
        {
            if (tem->countfilterkey3 < KEY_FILTER)
            {
                tem->countfilterkey3++;
            }
            else
            {
                if (waiter_speedup < 200)
                    waiter_speedup++;

                if (tem->key3 == 0)
                {

                    selectplayaudio(KEYPRESS_AUDIO);
                }
                tem->key3 = 1;
            }
        }
        else
        {
            if (tem->countfilterkey3 > 0)
            {
                tem->countfilterkey3--;
            }
            else
            {
                tem->key3 = 0;
                waiter_speedup = 0;
            }
        }

        if ((gpio_get_level(KEY1) == 1) && (gpio_get_level(KEY2) == 1) && (gpio_get_level(KEY3) == 1) && (gpio_get_level(KEY4) == 0))
        {
            if (tem->countfilterkey4 < KEY_FILTER)
            {
                tem->countfilterkey4++;
            }
            else
            {
                if (waiter_speeddown < 200)
                    waiter_speeddown++;
                if (tem->key4 == 0)
                {

                    selectplayaudio(KEYPRESS_AUDIO);
                }

                tem->key4 = 1;
            }
        }
        else
        {
            if (tem->countfilterkey4 > 0)
            {
                tem->countfilterkey4--;
            }
            else
            {
                tem->key4 = 0;
                waiter_speeddown = 0;
            }
        }



        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
//-----------------------------------------------------------------------------//

bool getkeymenu()
{
    static uint8_t key1_trig;
    bool ret = false;

    if (parametrkey.key1 == 1)
    {
        if (key1_trig == 1)
        {
            key1_trig = 0;
            ret = true;
        }
    }
    else
    {
        key1_trig = 1;
    }
    return ret;
}

//-----------------------------------------------------------------------------//
bool getkeyenter()
{
    static uint8_t key2_trig;
    bool ret = false;

    if (parametrkey.key2 == 1)
    {
        if (key2_trig == 1)
        {
            key2_trig = 0;
            ret = true;
        }
    }
    else
    {
        key2_trig = 1;
    }
    return ret;
}

//-----------------------------------------------------------------------------//
bool getkeyup()
{
    static uint8_t key3_trig;
    bool ret = false;

    if (parametrkey.key3 == 1)
    {
        if (key3_trig == 1)
        {
            key3_trig = 0;
            ret = true;
        }
    }
    else
    {
        key3_trig = 1;
    }
    return ret;
}

//-----------------------------------------------------------------------------//
bool getkeydown()
{
    static uint8_t key4_trig;
    bool ret = false;

    if (parametrkey.key4 == 1)
    {
        if (key4_trig == 1)
        {
            key4_trig = 0;
            ret = 1;
        }
    }
    else
    {
        key4_trig = true;
    }
    return ret;
}

//-----------------------------------------------------------------------------//