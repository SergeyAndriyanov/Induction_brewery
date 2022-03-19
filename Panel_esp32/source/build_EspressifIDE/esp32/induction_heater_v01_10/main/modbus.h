//---------------------------------------------------------------------------------------------------
#ifndef MODBUS_H_
#define MODBUS_H_
//-----------------------------------------------------------------------------//
#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "datablock.h"
//-----------------------------------------------------------------------------//

//-----------------------------------------------------------------------------//
#define MB_TCP_PORT_NUMBER (CONFIG_FMB_TCP_PORT_DEFAULT)
#define MB_MDNS_PORT (502)
#define SLAVE_TAG "modbus"
#define STACK_SIZE_MODBUS_TASK 20000
#define STACK_SIZE_MODBUS_UPDATEREGTASK 10000
#define MB_PAR_INFO_GET_TOUT (10) // Timeout for get parameter info
#define MB_SLAVE_ADDR (CONFIG_MB_SLAVE_ADDR)

#if CONFIG_FMB_CONTROLLER_SLAVE_ID_SUPPORT
#define MB_DEVICE_ID (uint32_t) CONFIG_FMB_CONTROLLER_SLAVE_ID
#endif

#define MB_ID_BYTE0(id) ((uint8_t)(id))
#define MB_ID_BYTE1(id) ((uint8_t)(((uint16_t)(id) >> 8) & 0xFF))
#define MB_ID_BYTE2(id) ((uint8_t)(((uint32_t)(id) >> 16) & 0xFF))
#define MB_ID_BYTE3(id) ((uint8_t)(((uint32_t)(id) >> 24) & 0xFF))
#define MB_ID2STR(id) MB_ID_BYTE0(id), MB_ID_BYTE1(id), MB_ID_BYTE2(id), MB_ID_BYTE3(id)

#define MB_READ_MASK (MB_EVENT_INPUT_REG_RD | MB_EVENT_HOLDING_REG_RD | MB_EVENT_DISCRETE_RD | MB_EVENT_COILS_RD)
#define MB_WRITE_MASK (MB_EVENT_HOLDING_REG_WR | MB_EVENT_COILS_WR)
#define MB_READ_WRITE_MASK (MB_READ_MASK | MB_WRITE_MASK)

#define MB_EVENT_READ_WRITE_ESP_LOGI_ON 0

//-----------------------------------------------------------------------------//
void start_mdns_service();
void modbus_init_sl(void *mbhandl);
void runtaskmodbus();
void task_modbus(void *arg);
void modbus_deinit_mb();
void filltablereg();
void task_updatereg();
//-----------------------------------------------------------------------------//
typedef struct
{
    uint8_t modbustaskout;
    uint8_t eventwrite;

} Tparametrmodbus;
extern Tparametrmodbus parametrmodbus;

typedef struct
{
    float *datafloat[71];
    uint32_t *datauint[89];

} Ttablepointdatareg;
extern Ttablepointdatareg tablereg;

//---------------------------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------------------------------
