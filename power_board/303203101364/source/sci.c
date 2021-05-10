
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


TUOutData  serout;

void sci_tx(unsigned char cr)
{
	SCIDR = cr;
	while (!(SCISR & STATUS_BIT_TC));
}

void SCI_Send(unsigned char* SenData, unsigned char num)
{
	int i = 0;
	for (i = 0; i < num; i++)
	{
		sci_tx(SenData[i]);
	}


}


void serialinterfacepc(void)
{

	unsigned int tempcrc = 0;
	if (flagdatardy == 1)
	{
		tempcrc = temparrayrec[numbitrec - 2];
		tempcrc = tempcrc << 8;
		tempcrc = tempcrc | (temparrayrec[numbitrec - 1] & 0x00ff);
		if (crc_chk(&temparrayrec[0], numbitrec - 2) == tempcrc)
		{
			if (temparrayrec[0] == ADRESS_BOARD)
			{

				serout.str.statusinductor = serout.str.statusinductor & 0xfc;
				serout.str.statusinductor = serout.str.statusinductor | (temparrayrec[1] & 0x03);

				serout.str.inductor1pwm = temparrayrec[2];
				serout.str.inductor2pwm = temparrayrec[3];

				if (serout.str.inductor1pwm > MAXIMUM_PWM_1)serout.str.inductor1pwm = MAXIMUM_PWM_1;
				if (serout.str.inductor2pwm > MAXIMUM_PWM_2)serout.str.inductor2pwm = MAXIMUM_PWM_2;
				if (serout.str.inductor1pwm < MINIMUM_PWM_1)serout.str.inductor1pwm = MINIMUM_PWM_1;
				if (serout.str.inductor2pwm < MINIMUM_PWM_2)serout.str.inductor2pwm = MINIMUM_PWM_2;

				serout.str.crc = crc_chk(serout.buf, sizeof(serout) - 2);
				SCI_Send(serout.buf, sizeof(serout));
			}
		}
		flagdatardy = 0;
	}
}


//temparrayrec[0]==ADRESS_BOARD!crc_chk(temparrayrec, numbitrec-2) == tempcrc


unsigned int crc_chk(unsigned char* data, unsigned char leng)
{
	int j;
	unsigned int reg_crc = 0xFFFF;
	while (leng--)
	{
		reg_crc ^= *data++;
		for (j = 0; j < 8; j++)
		{
			if (reg_crc & 0x01) reg_crc = (reg_crc >> 1) ^ 0xA001;
			else reg_crc = reg_crc >> 1;
		}

	}
	return reg_crc;
}




/*

extern char flagdatardy;
extern char numbitrec;
extern char temparrayrec[MAXARR];
*/

