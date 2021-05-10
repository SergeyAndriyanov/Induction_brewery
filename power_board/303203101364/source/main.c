
/*
 *  
 *
 *   Created on: 01 апр. 2021 г.
 *   Author: Sergey Andriyanov
 *   emai:asvlabpost@gmail.com
 *  
 */
#include "hrdw_init.h"
#include "sci.h"
#include "isr.h"
#include "adc.h"
#include "io72324.h"


unsigned char owertempind1_igbt1 = 0;
unsigned char owertempind2_igbt2 = 0;



unsigned char ind1testend = 0;
unsigned char ind2testend = 0;
unsigned char ind1 = 0;
unsigned char ind2 = 0;
unsigned int time4ms = 0;
unsigned int time12ms = 0;
unsigned int time48ms = 0;
unsigned int time96ms = 0;
unsigned int time384ms = 0;
unsigned int time768ms = 0;
unsigned int time1536ms = 0;
const unsigned char inductorcurent[11] = { 2,2,10,18,30,40,50,65,80,80,80 };

char inductor1_curent = 0;
char pandetectind1 = 0;
char freqdetectind1 = 0;
char detectlowcurent1 = 0;

char pandetectind2 = 0;
char freqdetectind2 = 0;
char inductor2_curent = 0;
char detectlowcurent2 = 0;




char stepgetstatus1 = 0;
char stepgetstatus2 = 0;

