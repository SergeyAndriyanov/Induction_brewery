   1                     ; C Compiler for ST7 (COSMIC Software)
   2                     ; Generator V4.5.10 - 16 Dec 2010
 988                     ; 6 void sci_tx(unsigned char cr)
 988                     ; 7 {
 989                     	switch	.text
 991                     	xref.b	_sci_tx$L
 992  0000               _sci_tx:
 995                     ; 9 	SCIDR = cr;
 997  0000 b751          	ld	_SCIDR,a
 999  0002               L165:
1000                     ; 12 	while(!(SCISR&STATUS_BIT_TC));	
1002  0002 b650          	ld	a,_SCISR
1003  0004 a540          	bcp	a,#64
1004  0006 27fa          	jreq	L165
1005                     ; 13 	SCISR&(~STATUS_BIT_TC);
1007  0008 b650          	ld	a,_SCISR
1008  000a 5f            	clr	x
1009  000b a4bf          	and	a,#191
1010  000d 88            	push	a
1011  000e 9f            	ld	a,x
1012  000f a4ff          	and	a,#255
1013  0011 97            	ld	x,a
1014  0012 84            	pop	a
1015                     ; 17 }
1018  0013 81            	ret
1030                     	xdef	_sci_tx
1049                     	end
