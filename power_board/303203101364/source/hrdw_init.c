
/*
 *  
 *
 *   Created on: 01 апр. 2021 г.
 *   Author: Sergey Andriyanov
 *   emai:asvlabpost@gmail.com
 *  
 */
#include "hrdw_init.h"
#include "io72324.h"

void port_gpio_init(void)
{
	PFDDR = (ADC_RT1_PORT | ADC_RT2_PORT | INDUCTOR_2_ONOFF_PORT);
	PFOR = (ADC_RT1_PORT | ADC_RT2_PORT | INDUCTOR_2_ONOFF_PORT);
	ADC_RT1_RELEASE;
	ADC_RT2_RELEASE;

	PCDDR = (INDUCTOR_1_ONOFF_PORT);
	PCOR = (INDUCTOR_1_ONOFF_PORT);
	INDUCTOR_1_OFF;
	INDUCTOR_2_OFF;


	PBOR = (INDUCTOR_1_RUN_PULSE_PORT | INDUCTOR_2_RUN_PULSE_PORT);
	INDUCTOR_1_RUN_PULSE_ON;
	INDUCTOR_2_RUN_PULSE_ON;
	INDUCT1_PINSTART_SET_IN;
	INDUCT2_PINSTART_SET_IN;

}



void sci_init(void)
{

	SCIERPR = 0;
	SCIETPR = 0;
	SCIBRR = (BR_SCP_13 | BR_SCT_4 | BR_SCR_4);
	SCICR2 = (CNTR2_BIT_TE | CNTR2_BIT_RE | CNTR2_BIT_RIE);




}


void rtc_init(void)
{
	sim();
	MCCSR = MCCSR_OIE;
	rim();



}


void isr_prior_set(void)
{

	ITSPR0 = ITSPR0 & MCC_ISR_PRIOR_2;
	ITSPR1 = ITSPR1 & PB_ISR_PRIOR_2EI3;
	ITSPR1 = ITSPR1 & PB_ISR_PRIOR_2EI2;
	ITSPR2 = ITSPR2 & SCI_ISR_PRIOR_1;
	ITSPR3 = 0xff;
	EICR = INTERRUPT_PIN_FALLING;

}


void adc_init(void)
{
	ADCCSR = ADC_ON;

}


void pwm_init(void)
{
	TACR1 = (TIMER_OLVL2);
	TACR2 = (TIMER_OC1E | TIMER_PWM | TIMER_CC0);
	TAOC2R = INDUCTOR_1_PERIOD;

	TBCR1 = (TIMER_OLVL2);
	TBCR2 = (TIMER_OC1E | TIMER_PWM | TIMER_CC0);
	TBOC2R = INDUCTOR_2_PERIOD;


	INDUCTOR_1_SET_PWM(5);
	INDUCTOR_2_SET_PWM(5);

}


void delay_nop(unsigned char t)
{
	volatile char i = 0;
	for (i = 0; i < t; i++)
	{
		_asm("nop");
	}

}


