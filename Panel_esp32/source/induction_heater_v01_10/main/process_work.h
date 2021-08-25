//---------------------------------------------------------------------------------------------------
#ifndef PROCESS_WORK_H_
#define PROCESS_WORK_H_
//-----------------------------------------------------------------------------//
#define STACK_SIZE_PROCESS_WRK_TASK 15000
#define INDUCTOR1_ON 0x01
#define INDUCTOR1_OFF 0xFE
#define INDUCTOR2_ON 0x02
#define INDUCTOR2_OFF 0xFD
#define WAITHEATEROFF 20
#define WAITPREP 30
#define PID_OUTMIN 25.0

//-----------------------------------------------------------------------------//
typedef struct
{

    uint8_t actualstepproc_mesh;
    uint8_t playonetrig_mesh;
    uint8_t oneevent_mesh;
    uint8_t waitprep_mesh;
    uint8_t actualpause_mesh;

    uint8_t actualstepproc_hops;
    uint8_t playonetrig_hops;
    uint8_t oneevent_hops;
    uint8_t waitprep_hops;
    uint8_t actualpause_hops;
    uint8_t shownumberstr;
    uint8_t counttimesowstr;

} TprocData;

typedef struct
{
    float lastProcessValue;
    float sumError;
    float P_Factor;
    float I_Factor;
    float D_Factor;
    float maxError_up;
    float maxSumError_up;
    float maxError_down;
    float maxSumError_down;
    float scalfac;
    uint8_t oldnumproc;
} TStructPid;
//-----------------------------------------------------------------------------//

extern TprocData processworkdata;
extern TStructPid StructPid;
extern float tempreqmesh_hops;

//-----------------------------------------------------------------------------//
void run_task_process_work();
void proc_mesh_boil(void *arg);
void heater_process_mesh(DataToIndHeater *induc_board, Tdatapause_mashing *meshdata, Tpidstr *piddata, float temper, uint8_t nuberproc);
void pumponoff(TPump *pPom);
void heater_process_hops(DataToIndHeater *induc_board, Tdatahops *hops, Tpidstr *piddata, float temper, uint8_t nuberproc);
void pid_reg_init(float p_factor, float i_factor, float d_factor, float scal_fac, float max, float maxterm, float min);
float pid_reg(float setPoint, float processValue, float max, float min);

//-----------------------------------------------------------------------------//

#endif
//---------------------------------------------------------------------------------------------------
