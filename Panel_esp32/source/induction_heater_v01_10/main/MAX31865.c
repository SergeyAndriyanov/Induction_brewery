
/*
 *  
 *
 *   Created on: 01 апр. 2021 г.
 *   Author: Sergey Andriyanov
 *   emai:asvlabpost@gmail.com
 *  
 */
#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "lcd.h"
#include "hw_init.h"
#include "MAX31865.h"
#include "dac_aud.h"
#include "math.h"
//-----------------------------------------------------------------------------//
uint8_t statusmax31865 = 0;
uint16_t rtd = 0;
double rtd_temp = 0;
uint8_t step_getstatus = 0;
float temperature = 0.0;
uint8_t trigerrone = 0;
uint8_t countaverage = 0;
double rtd_average = 0;
//-----------------------------------
//------------------------------------------//
esp_err_t max31865senddataspi(uint8_t addr, uint8_t *data, size_t size)
{
    esp_err_t err = ESP_OK;
    spi_transaction_t transaction_spi = {};
    transaction_spi.length = CHAR_BIT * size;
    transaction_spi.rxlength = 0;
    transaction_spi.addr = addr | MAX31865_REG_WRITE_OFFSET;
    transaction_spi.flags = SPI_TRANS_USE_TXDATA;
    memcpy(transaction_spi.tx_data, data, size);
    err = gpio_set_level(CS0, LOW);
    IF_ERR_YES_RETURN_ERR
    err = spi_device_polling_transmit(spi_max31865_handle, &transaction_spi);
    IF_ERR_YES_RETURN_ERR
    err = gpio_set_level(CS0, HIGHT);

    return err;
}