void main(void)
{
	float tempf = 0.0;
	unsigned char pwmselect1, pwmselect2;

	port_gpio_init();
	sci_init();
	isr_prior_set();
	rtc_init();
	adc_init();
	pwm_init();
	serout.str.adress = ADRESS_BOARD;

	WDGCR = WATCHDOG_RESET;

	for (;;)
	{

		if ((timecounterrtc - time4ms) > 1)
		{
			time4ms = timecounterrtc;
			WDGCR = WATCHDOG_RESET;

		}//4ms

		if ((timecounterrtc - time12ms) > 6)
		{
			time12ms = timecounterrtc;
			WDGCR = WATCHDOG_RESET;

		}//12ms	 

		if ((timecounterrtc - time48ms) > 24)
		{
			time48ms = timecounterrtc;
			WDGCR = WATCHDOG_RESET;
			serialinterfacepc();
		}//48ms		 
		if ((timecounterrtc - time96ms) > 48)
		{
			time96ms = timecounterrtc;
			WDGCR = WATCHDOG_RESET;
			adc_measure_run();
			pwmselect1 = serout.str.factpwm1 / 10;
			pwmselect2 = serout.str.factpwm2 / 10;

			if (inductor1_curent == 1)
			{
				if (serout.str.factpwm1 < serout.str.inductor1pwm)
				{
					serout.str.factpwm1++;
				}
				else if (serout.str.factpwm1 > serout.str.inductor1pwm) {
					serout.str.factpwm1--;
				}

				INDUCTOR_1_SET_PWM(PROCENT_ONE_IND1 * serout.str.factpwm1);
			}
			else {
				serout.str.factpwm1 = MINIMUM_PWM_1;
				INDUCTOR_1_SET_PWM(PROCENT_ONE_IND1 * serout.str.factpwm1);
			}


			if (inductor2_curent == 1)
			{
				if (serout.str.factpwm2 < serout.str.inductor2pwm)
				{
					serout.str.factpwm2++;
				}
				else if (serout.str.factpwm2 > serout.str.inductor2pwm) {
					serout.str.factpwm2--;
				}

				INDUCTOR_2_SET_PWM(PROCENT_ONE_IND2 * serout.str.factpwm2);
			}
			else {
				serout.str.factpwm2 = MINIMUM_PWM_2;
				INDUCTOR_2_SET_PWM(PROCENT_ONE_IND2 * serout.str.factpwm2);
			}


			if ((serout.str.temperature_inductor_1 > OWERTEMP_INDUCTOR_1) || (serout.str.temperature_igbt_1 > OWERTEMP_IGBT_1))
			{
				owertempind1_igbt1 = 1;
				serout.str.statusinductor = serout.str.statusinductor | INDUCTOR1_OWERTEMP;
			}
			else if ((serout.str.temperature_inductor_1 < (OWERTEMP_INDUCTOR_1 - GISTER_OWERTEMP_INDUCTOR_1)   ) && (serout.str.temperature_igbt_1 < (OWERTEMP_IGBT_1- GISTER_OWERTEMP_IGBT_1 ))) {
				owertempind1_igbt1 = 0;
				serout.str.statusinductor = serout.str.statusinductor & (~INDUCTOR1_OWERTEMP);
			}

			if ((serout.str.temperature_inductor_2 > OWERTEMP_INDUCTOR_2) || (serout.str.temperature_igbt_2 > OWERTEMP_IGBT_2))
			{
				owertempind2_igbt2 = 1;
				serout.str.statusinductor = serout.str.statusinductor | INDUCTOR2_OWERTEMP;
			}
			else if ((serout.str.temperature_inductor_2 < (OWERTEMP_INDUCTOR_2 - GISTER_OWERTEMP_INDUCTOR_2)   ) && (serout.str.temperature_igbt_2 < (OWERTEMP_IGBT_2- GISTER_OWERTEMP_IGBT_2 ))) {
				owertempind2_igbt2 = 0;
				serout.str.statusinductor = serout.str.statusinductor & (~INDUCTOR2_OWERTEMP);
			}


			WDGCR = WATCHDOG_RESET;

		}//96ms		 	 

		if ((timecounterrtc - time384ms) > 192)
		{
			time384ms = timecounterrtc;
			WDGCR = WATCHDOG_RESET;


		}//384ms			 
		if ((timecounterrtc - time768ms) > 384)
		{
			time768ms = timecounterrtc;
			WDGCR = WATCHDOG_RESET;


			//**********************//

			switch (stepgetstatus1)
			{
			case 0: {


				if ((serout.str.statusinductor & INDUCTOR1_ON_STATUS) && (owertempind1_igbt1 == 0))
				{
					INDUCTOR_1_ON;
					INDUCT1_PINSTART_SET_IN;
					stepgetstatus1 = 1;
					if ((freqdetectind1 == 1) && (pandetectind1 == 1) && (inductor1_curent == 1))
					{
						ind1testend = 1;
					}
					else {
						ind1testend = 0;
					}

				}
				else {
					INDUCT1_PINSTART_SET_IN;
					serout.str.factpwm1 = MINIMUM_PWM_1;
					freqdetectind1 = 0;
					pandetectind1 = 0;
					inductor1_curent = 0;
					INDUCTOR_1_OFF;
					stepgetstatus1 = 0;
					ind1testend = 1;
				}


				if (pandetectind1 == 1) {
					serout.str.statusinductor = serout.str.statusinductor | INDUCTOR1_PAN_YES;
				}
				else {
					serout.str.statusinductor = serout.str.statusinductor & (~INDUCTOR1_PAN_YES);
				}

				if (freqdetectind1 == 1) {
					serout.str.statusinductor = serout.str.statusinductor | INDUCTOR1_FREQ_YES;
				}
				else {
					serout.str.statusinductor = serout.str.statusinductor & (~INDUCTOR1_FREQ_YES);
				}


				WDGCR = WATCHDOG_RESET;
				break;
			}//case
			case 1: {
				stepgetstatus1 = 2;

				break;
			}
			case 2: {
				//freq yes pan detect
				if (serout.str.i_inductor_1 < inductorcurent[pwmselect1])
				{
					if (detectlowcurent1 < NUMBER_LOW_CURENT_EVENT)
					{
						detectlowcurent1++;
					}
					else {
						INDUCTOR_1_OFF;
						detectlowcurent1 = 0;
						serout.str.factpwm1 = MINIMUM_PWM_1;
						freqdetectind1 = 0;
						pandetectind1 = 0;
						inductor1_curent = 0;



						if (ind2testend != 2) { stepgetstatus1 = 3; ind1testend = 2; }
						else { stepgetstatus1 = 0; }


					}

				}
				else {
					inductor1_curent = 1;
					freqdetectind1 = 1;
					pandetectind1 = 1;
					detectlowcurent1 = 0;
					stepgetstatus1 = 0;
				}
				WDGCR = WATCHDOG_RESET;
				break;
			}
			case 3: {


				stepgetstatus1 = 4;

				break;
			}
			case 4: {

				INDUCTOR_1_ON;
				stepgetstatus1 = 5;

				break;
			}
			case 5: {
				INDUCT1_PINSTART_SET_OUT;
				INDUCTOR_1_RUN_PULSE_OFF;
				delay_nop(TIMEPULSE1);
				INDUCTOR_1_RUN_PULSE_ON;
				ind1 = 0;
				INDUCT1_PINSTART_SET_INTERRUPT;
				INDUCTOR_1_OFF;
				stepgetstatus1 = 6;
				break;
			}
			case 6: {

				INDUCT1_PINSTART_SET_IN;

				if ((ind1 < MINIMUMNUMBERPULSEDETECTPAN) || (ind1 > NUMBERPULSEDETECTPAN))

				{
					stepgetstatus1 = 0;
					pandetectind1 = 0;
				}
				else {
					stepgetstatus1 = 7;
					pandetectind1 = 1;
				}


				break;
			}
			case 7: {

				INDUCTOR_1_ON;
				stepgetstatus1 = 8;

				break;
			}
			case 8: {
				INDUCT1_PINSTART_SET_OUT;
				INDUCTOR_1_RUN_PULSE_OFF;
				delay_nop(TIMEPULSE1);
				INDUCTOR_1_RUN_PULSE_ON;
				ind1 = 0;
				INDUCT1_PINSTART_SET_INTERRUPT;

				stepgetstatus1 = 9;

				break;
			}
			case 9: {
				INDUCT1_PINSTART_SET_IN;

				if (ind1 < MINPULSEDETECTFREQ)
				{

					freqdetectind1 = 0;
				}
				else {
					freqdetectind1 = 1;
				}

				stepgetstatus1 = 0;
				break;
			}
			default: {
				stepgetstatus1 = 0;
				break;
			}//default			
			}//switch


//**********************//

			switch (stepgetstatus2)
			{
			case 0: {


				if ((serout.str.statusinductor & INDUCTOR2_ON_STATUS) && (owertempind2_igbt2 == 0))
				{
					INDUCTOR_2_ON;
					INDUCT2_PINSTART_SET_IN;
					stepgetstatus2 = 1;
					if ((freqdetectind2 == 1) && (pandetectind2 == 1) && (inductor2_curent == 1))
					{
						ind2testend = 1;
					}
					else {
						ind2testend = 0;
					}

				}
				else {
					INDUCT2_PINSTART_SET_IN;
					serout.str.factpwm2 = MINIMUM_PWM_2;
					freqdetectind2 = 0;
					pandetectind2 = 0;
					inductor2_curent = 0;
					INDUCTOR_2_OFF;
					stepgetstatus2 = 0;
					ind2testend = 1;
				}


				if (pandetectind2 == 1) {
					serout.str.statusinductor = serout.str.statusinductor | INDUCTOR2_PAN_YES;
				}
				else {
					serout.str.statusinductor = serout.str.statusinductor & (~INDUCTOR2_PAN_YES);
				}

				if (freqdetectind2 == 1) {
					serout.str.statusinductor = serout.str.statusinductor | INDUCTOR2_FREQ_YES;
				}
				else {
					serout.str.statusinductor = serout.str.statusinductor & (~INDUCTOR2_FREQ_YES);
				}


				WDGCR = WATCHDOG_RESET;
				break;
			}//case
			case 1: {
				stepgetstatus2 = 2;


				break;
			}
			case 2: {
				//freq yes pan detect
				if (serout.str.i_inductor_2 < inductorcurent[pwmselect2])
				{
					if (detectlowcurent2 < NUMBER_LOW_CURENT_EVENT)
					{
						detectlowcurent2++;
					}
					else {
						INDUCTOR_2_OFF;
						detectlowcurent2 = 0;
						serout.str.factpwm2 = MINIMUM_PWM_2;
						freqdetectind2 = 0;
						pandetectind2 = 0;
						inductor2_curent = 0;

						if (ind1testend != 2) { stepgetstatus2 = 3; ind2testend = 2; }
						else { stepgetstatus2 = 0; }
					}

				}
				else {
					inductor2_curent = 1;
					freqdetectind2 = 1;
					pandetectind2 = 1;
					detectlowcurent2 = 0;
					stepgetstatus2 = 0;
				}
				WDGCR = WATCHDOG_RESET;
				break;
			}
			case 3: {


				stepgetstatus2 = 4;

				break;
			}
			case 4: {

				INDUCTOR_2_ON;
				stepgetstatus2 = 5;

				break;
			}
			case 5: {
				INDUCT2_PINSTART_SET_OUT;
				INDUCTOR_2_RUN_PULSE_OFF;
				delay_nop(TIMEPULSE2);
				INDUCTOR_2_RUN_PULSE_ON;
				ind2 = 0;
				INDUCT2_PINSTART_SET_INTERRUPT;
				INDUCTOR_2_OFF;
				stepgetstatus2 = 6;
				break;
			}
			case 6: {

				INDUCT2_PINSTART_SET_IN;

				if ((ind2 < MINIMUMNUMBERPULSEDETECTPAN) || (ind2 > NUMBERPULSEDETECTPAN))

				{
					stepgetstatus2 = 0;
					pandetectind2 = 0;
				}
				else {
					stepgetstatus2 = 7;
					pandetectind2 = 1;
				}


				break;
			}
			case 7: {

				INDUCTOR_2_ON;
				stepgetstatus2 = 8;

				break;
			}
			case 8: {
				INDUCT2_PINSTART_SET_OUT;
				INDUCTOR_2_RUN_PULSE_OFF;
				delay_nop(TIMEPULSE2);
				INDUCTOR_2_RUN_PULSE_ON;
				ind2 = 0;
				INDUCT2_PINSTART_SET_INTERRUPT;

				stepgetstatus2 = 9;

				break;
			}
			case 9: {
				INDUCT2_PINSTART_SET_IN;

				if (ind2 < MINPULSEDETECTFREQ)
				{

					freqdetectind2 = 0;
				}
				else {
					freqdetectind2 = 1;
				}

				stepgetstatus2 = 0;
				break;
			}
			default: {
				stepgetstatus2 = 0;
				break;
			}//default			
			}//switch


		}//768ms				 
		if ((timecounterrtc - time1536ms) > 768)
		{
			time1536ms = timecounterrtc;
			WDGCR = WATCHDOG_RESET;


		}//1536ms	


		WDGCR = WATCHDOG_RESET;
	}//for



}//main
