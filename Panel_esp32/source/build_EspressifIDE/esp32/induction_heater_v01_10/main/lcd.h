//---------------------------------------------------------------------------------------------------
/*
*/
//---------------------------------------------------------------------------------------------------
#ifndef LCD_H_
#define LCD_H_
//-----------------------------------------------------------------------------//
#define LCD_CLEAR_COM 0x01
#define LCD_RETURN_HOME_COM 0x02
#define LCD_ENTRY_MODE_SET_COM 0x04
#define LCD_DYSPLAY_ONOFF_COM 0x08
#define LCD_DYSPLAY_OR_CRS_SHIFT_COM 0x10
#define LCD_FUNCTION_SET_COM 0x28
#define LCD_SET_SGRAM_ADR_COM 0x40
#define LCD_SET_DDRAM_ADR_COM 0x80
/*    */
#define LCD_ID_FLAF 0x02
#define LCD_SH_FLAF 0x01
#define LCD_D_FLAF 0x04
#define LCD_CRS_NO_BLINK_NO_FLAF 0x00
#define LCD_CRS_NO_BLINK_YES_FLAF 0x01
#define LCD_CRS_YES_BLINK_NO_FLAF 0x02
#define LCD_CRS_YES_BLINK_YES_FLAF 0x03
#define LCD_SC_FLAF 0x08
#define LCD_RL_FLAF 0x04
#define LCD_DL_FLAF 0x10
#define LCD_P_FLAF 0x02
#define LCDLEDTIMEOUT 1000
//-----------------------------------------------------------------------------//

//-----------------------------------------------------------------------------//
#define LOW 0
#define HIGHT 1
#define NOP() asm volatile("nop")

void lcdinit4bit(void);
void lcdsendcmd(uint8_t cmd);
void lcdsenddata(wchar_t senddata);
void lcdmessage(uint8_t row, uint8_t countsym, uint8_t countmess, uint8_t len);
void lcdprint(uint8_t row, uint8_t countsym, wchar_t *mess, uint8_t len);
void lcdbufclean(void);
void IRAM_ATTR delayMicroseconds(uint32_t us);
unsigned long IRAM_ATTR micros();
//-----------------------------------------------------------------------------//

uint8_t buflcdrow1[24];
uint8_t buflcdrow2[24];
uint8_t buflcdrow3[24];
uint8_t buflcdrow4[24];

//---------------------------------------------------------------------------------------------------
#endif /* LCD_H_ */
//---------------------------------------------------------------------------------------------------
