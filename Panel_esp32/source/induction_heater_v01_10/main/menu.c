
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
#include "menu.h"
#include "wchar.h"
//-----------------------------------------------------------------------------//
Tparametrmenu parametrmenu;
const wchar_t menusettname[6][20] = {
    L"Выбор режима работы\0",
    L"Управление насосами\0",
    L"Пар-ры затирания   \0",
    L"Пар-ры кипячения   \0",
    L"Пар-ры ПИД регул   \0",
    L"                   \0"};
const wchar_t parametr_mesh_select[12][20] = {
    L"Ввод темпер.Пауз   \0",
    L"Ввод темпер.Панели1\0",
    L"Ввод темпер.Панели2\0",
    L"Гистерезис рег.    \0",
    L"Гистерезис панели  \0",
    L"Вкл/Выкл панель1   \0",
    L"Вкл/Выкл панель2   \0",
    L"Макс.Допус.Вых.М%  \0",
    L"Время паузы мин.   \0",
    L"Количество пауз    \0",
    L"Возврат к выбору Пр\0",
    L"                   \0"};
const wchar_t parametr_hops_select[12][20] = {
    L"Ввод темпер.Кипяч  \0",
    L"Ввод темпер.Панели1\0",
    L"Ввод темпер.Панели2\0",
    L"Гистерезис рег.    \0",
    L"Гистерезис панели  \0",
    L"Вкл/Выкл панель1   \0",
    L"Вкл/Выкл панель2   \0",
    L"Макс.Допус.Вых.М%  \0",
    L"Время кипяч-ия мин.\0",
    L"Кол-во Засып.Хмеля \0",
    L"Возврат к выбору Пр\0",
    L"                   \0"};
const wchar_t progressbarr[22][21] = {
    L"                    \0",
    L"                   <\0",
    L"                  <<\0",
    L"                 <<<\0",
    L"                <<<<\0",
    L"               <<<<<\0",
    L"              <<<<<<\0",
    L"             <<<<<<<\0",
    L"            <<<<<<<<\0",
    L"           <<<<<<<<<\0",
    L"          <<<<<<<<<<\0",
    L"         <<<<<<<<<<<\0",
    L"        <<<<<<<<<<<<\0",
    L"       <<<<<<<<<<<<<\0",
    L"      <<<<<<<<<<<<<<\0",
    L"     <<<<<<<<<<<<<<<\0",
    L"    <<<<<<<<<<<<<<<<\0",
    L"   <<<<<<<<<<<<<<<<<\0",
    L"  <<<<<<<<<<<<<<<<<<\0",
    L" <<<<<<<<<<<<<<<<<<<\0",
    L"<<<<<<<<<<<<<<<<<<<<\0",
    L"<<<<<<<<<<<<<<<<<<<<\0"};
const wchar_t simv_blink[2][2] = {
    L" \0",
    L"<\0"};
const wchar_t parametr_pid_select[4][20] = {
    L"Ввод коэф.Пропорц. \0",
    L"Ввод коэф.Интегр.  \0",
    L"Ввод коэф.Дифферен.\0",
    L"Возврат к выбору Пр\0"};
