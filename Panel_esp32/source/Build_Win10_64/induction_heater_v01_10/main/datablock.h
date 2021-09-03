//---------------------------------------------------------------------------------------------------
#ifndef DATABLOCK_H_
#define DATABLOCK_H_
//-----------------------------------------------------------------------------//
#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
//-----------------------------------------------------------------------------//
#define MAXPAUSMASHING 6
#define MAXXOPS 6
//#define STARTBOILINGTEMP 99
#define MODE_OFF  0
#define MODE_MESH 1
#define MODE_HOPS 2

#define MAXTEMPMESH 120.0
#define MAXTEMPHOPS 120.0
#define MAXTEMPPAN1 220
#define MAXTEMPPAN2 220
#define GUSTER_MES_MAX 40
#define GUSTER_PAN_MAX 40
#define MAXPOWEROUT 100
#define MAXTIMEMESH 4010800000
#define MAXTIMEHOPS  4010800000

//-----------------------------------------------------------------------------//
#define TEMPMESHDEF 62
#define MESHTEMPPAN1 180
#define MESHTEMPPAN2 180
#define TEMPMESHDELTA 2
#define TEMPPANMESHDELTA 30
#define MESHINDUCTOR1_USE 1
#define MESHINDUCTOR2_USE 1
#define MESHMAXPWM 60
#define TIME_MASH 1800000
//-----------------------------------------------------------------------------//
#define TEMPHOPSDEF 101
#define HOPSTEMPPAN1 190
#define HOPSTEMPPAN2 190
#define TEMPHOPSDELTA 2
#define TEMPPANHOPSDELTA 30
#define HOPSINDUCTOR1_USE 1
#define HOPSINDUCTOR2_USE 1
#define HOPSMAXPWM 65
#define TIME_HOPS 1800000

#define MAXPID_P 100
#define MAXPID_D 100
#define MAXPID_I 100

#define KEY_DATAPROCESS (const char*)"dataprocess"
#define STORAGE_NAMESPACE_P (const char*)"storparamberr"

//-----------------------------------------------------------------------------//
#pragma pack(push, 1)
typedef struct
{
    float temperature_mesh;
    float temperature_pan1;
    float temperature_pan2;
    float temperature_mesh_delta;
    float temperature_pan_delta;
    uint32_t induct1_use;
    uint32_t induct2_use;
    uint32_t maxpwm;
    uint32_t time_ms;
    uint32_t numbermesh;

} Tdatapause_mashing;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
    float temperature_hops;
    float temperature_pan1;
    float temperature_pan2;
    float temperature_hops_delta;
    float temperature_pan_delta;
    uint32_t induct1_use;
    uint32_t induct2_use;
    uint32_t maxpwm;
    uint32_t time_ms;
    uint32_t numberhops;
} Tdatahops;
#pragma pack(pop)


#pragma pack(push, 1)
typedef struct
{
    Tdatapause_mashing mash[(MAXPAUSMASHING+3)];
    Tdatahops hops[(MAXXOPS+3)];
    uint32_t mode_work;
    uint16_t crc;
} TdataProcess;
#pragma pack(pop)

#pragma pack(push, 1)
typedef union
{
    TdataProcess dataproc;
    uint8_t buf[sizeof(TdataProcess)];
} TUDataProc;
#pragma pack(pop)


#pragma pack(push, 1)
typedef struct{
float pid_p;
float pid_i;
float pid_d;
float pid_dt;
float pid_max;
float pid_min;
float pid_out;

}Tpidstr;
#pragma pack(pop)


#pragma pack(push, 1)
typedef struct
{
uint8_t pump1;
uint8_t pump2;
uint8_t pump3;
}TPump;
#pragma pack(pop)

//-----------------------------------------------------------------------------//
extern  TUDataProc dataprocess;
extern uint8_t mesh_num;
extern uint8_t hops_num;
extern uint32_t mesh_act_time;
extern uint32_t hops_act_time;
extern Tpidstr pidparametr;
extern TPump pumponof;
 

//-----------------------------------------------------------------------------//
void load_dataproc();
void loaddefault();
esp_err_t save_data_process();
uint8_t read_data_process();
//---------------------------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------------------------------
 