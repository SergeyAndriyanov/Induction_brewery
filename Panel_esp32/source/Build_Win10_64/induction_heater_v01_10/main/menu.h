//---------------------------------------------------------------------------------------------------
#ifndef MENU_H_
#define MENU_H_
//-----------------------------------------------------------------------------//
#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "wchar.h"
//-----------------------------------------------------------------------------//
#define STACK_SIZE_MENU_TASK   25000
#define SIZESTRBUF  sizeof(wchar_t) *30

//-----------------------------------------------------------------------------//
typedef struct{
uint8_t menunumber;
uint8_t submenumesh0;
uint8_t submenuhops0;
uint8_t menusett;
uint8_t selectmenusett;
uint8_t selectpump;
uint8_t meshselectmenu;
uint8_t meshelectparam;
uint8_t meshpausenum;
uint8_t enterparampress;
float   tempfloat;
uint8_t tempu8t;
uint32_t tempu32t;
wchar_t *row;
uint8_t hopsselectmenu;
uint8_t hopselectparam;
uint8_t hopspausenum;
uint8_t submenupid;

uint8_t maxpwmreqp1;
uint8_t maxpwmreqp2;


uint8_t temp_enterwifi;
}Tparametrmenu;



//-----------------------------------------------------------------------------//
extern Tparametrmenu parametrmenu;
extern wchar_t messageprocess[21];
extern uint8_t counterblinr;
extern uint8_t show_sim;
extern const wchar_t simv_blink[2][2];

extern uint32_t donetimemash;
extern uint32_t donetimehops;
extern uint32_t actualmashpause;
extern uint32_t actualhopspause;

//-----------------------------------------------------------------------------//
void runtaskmenu();
void task_menu(void *arg);
uint8_t animatind();
//-----------------------------------------------------------------------------//




//---------------------------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------------------------------
