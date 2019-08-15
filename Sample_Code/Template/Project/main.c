/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2019 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/***********************************************************************************************************/
/* Website: http://www.nuvoton.com                                                                         */
/*  E-Mail : MicroC-8bit@nuvoton.com                                                                       */
/*  Date   : Jan/21/2019                                                                                   */
/***********************************************************************************************************/

/************************************************************************************************************/
/*  File Function: MS51 DEMO project                                                                        */
/************************************************************************************************************/

#include "MS51.h"


#define SYS_CLOCK 			(24000000ul)
#define PWM_FREQ 			(15000ul)

#define LED_REVERSE(x)		(100-x)			// because lED in EVM schematic , need to reverse level

uint8_t 	u8TH0_Tmp=0;
uint8_t 	u8TL0_Tmp=0;

uint8_t 	DUTY_LED = 0;
uint8_t 	FLAG_LED = 1;
uint8_t 	CNT_LED = 0;	

void PWM0_CH0_SetDuty(uint16_t d)
{
    PWM0H = HIBYTE(d);
    PWM0L = LOBYTE(d);

    set_PWMCON0_LOAD;
    set_PWMCON0_PWMRUN;
	
}

void PWM0_CH0_Init(uint16_t uFrequency)
{
	P12_PUSHPULL_MODE;	//Add this to enhance MOS output capability	
    PWM0_P12_OUTPUT_ENABLE;
  
    PWM_IMDEPENDENT_MODE;
    PWM_CLOCK_DIV_16;

/*
	PWM frequency   = Fpwm/((PWMPH,PWMPL)+1) = (24MHz/2)/(PWMPH,PWMPL)+1) = 20KHz
*/	
    PWMPH = HIBYTE((SYS_CLOCK>>4)/uFrequency-1);
    PWMPL = LOBYTE((SYS_CLOCK>>4)/uFrequency-1);

	printf("\r\nPWM:0x%x  ,0x%x\r\n\r\n" , PWMPH,PWMPL);
	
	PWM0_CH0_SetDuty(LED_REVERSE(0));	
}


void Timer0_IRQHandler(void)
{
	static uint16_t LOG_TIMER = 0;
	static uint16_t CNT_TIMER = 0;

	if (P05 == 1)
	{
		if (CNT_LED++ >= 18)
		{		
			CNT_LED = 0;
			PWM0_CH0_SetDuty(LED_REVERSE(DUTY_LED));
//			printf("DUTY : %d\r\n" ,DUTY_LED );
			if (FLAG_LED)
			{
				if ( ++DUTY_LED == 100)
				{
					FLAG_LED = 0;
					DUTY_LED = 100;
				}
			}
			else
			{
				if ( --DUTY_LED == 0)
				{
					FLAG_LED = 1;
					DUTY_LED = 0;
				}			
			}
		}
	}

	if (CNT_TIMER++ >= 1000)
	{		
		CNT_TIMER = 0;
    	printf("IRQ: %d\r\n",LOG_TIMER++);
	}

}

void Timer0_ISR (void) interrupt 1  //interrupt address is 0x000B
{
    TH0 = u8TH0_Tmp;
    TL0 = u8TL0_Tmp;
	TF0 = 0;
	
	Timer0_IRQHandler();
}

void BasicTimer_TIMER0_Init(void)
{
	uint16_t res = 0;

	ENABLE_TIMER0_MODE1;
	
	u8TH0_Tmp = HIBYTE(TIMER_DIV12_VALUE_1ms_FOSC_240000);
	u8TL0_Tmp = LOBYTE(TIMER_DIV12_VALUE_1ms_FOSC_240000); 

    TH0 = u8TH0_Tmp;
    TL0 = u8TL0_Tmp;

    ENABLE_TIMER0_INTERRUPT;                       //enable Timer0 interrupt
    ENABLE_GLOBAL_INTERRUPT;                       //enable interrupts
  
    set_TCON_TR0;                                  //Timer0 run
}

void PinInterrupt_ISR (void) interrupt 7
{
    if(PIF & SET_BIT5)
    {	
//        printf("P0.5 INT occurred\n");
		PIF = 0;	
		DUTY_LED = 0;
		FLAG_LED = 1;
 		CNT_LED = 0;
		PWM0_CH0_SetDuty(LED_REVERSE(DUTY_LED));
    }
	
}

void GPIO_Init(void)
{
    ALL_GPIO_QUASI_MODE;
	P05_PUSHPULL_MODE;

    ENABLE_INT_PORT0;
    ENABLE_BIT5_FALLINGEDGE_TRIG;
    set_EIE_EPI;                            // Enable pin interrupt
    ENABLE_GLOBAL_INTERRUPT;                // global enable bit
	
}

void UART0_Init(void)
{
    UART_Open(SYS_CLOCK,UART0_Timer3,115200);
    ENABLE_UART0_PRINTF;   
}

void SYS_Init(void)
{
    MODIFY_HIRC_24();
}

void main (void) 
{
    SYS_Init();

    UART0_Init();

	GPIO_Init();//P0.5

	Timer0_Delay(SYS_CLOCK,2000,1000);

	PWM0_CH0_Init(PWM_FREQ);	//P1.2 , LED1
			
	BasicTimer_TIMER0_Init();
	

    while(1)
    {

    }


}



