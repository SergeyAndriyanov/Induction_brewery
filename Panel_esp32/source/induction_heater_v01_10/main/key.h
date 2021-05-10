//---------------------------------------------------------------------------------------------------
/*
*/
//---------------------------------------------------------------------------------------------------
#ifndef KEY_H_
#define KEY_H_
//-----------------------------------------------------------------------------//
#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <string.h>
//-----------------------------------------------------------------------------//
#define STACK_SIZE_KEY_TASK  2000
#define KEY_FILTER 5
//-----------------------------------------------------------------------------//

bool getkeymenu();
bool getkeyenter();
bool getkeyup();
bool getkeydown();


void runtaskkey();
void task_key(void*arg);


typedef struct
{
uint8_t key1;
uint8_t key2;
uint8_t key3;
uint8_t key4;
int16_t countfilterkey1;
int16_t countfilterkey2;
int16_t countfilterkey3;
int16_t countfilterkey4;

}Tparametrkey;

//-----------------------------------------------------------------------------//
extern Tparametrkey  parametrkey;
extern uint8_t waiter_speedup;
extern uint8_t waiter_speeddown;
//---------------------------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------------------------------
