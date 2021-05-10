
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
#include "isr.h"
#include "sci.h"
#include "string.h"
#include "adc.h"


void RTC_is(void);
void SCI_Receive_is(void);
void PB_isr_ei2(void);
void PB_isr_ei3(void);


unsigned char temparrayrec[MAXARR];
unsigned char timereceive = 0;
unsigned char numbitrec = 0;
unsigned char flagprocess = 0;
unsigned char flagdatardy = 0;
unsigned  int timecounterrtc = 0;



@interrupt void RTC_is(void)
{
	unsigned char isr = MCCSR;

	if (timereceive < TIMEOUTRECEIVE)
	{
		timereceive++;
	}
	else {

		if (flagprocess == 1)
		{
			flagprocess = 0;
			flagdatardy = 1;
		}
	}

	timecounterrtc++;

}

@interrupt void SCI_Receive_is(void)
{
	unsigned char isr = SCISR;
	if (isr & STATUS_BIT_RDRF)
	{
		if (flagdatardy != 0)
		{
			isr = SCIDR;
			SCIDR = SCI_BYSY;

		}
		else {
			flagprocess = 1;
			if (timereceive >= TIMEOUTRECEIVE)
			{
				memset(temparrayrec, 0, MAXARR);
				timereceive = 0;
				numbitrec = 1;
				temparrayrec[0] = SCIDR;
			}
			else {
				timereceive = 0;
				temparrayrec[numbitrec] = SCIDR;
				numbitrec++;
			}


		}





	}
}

@interrupt void PB_isr_ei2(void)
{

	if (ind2 < 255)ind2++;
}

@interrupt void PB_isr_ei3(void)
{
	if (ind1 < 255)ind1++;

}










