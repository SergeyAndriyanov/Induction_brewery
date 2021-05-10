
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
#include "inductionboard.h"
#include "CRCTable.c"
#include "math.h"
//-----------------------------------------------------------------------------//
TUFromind DataFromIndHeater;
uint8_t flagdatardy = 0;
TUsenddev DataToInd;
Tfanrtlparametr parametrfasnrtl;
//-----------------------------------------------------------------------------//
void taskuart2_read(void *arg)
{

    Tfanrtlparametr *tem = (Tfanrtlparametr *)arg;
    TUFromind tempinfromheater;
    uint8_t littlendbuf[sizeof(TUFromind)];
    uint16_t crc = 0;
    while (1)
    {
        //Read data from UART
        uart_read_bytes(INDUCTION_UART_NUM, littlendbuf, sizeof(TUFromind), PACKET_READ_UART2_TICS);
        tempinfromheater.buf[0] = littlendbuf[0];
        tempinfromheater.buf[1] = littlendbuf[2];
        tempinfromheater.buf[2] = littlendbuf[1];
        tempinfromheater.buf[3] = littlendbuf[4];
        tempinfromheater.buf[4] = littlendbuf[3];
        tempinfromheater.buf[5] = littlendbuf[6];
        tempinfromheater.buf[6] = littlendbuf[5];
        tempinfromheater.buf[7] = littlendbuf[7];
        tempinfromheater.buf[8] = littlendbuf[8];
        tempinfromheater.buf[9] = littlendbuf[9];
        tempinfromheater.buf[10] = littlendbuf[10];
        tempinfromheater.buf[11] = littlendbuf[11];
        tempinfromheater.buf[12] = littlendbuf[12];
        tempinfromheater.buf[13] = littlendbuf[13];
        tempinfromheater.buf[14] = littlendbuf[14];
        tempinfromheater.buf[15] = littlendbuf[15];
        tempinfromheater.buf[16] = littlendbuf[17];
        tempinfromheater.buf[17] = littlendbuf[16];
        tempinfromheater.buf[18] = littlendbuf[19];
        tempinfromheater.buf[19] = littlendbuf[18];
        crc = DataCrc(littlendbuf, (sizeof(TUFromind) - 2));

        if (crc == tempinfromheater.structurdata.crc)
        {
            if (tempinfromheater.structurdata.adress == INDUCTION_PCB_ADRESS)
            {
                memcpy(DataFromIndHeater.buf, tempinfromheater.buf, sizeof(TUFromind));
                flagdatardy = 1;
                tem->uarterrnum = 0;
            }
        }
        else
        {
            if (tem->stat == INDUC_ON)
            {
                tem->uarterrnum++;
            }
            else
            {
                tem->uarterrnum = 0;
            }
        }
        
    }
    vTaskDelete(NULL);
}
//-----------------------------------------------------------------------------//
//-----------------------------------------------------------------------------//
void taskuart2_write()
{
    while (1)
    {
        uint8_t sendbigend[sizeof(TUsenddev)];
        memset(sendbigend, 0, sizeof(TUsenddev));

        DataToInd.structurdata.adress = INDUCTION_PCB_ADRESS;
        DataToInd.structurdata.crc = DataCrc(DataToInd.buf, (sizeof(TUsenddev) - 2));

        sendbigend[0] = DataToInd.buf[0];
        sendbigend[1] = DataToInd.buf[1];
        sendbigend[2] = DataToInd.buf[2];
        sendbigend[3] = DataToInd.buf[3];
        sendbigend[4] = DataToInd.buf[5];
        sendbigend[5] = DataToInd.buf[4];

        uart_write_bytes(INDUCTION_UART_NUM, (char *)sendbigend, sizeof(TUsenddev));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}
//-----------------------------------------------------------------------------//
void runtaskuart2()
{
    xTaskCreate(taskuart2_read, "taskuart2_read", UART2_TASK_STACK_SIZE, (void *)&parametrfasnrtl, (configMAX_PRIORITIES-2), NULL);
    xTaskCreate(taskuart2_write, "taskuart2_write", UART2_TASK_STACK_SIZE, NULL, (configMAX_PRIORITIES-2), NULL);
}
//-----------------------------------------------------------------------------//
uint16_t DataCrc(uint8_t nData[], uint16_t wLength)
{

    uint32_t nTemp;
    uint16_t wCRCWord = 0xFFFF;
    uint16_t ii = 0;

    while (wLength != 0)
    {
        nTemp = (nData[ii] ^ wCRCWord) & 0x000000ff;
        ii++;
        wCRCWord >>= 8;
        wCRCWord ^= wCRCTable[nTemp];
        wLength--;
    }
    return wCRCWord;

} //end

//-----------------------------------------------------------------------------//
// calculate temperature termistor
double stainhart(uint8_t ValAdc, double A, double B, double C, double ResistorDiv, uint16_t adcmaxcod, double adcmincod, double adcrefV)
{
    double temp = 0, res = 0;
    if (ValAdc > 0)
    {
        res = adcrefV / (adcrefV / adcmaxcod * ValAdc) * ResistorDiv;
        temp = 1.0 / (A + B * log10(res) + C * pow(fabs((double)log10(res)), 3));
    }
    return temp;
}
//-----------------------------------------------------------------------------//
void ControlFanRtlTask_run()
{

    gpio_set_level(FAN, HIGHT);
    xTaskCreate(taskfanrtlcontrol, "taskfanrtlcontrol", TASK_STACKSIZE_FAN_RTL, (void *)&parametrfasnrtl, (configMAX_PRIORITIES-3), NULL);
}
//-----------------------------------------------------------------------------//
void taskfanrtlcontrol(void *arg)
{
    Tfanrtlparametr *tem = (Tfanrtlparametr *)arg;
    while (1)
    {
        switch (tem->stat)
        {
        case INDUC_ON:
        {
            gpio_set_level(FAN, HIGHT);
            if (tem->uarterrnum >= UART_MAX_ERR)
            {
                rebbotpowerboard();
                tem->uarterrnum = 0;
            }
            else
            {
                gpio_set_level(RTL, HIGHT);
            }

            tem->timeoff = 0;
            break;
        }
        case INDUC_OFF:
        {
            if (tem->timeoff <= TIME_OF_FUN)
            {
                tem->timeoff++;
            }
            else
            {
                gpio_set_level(FAN, LOW);
            }
            gpio_set_level(RTL, LOW);
            break;
        }
        default:
        {
            tem->stat = INDUC_OFF;
            break;
        }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
//-----------------------------------------------------------------------------//
void rebbotpowerboard()
{
    gpio_set_level(RTL, LOW);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    gpio_set_level(RTL, HIGHT);
}
//-----------------------------------------------------------------------------//
