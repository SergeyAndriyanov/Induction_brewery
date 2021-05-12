
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
#include "datablock.h"
#include "inductionboard.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "soc/rtc_wdt.h"
#include "process_work.h"
#include "menu.h"
#include "wchar.h"

//-----------------------------------------------------------------------------//
TprocData processworkdata;
uint8_t onofowertempan1 = 0, onofowertempan2 = 0, onofowertemmesh = 0, onofowertemhops = 0;
TStructPid StructPid;

//-----------------------------------------------------------------------------//
void run_task_process_work()
{
    xTaskCreate(proc_mesh_boil, "proc_mesh_boil", STACK_SIZE_PROCESS_WRK_TASK, (void *)&processworkdata, (configMAX_PRIORITIES - 4), NULL);
}
//-----------------------------------------------------------------------------//
void proc_mesh_boil(void *arg)
{
    TprocData *pProcDat = (TprocData *)arg;
    StructPid.oldnumproc = 99;

    while (1)
    {
        mesh_num = pProcDat->actualpause_mesh;
        hops_num = pProcDat->actualpause_hops;

        if (dataprocess.dataproc.mode_work == MODE_OFF)
        {
            pumponoff(&pumponof);
            pProcDat->actualstepproc_mesh = 0;
            pProcDat->playonetrig_mesh = 0;
            pProcDat->oneevent_mesh = 0;
            pProcDat->waitprep_mesh = 0;
            pProcDat->actualpause_mesh = 0;

            pProcDat->actualstepproc_hops = 0;
            pProcDat->playonetrig_hops = 0;
            pProcDat->oneevent_hops = 0;
            pProcDat->waitprep_hops = 0;
            pProcDat->actualpause_hops = 0;
            parametrfasnrtl.stat = INDUC_OFF;
            StructPid.oldnumproc = 99;
        }
        else if (dataprocess.dataproc.mode_work == MODE_MESH)
        {
            pumponoff(&pumponof);
            parametrfasnrtl.stat = INDUC_ON;
            switch (pProcDat->actualstepproc_mesh)
            {
            case 0:
            {
                if (pProcDat->oneevent_mesh == 0)
                {

                    pProcDat->oneevent_mesh = 1;
                }
                memset(messageprocess, 0, sizeof(messageprocess));
                swprintf(messageprocess, sizeof(messageprocess), L"Подготовка оборуд. %S ", &simv_blink[show_sim][0]);

                if (pProcDat->waitprep_mesh < WAITPREP)
                {
                    pProcDat->waitprep_mesh++;
                }
                else
                {
                    pProcDat->actualstepproc_mesh = 1;
                    pProcDat->playonetrig_mesh = 0;
                }

                break;
            }

            case 1:
            {
                memset(messageprocess, 0, sizeof(messageprocess));
                swprintf(messageprocess, sizeof(messageprocess), L"Зат.М1:%03i М2:%03i  %S ", DataFromIndHeater.structurdata.factpwm1, DataFromIndHeater.structurdata.factpwm2, &simv_blink[show_sim][0]);
                if (dataprocess.dataproc.mash[pProcDat->actualpause_mesh].numbermesh > pProcDat->actualpause_mesh)
                {
                    pProcDat->actualstepproc_mesh = 2;
                    pProcDat->playonetrig_mesh = 0;
                }
                else
                {
                    pProcDat->actualstepproc_mesh = 4;
                    pProcDat->playonetrig_mesh = 0;
                }

                break;
            }
            case 2:
            {
                if (pProcDat->playonetrig_mesh == 0)
                {
                    selectplayaudio(MESHRUN_AUDIO); //start process heater
                    pProcDat->playonetrig_mesh = 1;
                }

                memset(messageprocess, 0, sizeof(messageprocess));
                swprintf(messageprocess, sizeof(messageprocess), L"Зат.М1:%03i М2:%03i  %S ", DataFromIndHeater.structurdata.factpwm1, DataFromIndHeater.structurdata.factpwm2, &simv_blink[show_sim][0]);

                heater_process_mesh(&DataToInd.structurdata, &dataprocess.dataproc.mash[pProcDat->actualpause_mesh], &pidparametr, temperature, pProcDat->actualpause_mesh);
                if (temperature >= dataprocess.dataproc.mash[pProcDat->actualpause_mesh].temperature_mesh)
                {
                    pProcDat->playonetrig_mesh = 0;
                    pProcDat->actualstepproc_mesh = 3;
                    mesh_act_time = 0;
                }
                break;
            }

            case 3:
            {

                if (pProcDat->playonetrig_mesh == 0)
                {
                    selectplayaudio(TERMPPAUSENRUN_AUDIO); //start pause1
                    pProcDat->playonetrig_mesh = 1;
                }

                uint32_t min_ost = (dataprocess.dataproc.mash[pProcDat->actualpause_mesh].time_ms - mesh_act_time) / 1000 / 60;
                uint32_t sec_ost = (dataprocess.dataproc.mash[pProcDat->actualpause_mesh].time_ms - mesh_act_time) / 1000 % 60;

                memset(messageprocess, 0, sizeof(messageprocess));
                swprintf(messageprocess, sizeof(messageprocess), L"Пауза:%01i М:%04i:%02i  %S ", pProcDat->actualpause_mesh, min_ost, sec_ost, &simv_blink[show_sim][0]);

                heater_process_mesh(&DataToInd.structurdata, &dataprocess.dataproc.mash[pProcDat->actualpause_mesh], &pidparametr, temperature, pProcDat->actualpause_mesh);
                mesh_act_time += 100;
                if (mesh_act_time >= dataprocess.dataproc.mash[pProcDat->actualpause_mesh].time_ms)
                {
                    pProcDat->playonetrig_mesh = 0;
                    if (dataprocess.dataproc.mash[pProcDat->actualpause_mesh].numbermesh > pProcDat->actualpause_mesh)
                    {
                        pProcDat->actualstepproc_mesh = 2;
                        pProcDat->actualpause_mesh++;
                    }
                    else
                    {
                        pProcDat->actualstepproc_mesh = 4;
                    }
                }
                break;
            }
            case 4:
            {

                if (pProcDat->playonetrig_mesh == 0)
                {
                    selectplayaudio(MESHDONE_AUDIO); //process done
                    pProcDat->playonetrig_mesh = 1;
                }
                memset(messageprocess, 0, sizeof(messageprocess));
                swprintf(messageprocess, sizeof(messageprocess), L"Проц.Выпол.Пауз:%01i  %S ", pProcDat->actualpause_mesh, &simv_blink[show_sim][0]);

                DataFromIndHeater.structurdata.inductor1pwm = 0;
                parametrmenu.maxpwmreqp1 = 0;
                DataFromIndHeater.structurdata.inductor2pwm = 0;
                parametrmenu.maxpwmreqp2 = 0;
                parametrfasnrtl.stat = INDUC_OFF;
                break;
            }

            default:
            {
                pProcDat->actualstepproc_mesh = 0;
                break;
            }
            }
        }
        else if (dataprocess.dataproc.mode_work == MODE_HOPS)
        {
            pumponoff(&pumponof);
            parametrfasnrtl.stat = INDUC_ON;
            switch (pProcDat->actualstepproc_hops)
            {
            case 0:
            {

                if (pProcDat->oneevent_hops == 0)
                {
                    pProcDat->oneevent_hops = 1;
                }
                memset(messageprocess, 0, sizeof(messageprocess));
                swprintf(messageprocess, sizeof(messageprocess), L"Подготовка оборуд. %S ", &simv_blink[show_sim][0]);

                if (pProcDat->waitprep_hops < WAITPREP)
                {
                    pProcDat->waitprep_hops++;
                }
                else
                {
                    pProcDat->actualstepproc_hops = 1;
                    pProcDat->playonetrig_hops = 0;
                }

                break;
            }

            case 1:
            {
                memset(messageprocess, 0, sizeof(messageprocess));
                swprintf(messageprocess, sizeof(messageprocess), L"Кип.М1:%03i М2:%03i  %S ", DataFromIndHeater.structurdata.factpwm1, DataFromIndHeater.structurdata.factpwm2, &simv_blink[show_sim][0]);
                if (dataprocess.dataproc.hops[pProcDat->actualpause_hops].numberhops > pProcDat->actualpause_hops)
                {
                    pProcDat->actualstepproc_hops = 2;
                    pProcDat->playonetrig_hops = 0;
                    pProcDat->shownumberstr = 0;
                }
                else
                {
                    pProcDat->actualstepproc_hops = 5;
                    pProcDat->playonetrig_hops = 0;
                }

                break;
            }
            case 2:
            {
                if (pProcDat->playonetrig_hops == 0)
                {
                    selectplayaudio(BOILINRUN_AUDIO); //start process heater
                    pProcDat->playonetrig_hops = 1;
                }

                memset(messageprocess, 0, sizeof(messageprocess));
                if (pProcDat->shownumberstr == 0)
                {
                    swprintf(messageprocess, sizeof(messageprocess), L"Кип.М1:%03i М2:%03i  %S ", DataFromIndHeater.structurdata.factpwm1, DataFromIndHeater.structurdata.factpwm2, &simv_blink[show_sim][0]);
                }
                else if (pProcDat->shownumberstr == 1)
                {
                    swprintf(messageprocess, sizeof(messageprocess), L"Кип.Ожидание закип.%S ", &simv_blink[show_sim][0]);
                }
                else if (pProcDat->shownumberstr == 2)
                {
                    swprintf(messageprocess, sizeof(messageprocess), L"Кип.Посл.закипания.%S ", &simv_blink[show_sim][0]);
                }
                else if (pProcDat->shownumberstr == 3)
                {
                    swprintf(messageprocess, sizeof(messageprocess), L"Кип.Нажмите ввод.. %S ", &simv_blink[show_sim][0]);
                }
                heater_process_hops(&DataToInd.structurdata, &dataprocess.dataproc.hops[pProcDat->actualpause_hops], &pidparametr, temperature, pProcDat->actualpause_hops);

                // if (temperature >= dataprocess.dataproc.hops[pProcDat->actualpause_hops].temperature_hops)
                if ((getkeyenter() == 1) && (parametrmenu.submenuhops0 == 0) && (parametrmenu.menunumber == 0))
                {
                    pProcDat->playonetrig_hops = 0;
                    pProcDat->actualstepproc_hops = 3;
                    hops_act_time = 0;
                }
                break;
            }

            case 3:
            {

                if (pProcDat->playonetrig_hops == 0)
                {
                    selectplayaudio(TERMPPAUSENRUN_AUDIO); //start pause1
                    pProcDat->playonetrig_hops = 1;
                }

                uint32_t min_ost = (dataprocess.dataproc.hops[pProcDat->actualpause_hops].time_ms - hops_act_time) / 1000 / 60;
                uint32_t sec_ost = (dataprocess.dataproc.hops[pProcDat->actualpause_hops].time_ms - hops_act_time) / 1000 % 60;

                memset(messageprocess, 0, sizeof(messageprocess));
                swprintf(messageprocess, sizeof(messageprocess), L"Хмель:%01i М:%04i:%02i  %S ", pProcDat->actualpause_hops, min_ost, sec_ost, &simv_blink[show_sim][0]);

                heater_process_hops(&DataToInd.structurdata, &dataprocess.dataproc.hops[pProcDat->actualpause_hops], &pidparametr, temperature, pProcDat->actualpause_hops);
                hops_act_time += 100;
                if (hops_act_time >= dataprocess.dataproc.hops[pProcDat->actualpause_hops].time_ms)
                {
                    pProcDat->playonetrig_hops = 0;
                    if (dataprocess.dataproc.hops[pProcDat->actualpause_hops].numberhops > pProcDat->actualpause_hops)
                    {
                        pProcDat->actualstepproc_hops = 4;
                        pProcDat->shownumberstr = 0;
                    }
                    else
                    {
                        pProcDat->actualstepproc_hops = 5;
                    }
                }
                break;
            }
            case 4:
            {

                if (pProcDat->playonetrig_hops == 0)
                {
                    selectplayaudio(HOPSADD_AUDIO); //add hops
                    pProcDat->playonetrig_hops = 1;
                }

                memset(messageprocess, 0, sizeof(messageprocess));
                if (pProcDat->shownumberstr == 0)
                {
                    swprintf(messageprocess, sizeof(messageprocess), L"Кип.М1:%03i М2:%03i  %S ", DataFromIndHeater.structurdata.factpwm1, DataFromIndHeater.structurdata.factpwm2, &simv_blink[show_sim][0]);
                }
                else if (pProcDat->shownumberstr == 1)
                {
                    swprintf(messageprocess, sizeof(messageprocess), L"Кип.Ожидание хмеля.%S ", &simv_blink[show_sim][0]);
                }
                else if (pProcDat->shownumberstr == 2)
                {
                    swprintf(messageprocess, sizeof(messageprocess), L"Кип.Посл.внесения. %S ", &simv_blink[show_sim][0]);
                }
                else if (pProcDat->shownumberstr == 3)
                {
                    swprintf(messageprocess, sizeof(messageprocess), L"Кип.Нажмите ввод.. %S ", &simv_blink[show_sim][0]);
                }
                heater_process_hops(&DataToInd.structurdata, &dataprocess.dataproc.hops[pProcDat->actualpause_hops], &pidparametr, temperature, pProcDat->actualpause_hops);

                if ((getkeyenter() == 1) && (parametrmenu.submenuhops0 == 0) && (parametrmenu.menunumber == 0))
                {
                    pProcDat->actualstepproc_hops = 2;
                    pProcDat->playonetrig_hops = 0;
                }

                break;
            }
            case 5:
            {

                if (pProcDat->playonetrig_hops == 0)
                {
                    selectplayaudio(BOILINGDONE_AUDIO); //process done
                    pProcDat->playonetrig_hops = 1;
                }
                memset(messageprocess, 0, sizeof(messageprocess));
                swprintf(messageprocess, sizeof(messageprocess), L"Проц.Выпол.Хмель:%01i %S ", pProcDat->actualpause_hops, &simv_blink[show_sim][0]);

                DataFromIndHeater.structurdata.inductor1pwm = 0;
                parametrmenu.maxpwmreqp1 = 0;
                DataFromIndHeater.structurdata.inductor2pwm = 0;
                parametrmenu.maxpwmreqp2 = 0;
                parametrfasnrtl.stat = INDUC_OFF;
                break;
            }

            default:
            {
                pProcDat->actualstepproc_hops = 0;
                break;
            }
            }
        }

        if (pProcDat->counttimesowstr < 15)
        {
            pProcDat->counttimesowstr++;
        }
        else
        {
            pProcDat->counttimesowstr = 0;

            if (pProcDat->shownumberstr < 3)
            {
                pProcDat->shownumberstr++;
            }
            else
            {
                pProcDat->shownumberstr = 0;
            }
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
//-----------------------------------------------------------------------------//
void heater_process_mesh(DataToIndHeater *induc_board, Tdatapause_mashing *meshdata, Tpidstr *piddata, float temper, uint8_t nuberproc)
{

    if (onofowertempan1 == 0)
    {
        if (TEM_IND1 > meshdata->temperature_pan1)
        {
            onofowertempan1 = 1;
        }
    }
    else
    {
        if (TEM_IND1 < (meshdata->temperature_pan1 - meshdata->temperature_pan_delta))
        {
            onofowertempan1 = 0;
        }
    }

    if (onofowertempan2 == 0)
    {
        if (TEM_IND2 > meshdata->temperature_pan2)
        {
            onofowertempan2 = 1;
        }
    }
    else
    {
        if (TEM_IND2 < (meshdata->temperature_pan2 - meshdata->temperature_pan_delta))
        {
            onofowertempan2 = 0;
        }
    }

    if (statusmax31865 != 0)
    {

        piddata->pid_out = 0;
        piddata->pid_max = meshdata->maxpwm;
        piddata->pid_min = PID_OUTMIN;
        pid_reg_init(piddata->pid_p, piddata->pid_i, piddata->pid_d, 1.0, piddata->pid_max, piddata->pid_max, piddata->pid_min);
        induc_board->pwm_ind1 = (uint8_t)PID_OUTMIN;
        parametrmenu.maxpwmreqp1 = (uint8_t)PID_OUTMIN;
        induc_board->pwm_ind2 = (uint8_t)PID_OUTMIN;
        parametrmenu.maxpwmreqp2 = (uint8_t)PID_OUTMIN;
    }
    else
    {

        if (StructPid.oldnumproc != nuberproc)
        {
            StructPid.oldnumproc = nuberproc;
            piddata->pid_max = meshdata->maxpwm;
            piddata->pid_min = PID_OUTMIN;
            pid_reg_init(piddata->pid_p, piddata->pid_i, piddata->pid_d, 1.0, piddata->pid_max, piddata->pid_max, piddata->pid_min);
        }
        piddata->pid_out = pid_reg(meshdata->temperature_mesh, temper, piddata->pid_max, piddata->pid_min);
        induc_board->pwm_ind1 = (uint8_t)piddata->pid_out;
        parametrmenu.maxpwmreqp1 = (uint8_t)piddata->pid_out;
        induc_board->pwm_ind2 = (uint8_t)piddata->pid_out;
        parametrmenu.maxpwmreqp2 = (uint8_t)piddata->pid_out;
    }

    if (onofowertemmesh == 0)
    {
        if (temper > (meshdata->temperature_mesh + meshdata->temperature_mesh_delta))
        {
            onofowertemmesh = 1;
        }
    }
    else
    {
        if (temper <= meshdata->temperature_mesh)
        {
            onofowertemmesh = 0;
        }
    }

    if ((meshdata->induct1_use == 1) && (onofowertempan1 == 0) && (onofowertemmesh == 0) && (statusmax31865 == 0))
    {
        induc_board->OnOffHeater |= INDUCTOR1_ON;
    }
    else
    {
        induc_board->OnOffHeater &= INDUCTOR1_OFF;
    }

    if ((meshdata->induct2_use == 1) && (onofowertempan2 == 0) && (onofowertemmesh == 0) && (statusmax31865 == 0))
    {
        induc_board->OnOffHeater |= INDUCTOR2_ON;
    }
    else
    {
        induc_board->OnOffHeater &= INDUCTOR2_OFF;
    }
}
//-----------------------------------------------------------------------------//
void heater_process_hops(DataToIndHeater *induc_board, Tdatahops *hops, Tpidstr *piddata, float temper, uint8_t nuberproc)
{

    if (onofowertempan1 == 0)
    {
        if (TEM_IND1 > hops->temperature_pan1)
        {
            onofowertempan1 = 1;
        }
    }
    else
    {
        if (TEM_IND1 < (hops->temperature_pan1 - hops->temperature_pan_delta))
        {
            onofowertempan1 = 0;
        }
    }

    if (onofowertempan2 == 0)
    {
        if (TEM_IND2 > hops->temperature_pan2)
        {
            onofowertempan2 = 1;
        }
    }
    else
    {
        if (TEM_IND2 < (hops->temperature_pan2 - hops->temperature_pan_delta))
        {
            onofowertempan2 = 0;
        }
    }

    if (statusmax31865 != 0)
    {
        piddata->pid_out = 0;
        piddata->pid_max = hops->maxpwm;
        piddata->pid_min = PID_OUTMIN;
        induc_board->pwm_ind1 = (uint8_t)PID_OUTMIN;
        parametrmenu.maxpwmreqp1 = (uint8_t)PID_OUTMIN;
        induc_board->pwm_ind2 = (uint8_t)PID_OUTMIN;
        parametrmenu.maxpwmreqp2 = (uint8_t)PID_OUTMIN;
        pid_reg_init(piddata->pid_p, piddata->pid_i, piddata->pid_d, 1.0, piddata->pid_max, piddata->pid_max, piddata->pid_min);
    }
    else
    {

        if (StructPid.oldnumproc != nuberproc)
        {
            StructPid.oldnumproc = nuberproc;
            piddata->pid_max = hops->maxpwm;
            piddata->pid_min = PID_OUTMIN;
            pid_reg_init(piddata->pid_p, piddata->pid_i, piddata->pid_d, 1.0, piddata->pid_max, piddata->pid_max, piddata->pid_min);
        }

        piddata->pid_out = pid_reg(hops->temperature_hops, temper, piddata->pid_max, piddata->pid_min);
        induc_board->pwm_ind1 = (uint8_t)piddata->pid_out;
        parametrmenu.maxpwmreqp1 = (uint8_t)piddata->pid_out;
        induc_board->pwm_ind2 = (uint8_t)piddata->pid_out;
        parametrmenu.maxpwmreqp2 = (uint8_t)piddata->pid_out;
    }

    if (onofowertemhops == 0)
    {
        if (temper > (hops->temperature_hops + hops->temperature_hops_delta))
        {
            onofowertemhops = 1;
        }
    }
    else
    {
        if (temper <= hops->temperature_hops)
        {
            onofowertemhops = 0;
        }
    }

    if ((hops->induct1_use == 1) && (onofowertempan1 == 0) && (onofowertemhops == 0) && (statusmax31865 == 0))
    {
        induc_board->OnOffHeater |= INDUCTOR1_ON;
    }
    else
    {
        induc_board->OnOffHeater &= INDUCTOR1_OFF;
    }

    if ((hops->induct2_use == 1) && (onofowertempan2 == 0) && (onofowertemhops == 0) && (statusmax31865 == 0))
    {
        induc_board->OnOffHeater |= INDUCTOR2_ON;
    }
    else
    {
        induc_board->OnOffHeater &= INDUCTOR2_OFF;
    }
}
//-----------------------------------------------------------------------------//
void pumponoff(TPump *pPom)
{
    if (pPom->pump1 == 1)
    {
        gpio_set_level(K1, HIGHT);
    }
    else
    {
        gpio_set_level(K1, LOW);
    }

    if (pPom->pump2 == 1)
    {
        gpio_set_level(K2, HIGHT);
    }
    else
    {
        gpio_set_level(K2, LOW);
    }

    if (pPom->pump3 == 1)
    {
        gpio_set_level(K3, HIGHT);
    }
    else
    {
        gpio_set_level(K3, LOW);
    }
}
//-----------------------------------------------------------------------------//
void pid_reg_init(float p_factor, float i_factor, float d_factor, float scal_fac, float max, float maxterm, float min)
{

    StructPid.sumError = 0;
    StructPid.lastProcessValue = 0;
    StructPid.P_Factor = p_factor;
    StructPid.I_Factor = i_factor;
    StructPid.D_Factor = d_factor;

    StructPid.maxError_up = max;
    StructPid.maxSumError_up = maxterm;
    StructPid.maxError_down = 0;
    StructPid.maxSumError_down = 0;

    StructPid.scalfac = scal_fac;
}
//-----------------------------------------------------------------------------//
float pid_reg(float setPoint, float processValue, float max, float min)
{
    float error, p_term, d_term;
    float i_term, ret, temp;

    error = setPoint - processValue;

    if (error > StructPid.maxError_up)
    {
        p_term = max;
    }
    else if (error < StructPid.maxError_down)
    {
        p_term = 0;
    }
    else
    {
        p_term = StructPid.P_Factor * error;
    }

    temp = StructPid.sumError + error;
    if (temp > StructPid.maxSumError_up)
    {
        i_term = max;
        StructPid.sumError = StructPid.maxSumError_up;
    }
    else if (temp < StructPid.maxSumError_down)
    {
        i_term = 0;
        StructPid.sumError = StructPid.maxSumError_down;
    }
    else
    {
        StructPid.sumError = temp;
        i_term = StructPid.I_Factor * StructPid.sumError;
    }

    //----- Calculate Dterm
    d_term = StructPid.D_Factor * (StructPid.lastProcessValue - processValue);

    StructPid.lastProcessValue = processValue;

    ret = (p_term + i_term + d_term) / StructPid.scalfac;
    if (ret > max)
    {
        ret = max;
    }
    else if (ret < min)
    {
        ret = min;
    }

    return ret;
}
//-----------------------------------------------------------------------------//
