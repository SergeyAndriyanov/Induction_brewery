
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
#include "sci.h"
#include "isr.h"
#include "string.h"
#include "adc.h"
#include "math.h"



unsigned int selectchtempigbt = 0;
void adc_measure_run(void)
{
	unsigned char temp = 0;
	int tempi = 0;
	float tempf = 0.0;
	unsigned int currenttemp = 0;
	sim();

	ADCCSR = ADC_ON | ADC_VOLT_CH;
	while (!(ADCCSR & ADC_EOC));
	temp = ADCDRH;
	tempf = temp * ADC_VOLT_MUL_KOEFF;
	serout.str.u_rectifler = (int)tempf;

	ADCCSR = ADC_ON | ADC_CURRENT_INDUCT1_CH;
	while (!(ADCCSR & ADC_EOC));
	currenttemp = ADCDRL;
	currenttemp = currenttemp | (ADCDRH << 2);
	tempf = (currenttemp * ADC_I_IND1_MUL_KOEFF) * 10;
	serout.str.i_inductor_1 = (int)tempf;

	ADCCSR = ADC_ON | ADC_CURRENT_INDUCT2_CH;
	while (!(ADCCSR & ADC_EOC));
	currenttemp = ADCDRL;
	currenttemp = currenttemp | (ADCDRH << 2);
	tempf = (currenttemp * ADC_I_IND2_MUL_KOEFF) * 10;
	serout.str.i_inductor_2 = (int)tempf;

	ADCCSR = ADC_ON | ADC_TEMPER_INDUCT1_CH;
	while (!(ADCCSR & ADC_EOC));
	temp = ADCDRH;
	serout.str.temperature_inductor_1 = temp;

	ADCCSR = ADC_ON | ADC_TEMPER_INDUCT2_CH;
	while (!(ADCCSR & ADC_EOC));
	temp = ADCDRH;
	serout.str.temperature_inductor_2 = temp;


	if (selectchtempigbt < MAXNUMBERCONVERT_ADCTEMPIGBT)
	{
		selectchtempigbt++;
	}
	else {
		selectchtempigbt = 0;
	}

	if ((selectchtempigbt > 0) && (selectchtempigbt < (MAXNUMBERCONVERT_ADCTEMPIGBT / 2)))
	{
		ADC_RT1_SELECT;
		ADC_RT2_RELEASE;
	}
	else {
		ADC_RT1_RELEASE;
		ADC_RT2_SELECT;
	}

	ADCCSR = ADC_ON | ADC_TEMPER_IGBT_1_2_CH;
	while (!(ADCCSR & ADC_EOC));
	temp = ADCDRH;

	if ((selectchtempigbt > ADC_IGNOR_COM_NOISE) && (selectchtempigbt < (MAXNUMBERCONVERT_ADCTEMPIGBT / 2)))
	{
		serout.str.temperature_igbt_1 = temp;

	}
	else	if ((selectchtempigbt > (MAXNUMBERCONVERT_ADCTEMPIGBT / 2) + ADC_IGNOR_COM_NOISE) && (selectchtempigbt < MAXNUMBERCONVERT_ADCTEMPIGBT)) {

		serout.str.temperature_igbt_2 = temp;
	}


	ADCCSR = 0;
	rim();

}






