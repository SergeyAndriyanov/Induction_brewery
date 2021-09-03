
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
#include "esp_task_wdt.h"
#include "driver/i2s.h"

//-----------------------------------------------------------------------------//
spi_device_handle_t spi_max31865_handle;
SemaphoreHandle_t drdySemaphore;
SemaphoreHandle_t Timer0Semaphore;
TaskHandle_t task_handles[portNUM_PROCESSORS];
//-----------------------------------------------------------------------------//

//-----------------------------------------------------------------------------//
void gpio_conf()
{

	gpio_pad_select_gpio(LCD_A0);
	gpio_set_direction(LCD_A0, GPIO_MODE_DEF_OUTPUT);
	gpio_set_pull_mode(LCD_A0, GPIO_PULLUP_ONLY);
	gpio_pad_select_gpio(LCD_E);
	gpio_set_direction(LCD_E, GPIO_MODE_DEF_OUTPUT);
	gpio_set_pull_mode(LCD_E, GPIO_PULLUP_ONLY);
	gpio_pad_select_gpio(LCD_B7);
	gpio_set_direction(LCD_B7, GPIO_MODE_DEF_OUTPUT);
	gpio_set_pull_mode(LCD_B7, GPIO_PULLUP_ONLY);
	gpio_pad_select_gpio(LCD_B6);
	gpio_set_direction(LCD_B6, GPIO_MODE_DEF_OUTPUT);
	gpio_set_pull_mode(LCD_B6, GPIO_PULLUP_ONLY);
	gpio_pad_select_gpio(LCD_B5);
	gpio_set_direction(LCD_B5, GPIO_MODE_DEF_OUTPUT);
	gpio_set_pull_mode(LCD_B5, GPIO_PULLUP_ONLY);
	gpio_pad_select_gpio(LCD_B4);
	gpio_set_direction(LCD_B4, GPIO_MODE_DEF_OUTPUT);
	gpio_set_pull_mode(LCD_B4, GPIO_PULLUP_ONLY);

	gpio_pad_select_gpio(FAN);
	gpio_set_direction(FAN, GPIO_MODE_DEF_OUTPUT);
	gpio_set_pull_mode(FAN, GPIO_PULLDOWN_ONLY);

	gpio_pad_select_gpio(RTL);
	gpio_set_direction(RTL, GPIO_MODE_DEF_OUTPUT);
	gpio_set_pull_mode(RTL, GPIO_PULLDOWN_ONLY);

	gpio_pad_select_gpio(K1);
	gpio_set_direction(K1, GPIO_MODE_DEF_OUTPUT);
	gpio_set_pull_mode(K1, GPIO_PULLDOWN_ONLY);

	gpio_pad_select_gpio(K2);
	gpio_set_direction(K2, GPIO_MODE_DEF_OUTPUT);
	gpio_set_pull_mode(K2, GPIO_PULLDOWN_ONLY);

	gpio_pad_select_gpio(K3);
	gpio_set_direction(K3, GPIO_MODE_DEF_OUTPUT);
	gpio_set_pull_mode(K3, GPIO_PULLDOWN_ONLY);

	gpio_pad_select_gpio(KEY1);
	gpio_set_direction(KEY1, GPIO_MODE_INPUT);
	gpio_set_pull_mode(KEY1, GPIO_PULLUP_ONLY);

	gpio_pad_select_gpio(KEY2);
	gpio_set_direction(KEY2, GPIO_MODE_INPUT);
	gpio_set_pull_mode(KEY2, GPIO_PULLUP_ONLY);

	gpio_pad_select_gpio(KEY3);
	gpio_set_direction(KEY3, GPIO_MODE_INPUT);
	gpio_set_pull_mode(KEY3, GPIO_PULLUP_ONLY);

	gpio_pad_select_gpio(KEY4);
	gpio_set_direction(KEY4, GPIO_MODE_INPUT);
	gpio_set_pull_mode(KEY4, GPIO_PULLUP_ONLY);

	gpio_set_level(LCD_E, LOW);
	gpio_set_level(LCD_B7, LOW);
	gpio_set_level(LCD_B6, LOW);
	gpio_set_level(LCD_B5, LOW);
	gpio_set_level(LCD_B4, LOW);
	gpio_set_level(LCD_A0, LOW);

	gpio_set_level(FAN, LOW);
	gpio_set_level(RTL, LOW);

	gpio_set_level(K1, LOW);
	gpio_set_level(K2, LOW);
	gpio_set_level(K3, LOW);

	gpio_pad_select_gpio(DRDY);
	gpio_set_direction(DRDY, GPIO_MODE_INPUT);
	gpio_set_pull_mode(DRDY, GPIO_PULLUP_ONLY);
	gpio_set_intr_type(DRDY, GPIO_INTR_NEGEDGE);

	gpio_pad_select_gpio(CS0);
	gpio_set_direction(CS0, GPIO_MODE_DEF_OUTPUT);
	gpio_set_pull_mode(CS0, GPIO_PULLUP_ONLY);
	gpio_set_level(CS0, HIGHT);

	drdySemaphore = xSemaphoreCreateBinary();
	if (gpio_get_level(DRDY) == 0)
	{
		xSemaphoreGive(drdySemaphore);
	}
	gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
	gpio_isr_handler_add(DRDY, &drdyInterruptHandler, drdySemaphore);
}
//-----------------------------------------------------------------------------//
void init_spi()
{

	spi_bus_config_t spibusconf = {};
	spibusconf.miso_io_num = MISO;
	spibusconf.mosi_io_num = MOSI;
	spibusconf.sclk_io_num = SCLK;
	spibusconf.quadhd_io_num = -1;
	spibusconf.quadwp_io_num = -1;
	spi_bus_initialize(SPI3_HOST, &spibusconf, 0);

	spi_device_interface_config_t spi_max31865_conf = {};
	spi_max31865_conf.spics_io_num = -1;
	spi_max31865_conf.clock_speed_hz = 3000000;
	spi_max31865_conf.mode = 1;
	spi_max31865_conf.address_bits = CHAR_BIT;
	spi_max31865_conf.command_bits = 0;
	spi_max31865_conf.flags = SPI_DEVICE_HALFDUPLEX;
	spi_max31865_conf.queue_size = 1;
	spi_bus_add_device(SPI3_HOST, &spi_max31865_conf, &spi_max31865_handle);
}

