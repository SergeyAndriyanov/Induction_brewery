
/*
 *  
 *
 *   Created on: 01 апр. 2021 г.
 *   Author: Sergey Andriyanov
 *   emai:asvlabpost@gmail.com
 *  
 */
void sci_tx(unsigned char cr);
void SCI_Send(unsigned char* SenData, unsigned char num);
void serialinterfacepc(void);
unsigned int crc_chk(unsigned char* data, unsigned char leng);


typedef struct {
	unsigned char adress;
	unsigned int u_rectifler;
	unsigned int i_inductor_1;
	unsigned int i_inductor_2;
	unsigned char temperature_inductor_1;
	unsigned char temperature_inductor_2;
	unsigned char temperature_igbt_1;
	unsigned char temperature_igbt_2;
	unsigned char statusinductor;
	unsigned char inductor1pwm;
	unsigned char inductor2pwm;
	unsigned char factpwm1;
	unsigned char factpwm2;
	unsigned int stepst;

	unsigned int crc;
}TSerialOutDat;


typedef union {
	TSerialOutDat str;
	unsigned char buf[sizeof(TSerialOutDat)];

}TUOutData;

extern TUOutData  serout;