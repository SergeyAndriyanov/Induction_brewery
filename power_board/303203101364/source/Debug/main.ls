   1                     ; C Compiler for ST7 (COSMIC Software)
   2                     ; Generator V4.5.10 - 16 Dec 2010
  49                     ; 6 main()
  49                     ; 7 {
  50                     	switch	.text
  51  0000               _main:
  54                     ; 8 	 port_gpio_init();
  56  0000 cd0000        	call	_port_gpio_init
  58                     ; 9 	 sci_init();
  60  0003 cd0000        	call	_sci_init
  62                     ; 10 	 timer_init();
  64  0006 cd0000        	call	_timer_init
  66                     ; 11 	 isr_prior_set();
  68  0009 cd0000        	call	_isr_prior_set
  70  000c               L12:
  71                     ; 19 		if(time_4ms==1)
  73  000c b600          	ld	a,_time_4ms
  74  000e a101          	cp	a,#1
  75  0010 2614          	jrne	L52
  76                     ; 21 			time_4ms = 0;
  78  0012 3f00          	clr	_time_4ms
  79                     ; 22 			count_4ms = 0;
  81  0014 a600          	ld	a,#0
  82  0016 b703          	ld	_count_4ms+3,a
  83  0018 a600          	ld	a,#0
  84  001a b702          	ld	_count_4ms+2,a
  85  001c a600          	ld	a,#0
  86  001e b701          	ld	_count_4ms+1,a
  87  0020 a600          	ld	a,#0
  88  0022 b700          	ld	_count_4ms,a
  90  0024 20e6          	jra	L12
  91  0026               L52:
  92                     ; 26 		}else if(time_12ms==1)
  94  0026 b600          	ld	a,_time_12ms
  95  0028 a101          	cp	a,#1
  96  002a 2614          	jrne	L13
  97                     ; 28 			time_12ms = 0;
  99  002c 3f00          	clr	_time_12ms
 100                     ; 29 			count_12ms = 0;
 102  002e a600          	ld	a,#0
 103  0030 b703          	ld	_count_12ms+3,a
 104  0032 a600          	ld	a,#0
 105  0034 b702          	ld	_count_12ms+2,a
 106  0036 a600          	ld	a,#0
 107  0038 b701          	ld	_count_12ms+1,a
 108  003a a600          	ld	a,#0
 109  003c b700          	ld	_count_12ms,a
 111  003e 20cc          	jra	L12
 112  0040               L13:
 113                     ; 32 		}else if(time_48ms==1)
 115  0040 b600          	ld	a,_time_48ms
 116  0042 a101          	cp	a,#1
 117  0044 2614          	jrne	L53
 118                     ; 34 			time_48ms = 0;
 120  0046 3f00          	clr	_time_48ms
 121                     ; 35 			count_48ms = 0;
 123  0048 a600          	ld	a,#0
 124  004a b703          	ld	_count_48ms+3,a
 125  004c a600          	ld	a,#0
 126  004e b702          	ld	_count_48ms+2,a
 127  0050 a600          	ld	a,#0
 128  0052 b701          	ld	_count_48ms+1,a
 129  0054 a600          	ld	a,#0
 130  0056 b700          	ld	_count_48ms,a
 132  0058 20b2          	jra	L12
 133  005a               L53:
 134                     ; 40 		}else if(time_96ms==1)
 136  005a b600          	ld	a,_time_96ms
 137  005c a101          	cp	a,#1
 138  005e 2614          	jrne	L14
 139                     ; 42 			time_96ms = 0;
 141  0060 3f00          	clr	_time_96ms
 142                     ; 43 			count_96ms = 0;
 144  0062 a600          	ld	a,#0
 145  0064 b703          	ld	_count_96ms+3,a
 146  0066 a600          	ld	a,#0
 147  0068 b702          	ld	_count_96ms+2,a
 148  006a a600          	ld	a,#0
 149  006c b701          	ld	_count_96ms+1,a
 150  006e a600          	ld	a,#0
 151  0070 b700          	ld	_count_96ms,a
 153  0072 2098          	jra	L12
 154  0074               L14:
 155                     ; 48 		}else	if(time_384ms==1)
 157  0074 b600          	ld	a,_time_384ms
 158  0076 a101          	cp	a,#1
 159  0078 261a          	jrne	L54
 160                     ; 50 			time_384ms = 0;
 162  007a 3f00          	clr	_time_384ms
 163                     ; 51 			count_384ms = 0;
 165  007c a600          	ld	a,#0
 166  007e b703          	ld	_count_384ms+3,a
 167  0080 a600          	ld	a,#0
 168  0082 b702          	ld	_count_384ms+2,a
 169  0084 a600          	ld	a,#0
 170  0086 b701          	ld	_count_384ms+1,a
 171  0088 a600          	ld	a,#0
 172  008a b700          	ld	_count_384ms,a
 173                     ; 54 			sci_tx(25);
 175  008c a619          	ld	a,#25
 176  008e cd0000        	call	_sci_tx
 179  0091 cc000c        	jra	L12
 180  0094               L54:
 181                     ; 56 		}else	if(time_1536ms==1)
 183  0094 b600          	ld	a,_time_1536ms
 184  0096 a101          	cp	a,#1
 185  0098 26f7          	jrne	L12
 186                     ; 58 			time_1536ms = 0;
 188  009a 3f00          	clr	_time_1536ms
 189                     ; 59 			count_1536ms = 0;
 191  009c a600          	ld	a,#0
 192  009e b703          	ld	_count_1536ms+3,a
 193  00a0 a600          	ld	a,#0
 194  00a2 b702          	ld	_count_1536ms+2,a
 195  00a4 a600          	ld	a,#0
 196  00a6 b701          	ld	_count_1536ms+1,a
 197  00a8 a600          	ld	a,#0
 198  00aa b700          	ld	_count_1536ms,a
 199  00ac cc000c        	jp	L12
 211                     	xdef	_main
 212                     	xref.b	_count_1536ms
 213                     	xref.b	_count_384ms
 214                     	xref.b	_count_96ms
 215                     	xref.b	_count_48ms
 216                     	xref.b	_count_12ms
 217                     	xref.b	_count_4ms
 218                     	xref.b	_time_1536ms
 219                     	xref.b	_time_384ms
 220                     	xref.b	_time_96ms
 221                     	xref.b	_time_48ms
 222                     	xref.b	_time_12ms
 223                     	xref.b	_time_4ms
 224                     	xref	_sci_tx
 225                     	xref	_isr_prior_set
 226                     	xref	_timer_init
 227                     	xref	_sci_init
 228                     	xref	_port_gpio_init
 247                     	end
