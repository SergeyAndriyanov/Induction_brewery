
/*
 *  
 *
 *   Created on: 01 апр. 2021 г.
 *   Author: Sergey Andriyanov
 *   emai:asvlabpost@gmail.com
 *  
 */

void port_gpio_init(void);
void sci_init(void);
void rtc_init(void);
void isr_prior_set(void);
void adc_init(void);
void pwm_init(void);





//--------------------------------------sci//
#define    ADRESS_BOARD         0x01

#define    STATUS_BIT_TDRE      0x80
#define    STATUS_BIT_TC        0x40
#define    STATUS_BIT_RDRF      0x20
#define    STATUS_BIT_IDLE      0x10
#define    STATUS_BIT_OR        0x08
#define    STATUS_BIT_NF        0x04
#define    STATUS_BIT_FE        0x02
#define    STATUS_BIT_PE        0x01


#define    CNTR1_BIT_SCID        0x20
#define    CNTR1_BIT_M           0x10
#define    CNTR1_BIT_WAKE        0x08
#define    CNTR1_BIT_PCE         0x04
#define    CNTR1_BIT_PS          0x02
#define    CNTR1_BIT_PIE         0x01

#define    CNTR2_BIT_TIE         0x80
#define    CNTR2_BIT_TCIE        0x40
#define    CNTR2_BIT_RIE         0x20
#define    CNTR2_BIT_ILIE        0x10
#define    CNTR2_BIT_TE          0x08
#define    CNTR2_BIT_RE          0x04
#define    CNTR2_BIT_RWU         0x02
#define    CNTR2_BIT_SBK         0x01

#define    SCI_ISR_PRIOR_OFF   0xff
#define    SCI_ISR_PRIOR_2     0xcf
#define    SCI_ISR_PRIOR_1     0xdf
#define    SCI_ISR_PRIOR_0     0xef



#define    BR_SCP_1             0x00
#define    BR_SCP_3             0x40
#define    BR_SCP_4             0x80
#define    BR_SCP_13            0xc0

#define    BR_SCT_1             0x00
#define    BR_SCT_2             0x08
#define    BR_SCT_4             0x10
#define    BR_SCT_8             0x18
#define    BR_SCT_16            0x20
#define    BR_SCT_32            0x28
#define    BR_SCT_64            0x30
#define    BR_SCT_128           0x38

#define    BR_SCR_1             0x00
#define    BR_SCR_2             0x01
#define    BR_SCR_4             0x02
#define    BR_SCR_8             0x03
#define    BR_SCR_16            0x04
#define    BR_SCR_32            0x05
#define    BR_SCR_64            0x06
#define    BR_SCR_128           0x07
//--------------------------------------sci//

//--------------------------------------mcc//
#define    MCCSR_MCO            0x80
#define    MCCSR_CP_DIV_2       0x00
#define    MCCSR_CP_DIV_4       0x20
#define    MCCSR_CP_DIV_8       0x40
#define    MCCSR_CP_DIV_16      0x60
#define    MCCSR_SMS            0x10
#define    MCCSR_TB_16000       0x00
#define    MCCSR_TB_32000       0x04
#define    MCCSR_TB_80000       0x08
#define    MCCSR_TB_200000      0x0c
#define    MCCSR_OIE            0x02
#define    MCCSR_OIF            0x01



#define    MCC_ISR_PRIOR_OFF   0x0c
#define    MCC_ISR_PRIOR_2     0xf3
#define    MCC_ISR_PRIOR_1     0xf7
#define    MCC_ISR_PRIOR_0     0xfb
//--------------------------------------mcc//


//--------------------------------------adc//
#define    ADC_EOC                          0x80
#define    ADC_SPEED                        0x40
#define    ADC_ON                           0x20
#define    ADC_MASK_CH                      0x0f

#define    ADC_VOLT_CH                      0x00
#define    ADC_TEMPER_IGBT_1_2_CH           0x0c
#define    ADC_TEMPER_INDUCT1_CH            0x08
#define    ADC_TEMPER_INDUCT2_CH            0x01
#define    ADC_CURRENT_INDUCT1_CH           0x0e
#define    ADC_CURRENT_INDUCT2_CH           0x0f


#define    ADC_RT1_SELECT                  PFDR = PFDR & 0x7f
#define    ADC_RT2_SELECT                  PFDR = PFDR & 0xbf

#define    ADC_RT1_RELEASE                 PFDR = PFDR | 0x80
#define    ADC_RT2_RELEASE                 PFDR = PFDR | 0x40

#define    ADC_RT1_PORT                    0x80
#define    ADC_RT2_PORT                    0x40
//--------------------------------------adc//

//--------------------------------------ind//
#define   INDUCTOR_1_ONOFF_PORT             0x08
#define   INDUCTOR_2_ONOFF_PORT             0x02

#define   INDUCTOR_1_ON                    PCDR = PCDR | 0x08
#define   INDUCTOR_1_OFF                   PCDR = PCDR & 0xf7

#define   INDUCTOR_2_ON                    PFDR = PFDR | 0x02
#define   INDUCTOR_2_OFF                   PFDR = PFDR & 0xfd

#define   INDUCTOR_1_RUN_PULSE_ON          PBDR = PBDR|0x10 
#define   INDUCTOR_1_RUN_PULSE_OFF         PBDR = PBDR&0xEF          
#define   INDUCTOR_2_RUN_PULSE_ON          PBDR = PBDR|0x01 
#define   INDUCTOR_2_RUN_PULSE_OFF         PBDR = PBDR&0xFE  