//-----------------------------------------------------------------------------//
void dac_init_aud()
{
	/*
	timer_config_t config = {
		.divider = 8,
		.counter_dir = TIMER_COUNT_UP,
		.counter_en = TIMER_PAUSE,
		.alarm_en = TIMER_ALARM_EN,
		.intr_type = TIMER_INTR_LEVEL,
		.auto_reload = 1,
	};
	timer_init(TIMER_GROUP_0, TIMER_0, &config);
	timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL);
	timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, TIMER_BASE_CLK / config.divider / sound_wav_info.sampleRate);
	timer_enable_intr(TIMER_GROUP_0, TIMER_0);
	Timer0Semaphore = xSemaphoreCreateBinary();
	xSemaphoreGive(Timer0Semaphore);
	timer_isr_register(TIMER_GROUP_0, TIMER_0, timer0_ISR, Timer0Semaphore, ESP_INTR_FLAG_IRAM, NULL);
	timer_start(TIMER_GROUP_0, TIMER_0);

	dac_output_enable(DAC_CHANNEL_2);
	dac_output_voltage(DAC_CHANNEL_2, 0);
	*/

	i2s_config_t i2s_config = {};
	i2s_config.mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN |I2S_CHANNEL_MONO;
	i2s_config.sample_rate = DACSAMPLERAIT;
	i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
	i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
	i2s_config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
	i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
	i2s_config.dma_buf_count = 8;
	i2s_config.dma_buf_len = 64;
	i2s_config.tx_desc_auto_clear=true;
	i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);		
	i2s_set_dac_mode(I2S_DAC_CHANNEL_LEFT_EN);
	//i2s_set_sample_rates(I2S_NUM_0, sound_wav_info.sampleRate); //set sample rates
	//i2s_driver_uninstall(I2S_NUM_0);		  //stop & destroy i2s driver
	createsemaphre();
}
//-----------------------------------------------------------------------------//
void uart_2_induc_pcb_init()
{

	uart_config_t uart2_config = {
		.baud_rate = BAUD_RATE,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.rx_flow_ctrl_thresh = 122,
		.source_clk = UART_SCLK_APB,
	};
	uart_driver_install(INDUCTION_UART_NUM, BUF_SIZE_UART2 * 2, 0, 0, NULL, 0);
	uart_param_config(INDUCTION_UART_NUM, &uart2_config);
	uart_set_pin(INDUCTION_UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
	uart_set_mode(INDUCTION_UART_NUM, UART_MODE_UART);
	uart_set_rx_timeout(INDUCTION_UART_NUM, UART2_READ_TOUT);
}
//-----------------------------------------------------------------------------//
