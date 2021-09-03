
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

const wchar_t messarr[6][21] = {
    L"                    \0",
    L"     Пивоварня      \0",
    L"    Asv-Lab-Beer    \0",
    L"   www.asv-lab.ru   \0",
    L" Версия (1.1)  WiFi \0",
    L"                    \0"}; //end arr mess
                              //-----------------------------------------------------------------------------//
void lcdinit4bit(void)
{
    delayMicroseconds(40000);
    gpio_set_level(LCD_E, LOW);
    gpio_set_level(LCD_A0, LOW);
    gpio_set_level(LCD_B7, LOW);
    gpio_set_level(LCD_B6, LOW);
    gpio_set_level(LCD_B5, HIGHT);
    gpio_set_level(LCD_B4, HIGHT);

    gpio_set_level(LCD_E, HIGHT);
    delayMicroseconds(20);
    gpio_set_level(LCD_E, LOW);
    delayMicroseconds(100);

    gpio_set_level(LCD_E, HIGHT);
    delayMicroseconds(20);
    gpio_set_level(LCD_E, LOW);
    delayMicroseconds(100);

    gpio_set_level(LCD_E, HIGHT);
    delayMicroseconds(20);
    gpio_set_level(LCD_E, LOW);
    delayMicroseconds(100);

    gpio_set_level(LCD_B4, LOW);

    gpio_set_level(LCD_E, HIGHT);
    delayMicroseconds(20);
    gpio_set_level(LCD_E, LOW);
    delayMicroseconds(100);

} //end lcdinit
//-----------------------------------------------------------------------------//
void lcdsendcmd(uint8_t cmd)
{

    gpio_set_level(LCD_A0, LOW);
    delayMicroseconds(1);
    (cmd & 0x80) ? gpio_set_level(LCD_B7, HIGHT) : gpio_set_level(LCD_B7, LOW);
    (cmd & 0x40) ? gpio_set_level(LCD_B6, HIGHT) : gpio_set_level(LCD_B6, LOW);
    (cmd & 0x20) ? gpio_set_level(LCD_B5, HIGHT) : gpio_set_level(LCD_B5, LOW);
    (cmd & 0x10) ? gpio_set_level(LCD_B4, HIGHT) : gpio_set_level(LCD_B4, LOW);
    delayMicroseconds(1);
    gpio_set_level(LCD_E, HIGHT);
    delayMicroseconds(20);
    gpio_set_level(LCD_E, LOW);
    delayMicroseconds(100);
    delayMicroseconds(1);
    (cmd & 0x08) ? gpio_set_level(LCD_B7, HIGHT) : gpio_set_level(LCD_B7, LOW);
    (cmd & 0x04) ? gpio_set_level(LCD_B6, HIGHT) : gpio_set_level(LCD_B6, LOW);
    (cmd & 0x02) ? gpio_set_level(LCD_B5, HIGHT) : gpio_set_level(LCD_B5, LOW);
    (cmd & 0x01) ? gpio_set_level(LCD_B4, HIGHT) : gpio_set_level(LCD_B4, LOW);
    delayMicroseconds(1);
    gpio_set_level(LCD_E, HIGHT);
    delayMicroseconds(20);
    gpio_set_level(LCD_E, LOW);
    delayMicroseconds(100);

    if (LCD_CLEAR_COM == cmd)
        delayMicroseconds(2000);

} //end lcdsendcmd
//-----------------------------------------------------------------------------//
void lcdsenddata(wchar_t senddata)
{
    char ch = 0;
    if (senddata > 0x80)
    {
        ch = senddata + 0xb0;
    }
    else
    {

        ch = senddata;
    }

    gpio_set_level(LCD_A0, HIGHT);
    delayMicroseconds(1);
    (ch & 0x80) ? gpio_set_level(LCD_B7, HIGHT) : gpio_set_level(LCD_B7, LOW);
    (ch & 0x40) ? gpio_set_level(LCD_B6, HIGHT) : gpio_set_level(LCD_B6, LOW);
    (ch & 0x20) ? gpio_set_level(LCD_B5, HIGHT) : gpio_set_level(LCD_B5, LOW);
    (ch & 0x10) ? gpio_set_level(LCD_B4, HIGHT) : gpio_set_level(LCD_B4, LOW);
    delayMicroseconds(1);
    gpio_set_level(LCD_E, HIGHT);
    delayMicroseconds(20);
    gpio_set_level(LCD_E, LOW);
    delayMicroseconds(100);

    (ch & 0x08) ? gpio_set_level(LCD_B7, HIGHT) : gpio_set_level(LCD_B7, LOW);
    (ch & 0x04) ? gpio_set_level(LCD_B6, HIGHT) : gpio_set_level(LCD_B6, LOW);
    (ch & 0x02) ? gpio_set_level(LCD_B5, HIGHT) : gpio_set_level(LCD_B5, LOW);
    (ch & 0x01) ? gpio_set_level(LCD_B4, HIGHT) : gpio_set_level(LCD_B4, LOW);
    delayMicroseconds(1);
    gpio_set_level(LCD_E, HIGHT);
    delayMicroseconds(20);
    gpio_set_level(LCD_E, LOW);
    delayMicroseconds(100);

} //end lcdsenddata
  //-----------------------------------------------------------------------------//
