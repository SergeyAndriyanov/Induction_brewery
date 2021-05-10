/*	BASIC INTERRUPT VECTORS TABLE FOR ST7 devices
 *	Copyright (c) 2002-2005 STMicroelectronics
 */

#include "sci.h"
#include "isr.h"

extern void _stext(void);		/* startup routine */ 
extern void RTC_is(void);
extern void SCI_Receive_is(void);
extern void PB_isr_ei2(void);
extern void PB_isr_ei3(void);

@interrupt void NonHandledInterrupt (void)
{
	/* in order to detect unexpected events during development, 
	   it is recommended to set a breakpoint on the following instruction
	*/
		

	return;
}


/* Interrupt vector table, to be linked at the address
   0xFFE0 (in ROM) */ 
volatile void (* const _vectab[])() = {
	NonHandledInterrupt,  		/* 0xFFE0 */
	NonHandledInterrupt,			/* 0xFFE2 */
	NonHandledInterrupt,			/* 0xFFE4 */
	SCI_Receive_is,			/* 0xFFE6 */
	NonHandledInterrupt,			/* 0xFFE8 */
	NonHandledInterrupt,			/* 0xFFEA */
	NonHandledInterrupt,			/* 0xFFEC */
	NonHandledInterrupt,			/* 0xFFEE */
	PB_isr_ei3,			/* 0xFFF0 */
	PB_isr_ei2,			/* 0xFFF2 */
	NonHandledInterrupt,			/* 0xFFF4 */
	NonHandledInterrupt,			/* 0xFFF6 */
	RTC_is,			/* 0xFFF8 */
	NonHandledInterrupt,			/* 0xFFFA */
	NonHandledInterrupt,			/* Trap vector */
	_stext,			/* Reset Vector */
};