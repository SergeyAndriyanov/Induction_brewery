   1                     ; C Compiler for ST7 (COSMIC Software)
   2                     ; Generator V4.5.10 - 16 Dec 2010
 977                     ; 4 void port_gpio_init(void)
 977                     ; 5 {
 978                     	switch	.text
 979  0000               _port_gpio_init:
 982                     ; 10 }
 985  0000 81            	ret
1011                     ; 14 void sci_init(void)
1011                     ; 15 {
1012                     	switch	.text
1013  0001               _sci_init:
1016                     ; 17 	SCIERPR=0;
1018  0001 3f55          	clr	_SCIERPR
1019                     ; 18 	SCIETPR=0;
1021  0003 3f57          	clr	_SCIETPR
1022                     ; 19 	SCIBRR=(BR_SCP_13|BR_SCT_4|BR_SCR_4);
1024  0005 a6d2          	ld	a,#210
1025  0007 b752          	ld	_SCIBRR,a
1026                     ; 20 	SCICR2=(CNTR2_BIT_TE |CNTR2_BIT_RE);
1028  0009 a60c          	ld	a,#12
1029  000b b754          	ld	_SCICR2,a
1030                     ; 25 }
1033  000d 81            	ret
1056                     ; 28 void timer_init(void)
1056                     ; 29 {
1057                     	switch	.text
1058  000e               _timer_init:
1061                     ; 31 	MCCSR  |= (MCCSR_OIE);
1063  000e 122c          	bset	_MCCSR,#1
1064                     ; 35 }
1067  0010 81            	ret
1090                     ; 38 void isr_prior_set(void)
1090                     ; 39 {	
1091                     	switch	.text
1092  0011               _isr_prior_set:
1095                     ; 40 		ITSPR0 &= (MCC_ISR_PRIOR_2);
1097  0011 b624          	ld	a,_ITSPR0
1098  0013 a4f3          	and	a,#243
1099  0015 b724          	ld	_ITSPR0,a
1100                     ; 41 }
1103  0017 81            	ret
1115                     	xdef	_isr_prior_set
1116                     	xdef	_timer_init
1117                     	xdef	_sci_init
1118                     	xdef	_port_gpio_init
1137                     	end
