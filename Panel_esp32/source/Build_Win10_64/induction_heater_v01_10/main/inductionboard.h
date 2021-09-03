//---------------------------------------------------------------------------------------------------
/*
*/
//---------------------------------------------------------------------------------------------------
#ifndef INDUCTIONBOARD_H_
#define INDUCTIONBOARD_H_
//-----------------------------------------------------------------------------//

void taskuart2_read(void*arg);
void taskuart2_write();
void runtaskuart2();
uint16_t DataCrc(uint8_t nData[], uint16_t wLength);
double stainhart(uint8_t ValAdc, double A, double B, double C, double ResistorDiv, uint16_t adcmaxcod, double adcmincod, double adcrefV);
void ControlFanRtlTask_run();
void taskfanrtlcontrol(void*arg);
void rebbotpowerboard();
//-----------------------------------------------------------------------------//
#define UART2_TASK_STACK_SIZE 2000
#define PACKET_READ_UART2_TICS (300 / portTICK_RATE_MS)
#define INDUCTION_PCB_ADRESS 1
#define TIME_OF_FUN 120
#define TASK_STACKSIZE_FAN_RTL 2000
#define TEM_IND1 (float)stainhart(DataFromIndHeater.structurdata.temperature_inductor_1, 0.022485944, -0.012328307, 0.000566819, 3300, 255, 2, 4.982)
#define TEM_IND2 (float)stainhart(DataFromIndHeater.structurdata.temperature_inductor_2, 0.022485944, -0.012328307, 0.000566819, 3300, 255, 2, 4.982)
#define TEM_IGBT1 (float)stainhart(DataFromIndHeater.structurdata.temperature_igbt_1, -0.067764425, 0.016142443, 0.000163046, 5100, 255, 2, 4.982)
#define TEM_IGBT2 (float)stainhart(DataFromIndHeater.structurdata.temperature_igbt_2, -0.067764425, 0.016142443, 0.000163046, 5100, 255, 2, 4.982)
#define INDUC_ON 1
#define INDUC_OFF 0
#define UART_MAX_ERR  30
//-----------------------------------------------------------------------------//
#pragma pack(push, 1)
typedef struct
{
    uint8_t adress;
    uint16_t u_rectifler;
    uint16_t i_inductor_1;
    uint16_t i_inductor_2;
    uint8_t temperature_inductor_1;
    uint8_t temperature_inductor_2;
    uint8_t temperature_igbt_1;
    uint8_t temperature_igbt_2;
    uint8_t statusinductor;
    uint8_t inductor1pwm;
    uint8_t inductor2pwm;
    uint8_t factpwm1;
    uint8_t factpwm2;
    uint16_t stepst;
    uint16_t crc;
} TStructDataFromInd;
#pragma pack(push, 1)
typedef struct
{
    uint8_t adress;
    uint8_t OnOffHeater;
    uint8_t pwm_ind1;
    uint8_t pwm_ind2;
    uint16_t crc;

} DataToIndHeater;
#pragma pack(push, 1)
typedef union
{
    DataToIndHeater structurdata;
    uint8_t buf[sizeof(DataToIndHeater)];
} TUsenddev;
#pragma pack(push, 1)
typedef union
{
    TStructDataFromInd structurdata;
    uint8_t buf[sizeof(TStructDataFromInd)];
} TUFromind;
typedef struct
{
uint8_t stat;
uint16_t timeoff;
uint16_t uarterrnum;
}Tfanrtlparametr;


//-----------------------------------------------------------------------------//
extern TUFromind DataFromIndHeater;
extern uint8_t flagdatardy;
extern TUsenddev DataToInd;
extern Tfanrtlparametr parametrfasnrtl;

//-----------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------------------------------