void lcdmessage(uint8_t row, uint8_t countsym, uint8_t countmess, uint8_t len)
{
    uint8_t i = 0;

    switch (row)
    {
    case 1:
    {
        lcdsendcmd(LCD_SET_DDRAM_ADR_COM | countsym);
        break;
    } //end case
    case 2:
    {
        lcdsendcmd(LCD_SET_DDRAM_ADR_COM | countsym | 0x40);
        break;
    } //end case
    case 3:
    {
        lcdsendcmd(LCD_SET_DDRAM_ADR_COM | countsym | 0x14);
        break;
    } //end case
    case 4:
    {
        lcdsendcmd(LCD_SET_DDRAM_ADR_COM | countsym | 0x54);
        break;
    } //end case
    default:
        break;
    } //end switch

    while (len--)
    {
        lcdsenddata(messarr[countmess][i]);
        i++;
    } //end while

} //end  lcdmessage
  //-----------------------------------------------------------------------------//
void lcdprint(uint8_t row, uint8_t countsym, wchar_t *mess, uint8_t len)
{

    switch (row)
    {
    case 1:
    {
        lcdsendcmd(LCD_SET_DDRAM_ADR_COM | countsym);
        break;
    } //end case
    case 2:
    {
        lcdsendcmd(LCD_SET_DDRAM_ADR_COM | countsym | 0x40);
        break;
    } //end case
    case 3:
    {
        lcdsendcmd(LCD_SET_DDRAM_ADR_COM | countsym | 0x14);
        break;
    } //end case
    case 4:
    {
        lcdsendcmd(LCD_SET_DDRAM_ADR_COM | countsym | 0x54);
        break;
    } //end case
    default:
        break;
    } //end switch

    while (len--)
    {
        lcdsenddata(*mess);
        mess++;
    } //end while

} //end	lcdprint
  //-----------------------------------------------------------------------------//

unsigned long IRAM_ATTR micros()
{
    return (unsigned long)(esp_timer_get_time());
}
void IRAM_ATTR delayMicroseconds(uint32_t us)
{
    uint32_t m = micros();
    if (us)
    {
        uint32_t e = (m + us);
        if (m > e)
        { //overflow
            while (micros() > e)
            {
                NOP();
            }
        }
        while (micros() < e)
        {
            NOP();
        }
    }
}

//-----------------------------------------------------------------------------//
void lcdbufclean(void)
{

    memset(buflcdrow1, 0x00, 24);
    memset(buflcdrow2, 0x00, 24);
    memset(buflcdrow3, 0x00, 24);
    memset(buflcdrow4, 0x00, 24);
}
//-----------------------------------------------------------------------------//