#define   INDUCTOR_1_RUN_PULSE_PORT        0x10        
#define   INDUCTOR_2_RUN_PULSE_PORT        0x01 


#define   INDUCTOR1_ON_STATUS              0x01
#define   INDUCTOR2_ON_STATUS              0x02

#define   MIN_CURRENT_IND1                 2
#define   MIN_CURRENT_IND2                 2


#define   INDUCTOR1_PAN_YES                0x04
#define   INDUCTOR2_PAN_YES                0x08


#define   INDUCTOR1_FREQ_YES               0x10
#define   INDUCTOR2_FREQ_YES               0x20

#define   INDUCTOR1_OWERTEMP               0x40
#define   INDUCTOR2_OWERTEMP               0x80

#define   PB_ISR_PRIOR_2EI2                0xfc
#define   PB_ISR_PRIOR_2EI3                0xf3


#define   INDUCTOR1_FREQ_MAX_COUNT         0x7f
#define   INDUCTOR2_FREQ_MAX_COUNT         0x7f

#define   INTERRUPT_PIN_FALLING            0x80
#define   INTERRUPT_PIN_RISING             0x40

#define   WATCHDOG_RESET                   0xff

//--------------------------------------ind//

//--------------------------------------pwm//
#define   TIMER_ICIE                      0x80
#define   TIMER_OCIE                      0x40
#define   TIMER_TOIE                      0x20
#define   TIMER_FOLV2                     0x10
#define   TIMER_FOLV1                     0x08
#define   TIMER_OLVL2                     0x04
#define   TIMER_IEDG1                     0x02
#define   TIMER_OLVL1                     0x01


#define   TIMER_OC1E                      0x80
#define   TIMER_OC2E                      0x40
#define   TIMER_OPM                       0x20
#define   TIMER_PWM                       0x10
#define   TIMER_CC1                       0x08
#define   TIMER_CC0                       0x04
#define   TIMER_IEDG2                     0x02
#define   TIMER_EXEDG                     0x01


#define   TIMER_ICF1                      0x80
#define   TIMER_OCF1                      0x40
#define   TIMER_TOF                       0x20
#define   TIMER_ICF2                      0x10
#define   TIMER_OCF2                      0x08
#define   TIMER_TIMD                      0x04

#define   INDUCTOR_1_MAX_PWM              74
#define   INDUCTOR_1_PERIOD               75
#define   INDUCTOR_1_SET_PWM(x)           TAOC1R = x

#define   INDUCTOR_2_MAX_PWM              74
#define   INDUCTOR_2_PERIOD               75
#define   INDUCTOR_2_SET_PWM(x)           TBOC1R = x





//--------------------------------------pwm//

#define   PROCENT_ONE_IND1                    (INDUCTOR_1_PERIOD/100.0)
#define   PROCENT_ONE_IND2                    (INDUCTOR_2_PERIOD/100.0)



#define  INDUCT1_PINSTART_SET_IN      	   PBDDR = ( PBDDR & (~INDUCTOR_1_RUN_PULSE_PORT)); 	PBOR = (PBOR & (~INDUCTOR_1_RUN_PULSE_PORT))  
#define  INDUCT1_PINSTART_SET_OUT      	   PBDDR =   PBDDR | INDUCTOR_1_RUN_PULSE_PORT; 	    PBOR = (PBOR | INDUCTOR_1_RUN_PULSE_PORT)  
#define  INDUCT1_PINSTART_SET_INTERRUPT    PBDDR = ( PBDDR & (~INDUCTOR_1_RUN_PULSE_PORT)); 	PBOR = (PBOR |    INDUCTOR_1_RUN_PULSE_PORT)  

#define  INDUCT2_PINSTART_SET_IN      	   PBDDR = ( PBDDR & (~INDUCTOR_2_RUN_PULSE_PORT)); 	PBOR = (PBOR & (~INDUCTOR_2_RUN_PULSE_PORT))
#define  INDUCT2_PINSTART_SET_OUT      	   PBDDR =   PBDDR | INDUCTOR_2_RUN_PULSE_PORT; 	    PBOR = (PBOR | INDUCTOR_2_RUN_PULSE_PORT)
#define  INDUCT2_PINSTART_SET_INTERRUPT    PBDDR = ( PBDDR & (~INDUCTOR_2_RUN_PULSE_PORT)); 	PBOR = (PBOR |   INDUCTOR_2_RUN_PULSE_PORT)  


#define   NUMBERPULSEDETECTPAN            15
#define   MINIMUMNUMBERPULSEDETECTPAN     1
#define   MINPULSEDETECTFREQ              100
#define   NUMBER_LOW_CURENT_EVENT         3

#define   MINIMUM_PWM_1                   25
#define   MAXIMUM_PWM_1                   100
#define   MINIMUM_PWM_2                   25
#define   MAXIMUM_PWM_2                   100

#define   TIMEPULSE1                      2
#define   TIMEPULSE2                      2


#define               OWERTEMP_INDUCTOR_1              220
#define               OWERTEMP_INDUCTOR_2              220

#define               OWERTEMP_IGBT_1                  100
#define               OWERTEMP_IGBT_2                  100

#define               GISTER_OWERTEMP_INDUCTOR_1              40
#define               GISTER_OWERTEMP_INDUCTOR_2              40

#define               GISTER_OWERTEMP_IGBT_1                  30
#define               GISTER_OWERTEMP_IGBT_2                  30









#define sim()	_asm("sim")	// set interrupt mask
#define rim()	_asm("rim")	// reset interrupt mask



void delay_nop(unsigned char t);