uint8_t counterblinr = 0;
uint8_t show_sim = 0;
wchar_t messageprocess[21];
//-----------------------------------------------------------------------------//
void runtaskmenu()
{
    xTaskCreate(task_menu, "task_menu", STACK_SIZE_MENU_TASK, (void *)&parametrmenu, (configMAX_PRIORITIES - 4), NULL);
}
//-----------------------------------------------------------------------------//
void task_menu(void *arg)
{
    Tparametrmenu *parg = (Tparametrmenu *)arg;
    while (1)
    {

        vTaskSuspendAll();
        parg->row = (wchar_t *)malloc(SIZESTRBUF + 4);
        xTaskResumeAll();
        switch (parg->menunumber)
        {
        case 0:
        {
            if (dataprocess.dataproc.mode_work == MODE_OFF)
            {
                memset(parg->row, 0, SIZESTRBUF);
                swprintf(parg->row, SIZESTRBUF, L"Режим работы:(Выкл.)");
                lcdprint(1, 0, parg->row, 20);

                memset(parg->row, 0, SIZESTRBUF);
                if (statusmax31865 == 0)
                {
                    swprintf(parg->row, SIZESTRBUF, L"Темпер. сусла:%05.01f  ", temperature);
                }
                else
                {
                    swprintf(parg->row, SIZESTRBUF, L"Темпер.сусла:(обрыв) ");
                }
                lcdprint(2, 0, parg->row, 20);
                memset(parg->row, 0, SIZESTRBUF);
                swprintf(parg->row, SIZESTRBUF, L"П1:(%05.01f)П2:(%05.01f)", TEM_IND1, TEM_IND2);
                lcdprint(3, 0, parg->row, 20);

                memset(parg->row, 0, SIZESTRBUF);
                swprintf(parg->row, SIZESTRBUF, L"Т1:(%05.01f)Т2:(%05.01f)", TEM_IGBT1, TEM_IGBT2);
                lcdprint(4, 0, parg->row, 20);
            }
            else if (dataprocess.dataproc.mode_work == MODE_MESH)
            {

                switch (parg->submenumesh0)
                {
                case 0:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"%S", messageprocess);
                    lcdprint(1, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    if (statusmax31865 == 0)
                    {
                        swprintf(parg->row, SIZESTRBUF, L"Треб:%05.01f ФK:%05.01f ", dataprocess.dataproc.mash[mesh_num].temperature_mesh, temperature);
                    }
                    else
                    {
                        swprintf(parg->row, SIZESTRBUF, L"Темпер.сусла:(обрыв) ");
                    }
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"П1:(%05.01f)П2:(%05.01f)", TEM_IND1, TEM_IND2);
                    lcdprint(3, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Т1:(%05.01f)Т2:(%05.01f)", TEM_IGBT1, TEM_IGBT2);
                    lcdprint(4, 0, parg->row, 20);
                    if (parg->submenumesh0 < 7)
                        if (getkeyup())
                            parg->submenumesh0++;
                    if (parg->submenumesh0 > 0)
                        if (getkeydown())
                            parg->submenumesh0--;

                    break;
                } //submenu 0
                case 1:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Режим работы:(Затир)");
                    lcdprint(1, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель1 Тогр.:%05.01f ", dataprocess.dataproc.mash[mesh_num].temperature_pan1);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель1 факт.:%05.01f ", TEM_IND1);
                    lcdprint(3, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель1 Гист.:%05.01f ", dataprocess.dataproc.mash[mesh_num].temperature_pan_delta);
                    lcdprint(4, 0, parg->row, 20);
                    if (parg->submenumesh0 < 7)
                        if (getkeyup())
                            parg->submenumesh0++;
                    if (parg->submenumesh0 > 0)
                        if (getkeydown())
                            parg->submenumesh0--;

                    break;
                } //submenu 1
                case 2:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Режим работы:(Затир)");
                    lcdprint(1, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель2 Тогр.:%05.01f ", dataprocess.dataproc.mash[mesh_num].temperature_pan2);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель2 факт.:%05.01f ", TEM_IND2);
                    lcdprint(3, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Макс.Вых.Мощ.%% :%03i ", dataprocess.dataproc.mash[mesh_num].maxpwm);
                    lcdprint(4, 0, parg->row, 20);
                    if (parg->submenumesh0 < 7)
                        if (getkeyup())
                            parg->submenumesh0++;
                    if (parg->submenumesh0 > 0)
                        if (getkeydown())
                            parg->submenumesh0--;

                    break;
                } //submenu 2
                case 3:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Режим работы:(Затир)");
                    lcdprint(1, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель1 Вкл/Выкл.:%i ", dataprocess.dataproc.mash[mesh_num].induct1_use);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель2 Вкл/Выкл.:%i ", dataprocess.dataproc.mash[mesh_num].induct2_use);
                    lcdprint(3, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Время паузы мин:%03i ", dataprocess.dataproc.mash[mesh_num].time_ms / 1000 / 60);
                    lcdprint(4, 0, parg->row, 20);
                    if (parg->submenumesh0 < 7)
                        if (getkeyup())
                            parg->submenumesh0++;
                    if (parg->submenumesh0 > 0)
                        if (getkeydown())
                            parg->submenumesh0--;

                    break;
                } //submenu 3
                case 4:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Режим работы:(Затир)");
                    lcdprint(1, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Напряжение пост:%03i ", DataFromIndHeater.structurdata.u_rectifler);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель1 фк.ток:%04.01f ", (float)DataFromIndHeater.structurdata.i_inductor_1 / 10);
                    lcdprint(3, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель2 фк.ток:%04.01f ", (float)DataFromIndHeater.structurdata.i_inductor_2 / 10);
                    lcdprint(4, 0, parg->row, 20);
                    if (parg->submenumesh0 < 7)
                        if (getkeyup())
                            parg->submenumesh0++;
                    if (parg->submenumesh0 > 0)
                        if (getkeydown())
                            parg->submenumesh0--;

                    break;
                } //submenu 4
                case 5:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Режим работы:(Затир)");
                    lcdprint(1, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Макс число пауз:%02i  ", dataprocess.dataproc.mash[mesh_num].numbermesh);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Актуальная пауза:%02i  ", mesh_num);
                    lcdprint(3, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"До конца паузы:%03i  ", (dataprocess.dataproc.mash[mesh_num].time_ms - mesh_act_time) / 1000 / 60);
                    lcdprint(4, 0, parg->row, 20);
                    if (parg->submenumesh0 < 7)
                        if (getkeyup())
                            parg->submenumesh0++;
                    if (parg->submenumesh0 > 0)
                        if (getkeydown())
                            parg->submenumesh0--;

                    break;
                } //submenu 5
                case 6:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Режим работы:(Затир)");
                    lcdprint(1, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Пид рег К Проп:%04.01f  ", pidparametr.pid_p);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Пид рег К Интг:%04.01f  ", pidparametr.pid_i);
                    lcdprint(3, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Пид рег К Диф :%04.01f  ", pidparametr.pid_d);
                    lcdprint(4, 0, parg->row, 20);
                    if (parg->submenumesh0 < 7)
                        if (getkeyup())
                            parg->submenumesh0++;
                    if (parg->submenumesh0 > 0)
                        if (getkeydown())
                            parg->submenumesh0--;

                    break;
                } //submenu 6
                case 7:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Режим работы:(Затир)");
                    lcdprint(1, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Насос1/2/3 вкл(%i%i%i)  ", pumponof.pump1, pumponof.pump2, pumponof.pump3);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Мощн:П1(%03i) П2(%03i)  " , parg->maxpwmreqp1, parg->maxpwmreqp2);
                    lcdprint(3, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"М.Фк:П1(%03i) П2(%03i)  ", DataFromIndHeater.structurdata.factpwm1, DataFromIndHeater.structurdata.factpwm2);
                    lcdprint(4, 0, parg->row, 20);
                    if (parg->submenumesh0 < 7)
                        if (getkeyup())
                            parg->submenumesh0++;
                    if (parg->submenumesh0 > 0)
                        if (getkeydown())
                            parg->submenumesh0--;

                    break;
                } //submenu 7
                default:
                {
                    parg->submenumesh0 = 0;
                    break;
                }
                }
            }
            else if (dataprocess.dataproc.mode_work == MODE_HOPS)
            {

                switch (parg->submenuhops0)
                {
                case 0:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"%S", messageprocess);
                    lcdprint(1, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    if (statusmax31865 == 0)
                    {
                        swprintf(parg->row, SIZESTRBUF, L"Треб:%05.01f ФK:%05.01f ", dataprocess.dataproc.hops[hops_num].temperature_hops, temperature);
                    }
                    else
                    {
                        swprintf(parg->row, SIZESTRBUF, L"Темпер.сусла:(обрыв) ");
                    }
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"П1:(%05.01f)П2:(%05.01f)", TEM_IND1, TEM_IND2);
                    lcdprint(3, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Т1:(%05.01f)Т2:(%05.01f)", TEM_IGBT1, TEM_IGBT2);
                    lcdprint(4, 0, parg->row, 20);
                    if (parg->submenuhops0 < 7)
                        if (getkeyup())
                            parg->submenuhops0++;
                    if (parg->submenuhops0 > 0)
                        if (getkeydown())
                            parg->submenuhops0--;

                    break;
                } //submenu 0
                case 1:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Режим работы:(Кипяч)");
                    lcdprint(1, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель1 Тогр.:%05.01f ", dataprocess.dataproc.hops[hops_num].temperature_pan1);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель1 факт.:%05.01f ", TEM_IND1);
                    lcdprint(3, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель1 Гист.:%05.01f ", dataprocess.dataproc.hops[hops_num].temperature_pan_delta);
                    lcdprint(4, 0, parg->row, 20);
                    if (parg->submenuhops0 < 7)
                        if (getkeyup())
                            parg->submenuhops0++;
                    if (parg->submenuhops0 > 0)
                        if (getkeydown())
                            parg->submenuhops0--;

                    break;
                } //submenu 1
                case 2:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Режим работы:(Кипяч)");
                    lcdprint(1, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель2 Тогр.:%05.01f ", dataprocess.dataproc.hops[hops_num].temperature_pan2);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель2 факт.:%05.01f ", TEM_IND2);
                    lcdprint(3, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Макс.Вых.Мощ.%% :%03i ", dataprocess.dataproc.hops[hops_num].maxpwm);
                    lcdprint(4, 0, parg->row, 20);
                    if (parg->submenuhops0 < 7)
                        if (getkeyup())
                            parg->submenuhops0++;
                    if (parg->submenuhops0 > 0)
                        if (getkeydown())
                            parg->submenuhops0--;

                    break;
                } //submenu 2
                case 3:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Режим работы:(Кипяч)");
                    lcdprint(1, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель1 Вкл/Выкл.:%i ", dataprocess.dataproc.hops[hops_num].induct1_use);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель2 Вкл/Выкл.:%i ", dataprocess.dataproc.hops[hops_num].induct2_use);
                    lcdprint(3, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Время паузы мин:%03i ", dataprocess.dataproc.hops[hops_num].time_ms / 1000 / 60);
                    lcdprint(4, 0, parg->row, 20);
                    if (parg->submenuhops0 < 7)
                        if (getkeyup())
                            parg->submenuhops0++;
                    if (parg->submenuhops0 > 0)
                        if (getkeydown())
                            parg->submenuhops0--;

                    break;
                } //submenu 3
                case 4:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Режим работы:(Кипяч)");
                    lcdprint(1, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Напряжение пост:%03i ", DataFromIndHeater.structurdata.u_rectifler);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель1 фк.ток:%04.01f ", (float)DataFromIndHeater.structurdata.i_inductor_1 / 10);
                    lcdprint(3, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель2 фк.ток:%04.01f ", (float)DataFromIndHeater.structurdata.i_inductor_2 / 10);
                    lcdprint(4, 0, parg->row, 20);
                    if (parg->submenuhops0 < 7)
                        if (getkeyup())
                            parg->submenuhops0++;
                    if (parg->submenuhops0 > 0)
                        if (getkeydown())
                            parg->submenuhops0--;

                    break;
                } //submenu 4
                case 5:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Режим работы:(Кипяч)");
                    lcdprint(1, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Макс число пауз:%02i  ", dataprocess.dataproc.hops[hops_num].numberhops);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Актуальная пауза:%02i  ", hops_num);
                    lcdprint(3, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"До конца паузы:%03i  ", (dataprocess.dataproc.hops[hops_num].time_ms - hops_act_time) / 1000 / 60);
                    lcdprint(4, 0, parg->row, 20);
                    if (parg->submenuhops0 < 7)
                        if (getkeyup())
                            parg->submenuhops0++;
                    if (parg->submenuhops0 > 0)
                        if (getkeydown())
                            parg->submenuhops0--;

                    break;
                } //submenu 5
                case 6:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Режим работы:(Кипяч)");
                    lcdprint(1, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Пид рег К Проп:%04.01f  ", pidparametr.pid_p);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Пид рег К Интг:%04.01f  ", pidparametr.pid_i);
                    lcdprint(3, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Пид рег К Диф :%04.01f  ", pidparametr.pid_d);
                    lcdprint(4, 0, parg->row, 20);
                    if (parg->submenuhops0 < 7)
                        if (getkeyup())
                            parg->submenuhops0++;
                    if (parg->submenuhops0 > 0)
                        if (getkeydown())
                            parg->submenuhops0--;

                    break;
                } //submenu 6
                case 7:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Режим работы:(Кипяч)");
                    lcdprint(1, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Насос1/2/3 вкл(%i%i%i)  ", pumponof.pump1, pumponof.pump2, pumponof.pump3);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Мощн:П1(%03i) П2(%03i)  ", parg->maxpwmreqp1, parg->maxpwmreqp2);
                    lcdprint(3, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"М.Фк:П1(%03i) П2(%03i)  ", DataFromIndHeater.structurdata.factpwm1, DataFromIndHeater.structurdata.factpwm2);
                    lcdprint(4, 0, parg->row, 20);
                    if (parg->submenuhops0 < 7)
                        if (getkeyup())
                            parg->submenuhops0++;
                    if (parg->submenuhops0 > 0)
                        if (getkeydown())
                            parg->submenuhops0--;

                    break;
                } //submenu 7
                default:
                {
                    parg->submenuhops0 = 0;
                    break;
                }
                }
            }
            if (getkeymenu())
            {
                parg->menunumber++;
                parg->meshselectmenu = 0;
                parg->meshelectparam = 0;
                parg->menusett = 0;
                parg->enterparampress = 0;
                parg->tempfloat = 0.0;
                parg->tempu8t = 0;
                parg->tempu32t = 0;
                parg->selectmenusett = 0;
                parg->hopsselectmenu = 0;
                parg->hopselectparam = 0;
                parg->submenupid = 0;
            }

            break;
        }

        case 1:
        {

            switch (parg->menusett)
            {
            case 0:
            {
                memset(parg->row, 0, SIZESTRBUF);
                swprintf(parg->row, SIZESTRBUF, L"Ввод парам.Пивоварни ");
                lcdprint(1, 0, parg->row, 20);
                memset(parg->row, 0, SIZESTRBUF);
                swprintf(parg->row, SIZESTRBUF, L"%S%S", &menusettname[parg->selectmenusett][0], &simv_blink[show_sim][0]);
                lcdprint(2, 0, parg->row, 20);
                memset(parg->row, 0, SIZESTRBUF);
                swprintf(parg->row, SIZESTRBUF, L"Для выбора нажмите  ");
                lcdprint(3, 0, parg->row, 20);
                memset(parg->row, 0, SIZESTRBUF);
                swprintf(parg->row, SIZESTRBUF, L"ввод,перемещение +/- ");
                lcdprint(4, 0, parg->row, 20);
                if (getkeyup())
                {
                    if (parg->selectmenusett < 4)
                    {
                        parg->selectmenusett++;
                    }
                }

                if (getkeydown())
                {
                    if (parg->selectmenusett > 0)
                    {
                        parg->selectmenusett--;
                    }
                }

                if (getkeyenter())
                {
                    parg->menusett = parg->selectmenusett + 1;
                }
                break;
            } //0
            case 1:
            {
                memset(parg->row, 0, SIZESTRBUF);
                swprintf(parg->row, SIZESTRBUF, L"Выбор режима работы  ");
                lcdprint(1, 0, parg->row, 20);

                memset(parg->row, 0, SIZESTRBUF);
                if (parg->tempu8t == MODE_OFF)
                {
                    swprintf(parg->row, SIZESTRBUF, L"Нагрев выключен    %S", &simv_blink[show_sim][0]);
                }
                else if (parg->tempu8t  == MODE_MESH)
                {
                    swprintf(parg->row, SIZESTRBUF, L"Процесс затир.сусл %S", &simv_blink[show_sim][0]);
                }
                else if (parg->tempu8t  == MODE_HOPS)
                {
                    swprintf(parg->row, SIZESTRBUF, L"Процесс кипяч.сусл %S", &simv_blink[show_sim][0]);
                }
                lcdprint(2, 0, parg->row, 20);

                if (getkeyup())
                {
                    if (parg->tempu8t  < 2)
                    {
                        parg->tempu8t ++;
                    }
                }
                if (getkeydown())
                {
                    if (parg->tempu8t  > 0)
                    {
                        parg->tempu8t --;
                        
                    }
                }

                if (getkeyenter())
                {
                    dataprocess.dataproc.mode_work= parg->tempu8t;
                    save_data_process();
                    parg->menusett = 0;
                }

                break;
            } //1
            case 2:
            {
                memset(parg->row, 0, SIZESTRBUF);
                swprintf(parg->row, SIZESTRBUF, L"Управление насосами ");
                lcdprint(1, 0, parg->row, 20);
                switch (parg->selectpump)
                {
                case 0:
                {
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Насос1 Включить    %S", &simv_blink[show_sim][0]);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Для выбора действия  ");
                    lcdprint(3, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"нажмите ввод.        ");
                    lcdprint(4, 0, parg->row, 20);

                    if (getkeyenter())
                    {
                        pumponof.pump1 = 1;
                        parg->menusett = 0;
                    }
                    break;
                } //0
                case 1:
                {
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Насос1 Выключить   %S", &simv_blink[show_sim][0]);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Для выбора действия  ");
                    lcdprint(3, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"нажмите ввод.        ");
                    lcdprint(4, 0, parg->row, 20);
                    if (getkeyenter())
                    {
                        pumponof.pump1 = 0;
                        parg->menusett = 0;
                    }
                    break;
                } //1
                case 2:
                {
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Насос2 Включить    %S", &simv_blink[show_sim][0]);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Для выбора действия  ");
                    lcdprint(3, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"нажмите ввод.        ");
                    lcdprint(4, 0, parg->row, 20);
                    if (getkeyenter())
                    {
                        pumponof.pump2 = 1;
                        parg->menusett = 0;
                    }
                    break;
                } //2
                case 3:
                {
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Насос2 Выключить   %S", &simv_blink[show_sim][0]);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Для выбора действия  ");
                    lcdprint(3, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"нажмите ввод.        ");
                    lcdprint(4, 0, parg->row, 20);
                    if (getkeyenter())
                    {
                        pumponof.pump2 = 0;
                        parg->menusett = 0;
                    }
                    break;
                } //3
                case 4:
                {
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Насос3 Включить    %S", &simv_blink[show_sim][0]);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Для выбора действия  ");
                    lcdprint(3, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"нажмите ввод.        ");
                    lcdprint(4, 0, parg->row, 20);
                    if (getkeyenter())
                    {
                        pumponof.pump3 = 1;
                        parg->menusett = 0;
                    }
                    break;
                } //4
                case 5:
                {
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Насос3 Выключить   %S", &simv_blink[show_sim][0]);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Для выбора действия  ");
                    lcdprint(3, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"нажмите ввод.        ");
                    lcdprint(4, 0, parg->row, 20);
                    if (getkeyenter())
                    {
                        pumponof.pump3 = 0;
                        parg->menusett = 0;
                    }
                    break;
                } //5
                case 6:
                {
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Назад на главное М.%S", &simv_blink[show_sim][0]);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Для выбора действия  ");
                    lcdprint(3, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"нажмите ввод.        ");
                    lcdprint(4, 0, parg->row, 20);
                    if (getkeyenter())
                    {
                        parg->menusett = 0;
                    }
                    break;
                } //6

                default:
                {
                    parg->selectpump = 0;
                    break;
                }
                }
                if (getkeyup())
                {
                    if (parg->selectpump < 6)
                    {
                        parg->selectpump++;
                    }
                }
                if (getkeydown())
                {
                    if (parg->selectpump > 0)
                    {
                        parg->selectpump--;
                    }
                }

                break;
            } //2
            case 3:
            {
                memset(parg->row, 0, SIZESTRBUF);
                swprintf(parg->row, SIZESTRBUF, L"Парам.Затир.Сусла   ");
                lcdprint(1, 0, parg->row, 20);
                switch (parg->meshselectmenu)
                {
                case 0:
                {
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"%S%S", &parametr_mesh_select[parg->meshelectparam][0], &simv_blink[show_sim][0]);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Для выбора нажмите  ");
                    lcdprint(3, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"ввод,перемещение +/- ");
                    lcdprint(4, 0, parg->row, 20);
                    if (getkeyup())
                    {
                        if (parg->meshelectparam < 10)
                        {
                            parg->meshelectparam++;
                        }
                    }

                    if (getkeydown())
                    {
                        if (parg->meshelectparam > 0)
                        {
                            parg->meshelectparam--;
                        }
                    }

                    if (getkeyenter())
                    {
                        parg->meshselectmenu = parg->meshelectparam + 1;
                        parg->meshpausenum = 0;
                        parg->enterparampress = 0;
                    }

                    break;
                } //0
                case 1:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Ввод температуры     ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Пауза:N%02i Тем:%05.01f  ", parg->meshpausenum, dataprocess.dataproc.mash[parg->meshpausenum].temperature_mesh);
                    lcdprint(2, 0, parg->row, 20);

                    if (parg->enterparampress == 0)
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Для уст.Температуры  ");
                        lcdprint(3, 0, parg->row, 20);
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Нажмите ввод         ");
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 1;
                            parg->tempfloat = 0.0;
                        }
                        if (getkeyup())
                        {
                            if (parg->meshpausenum < MAXPAUSMASHING)
                            {
                                parg->meshpausenum++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->meshpausenum > 0)
                            {
                                parg->meshpausenum--;
                            }
                        }
                    }
                    else
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Температура:(%05.01f)%S ", parg->tempfloat, &simv_blink[show_sim][0]);
                        lcdprint(3, 0, parg->row, 20);

                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 0;
                            dataprocess.dataproc.mash[parg->meshpausenum].temperature_mesh = parg->tempfloat;
                            save_data_process();
                            parg->meshselectmenu = 0;
                            parg->meshelectparam = 0;
                        }
                        if ((getkeyup() == 1) || (waiter_speedup > 199))
                        {
                            if (parg->tempfloat < MAXTEMPMESH)
                            {
                                if (waiter_speedup > 199)
                                {
                                    parg->tempfloat += 1.0;
                                }
                                else
                                {
                                    parg->tempfloat += 0.1;
                                }
                            }
                        }

                        if ((getkeydown() == 1) || (waiter_speeddown > 199))
                        {
                            if (parg->tempfloat > 0)
                            {
                                if (waiter_speeddown > 199)
                                {
                                    parg->tempfloat -= 1.0;
                                }
                                else
                                {
                                    parg->tempfloat -= 0.1;
                                }
                            }
                        }
                    }

                    break;
                } //1
                case 2:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Ввод темпер.панели1  ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Пауза:N%02i Тем:%05.01f  ", parg->meshpausenum, dataprocess.dataproc.mash[parg->meshpausenum].temperature_pan1);
                    lcdprint(2, 0, parg->row, 20);

                    if (parg->enterparampress == 0)
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Для уст.Температуры  ");
                        lcdprint(3, 0, parg->row, 20);
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Нажмите ввод         ");
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 1;
                            parg->tempfloat = 0.0;
                        }
                        if (getkeyup())
                        {
                            if (parg->meshpausenum < MAXPAUSMASHING)
                            {
                                parg->meshpausenum++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->meshpausenum > 0)
                            {
                                parg->meshpausenum--;
                            }
                        }
                    }
                    else
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Температура:(%05.01f)%S ", parg->tempfloat, &simv_blink[show_sim][0]);
                        lcdprint(3, 0, parg->row, 20);

                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 0;
                            dataprocess.dataproc.mash[parg->meshpausenum].temperature_pan1 = parg->tempfloat;
                            save_data_process();
                            parg->meshselectmenu = 0;
                            parg->meshelectparam = 0;
                        }
                        if ((getkeyup() == 1) || (waiter_speedup > 199))
                        {
                            if (parg->tempfloat < MAXTEMPPAN1)
                            {
                                if (waiter_speedup > 199)
                                {
                                    parg->tempfloat += 1.0;
                                }
                                else
                                {
                                    parg->tempfloat += 0.1;
                                }
                            }
                        }

                        if ((getkeydown() == 1) || (waiter_speeddown > 199))
                        {
                            if (parg->tempfloat > 0)
                            {
                                if (waiter_speeddown > 199)
                                {
                                    parg->tempfloat -= 1.0;
                                }
                                else
                                {
                                    parg->tempfloat -= 0.1;
                                }
                            }
                        }
                    }

                    break;
                } //2
                case 3:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Ввод темпер.панели2  ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Пауза:N%02i Тем:%05.01f  ", parg->meshpausenum, dataprocess.dataproc.mash[parg->meshpausenum].temperature_pan2);
                    lcdprint(2, 0, parg->row, 20);

                    if (parg->enterparampress == 0)
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Для уст.Температуры  ");
                        lcdprint(3, 0, parg->row, 20);
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Нажмите ввод         ");
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 1;
                            parg->tempfloat = 0.0;
                        }
                        if (getkeyup())
                        {
                            if (parg->meshpausenum < MAXPAUSMASHING)
                            {
                                parg->meshpausenum++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->meshpausenum > 0)
                            {
                                parg->meshpausenum--;
                            }
                        }
                    }
                    else
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Температура:(%05.01f)%S ", parg->tempfloat, &simv_blink[show_sim][0]);
                        lcdprint(3, 0, parg->row, 20);

                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 0;
                            dataprocess.dataproc.mash[parg->meshpausenum].temperature_pan2 = parg->tempfloat;
                            save_data_process();
                            parg->meshselectmenu = 0;
                            parg->meshelectparam = 0;
                        }
                        if ((getkeyup() == 1) || (waiter_speedup > 199))
                        {
                            if (parg->tempfloat < MAXTEMPPAN2)
                            {
                                if (waiter_speedup > 199)
                                {
                                    parg->tempfloat += 1.0;
                                }
                                else
                                {
                                    parg->tempfloat += 0.1;
                                }
                            }
                        }

                        if ((getkeydown() == 1) || (waiter_speeddown > 199))
                        {
                            if (parg->tempfloat > 0)
                            {
                                if (waiter_speeddown > 199)
                                {
                                    parg->tempfloat -= 1.0;
                                }
                                else
                                {
                                    parg->tempfloat -= 0.1;
                                }
                            }
                        }
                    }

                    break;
                } //3
                case 4:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Ввод темпер.гист рег   ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Пауза:N%02i Тем:%05.01f  ", parg->meshpausenum, dataprocess.dataproc.mash[parg->meshpausenum].temperature_mesh_delta);
                    lcdprint(2, 0, parg->row, 20);

                    if (parg->enterparampress == 0)
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Для уст.Температуры  ");
                        lcdprint(3, 0, parg->row, 20);
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Нажмите ввод         ");
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 1;
                            parg->tempfloat = 0.0;
                        }
                        if (getkeyup())
                        {
                            if (parg->meshpausenum < MAXPAUSMASHING)
                            {
                                parg->meshpausenum++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->meshpausenum > 0)
                            {
                                parg->meshpausenum--;
                            }
                        }
                    }
                    else
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Температура:(%05.01f)%S ", parg->tempfloat, &simv_blink[show_sim][0]);
                        lcdprint(3, 0, parg->row, 20);

                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 0;
                            dataprocess.dataproc.mash[parg->meshpausenum].temperature_mesh_delta = parg->tempfloat;
                            save_data_process();
                            parg->meshselectmenu = 0;
                            parg->meshelectparam = 0;
                        }
                        if ((getkeyup() == 1) || (waiter_speedup > 199))
                        {
                            if (parg->tempfloat < GUSTER_MES_MAX)
                            {
                                if (waiter_speedup > 199)
                                {
                                    parg->tempfloat += 1.0;
                                }
                                else
                                {
                                    parg->tempfloat += 0.1;
                                }
                            }
                        }

                        if ((getkeydown() == 1) || (waiter_speeddown > 199))
                        {
                            if (parg->tempfloat > 0)
                            {
                                if (waiter_speeddown > 199)
                                {
                                    parg->tempfloat -= 1.0;
                                }
                                else
                                {
                                    parg->tempfloat -= 0.1;
                                }
                            }
                        }
                    }

                    break;
                } //4
                case 5:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Ввод темпер.гист пан   ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Пауза:N%02i Тем:%05.01f  ", parg->meshpausenum, dataprocess.dataproc.mash[parg->meshpausenum].temperature_pan_delta);
                    lcdprint(2, 0, parg->row, 20);

                    if (parg->enterparampress == 0)
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Для уст.Температуры  ");
                        lcdprint(3, 0, parg->row, 20);
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Нажмите ввод         ");
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 1;
                            parg->tempfloat = 0.0;
                        }
                        if (getkeyup())
                        {
                            if (parg->meshpausenum < MAXPAUSMASHING)
                            {
                                parg->meshpausenum++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->meshpausenum > 0)
                            {
                                parg->meshpausenum--;
                            }
                        }
                    }
                    else
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Температура:(%05.01f)%S ", parg->tempfloat, &simv_blink[show_sim][0]);
                        lcdprint(3, 0, parg->row, 20);

                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 0;
                            dataprocess.dataproc.mash[parg->meshpausenum].temperature_pan_delta = parg->tempfloat;
                            save_data_process();
                            parg->meshselectmenu = 0;
                            parg->meshelectparam = 0;
                        }
                        if ((getkeyup() == 1) || (waiter_speedup > 199))
                        {
                            if (parg->tempfloat < GUSTER_PAN_MAX)
                            {
                                if (waiter_speedup > 199)
                                {
                                    parg->tempfloat += 1.0;
                                }
                                else
                                {
                                    parg->tempfloat += 0.1;
                                }
                            }
                        }

                        if ((getkeydown() == 1) || (waiter_speeddown > 199))
                        {
                            if (parg->tempfloat > 0)
                            {
                                if (waiter_speeddown > 199)
                                {
                                    parg->tempfloat -= 1.0;
                                }
                                else
                                {
                                    parg->tempfloat -= 0.1;
                                }
                            }
                        }
                    }

                    break;
                } //5
                case 6:
                {
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель1 вкл/выкл    ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Пауза:N%02i Вкл/Выкл %i  ", parg->meshpausenum, dataprocess.dataproc.mash[parg->meshpausenum].induct1_use);
                    lcdprint(2, 0, parg->row, 20);

                    if (parg->enterparampress == 0)
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Для изменения парам. ");
                        lcdprint(3, 0, parg->row, 20);
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Нажмите ввод         ");
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 1;
                            parg->tempfloat = 0.0;
                        }
                        if (getkeyup())
                        {
                            if (parg->meshpausenum < MAXPAUSMASHING)
                            {
                                parg->meshpausenum++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->meshpausenum > 0)
                            {
                                parg->meshpausenum--;
                            }
                        }
                    }
                    else
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Вкл=1/Выкл=0 (%i)   %S ", parg->tempu8t, &simv_blink[show_sim][0]);
                        lcdprint(3, 0, parg->row, 20);

                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 0;
                            dataprocess.dataproc.mash[parg->meshpausenum].induct1_use = parg->tempu8t;
                            save_data_process();
                            parg->meshselectmenu = 0;
                            parg->meshelectparam = 0;
                        }
                        if (getkeyup())
                        {
                            parg->tempu8t = 1;
                        }

                        if (getkeydown())
                        {
                            parg->tempu8t = 0;
                        }
                    }

                    break;
                } //6
                case 7:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель2 вкл/выкл    ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Пауза:N%02i Вкл/Выкл %i  ", parg->meshpausenum, dataprocess.dataproc.mash[parg->meshpausenum].induct2_use);
                    lcdprint(2, 0, parg->row, 20);

                    if (parg->enterparampress == 0)
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Для изменения парам. ");
                        lcdprint(3, 0, parg->row, 20);
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Нажмите ввод         ");
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 1;
                            parg->tempfloat = 0.0;
                        }
                        if (getkeyup())
                        {
                            if (parg->meshpausenum < MAXPAUSMASHING)
                            {
                                parg->meshpausenum++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->meshpausenum > 0)
                            {
                                parg->meshpausenum--;
                            }
                        }
                    }
                    else
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Вкл=1/Выкл=0 (%i)   %S ", parg->tempu8t, &simv_blink[show_sim][0]);
                        lcdprint(3, 0, parg->row, 20);

                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 0;
                            dataprocess.dataproc.mash[parg->meshpausenum].induct2_use = parg->tempu8t;
                            save_data_process();
                            parg->meshselectmenu = 0;
                            parg->meshelectparam = 0;
                        }
                        if (getkeyup())
                        {
                            parg->tempu8t = 1;
                        }

                        if (getkeydown())
                        {
                            parg->tempu8t = 0;
                        }
                    }

                    break;
                } //7

                case 8:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Макс.вых.Мощ. %%     ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Пауза:N%02i Мощ:%03i   ", parg->meshpausenum, dataprocess.dataproc.mash[parg->meshpausenum].maxpwm);
                    lcdprint(2, 0, parg->row, 20);

                    if (parg->enterparampress == 0)
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Для уст.Мощн.Нагрева");
                        lcdprint(3, 0, parg->row, 20);
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Нажмите ввод         ");
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 1;
                            parg->tempfloat = 0.0;
                        }
                        if (getkeyup())
                        {
                            if (parg->meshpausenum < MAXPAUSMASHING)
                            {
                                parg->meshpausenum++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->meshpausenum > 0)
                            {
                                parg->meshpausenum--;
                            }
                        }
                    }
                    else
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Мощность:(%03i)     %S ", parg->tempu8t, &simv_blink[show_sim][0]);
                        lcdprint(3, 0, parg->row, 20);

                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 0;
                            dataprocess.dataproc.mash[parg->meshpausenum].maxpwm = parg->tempu8t;
                            save_data_process();
                            parg->meshselectmenu = 0;
                            parg->meshelectparam = 0;
                        }
                        if ((getkeyup() == 1) || (waiter_speedup > 199))
                        {
                            if (parg->tempu8t < MAXPOWEROUT)
                            {
                                if (waiter_speedup > 199)
                                {
                                    parg->tempu8t += 5;
                                }
                                else
                                {
                                    parg->tempu8t += 1;
                                }
                            }
                        }

                        if ((getkeydown() == 1) || (waiter_speeddown > 199))
                        {
                            if (parg->tempu8t > 0)
                            {
                                if (waiter_speeddown > 199)
                                {
                                    parg->tempu8t -= 5;
                                }
                                else
                                {
                                    parg->tempu8t -= 1;
                                }
                            }
                        }
                        if (parg->tempu8t > MAXPOWEROUT)
                            parg->tempu8t = 0;
                    }

                    break;
                } //8

                case 9:
                {
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Время паузы в мин    ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Пауза:N%02i Врем:%05i ", parg->meshpausenum, dataprocess.dataproc.mash[parg->meshpausenum].time_ms / 1000 / 60);
                    lcdprint(2, 0, parg->row, 20);

                    if (parg->enterparampress == 0)
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Для уст.времени      ");
                        lcdprint(3, 0, parg->row, 20);
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Нажмите ввод         ");
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 1;
                            parg->tempfloat = 0.0;
                            parg->tempu32t = 0;
                        }
                        if (getkeyup())
                        {
                            if (parg->meshpausenum < MAXPAUSMASHING)
                            {
                                parg->meshpausenum++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->meshpausenum > 0)
                            {
                                parg->meshpausenum--;
                            }
                        }
                    }
                    else
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Вр-я паузы:(%05i) %S ", parg->tempu32t / 1000 / 60, &simv_blink[show_sim][0]);
                        lcdprint(3, 0, parg->row, 20);

                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 0;
                            dataprocess.dataproc.mash[parg->meshpausenum].time_ms = parg->tempu32t;
                            save_data_process();
                            parg->meshselectmenu = 0;
                            parg->meshelectparam = 0;
                        }
                        if ((getkeyup() == 1) || (waiter_speedup > 199))
                        {
                            if (parg->tempu32t < MAXTIMEMESH)
                            {
                                if (waiter_speedup > 199)
                                {
                                    parg->tempu32t += 60000;
                                }
                                else
                                {
                                    parg->tempu32t += 60000;
                                }
                            }
                        }

                        if ((getkeydown() == 1) || (waiter_speeddown > 199))
                        {
                            if (parg->tempu32t > 0)
                            {
                                if (waiter_speeddown > 199)
                                {
                                    parg->tempu32t -= 60000;
                                }
                                else
                                {
                                    parg->tempu32t -= 60000;
                                }
                            }
                        }
                        if (parg->tempu32t > MAXTIMEMESH)
                            parg->tempu32t = 0;
                    }

                    break;
                } //9
                case 10:
                {
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Количество исп. пауз ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"В процессе затир.:%02i ", dataprocess.dataproc.mash[0].numbermesh);
                    lcdprint(2, 0, parg->row, 20);

                    if (parg->enterparampress == 0)
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Для изменения пар.   ");
                        lcdprint(3, 0, parg->row, 20);
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Нажмите ввод         ");
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 1;
                            parg->tempu32t = 0;
                            parg->tempfloat = 0.0;
                            parg->tempu8t = 0;
                        }
                        if (getkeyup())
                        {
                            if (parg->meshpausenum < MAXPAUSMASHING)
                            {
                                parg->meshpausenum++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->meshpausenum > 0)
                            {
                                parg->meshpausenum--;
                            }
                        }
                    }
                    else
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Кол-во.темп.Пауз%02i  %S ", parg->tempu8t, &simv_blink[show_sim][0]);
                        lcdprint(3, 0, parg->row, 20);

                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 0;
                            for (int i = 0; i < MAXPAUSMASHING; i++)
                            {
                                dataprocess.dataproc.mash[i].numbermesh = parg->tempu8t;
                            }
                            save_data_process();
                            parg->meshselectmenu = 0;
                            parg->meshelectparam = 0;
                        }
                        if (getkeyup())
                        {
                            if (parg->tempu8t < MAXPAUSMASHING)
                            {
                                parg->tempu8t++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->tempu8t > 0)
                            {
                                parg->tempu8t--;
                            }
                        }
                        if (parg->tempu8t > MAXPAUSMASHING)
                            parg->tempu8t = 0;
                    }

                    break;
                } //10
                case 11:
                {

                    parg->meshselectmenu = 0;
                    parg->meshelectparam = 0;
                    parg->menusett = 0;
                    parg->enterparampress = 0;
                    parg->tempfloat = 0.0;
                    parg->tempu8t = 0;
                    parg->tempu32t = 0;
                    parg->selectmenusett = 0;
                    parg->hopsselectmenu = 0;
                    parg->hopselectparam = 0;
                    parg->submenupid = 0;
                    break;
                }

                default:
                {
                    parg->meshselectmenu = 0;
                    break;
                }
                } //switch
                break;
            } //parametr mesh

            case 4:
            {
                memset(parg->row, 0, SIZESTRBUF);
                swprintf(parg->row, SIZESTRBUF, L"Парам.Кипяч.Сусла   ");
                lcdprint(1, 0, parg->row, 20);
                switch (parg->hopsselectmenu)
                {
                case 0:
                {
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"%S%S", &parametr_hops_select[parg->hopselectparam][0], &simv_blink[show_sim][0]);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Для выбора нажмите  ");
                    lcdprint(3, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"ввод,перемещение +/- ");
                    lcdprint(4, 0, parg->row, 20);
                    if (getkeyup())
                    {
                        if (parg->hopselectparam < 10)
                        {
                            parg->hopselectparam++;
                        }
                    }

                    if (getkeydown())
                    {
                        if (parg->hopselectparam > 0)
                        {
                            parg->hopselectparam--;
                        }
                    }

                    if (getkeyenter())
                    {
                        parg->hopsselectmenu = parg->hopselectparam + 1;
                        parg->hopspausenum = 0;
                        parg->enterparampress = 0;
                    }

                    break;
                } //0
                case 1:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Ввод температуры     ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Хмель:N%02i Тем:%05.01f  ", parg->hopspausenum, dataprocess.dataproc.hops[parg->hopspausenum].temperature_hops);
                    lcdprint(2, 0, parg->row, 20);

                    if (parg->enterparampress == 0)
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Для уст.Температуры  ");
                        lcdprint(3, 0, parg->row, 20);
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Нажмите ввод         ");
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 1;
                            parg->tempfloat = 0.0;
                        }
                        if (getkeyup())
                        {
                            if (parg->hopspausenum < MAXXOPS)
                            {
                                parg->hopspausenum++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->hopspausenum > 0)
                            {
                                parg->hopspausenum--;
                            }
                        }
                    }
                    else
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Температура:(%05.01f)%S ", parg->tempfloat, &simv_blink[show_sim][0]);
                        lcdprint(3, 0, parg->row, 20);

                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 0;
                            dataprocess.dataproc.hops[parg->hopspausenum].temperature_hops = parg->tempfloat;
                            save_data_process();
                            parg->hopsselectmenu = 0;
                            parg->hopselectparam = 0;
                        }
                        if ((getkeyup() == 1) || (waiter_speedup > 199))
                        {
                            if (parg->tempfloat < MAXTEMPHOPS)
                            {
                                if (waiter_speedup > 199)
                                {
                                    parg->tempfloat += 1.0;
                                }
                                else
                                {
                                    parg->tempfloat += 0.1;
                                }
                            }
                        }

                        if ((getkeydown() == 1) || (waiter_speeddown > 199))
                        {
                            if (parg->tempfloat > 0)
                            {
                                if (waiter_speeddown > 199)
                                {
                                    parg->tempfloat -= 1.0;
                                }
                                else
                                {
                                    parg->tempfloat -= 0.1;
                                }
                            }
                        }
                    }

                    break;
                } //1
                case 2:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Ввод темпер.панели1  ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Хмель:N%02i Тем:%05.01f  ", parg->hopspausenum, dataprocess.dataproc.hops[parg->hopspausenum].temperature_pan1);
                    lcdprint(2, 0, parg->row, 20);

                    if (parg->enterparampress == 0)
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Для уст.Температуры  ");
                        lcdprint(3, 0, parg->row, 20);
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Нажмите ввод         ");
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 1;
                            parg->tempfloat = 0.0;
                        }
                        if (getkeyup())
                        {
                            if (parg->hopspausenum < MAXXOPS)
                            {
                                parg->hopspausenum++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->hopspausenum > 0)
                            {
                                parg->hopspausenum--;
                            }
                        }
                    }
                    else
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Температура:(%05.01f)%S ", parg->tempfloat, &simv_blink[show_sim][0]);
                        lcdprint(3, 0, parg->row, 20);

                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 0;
                            dataprocess.dataproc.hops[parg->hopspausenum].temperature_pan1 = parg->tempfloat;
                            save_data_process();
                            parg->hopsselectmenu = 0;
                            parg->hopselectparam = 0;
                        }
                        if ((getkeyup() == 1) || (waiter_speedup > 199))
                        {
                            if (parg->tempfloat < MAXTEMPPAN1)
                            {
                                if (waiter_speedup > 199)
                                {
                                    parg->tempfloat += 1.0;
                                }
                                else
                                {
                                    parg->tempfloat += 0.1;
                                }
                            }
                        }

                        if ((getkeydown() == 1) || (waiter_speeddown > 199))
                        {
                            if (parg->tempfloat > 0)
                            {
                                if (waiter_speeddown > 199)
                                {
                                    parg->tempfloat -= 1.0;
                                }
                                else
                                {
                                    parg->tempfloat -= 0.1;
                                }
                            }
                        }
                    }

                    break;
                } //2
                case 3:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Ввод темпер.панели2  ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Хмель:N%02i Тем:%05.01f  ", parg->hopspausenum, dataprocess.dataproc.hops[parg->hopspausenum].temperature_pan2);
                    lcdprint(2, 0, parg->row, 20);

                    if (parg->enterparampress == 0)
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Для уст.Температуры  ");
                        lcdprint(3, 0, parg->row, 20);
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Нажмите ввод         ");
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 1;
                            parg->tempfloat = 0.0;
                        }
                        if (getkeyup())
                        {
                            if (parg->hopspausenum < MAXXOPS)
                            {
                                parg->hopspausenum++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->hopspausenum > 0)
                            {
                                parg->hopspausenum--;
                            }
                        }
                    }
                    else
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Температура:(%05.01f)%S ", parg->tempfloat, &simv_blink[show_sim][0]);
                        lcdprint(3, 0, parg->row, 20);

                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 0;
                            dataprocess.dataproc.hops[parg->hopspausenum].temperature_pan2 = parg->tempfloat;
                            save_data_process();
                            parg->hopsselectmenu = 0;
                            parg->hopselectparam = 0;
                        }
                        if ((getkeyup() == 1) || (waiter_speedup > 199))
                        {
                            if (parg->tempfloat < MAXTEMPPAN2)
                            {
                                if (waiter_speedup > 199)
                                {
                                    parg->tempfloat += 1.0;
                                }
                                else
                                {
                                    parg->tempfloat += 0.1;
                                }
                            }
                        }

                        if ((getkeydown() == 1) || (waiter_speeddown > 199))
                        {
                            if (parg->tempfloat > 0)
                            {
                                if (waiter_speeddown > 199)
                                {
                                    parg->tempfloat -= 1.0;
                                }
                                else
                                {
                                    parg->tempfloat -= 0.1;
                                }
                            }
                        }
                    }

                    break;
                } //3
                case 4:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Ввод темпер.гист рег   ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Хмель:N%02i Тем:%05.01f  ", parg->hopspausenum, dataprocess.dataproc.hops[parg->hopspausenum].temperature_hops_delta);
                    lcdprint(2, 0, parg->row, 20);

                    if (parg->enterparampress == 0)
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Для уст.Температуры  ");
                        lcdprint(3, 0, parg->row, 20);
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Нажмите ввод         ");
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 1;
                            parg->tempfloat = 0.0;
                        }
                        if (getkeyup())
                        {
                            if (parg->hopspausenum < MAXXOPS)
                            {
                                parg->hopspausenum++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->hopspausenum > 0)
                            {
                                parg->hopspausenum--;
                            }
                        }
                    }
                    else
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Температура:(%05.01f)%S ", parg->tempfloat, &simv_blink[show_sim][0]);
                        lcdprint(3, 0, parg->row, 20);

                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 0;
                            dataprocess.dataproc.hops[parg->hopspausenum].temperature_hops_delta = parg->tempfloat;
                            save_data_process();
                            parg->hopsselectmenu = 0;
                            parg->hopselectparam = 0;
                        }
                        if ((getkeyup() == 1) || (waiter_speedup > 199))
                        {
                            if (parg->tempfloat < GUSTER_MES_MAX)
                            {
                                if (waiter_speedup > 199)
                                {
                                    parg->tempfloat += 1.0;
                                }
                                else
                                {
                                    parg->tempfloat += 0.1;
                                }
                            }
                        }

                        if ((getkeydown() == 1) || (waiter_speeddown > 199))
                        {
                            if (parg->tempfloat > 0)
                            {
                                if (waiter_speeddown > 199)
                                {
                                    parg->tempfloat -= 1.0;
                                }
                                else
                                {
                                    parg->tempfloat -= 0.1;
                                }
                            }
                        }
                    }

                    break;
                } //4
                case 5:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Ввод темпер.гист пан   ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Хмель:N%02i Тем:%05.01f  ", parg->hopspausenum, dataprocess.dataproc.hops[parg->hopspausenum].temperature_pan_delta);
                    lcdprint(2, 0, parg->row, 20);

                    if (parg->enterparampress == 0)
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Для уст.Температуры  ");
                        lcdprint(3, 0, parg->row, 20);
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Нажмите ввод         ");
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 1;
                            parg->tempfloat = 0.0;
                        }
                        if (getkeyup())
                        {
                            if (parg->hopspausenum < MAXXOPS)
                            {
                                parg->hopspausenum++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->hopspausenum > 0)
                            {
                                parg->hopspausenum--;
                            }
                        }
                    }
                    else
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Температура:(%05.01f)%S ", parg->tempfloat, &simv_blink[show_sim][0]);
                        lcdprint(3, 0, parg->row, 20);

                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 0;
                            dataprocess.dataproc.hops[parg->hopspausenum].temperature_pan_delta = parg->tempfloat;
                            save_data_process();
                            parg->hopsselectmenu = 0;
                            parg->hopselectparam = 0;
                        }
                        if ((getkeyup() == 1) || (waiter_speedup > 199))
                        {
                            if (parg->tempfloat < GUSTER_PAN_MAX)
                            {
                                if (waiter_speedup > 199)
                                {
                                    parg->tempfloat += 1.0;
                                }
                                else
                                {
                                    parg->tempfloat += 0.1;
                                }
                            }
                        }

                        if ((getkeydown() == 1) || (waiter_speeddown > 199))
                        {
                            if (parg->tempfloat > 0)
                            {
                                if (waiter_speeddown > 199)
                                {
                                    parg->tempfloat -= 1.0;
                                }
                                else
                                {
                                    parg->tempfloat -= 0.1;
                                }
                            }
                        }
                    }

                    break;
                } //5
                case 6:
                {
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель1 вкл/выкл    ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Хмель:N%02i Вкл/Выкл %i  ", parg->hopspausenum, dataprocess.dataproc.hops[parg->hopspausenum].induct1_use);
                    lcdprint(2, 0, parg->row, 20);

                    if (parg->enterparampress == 0)
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Для изменения парам. ");
                        lcdprint(3, 0, parg->row, 20);
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Нажмите ввод         ");
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 1;
                            parg->tempfloat = 0.0;
                        }
                        if (getkeyup())
                        {
                            if (parg->hopspausenum < MAXXOPS)
                            {
                                parg->hopspausenum++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->hopspausenum > 0)
                            {
                                parg->hopspausenum--;
                            }
                        }
                    }
                    else
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Вкл=1/Выкл=0 (%i)   %S ", parg->tempu8t, &simv_blink[show_sim][0]);
                        lcdprint(3, 0, parg->row, 20);

                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 0;
                            dataprocess.dataproc.hops[parg->hopspausenum].induct1_use = parg->tempu8t;
                            save_data_process();
                            parg->hopsselectmenu = 0;
                            parg->hopselectparam = 0;
                        }
                        if (getkeyup())
                        {
                            parg->tempu8t = 1;
                        }

                        if (getkeydown())
                        {
                            parg->tempu8t = 0;
                        }
                    }

                    break;
                } //6
                case 7:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Панель2 вкл/выкл    ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Хмель:N%02i Вкл/Выкл %i  ", parg->hopspausenum, dataprocess.dataproc.hops[parg->hopspausenum].induct2_use);
                    lcdprint(2, 0, parg->row, 20);

                    if (parg->enterparampress == 0)
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Для изменения парам. ");
                        lcdprint(3, 0, parg->row, 20);
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Нажмите ввод         ");
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 1;
                            parg->tempfloat = 0.0;
                        }
                        if (getkeyup())
                        {
                            if (parg->hopspausenum < MAXXOPS)
                            {
                                parg->hopspausenum++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->hopspausenum > 0)
                            {
                                parg->hopspausenum--;
                            }
                        }
                    }
                    else
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Вкл=1/Выкл=0 (%i)   %S ", parg->tempu8t, &simv_blink[show_sim][0]);
                        lcdprint(3, 0, parg->row, 20);

                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 0;
                            dataprocess.dataproc.hops[parg->hopspausenum].induct2_use = parg->tempu8t;
                            save_data_process();
                            parg->hopsselectmenu = 0;
                            parg->hopselectparam = 0;
                        }
                        if (getkeyup())
                        {
                            parg->tempu8t = 1;
                        }

                        if (getkeydown())
                        {
                            parg->tempu8t = 0;
                        }
                    }

                    break;
                } //7

                case 8:
                {

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Макс.вых.Мощ. %%     ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Хмель:N%02i Мощ:%03i   ", parg->hopspausenum, dataprocess.dataproc.hops[parg->hopspausenum].maxpwm);
                    lcdprint(2, 0, parg->row, 20);

                    if (parg->enterparampress == 0)
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Для уст.Мощн.Нагрева");
                        lcdprint(3, 0, parg->row, 20);
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Нажмите ввод         ");
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 1;
                            parg->tempfloat = 0.0;
                        }
                        if (getkeyup())
                        {
                            if (parg->hopspausenum < MAXXOPS)
                            {
                                parg->hopspausenum++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->hopspausenum > 0)
                            {
                                parg->hopspausenum--;
                            }
                        }
                    }
                    else
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Мощность:(%03i)     %S ", parg->tempu8t, &simv_blink[show_sim][0]);
                        lcdprint(3, 0, parg->row, 20);

                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 0;
                            dataprocess.dataproc.hops[parg->hopspausenum].maxpwm = parg->tempu8t;
                            save_data_process();
                            parg->hopsselectmenu = 0;
                            parg->hopselectparam = 0;
                        }
                        if ((getkeyup() == 1) || (waiter_speedup > 199))
                        {
                            if (parg->tempu8t < MAXPOWEROUT)
                            {
                                if (waiter_speedup > 199)
                                {
                                    parg->tempu8t += 5;
                                }
                                else
                                {
                                    parg->tempu8t += 1;
                                }
                            }
                        }

                        if ((getkeydown() == 1) || (waiter_speeddown > 199))
                        {
                            if (parg->tempu8t > 0)
                            {
                                if (waiter_speeddown > 199)
                                {
                                    parg->tempu8t -= 5;
                                }
                                else
                                {
                                    parg->tempu8t -= 1;
                                }
                            }
                        }
                        if (parg->tempu8t > MAXPOWEROUT)
                            parg->tempu8t = 0;
                    }

                    break;
                } //8

                case 9:
                {
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Время Кипяч. в мин    ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Хмель:N%02i Врем:%05i ", parg->hopspausenum, dataprocess.dataproc.hops[parg->hopspausenum].time_ms / 1000 / 60);
                    lcdprint(2, 0, parg->row, 20);

                    if (parg->enterparampress == 0)
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Для уст.времени      ");
                        lcdprint(3, 0, parg->row, 20);
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Нажмите ввод         ");
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 1;
                            parg->tempfloat = 0.0;
                            parg->tempu32t = 0;
                        }
                        if (getkeyup())
                        {
                            if (parg->hopspausenum < MAXXOPS)
                            {
                                parg->hopspausenum++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->hopspausenum > 0)
                            {
                                parg->hopspausenum--;
                            }
                        }
                    }
                    else
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Вр-я Кипяч:(%05i) %S ", parg->tempu32t / 1000 / 60, &simv_blink[show_sim][0]);
                        lcdprint(3, 0, parg->row, 20);

                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 0;
                            dataprocess.dataproc.hops[parg->hopspausenum].time_ms = parg->tempu32t;
                            save_data_process();
                            parg->hopsselectmenu = 0;
                            parg->hopselectparam = 0;
                        }
                        if ((getkeyup() == 1) || (waiter_speedup > 199))
                        {
                            if (parg->tempu32t < MAXTIMEHOPS)
                            {
                                if (waiter_speedup > 199)
                                {
                                    parg->tempu32t += 60000;
                                }
                                else
                                {
                                    parg->tempu32t += 60000;
                                }
                            }
                        }

                        if ((getkeydown() == 1) || (waiter_speeddown > 199))
                        {
                            if (parg->tempu32t > 0)
                            {
                                if (waiter_speeddown > 199)
                                {
                                    parg->tempu32t -= 60000;
                                }
                                else
                                {
                                    parg->tempu32t -= 60000;
                                }
                            }
                        }
                        if (parg->tempu32t > MAXTIMEHOPS)
                            parg->tempu32t = 0;
                    }

                    break;
                } //9
                case 10:
                {
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Кол-во засып.хмеля  ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"В процессе Кипяч.:%02i ", dataprocess.dataproc.hops[0].numberhops);
                    lcdprint(2, 0, parg->row, 20);

                    if (parg->enterparampress == 0)
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Для изменения пар.   ");
                        lcdprint(3, 0, parg->row, 20);
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Нажмите ввод         ");
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 1;
                            parg->tempu32t = 0;
                            parg->tempfloat = 0.0;
                            parg->tempu8t = 0;
                        }
                        if (getkeyup())
                        {
                            if (parg->hopspausenum < MAXXOPS)
                            {
                                parg->hopspausenum++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->hopspausenum > 0)
                            {
                                parg->hopspausenum--;
                            }
                        }
                    }
                    else
                    {
                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"Кол-во.Зас.Хмел.:%02i%S ", parg->tempu8t, &simv_blink[show_sim][0]);
                        lcdprint(3, 0, parg->row, 20);

                        memset(parg->row, 0, SIZESTRBUF);
                        swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                        lcdprint(4, 0, parg->row, 20);
                        if (getkeyenter())
                        {
                            parg->enterparampress = 0;
                            for (int i = 0; i < MAXXOPS; i++)
                            {
                                dataprocess.dataproc.hops[i].numberhops = parg->tempu8t;
                            }
                            save_data_process();
                            parg->hopsselectmenu = 0;
                            parg->hopselectparam = 0;
                        }
                        if (getkeyup())
                        {
                            if (parg->tempu8t < MAXXOPS)
                            {
                                parg->tempu8t++;
                            }
                        }

                        if (getkeydown())
                        {
                            if (parg->tempu8t > 0)
                            {
                                parg->tempu8t--;
                            }
                        }
                        if (parg->tempu8t > MAXXOPS)
                            parg->tempu8t = 0;
                    }

                    break;
                } //10
                case 11:
                {
                    parg->meshselectmenu = 0;
                    parg->meshelectparam = 0;
                    parg->menusett = 0;
                    parg->enterparampress = 0;
                    parg->tempfloat = 0.0;
                    parg->tempu8t = 0;
                    parg->tempu32t = 0;
                    parg->selectmenusett = 0;
                    parg->hopsselectmenu = 0;
                    parg->hopselectparam = 0;
                    parg->submenupid = 0;
                    break;
                }

                default:
                {
                    parg->hopsselectmenu = 0;
                    break;
                }
                } //switch
                break;
            } //parametr hops
            case 5:
            {
                switch (parg->submenupid)
                {
                case 0:
                {
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Ввод параметров ПИД  ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"%S%S", &parametr_pid_select[parg->meshelectparam][0], &simv_blink[show_sim][0]);
                    lcdprint(2, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Для выбора нажмите  ");
                    lcdprint(3, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"ввод,перемещение +/- ");
                    lcdprint(4, 0, parg->row, 20);
                    if (getkeyup())
                    {
                        if (parg->meshelectparam < 3)
                        {
                            parg->meshelectparam++;
                        }
                    }

                    if (getkeydown())
                    {
                        if (parg->meshelectparam > 0)
                        {
                            parg->meshelectparam--;
                        }
                    }
                    if (getkeyenter())
                    {
                        parg->submenupid = parg->meshelectparam + 1;
                    }

                    break;
                }

                case 1:
                {
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Ввод парамтеров ПИД  ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Коэф.Пропорц:(%05.01f) ", pidparametr.pid_p);
                    lcdprint(2, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"К.Проп.уст:(%05.01f) %S ", parg->tempfloat, &simv_blink[show_sim][0]);
                    lcdprint(3, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                    lcdprint(4, 0, parg->row, 20);
                    if (getkeyenter())
                    {
                        pidparametr.pid_p = parg->tempfloat;
                        parg->enterparampress = 0;
                        save_data_process();
                        parg->submenupid = 0;
                        parg->menusett = 0;
                    }

                    if ((getkeyup() == 1) || (waiter_speedup > 199))
                    {
                        if (parg->tempfloat < MAXPID_P)
                        {
                            if (waiter_speedup > 199)
                            {
                                parg->tempfloat += 1.0;
                            }
                            else
                            {
                                parg->tempfloat += 0.1;
                            }
                        }
                    }

                    if ((getkeydown() == 1) || (waiter_speeddown > 199))
                    {
                        if (parg->tempfloat > 0)
                        {
                            if (waiter_speeddown > 199)
                            {
                                parg->tempfloat -= 1.0;
                            }
                            else
                            {
                                parg->tempfloat -= 0.1;
                            }
                        }
                    }
                    if (parg->tempfloat > MAXPID_P)
                        parg->tempfloat = 0;
                    break;
                } // проп
                case 2:
                {
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Ввод парамтеров ПИД  ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Коэф.Интегр.:(%05.01f) ", pidparametr.pid_i);
                    lcdprint(2, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"К.Инте.уст:(%05.01f) %S ", parg->tempfloat, &simv_blink[show_sim][0]);
                    lcdprint(3, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                    lcdprint(4, 0, parg->row, 20);
                    if (getkeyenter())
                    {
                        pidparametr.pid_i = parg->tempfloat;
                        parg->enterparampress = 0;
                        save_data_process();
                        parg->submenupid = 0;
                        parg->menusett = 0;
                    }

                    if ((getkeyup() == 1) || (waiter_speedup > 199))
                    {
                        if (parg->tempfloat < MAXPID_I)
                        {
                            if (waiter_speedup > 199)
                            {
                                parg->tempfloat += 1.0;
                            }
                            else
                            {
                                parg->tempfloat += 0.1;
                            }
                        }
                    }

                    if ((getkeydown() == 1) || (waiter_speeddown > 199))
                    {
                        if (parg->tempfloat > 0)
                        {
                            if (waiter_speeddown > 199)
                            {
                                parg->tempfloat -= 1.0;
                            }
                            else
                            {
                                parg->tempfloat -= 0.1;
                            }
                        }
                    }
                    if (parg->tempfloat > MAXPID_I)
                        parg->tempfloat = 0;
                    break;
                } //интегр
                case 3:
                {
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Ввод парамтеров ПИД  ");
                    lcdprint(1, 0, parg->row, 20);
                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"Коэф.Дифф.:(%05.01f)  ", pidparametr.pid_d);
                    lcdprint(2, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"К.Дифф.уст:(%05.01f) %S ", parg->tempfloat, &simv_blink[show_sim][0]);
                    lcdprint(3, 0, parg->row, 20);

                    memset(parg->row, 0, SIZESTRBUF);
                    swprintf(parg->row, SIZESTRBUF, L"%S", &progressbarr[animatind()][0]);
                    lcdprint(4, 0, parg->row, 20);
                    if (getkeyenter())
                    {
                        pidparametr.pid_d = parg->tempfloat;
                        parg->enterparampress = 0;
                        save_data_process();
                        parg->submenupid = 0;
                        parg->menusett = 0;
                    }

                    if ((getkeyup() == 1) || (waiter_speedup > 199))
                    {
                        if (parg->tempfloat < MAXPID_D)
                        {
                            if (waiter_speedup > 199)
                            {
                                parg->tempfloat += 1.0;
                            }
                            else
                            {
                                parg->tempfloat += 0.1;
                            }
                        }
                    }

                    if ((getkeydown() == 1) || (waiter_speeddown > 199))
                    {
                        if (parg->tempfloat > 0)
                        {
                            if (waiter_speeddown > 199)
                            {
                                parg->tempfloat -= 1.0;
                            }
                            else
                            {
                                parg->tempfloat -= 0.1;
                            }
                        }
                    }
                    if (parg->tempfloat > MAXPID_D)
                        parg->tempfloat = 0;
                    break;
                } //дифф
                case 4:
                {
                    parg->meshselectmenu = 0;
                    parg->meshelectparam = 0;
                    parg->menusett = 0;
                    parg->enterparampress = 0;
                    parg->tempfloat = 0.0;
                    parg->tempu8t = 0;
                    parg->tempu32t = 0;
                    parg->selectmenusett = 0;
                    parg->hopsselectmenu = 0;
                    parg->hopselectparam = 0;
                    parg->submenupid = 0;

                    break;
                }
                default:
                {
                    parg->submenupid = 0;
                    break;
                }
                }

                break;
            } //parametr pid

            default:
            {
                parg->menusett = 0;
                break;
            }
            }

            if (getkeymenu())
                parg->menunumber--;

            break;
        }

        default:
        {
            parg->menunumber = 0;
            break;
        }
        }

        if (counterblinr < 6)
        {
            counterblinr++;
        }
        else
        {
            counterblinr = 0;
        }

        if (counterblinr < 3)
        {
            show_sim = 0;
        }
        else
        {
            show_sim = 1;
        }
        vTaskSuspendAll();
        free(parg->row);
        xTaskResumeAll();

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
//-----------------------------------------------------------------------------//
uint8_t animatind()
{
    uint8_t te = (uint8_t)((waiter_speedup + waiter_speeddown) / 10);
    if (te > 19)
        te = 19;

    return te;
}
//-----------------------------------------------------------------------------//