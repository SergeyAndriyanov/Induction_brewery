//---------------------------------------------------------------------------------------------------
/*
*/
//---------------------------------------------------------------------------------------------------
#ifndef HW_INIT_H_
#define HW_INIT_H_
//-----------------------------------------------------------------------------//
#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
//-----------------------------------------------------------------------------//
void gpio_conf();
void init_spi();
void dac_init_aud();
void uart_2_induc_pcb_init();


//pin of lcd
#define LCD_A0 GPIO_NUM_12
#define LCD_E GPIO_NUM_14
#define LCD_B7 GPIO_NUM_32
#define LCD_B6 GPIO_NUM_33
#define LCD_B5 GPIO_NUM_25
#define LCD_B4 GPIO_NUM_27

//pin of spi ma31865
#define MISO GPIO_NUM_19
#define MOSI GPIO_NUM_23
#define SCLK GPIO_NUM_18
#define CS0 GPIO_NUM_5
#define DRDY GPIO_NUM_21

//descrete io
#define FAN GPIO_NUM_22
#define RTL GPIO_NUM_4

#define K1 GPIO_NUM_13
#define K2 GPIO_NUM_15
#define K3 GPIO_NUM_2

#define KEY1 GPIO_NUM_36
#define KEY2 GPIO_NUM_39
#define KEY3 GPIO_NUM_34
#define KEY4 GPIO_NUM_35
//***

//uart induction
#define INDUCTION_UART_NUM UART_NUM_2
#define TXD_PIN GPIO_NUM_17
#define RXD_PIN GPIO_NUM_16
#define BAUD_RATE 9600
#define BUF_SIZE_UART2 127
// Timeout threshold for UART = number of symbols (~10 tics) with unchanged state on receive pin
#define UART2_READ_TOUT 5 // 3.5T * 8 = 28 ticks, TOUT=3 -> ~24..33 ticks


//-----------------------------------------------------------------------------//




extern spi_device_handle_t spi_max31865_handle;
extern SemaphoreHandle_t drdySemaphore;
extern SemaphoreHandle_t Timer0Semaphore;



//-----------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------------------------------
