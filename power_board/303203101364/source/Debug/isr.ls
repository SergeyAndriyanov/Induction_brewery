   1                     ; C Compiler for ST7 (COSMIC Software)
   2                     ; Generator V4.5.10 - 16 Dec 2010
 950                     	bsct
 951  0000               _time_4ms:
 952  0000 00            	dc.b	0
 953  0001               _time_12ms:
 954  0001 00            	dc.b	0
 955  0002               _time_48ms:
 956  0002 00            	dc.b	0
 957  0003               _time_96ms:
 958  0003 00            	dc.b	0
 959  0004               _time_384ms:
 960  0004 00            	dc.b	0
 961  0005               _time_1536ms:
 962  0005 00            	dc.b	0
 963  0006               _count_4ms:
 964  0006 00000000      	dc.l	0
 965  000a               _count_12ms:
 966  000a 00000000      	dc.l	0
 967  000e               _count_48ms:
 968  000e 00000000      	dc.l	0
 969  0012               _count_96ms:
 970  0012 00000000      	dc.l	0
 971  0016               _count_384ms:
 972  0016 00000000      	dc.l	0
 973  001a               _count_1536ms:
 974  001a 00000000      	dc.l	0
1025                     .const:	section	.text
1026  0000               L6:
1027  0000 00000001      	dc.l	1
1028  0004               L01:
1029  0004 00000005      	dc.l	5
1030  0008               L21:
1031  0008 0000000b      	dc.l	11
1032  000c               L41:
1033  000c 0000002f      	dc.l	47
1034  0010               L61:
1035  0010 000000bf      	dc.l	191
1036  0014               L02:
1037  0014 000002ff      	dc.l	767
1038                     ; 26 @interrupt void RTC_is(void)
1038                     ; 27 {
1039                     	switch	.text
1040  0000               _RTC_is:
1042       00000101      OFST:	set	257
1043  0000 b601          	ld	a,c_x+1
1044  0002 88            	push	a
1045  0003 b600          	ld	a,c_x
1046  0005 88            	push	a
1047  0006 b603          	ld	a,c_lreg+3
1048  0008 88            	push	a
1049  0009 b602          	ld	a,c_lreg+2
1050  000b 88            	push	a
1051  000c b601          	ld	a,c_lreg+1
1052  000e 88            	push	a
1053  000f b600          	ld	a,c_lreg
1054  0011 88            	push	a
1055  0012 88            	push	a
1058                     ; 28 	char isrmcc = MCCSR;
1060  0013 96            	ld	x,s
1061  0014 b62c          	ld	a,_MCCSR
1062  0016 d70101        	ld	(OFST+0,x),a
1063                     ; 29 	if (isrmcc&MCCSR_OIF)
1065  0019 96            	ld	x,s
1066  001a d60101        	ld	a,(OFST+0,x)
1067  001d a501          	bcp	a,#1
1068  001f 2603          	jrne	L22
1069  0021 cc0156        	jp	L555
1070  0024               L22:
1071                     ; 31 	if(time_4ms ==0)
1073  0024 3d00          	tnz	_time_4ms
1074  0026 261f          	jrne	L755
1075                     ; 33 		if(count_4ms<1)
1077  0028 ae06          	ld	x,#_count_4ms
1078  002a cd0000        	call	c_ltor
1080  002d ae00          	ld	x,#high(L6)
1081  002f bf00          	ld	c_x,x
1082  0031 ae00          	ld	x,#low(L6)
1083  0033 cd0000        	call	c_xlcmp
1085  0036 2a09          	jrpl	L165
1086                     ; 35 		   count_4ms++;
1088  0038 ae06          	ld	x,#_count_4ms
1089  003a a601          	ld	a,#1
1090  003c cd0000        	call	c_lgadc
1093  003f 2016          	jra	L565
1094  0041               L165:
1095                     ; 37 			time_4ms = 1;		
1097  0041 a601          	ld	a,#1
1098  0043 b700          	ld	_time_4ms,a
1099  0045 2010          	jra	L565
1100  0047               L755:
1101                     ; 40 		count_4ms=0;
1103  0047 a600          	ld	a,#0
1104  0049 b709          	ld	_count_4ms+3,a
1105  004b a600          	ld	a,#0
1106  004d b708          	ld	_count_4ms+2,a
1107  004f a600          	ld	a,#0
1108  0051 b707          	ld	_count_4ms+1,a
1109  0053 a600          	ld	a,#0
1110  0055 b706          	ld	_count_4ms,a
1111  0057               L565:
1112                     ; 43 	if(time_12ms ==0)
1114  0057 3d01          	tnz	_time_12ms
1115  0059 261f          	jrne	L765
1116                     ; 45 		if(count_12ms<5)
1118  005b ae0a          	ld	x,#_count_12ms
1119  005d cd0000        	call	c_ltor
1121  0060 ae04          	ld	x,#high(L01)
1122  0062 bf00          	ld	c_x,x
1123  0064 ae04          	ld	x,#low(L01)
1124  0066 cd0000        	call	c_xlcmp
1126  0069 2a09          	jrpl	L175
1127                     ; 47 		   count_12ms++;
1129  006b ae0a          	ld	x,#_count_12ms
1130  006d a601          	ld	a,#1
1131  006f cd0000        	call	c_lgadc
1134  0072 2016          	jra	L575
1135  0074               L175:
1136                     ; 49 			time_12ms = 1;
1138  0074 a601          	ld	a,#1
1139  0076 b701          	ld	_time_12ms,a
1140  0078 2010          	jra	L575
1141  007a               L765:
1142                     ; 52 		count_12ms=0;
1144  007a a600          	ld	a,#0
1145  007c b70d          	ld	_count_12ms+3,a
1146  007e a600          	ld	a,#0
1147  0080 b70c          	ld	_count_12ms+2,a
1148  0082 a600          	ld	a,#0
1149  0084 b70b          	ld	_count_12ms+1,a
1150  0086 a600          	ld	a,#0
1151  0088 b70a          	ld	_count_12ms,a
1152  008a               L575:
1153                     ; 56 	if(time_48ms ==0)
1155  008a 3d02          	tnz	_time_48ms
1156  008c 261f          	jrne	L775
1157                     ; 58 		if(count_48ms<11)
1159  008e ae0e          	ld	x,#_count_48ms
1160  0090 cd0000        	call	c_ltor
1162  0093 ae08          	ld	x,#high(L21)
1163  0095 bf00          	ld	c_x,x
1164  0097 ae08          	ld	x,#low(L21)
1165  0099 cd0000        	call	c_xlcmp
1167  009c 2a09          	jrpl	L106
1168                     ; 60 		   count_48ms++;
1170  009e ae0e          	ld	x,#_count_48ms
1171  00a0 a601          	ld	a,#1
1172  00a2 cd0000        	call	c_lgadc
1175  00a5 2016          	jra	L506
1176  00a7               L106:
1177                     ; 62 			time_48ms = 1;
1179  00a7 a601          	ld	a,#1
1180  00a9 b702          	ld	_time_48ms,a
1181  00ab 2010          	jra	L506
1182  00ad               L775:
1183                     ; 65 		count_48ms=0;
1185  00ad a600          	ld	a,#0
1186  00af b711          	ld	_count_48ms+3,a
1187  00b1 a600          	ld	a,#0
1188  00b3 b710          	ld	_count_48ms+2,a
1189  00b5 a600          	ld	a,#0
1190  00b7 b70f          	ld	_count_48ms+1,a
1191  00b9 a600          	ld	a,#0
1192  00bb b70e          	ld	_count_48ms,a
1193  00bd               L506:
1194                     ; 69 	if(time_96ms ==0)
1196  00bd 3d03          	tnz	_time_96ms
1197  00bf 261f          	jrne	L706
1198                     ; 71 		if(count_96ms<47)
1200  00c1 ae12          	ld	x,#_count_96ms
1201  00c3 cd0000        	call	c_ltor
1203  00c6 ae0c          	ld	x,#high(L41)
1204  00c8 bf00          	ld	c_x,x
1205  00ca ae0c          	ld	x,#low(L41)
1206  00cc cd0000        	call	c_xlcmp
1208  00cf 2a09          	jrpl	L116
1209                     ; 73 		   count_96ms++;
1211  00d1 ae12          	ld	x,#_count_96ms
1212  00d3 a601          	ld	a,#1
1213  00d5 cd0000        	call	c_lgadc
1216  00d8 2016          	jra	L516
1217  00da               L116:
1218                     ; 75 			time_96ms = 1;
1220  00da a601          	ld	a,#1
1221  00dc b703          	ld	_time_96ms,a
1222  00de 2010          	jra	L516
1223  00e0               L706:
1224                     ; 78 		count_96ms=0;
1226  00e0 a600          	ld	a,#0
1227  00e2 b715          	ld	_count_96ms+3,a
1228  00e4 a600          	ld	a,#0
1229  00e6 b714          	ld	_count_96ms+2,a
1230  00e8 a600          	ld	a,#0
1231  00ea b713          	ld	_count_96ms+1,a
1232  00ec a600          	ld	a,#0
1233  00ee b712          	ld	_count_96ms,a
1234  00f0               L516:
1235                     ; 81 	if(time_384ms ==0)
1237  00f0 3d04          	tnz	_time_384ms
1238  00f2 261f          	jrne	L716
1239                     ; 83 		if(count_384ms<191)
1241  00f4 ae16          	ld	x,#_count_384ms
1242  00f6 cd0000        	call	c_ltor
1244  00f9 ae10          	ld	x,#high(L61)
1245  00fb bf00          	ld	c_x,x
1246  00fd ae10          	ld	x,#low(L61)
1247  00ff cd0000        	call	c_xlcmp
1249  0102 2a09          	jrpl	L126
1250                     ; 85 		   count_384ms++;
1252  0104 ae16          	ld	x,#_count_384ms
1253  0106 a601          	ld	a,#1
1254  0108 cd0000        	call	c_lgadc
1257  010b 2016          	jra	L526
1258  010d               L126:
1259                     ; 87 			time_384ms = 1;
1261  010d a601          	ld	a,#1
1262  010f b704          	ld	_time_384ms,a
1263  0111 2010          	jra	L526
1264  0113               L716:
1265                     ; 90 		count_384ms=0;
1267  0113 a600          	ld	a,#0
1268  0115 b719          	ld	_count_384ms+3,a
1269  0117 a600          	ld	a,#0
1270  0119 b718          	ld	_count_384ms+2,a
1271  011b a600          	ld	a,#0
1272  011d b717          	ld	_count_384ms+1,a
1273  011f a600          	ld	a,#0
1274  0121 b716          	ld	_count_384ms,a
1275  0123               L526:
1276                     ; 94 	if(time_1536ms ==0)
1278  0123 3d05          	tnz	_time_1536ms
1279  0125 261f          	jrne	L726
1280                     ; 96 		if(count_1536ms<767)
1282  0127 ae1a          	ld	x,#_count_1536ms
1283  0129 cd0000        	call	c_ltor
1285  012c ae14          	ld	x,#high(L02)
1286  012e bf00          	ld	c_x,x
1287  0130 ae14          	ld	x,#low(L02)
1288  0132 cd0000        	call	c_xlcmp
1290  0135 2a09          	jrpl	L136
1291                     ; 98 		   count_1536ms++;
1293  0137 ae1a          	ld	x,#_count_1536ms
1294  0139 a601          	ld	a,#1
1295  013b cd0000        	call	c_lgadc
1298  013e 2016          	jra	L555
1299  0140               L136:
1300                     ; 100 			time_1536ms = 1;
1302  0140 a601          	ld	a,#1
1303  0142 b705          	ld	_time_1536ms,a
1304  0144 2010          	jra	L555
1305  0146               L726:
1306                     ; 103 		count_1536ms=0;
1308  0146 a600          	ld	a,#0
1309  0148 b71d          	ld	_count_1536ms+3,a
1310  014a a600          	ld	a,#0
1311  014c b71c          	ld	_count_1536ms+2,a
1312  014e a600          	ld	a,#0
1313  0150 b71b          	ld	_count_1536ms+1,a
1314  0152 a600          	ld	a,#0
1315  0154 b71a          	ld	_count_1536ms,a
1316  0156               L555:
1317                     ; 107 return;
1320  0156 84            	pop	a
1321  0157 84            	pop	a
1322  0158 b700          	ld	c_lreg,a
1323  015a 84            	pop	a
1324  015b b701          	ld	c_lreg+1,a
1325  015d 84            	pop	a
1326  015e b702          	ld	c_lreg+2,a
1327  0160 84            	pop	a
1328  0161 b703          	ld	c_lreg+3,a
1329  0163 84            	pop	a
1330  0164 b700          	ld	c_x,a
1331  0166 84            	pop	a
1332  0167 b701          	ld	c_x+1,a
1333  0169 80            	iret
1455                     	xdef	_RTC_is
1456                     	xdef	_count_1536ms
1457                     	xdef	_count_384ms
1458                     	xdef	_count_96ms
1459                     	xdef	_count_48ms
1460                     	xdef	_count_12ms
1461                     	xdef	_count_4ms
1462                     	xdef	_time_1536ms
1463                     	xdef	_time_384ms
1464                     	xdef	_time_96ms
1465                     	xdef	_time_48ms
1466                     	xdef	_time_12ms
1467                     	xdef	_time_4ms
1468                     	xref.b	c_lreg
1469                     	xref.b	c_x
1488                     	xref	c_lgadc
1489                     	xref	c_xlcmp
1490                     	xref	c_ltor
1491                     	end
