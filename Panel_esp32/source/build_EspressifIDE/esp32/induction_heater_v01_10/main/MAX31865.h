//---------------------------------------------------------------------------------------------------
/*
*/
//---------------------------------------------------------------------------------------------------
#ifndef MAX31865_H_
#define MAX31865_H_
//-----------------------------------------------------------------------------//
#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#define MAX31865_CONFIG_REG 0x00
#define MAX31865_RTD_REG 0x01
#define MAX31865_HIGH_FAULT_REG 0x03
#define MAX31865_LOW_FAULT_REG 0x05
#define MAX31865_FAULT_STATUS_REG 0x07

#define MAX31865_REG_WRITE_OFFSET 0x80

#define MAX31865_CONFIG_VBIAS_BIT 7
#define MAX31865_CONFIG_CONVERSIONMODE_BIT 6
#define MAX31865_CONFIG_1SHOT_BIT 5
#define MAX31865_CONFIG_NWIRES_BIT 4
#define MAX31865_CONFIG_FAULTDETECTION_BIT 2
#define MAX31865_CONFIG_FAULTSTATUS_BIT 1
#define MAX31865_CONFIG_MAINSFILTER_BIT 0

#define MAX31865_FAULT_DETECT_AUTOMAT 0x84
#define MAX31865_START_CONVERSION_NONSTOP 0x40

#define MAX31865_MIN_PT100 0x19b3 //-50C
#define MAX31865_MAX_PT100 0x3e1d //250C
#define STACK_SIZE_PT100_TASK 10000

#define IF_ERR_YES_RETURN_ERR \
    if (err != ESP_OK)        \
    {                         \
        return err;           \
    }
    #define R_REF  402.0
    #define A_PT100 0.00390830
    #define B_PT100 0.0000000577500
    #define RTD_ZERO_C 100.0
//-----------------------------------------------------------------------------//
esp_err_t max31865senddataspi(uint8_t addr, uint8_t *data, size_t size);
esp_err_t max31865readdataspi(uint8_t addr, uint8_t *result, size_t size);
esp_err_t max31865setrtdthresholds(uint16_t min, uint16_t max);
esp_err_t max31865resetfault();
esp_err_t msx31865getstatus();
void IRAM_ATTR drdyInterruptHandler(void *arg);
void run_task_getstatus_pt100();
void task_getstatus_pt100();
//-----------------------------------------------------------------------------//
extern uint8_t statusmax31865;
extern uint16_t rtd;
extern float temperature;
extern double rtd_average;
extern uint32_t errormesage;

//-----------------------------------------------------------------------------//
#define AVERAGETEM 5
//---------------------------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------------------------------
