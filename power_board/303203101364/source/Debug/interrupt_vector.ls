   1                     ; C Compiler for ST7 (COSMIC Software)
   2                     ; Generator V4.5.10 - 16 Dec 2010
  33                     ; 11 @interrupt void NonHandledInterrupt (void)
  33                     ; 12 {
  34                     	switch	.text
  35  0000               _NonHandledInterrupt:
  39                     ; 16 	return;
  42  0000 80            	iret
  44                     .const:	section	.text
  45  0000               __vectab:
  47  0000 0000          	dc.w	_NonHandledInterrupt
  49  0002 0000          	dc.w	_NonHandledInterrupt
  51  0004 0000          	dc.w	_NonHandledInterrupt
  53  0006 0000          	dc.w	_NonHandledInterrupt
  55  0008 0000          	dc.w	_NonHandledInterrupt
  57  000a 0000          	dc.w	_NonHandledInterrupt
  59  000c 0000          	dc.w	_NonHandledInterrupt
  61  000e 0000          	dc.w	_NonHandledInterrupt
  63  0010 0000          	dc.w	_NonHandledInterrupt
  65  0012 0000          	dc.w	_NonHandledInterrupt
  67  0014 0000          	dc.w	_NonHandledInterrupt
  69  0016 0000          	dc.w	_NonHandledInterrupt
  71  0018 0000          	dc.w	_RTC_is
  73  001a 0000          	dc.w	_NonHandledInterrupt
  75  001c 0000          	dc.w	_NonHandledInterrupt
  77  001e 0000          	dc.w	__stext
 102                     	xdef	__vectab
 103                     	xdef	_NonHandledInterrupt
 104                     	xref	_RTC_is
 105                     	xref	__stext
 124                     	end