//-----------------------------------------------------------------------------//
esp_err_t max31865readdataspi(uint8_t addr, uint8_t *result, size_t size)
{
    spi_transaction_t transaction_spi = {};
    esp_err_t err = ESP_OK;
    transaction_spi.length = 0;
    transaction_spi.rxlength = CHAR_BIT * size;
    transaction_spi.addr = addr & (MAX31865_REG_WRITE_OFFSET - 1);
    transaction_spi.flags = SPI_TRANS_USE_RXDATA;
    err = gpio_set_level(CS0, LOW);
    IF_ERR_YES_RETURN_ERR
    err = spi_device_polling_transmit(spi_max31865_handle, &transaction_spi);
    IF_ERR_YES_RETURN_ERR
    err = gpio_set_level(CS0, HIGHT);
    IF_ERR_YES_RETURN_ERR
    memcpy(result, transaction_spi.rx_data, size);
    return ESP_OK;
}
//-----------------------------------------------------------------------------//
esp_err_t max31865setrtdthresholds(uint16_t min, uint16_t max)
{
    uint8_t thresholds[4];
    thresholds[0] = (uint8_t)((max << 1) >> CHAR_BIT);
    thresholds[1] = (uint8_t)(max << 1);
    thresholds[2] = (uint8_t)((min << 1) >> CHAR_BIT);
    thresholds[3] = (uint8_t)(min << 1);
    return max31865senddataspi(MAX31865_HIGH_FAULT_REG, thresholds, sizeof(thresholds));
}
//-----------------------------------------------------------------------------//
esp_err_t max31865resetfault()
{
    uint8_t configByte = 0;
    step_getstatus = 0;
    esp_err_t err = max31865readdataspi(MAX31865_CONFIG_REG, &configByte, 1);
    IF_ERR_YES_RETURN_ERR
    configByte = configByte | (1U << MAX31865_CONFIG_FAULTSTATUS_BIT);
    return max31865senddataspi(MAX31865_CONFIG_REG, &configByte, 1);
}
//-----------------------------------------------------------------------------//
esp_err_t msx31865getstatus()
{
    static uint8_t rtdBytes[2];
    double tempdouble = 0.0;
    uint8_t faultByte = 0;
    uint8_t confreg = (MAX31865_FAULT_DETECT_AUTOMAT | (1UL << MAX31865_CONFIG_MAINSFILTER_BIT));
    esp_err_t err = ESP_OK;

    switch (step_getstatus)
    {
    case 0:
    {
        err = max31865resetfault();
        IF_ERR_YES_RETURN_ERR
        step_getstatus = 1;
        break;
    }
    case 1:
    {
        err = max31865senddataspi(MAX31865_CONFIG_REG, &confreg, 1);
        IF_ERR_YES_RETURN_ERR
        step_getstatus = 2;
        break;
    }
    case 2:
    {
        err = max31865readdataspi(MAX31865_CONFIG_REG, &confreg, 1);
        IF_ERR_YES_RETURN_ERR
        if ((~confreg) & 0x0c)
        {
            step_getstatus = 3;
        }
        break;
    }
    case 3:
    {
        err = max31865readdataspi(MAX31865_FAULT_STATUS_REG, &faultByte, 1);
        IF_ERR_YES_RETURN_ERR

        if (faultByte != 0)
        {
            statusmax31865 = faultByte;
            step_getstatus = 0;
        }
        else
        {
            step_getstatus = 4;
        }

        break;
    }
    case 4:
    {
        err = max31865readdataspi(MAX31865_CONFIG_REG, &confreg, 1);
        IF_ERR_YES_RETURN_ERR
        confreg = (confreg | MAX31865_START_CONVERSION_NONSTOP);
        err = max31865senddataspi(MAX31865_CONFIG_REG, &confreg, 1);
        IF_ERR_YES_RETURN_ERR
        step_getstatus = 5;
        break;
    }
    case 5:
    {

        xSemaphoreTake(drdySemaphore, portMAX_DELAY);
        err = max31865readdataspi(MAX31865_RTD_REG, rtdBytes, 2);
        IF_ERR_YES_RETURN_ERR
        step_getstatus = 6;

        break;
    }
    case 6:
    {

        err = max31865readdataspi(MAX31865_FAULT_STATUS_REG, &faultByte, 1);
        IF_ERR_YES_RETURN_ERR
        statusmax31865 = faultByte;
        if (faultByte == 0)
        {
            rtd = rtdBytes[0] << CHAR_BIT;
            rtd |= rtdBytes[1];
            rtd >>= 1U;

            if (countaverage < AVERAGETEM)
            {
                countaverage++;
                rtd_temp = rtd_temp + rtd;
            }
            else
            {
                rtd_average = ((rtd_temp / countaverage) * R_REF) / pow(2.0,15);
                tempdouble = (-A_PT100 + pow((pow(A_PT100, 2.0) - 4.0 * B_PT100 * (1.0 - (double)(rtd_average / RTD_ZERO_C))),0.5)) / (2.0 * B_PT100);
                temperature =  round(tempdouble*10)/10;
                countaverage = 0;
                rtd_temp = 0;
            }
        }
        step_getstatus = 0;
        break;
    }

    default:
    {
        step_getstatus = 0;
        break;
    }
    }

    return err;
}
//-----------------------------------------------------------------------------//
void IRAM_ATTR drdyInterruptHandler(void *arg)
{
    static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR((SemaphoreHandle_t)arg, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken)
    {
        portYIELD_FROM_ISR();
    }
}
//-----------------------------------------------------------------------------//
void run_task_getstatus_pt100()
{
    xTaskCreate(task_getstatus_pt100, "task_getstatus_pt100", STACK_SIZE_PT100_TASK, NULL, (configMAX_PRIORITIES - 4), NULL);
}
//-----------------------------------------------------------------------------//
void task_getstatus_pt100()
{
    while (1)
    {
        msx31865getstatus();
        if (statusmax31865 != 0)
        {
            if (trigerrone == 0)
            {
                trigerrone = 1;
                temperature = 0;
                selectplayaudio(ALARMSENSOR_AUDIO);
            }
        }
        else
        {
            trigerrone = 0;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
//-----------------------------------------------------------------------------//