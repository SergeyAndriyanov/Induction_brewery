
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
#include "esp_intr_alloc.h"
#include "driver/gpio.h"
#include <string.h>
#include <stdio.h>
#include "lcd.h"
#include "hw_init.h"
#include "MAX31865.h"
#include "dac_aud.h"
#include "driver/timer.h"
#include "driver/dac.h"
#include "audioloadsystem.c"
#include "audiokeypress.c"
#include "audioboilnrun.c"
#include "audiomeshrun.c"
#include "driver/i2s.h"
#include "esp_log.h"
#include "audiotermopause.c"
#include "audiomeshdone.c"
#include "audiohopsadd.c"
#include "audiobboilndone.c"
#include "audioalarmsensor.c"
#include "audioconnect.c"
#include "audiodisconnect.c"


//-----------------------------------------------------------------------------//
audio_info_t sound_wav_info;
SemaphoreHandle_t ReplaySemaphore;
char stopplay = 0;
//-----------------------------------------------------------------------------//
void selectplayaudio(uint8_t numaud)
{

    xSemaphoreTake(ReplaySemaphore, portMAX_DELAY);
    switch (numaud)
    {
    case LOAD_AUDIO:
    {
        if (sound_wav_info.startplay == STARTPLAY)
        {
            stopplay = 1;
            while (sound_wav_info.startplay != STOPPLAY)
            {
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
        }

        sound_wav_info.bytewrite = 0;
        sound_wav_info.sampleRate = DACSAMPLERAIT;
        sound_wav_info.dataLength = sizeof(audioloadsys);
        sound_wav_info.data = (uint8_t *)&audioloadsys[0];
        sound_wav_info.startplay = STARTPLAY;

        break;
    }
    case KEYPRESS_AUDIO:
    {

        if (sound_wav_info.startplay == STARTPLAY)
        {
            stopplay = 1;
            while (sound_wav_info.startplay != STOPPLAY)
            {
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
        }

        sound_wav_info.bytewrite = 0;
        sound_wav_info.sampleRate =  DACSAMPLERAIT;
        sound_wav_info.dataLength = sizeof(audiokeypress);
        sound_wav_info.data = (uint8_t *)&audiokeypress[0];
        sound_wav_info.startplay = STARTPLAY;

        break;
    }

    case MESHRUN_AUDIO:
    {

        if (sound_wav_info.startplay == STARTPLAY)
        {
            stopplay = 1;
            while (sound_wav_info.startplay != STOPPLAY)
            {
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
        }

        sound_wav_info.bytewrite = 0;
        sound_wav_info.sampleRate = DACSAMPLERAIT;
        sound_wav_info.dataLength = sizeof(audiomeshrun);
        sound_wav_info.data = (uint8_t *)&audiomeshrun[0];
        sound_wav_info.startplay = STARTPLAY;

        break;
    }
    case BOILINRUN_AUDIO:
    {

        if (sound_wav_info.startplay == STARTPLAY)
        {
            stopplay = 1;
            while (sound_wav_info.startplay != STOPPLAY)
            {
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
        }

        sound_wav_info.bytewrite = 0;
        sound_wav_info.sampleRate = DACSAMPLERAIT;
        sound_wav_info.dataLength = sizeof(audioboilin);
        sound_wav_info.data = (uint8_t *)&audioboilin[0];
        sound_wav_info.startplay = STARTPLAY;

        break;
    }
    case TERMPPAUSENRUN_AUDIO:
    {

        if (sound_wav_info.startplay == STARTPLAY)
        {
            stopplay = 1;
            while (sound_wav_info.startplay != STOPPLAY)
            {
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
        }

        sound_wav_info.bytewrite = 0;
        sound_wav_info.sampleRate = DACSAMPLERAIT;
        sound_wav_info.dataLength = sizeof(audiotermopause);
        sound_wav_info.data = (uint8_t *)&audiotermopause[0];
        sound_wav_info.startplay = STARTPLAY;

        break;
    }
    case MESHDONE_AUDIO:
    {

        if (sound_wav_info.startplay == STARTPLAY)
        {
            stopplay = 1;
            while (sound_wav_info.startplay != STOPPLAY)
            {
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
        }

        sound_wav_info.bytewrite = 0;
        sound_wav_info.sampleRate = DACSAMPLERAIT;
        sound_wav_info.dataLength = sizeof(audiomeshdone);
        sound_wav_info.data = (uint8_t *)&audiomeshdone[0];
        sound_wav_info.startplay = STARTPLAY;

        break;
    }
    case HOPSADD_AUDIO:
    {

        if (sound_wav_info.startplay == STARTPLAY)
        {
            stopplay = 1;
            while (sound_wav_info.startplay != STOPPLAY)
            {
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
        }

        sound_wav_info.bytewrite = 0;
        sound_wav_info.sampleRate = DACSAMPLERAIT;
        sound_wav_info.dataLength = sizeof(audiohopsadd);
        sound_wav_info.data = (uint8_t *)&audiohopsadd[0];
        sound_wav_info.startplay = STARTPLAY;

        break;
    }
    case BOILINGDONE_AUDIO:
    {

        if (sound_wav_info.startplay == STARTPLAY)
        {
            stopplay = 1;
            while (sound_wav_info.startplay != STOPPLAY)
            {
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
        }

        sound_wav_info.bytewrite = 0;
        sound_wav_info.sampleRate = DACSAMPLERAIT;
        sound_wav_info.dataLength = sizeof(audiobboilndone);
        sound_wav_info.data = (uint8_t *)&audiobboilndone[0];
        sound_wav_info.startplay = STARTPLAY;

        break;
    }
    case ALARMSENSOR_AUDIO:
    {

        if (sound_wav_info.startplay == STARTPLAY)
        {
            stopplay = 1;
            while (sound_wav_info.startplay != STOPPLAY)
            {
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
        }

        sound_wav_info.bytewrite = 0;
        sound_wav_info.sampleRate = DACSAMPLERAIT;
        sound_wav_info.dataLength = sizeof(audioalarmsensor);
        sound_wav_info.data = (uint8_t *)&audioalarmsensor[0];
        sound_wav_info.startplay = STARTPLAY;

        break;
    }

    case CONNECTWIFI_AUDIO:
    {

        if (sound_wav_info.startplay == STARTPLAY)
        {
            stopplay = 1;
            while (sound_wav_info.startplay != STOPPLAY)
            {
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
        }

        sound_wav_info.bytewrite = 0;
        sound_wav_info.sampleRate = DACSAMPLERAIT;
        sound_wav_info.dataLength = sizeof(audioconnect);
        sound_wav_info.data = (uint8_t *)&audioconnect[0];
        sound_wav_info.startplay = STARTPLAY;

        break;
    }

    case DISCONNECTWIFI_AUDIO:
    {

        if (sound_wav_info.startplay == STARTPLAY)
        {
            stopplay = 1;
            while (sound_wav_info.startplay != STOPPLAY)
            {
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
        }

        sound_wav_info.bytewrite = 0;
        sound_wav_info.sampleRate = DACSAMPLERAIT;
        sound_wav_info.dataLength = sizeof(audiodisconnect);
        sound_wav_info.data = (uint8_t *)&audiodisconnect[0];
        sound_wav_info.startplay = STARTPLAY;

        break;
    }



    default:
        
        {
            if (sound_wav_info.startplay == STARTPLAY)
            {
                stopplay = 1;
                while (sound_wav_info.startplay != STOPPLAY)
                {
                    vTaskDelay(10 / portTICK_PERIOD_MS);
                }
            }
            sound_wav_info.bytewrite = 0;
            sound_wav_info.sampleRate = DACSAMPLERAIT;
            sound_wav_info.dataLength = sizeof(audioloadsys);
            sound_wav_info.data = (uint8_t *)&audioloadsys[0];
            sound_wav_info.startplay = STARTPLAY;

            break;
        }
    }
    xSemaphoreGive(ReplaySemaphore);
}
//-----------------------------------------------------------------------------//
void runtaskaudio()
{
    xTaskCreate(task_audio, "task_audio", STACK_SIZE_AUDIO_TASK, (void *)&sound_wav_info, (configMAX_PRIORITIES - 5), NULL);
}
//-----------------------------------------------------------------------------//

void task_audio(void *arg)
{
    audio_info_t *aud = (audio_info_t *)arg;
    while (1)
    {

        if (aud->startplay == STARTPLAY)
        {
        	i2s_set_sample_rates(I2S_NUM_0, aud->sampleRate);
        	i2s_start(I2S_NUM_0);
        	i2s_write(I2S_NUM_0, (void *)aud->data, aud->dataLength, &aud->bytewrite, portMAX_DELAY, &stopplay);
            aud->startplay = STOPPLAY;
            i2s_stop(I2S_NUM_0);
            stopplay = 0;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
//-----------------------------------------------------------------------------//ESP_ERROR_CHECK(
void installdacdma()
{
    i2s_config_t i2s_config = {};
    i2s_config.mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN;
    i2s_config.sample_rate = DACSAMPLERAIT;
    i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
    i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
    i2s_config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
    i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
    i2s_config.dma_buf_count = 8;
    i2s_config.dma_buf_len = 64;
    i2s_config.tx_desc_auto_clear = false;
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_dac_mode(I2S_DAC_CHANNEL_LEFT_EN);
}
//-----------------------------------------------------------------------------//
void unistaldacdma()
{
    i2s_driver_uninstall(I2S_NUM_0);
}
//-----------------------------------------------------------------------------//
void createsemaphre()
{
    ReplaySemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(ReplaySemaphore);
}
//-----------------------------------------------------------------------------//
