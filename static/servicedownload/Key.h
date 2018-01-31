#ifndef _KEY_H_
#define _KEY_H_


	typedef enum{
            KEY_1 = 0x00,
            KEY_2,
            KEY_3,
            KEY_4,
			KEY_5,
			KEY_6,
            KEY_7,
            KEY_8,
            KEY_9,
            KEY_10,
			KEY_11,
			KEY_12,
            KEY_13,
            KEY_14,
			KEY_15,
			KEY_16,
        
            KEY_OFF,
            KEY_EDID,
            KEY_LOCK,

			KEY_A_M,
			KEY_LC_P,
            KEY_LC_3_SEC,   //Auto LC模式下，长按LC 3S切换到级联模式

			KEY_LOCK_IN1,
	//		KEY_LOCK_IN2,
			KEY_LOCK_IN3,
//			KEY_LOCK_IN4,
//			KEY_LOCK_IN5,
			KEY_LOCK_IN6,	
            KEY_LOCK_IN7,        
            KEY_LOCK_IN8,		
            KEY_LOCK_IN14,       
            KEY_LOCK_IN15,		
            KEY_LOCK_IN16,
			KEY_LOCK_3_SEC,
			KEY_EDID_3_SEC,

			KEY_10_SEC,
			KEY_OFFLOCK,
//            KEY_EDIDLOCK,

            KEY_OFF_IN3_3SEC,
        
			KEY_IN3IN4,
            KEY_IN3IN4_3_SEC,
//			KEY_IN5IN6,
			KEY_OFF_IN1,		
			KEY_OFF_IN2,
			KEY_OFF_IN3,
			KEY_OFF_IN4,
			KEY_OFF_IN5,
			KEY_OFF_IN6,
			KEY_NONE,
			KEY_VACANCY
    }T_KEY_id;
	
	typedef enum{
			KLINK_KEY_UP,
			KLINK_KEY_DOWN,
			KLINK_KEY_NONE
	}T_KLINK_KEY;

	#define LED_OFF  	0
	#define LED_ON   	1
	#define LED_BLINK 	3	
	#define LED_DIM		2
    
    //next defined for USB    
        /* LED Definitions */
    #define LED_NUM     8               /* Number of user LEDs */

    #define LED_MSK         0x00000000  /* P1.28, P1.29, P1.31, P2.2..6 */
    #define LED_RD          0x00000000  /* LED0 */
    #define LED_WR          0x00000000  /* LED1 */
    #define LED_CFG         0x00000000  /* LED2 */
    #define LED_SUSP        0x00000000  /* LED3 */

    #define LED_On(a) 	;
    #define LED_Off(a)  ;	
    //end for usb
    
//	#define LIGUO_A     2
//	#define LIGUO_B     3
//	#define KRM2000     0
//	#define KRM3000     1
	
	#define KEY3S_COUNT_BASE 60
	#define KEY5S_COUNT_BASE 25000

	#define TIMER_BASE     7//5  //ms
	#define TIMER_DIFF     10  //ms/s
	

	#define LED_BIT0_1		(LPC_GPIO2->FIOSET |= 1UL<<12)
	#define LED_BIT0_0		(LPC_GPIO2->FIOCLR |= 1UL<<12)
	#define LED_BIT1_1		(LPC_GPIO2->FIOSET |= 1UL<<13)
	#define LED_BIT1_0		(LPC_GPIO2->FIOCLR |= 1UL<<13)
	#define LED_BIT2_1		(LPC_GPIO1->FIOSET |= 1UL<<29)
	#define LED_BIT2_0		(LPC_GPIO1->FIOCLR |= 1UL<<29)
    #define LED_BIT3_1		(LPC_GPIO1->FIOSET |= 1UL<<28)
	#define LED_BIT3_0		(LPC_GPIO1->FIOCLR |= 1UL<<28)
    #define LED_BIT4_1		(LPC_GPIO1->FIOSET |= 1UL<<27)
	#define LED_BIT4_0		(LPC_GPIO1->FIOCLR |= 1UL<<27)
	
	#define LED_SEG0_1		(LPC_GPIO1->FIOSET |= 1UL<<26)
	#define LED_SEG0_0		(LPC_GPIO1->FIOCLR |= 1UL<<26)
	#define LED_SEG1_1		(LPC_GPIO1->FIOSET |= 1UL<<25)
	#define LED_SEG1_0		(LPC_GPIO1->FIOCLR |= 1UL<<25)
	#define LED_SEG2_1		(LPC_GPIO1->FIOSET |= 1UL<<24)
	#define LED_SEG2_0		(LPC_GPIO1->FIOCLR |= 1UL<<24)
	#define LED_SEG3_1		(LPC_GPIO1->FIOSET |= 1UL<<23)
	#define LED_SEG3_0		(LPC_GPIO1->FIOCLR |= 1UL<<23)
	#define LED_SEG4_1		(LPC_GPIO1->FIOSET |= 1UL<<22)
	#define LED_SEG4_0		(LPC_GPIO1->FIOCLR |= 1UL<<22)
	#define LED_SEG5_1		(LPC_GPIO1->FIOSET |= 1UL<<21)
	#define LED_SEG5_0		(LPC_GPIO1->FIOCLR |= 1UL<<21)
	#define LED_SEG6_1		(LPC_GPIO1->FIOSET |= 1UL<<20)
	#define LED_SEG6_0		(LPC_GPIO1->FIOCLR |= 1UL<<20)
	#define LED_SEG7_1		(LPC_GPIO1->FIOSET |= 1UL<<19)
	#define LED_SEG7_0		(LPC_GPIO1->FIOCLR |= 1UL<<19)

	
	#define DEFAULT_DIM_COUNT		6
	#define DEFAULT_BLINK_COUNT		200
	/******************************************************************************
	**键盘结构不一样，扫描的算法也会不一样
	*********************************************************************************/
	void SetLEDIN1(unsigned char ucState);
	void SetLEDIN2(unsigned char ucState);
	void SetLEDIN3(unsigned char ucState);
	void SetLEDIN4(unsigned char ucState);
	void SetLEDIN5(unsigned char ucState);
	void SetLEDIN6(unsigned char ucState);	
    void SetLEDIN7(unsigned char ucState);
	void SetLEDIN8(unsigned char ucState);
	void SetLEDIN9(unsigned char ucState);
	void SetLEDIN10(unsigned char ucState);
	void SetLEDIN11(unsigned char ucState);
	void SetLEDIN12(unsigned char ucState);	
    void SetLEDIN13(unsigned char ucState);
	void SetLEDIN14(unsigned char ucState);
	void SetLEDIN15(unsigned char ucState);
	void SetLEDIN16(unsigned char ucState);	

	void SetLEDEDID(unsigned char ucState);		
	void SetLEDRUN(unsigned char ucState);
	void SetLEDUART(unsigned char ucState);
	void SetLEDA_M(unsigned char ucState);
	void SetLEDL_P(unsigned char ucState);
	void SetLEDLOCK(unsigned char ucState);
	void SetLEDOFF(unsigned char ucState);


	#define SetAllLED(STATUS)    SetLEDOFF(STATUS);SetLEDLOCK(STATUS);SetLEDA_M(STATUS);SetLEDL_P(STATUS);SetLEDEDID(STATUS);\
                                 SetLEDIN1(STATUS);SetLEDIN2(STATUS);SetLEDIN3(STATUS);SetLEDIN4(STATUS);SetLEDIN5(STATUS);SetLEDIN6(STATUS);\
                                 SetLEDIN7(STATUS);SetLEDIN8(STATUS);SetLEDIN9(STATUS);SetLEDIN10(STATUS);SetLEDIN11(STATUS);SetLEDIN12(STATUS);\
                                 SetLEDIN13(STATUS);SetLEDIN14(STATUS);SetLEDIN15(STATUS);SetLEDIN16(STATUS);
									
	#define SetAllINLED(STATUS)  SetLEDIN1(STATUS);SetLEDIN2(STATUS);SetLEDIN3(STATUS);SetLEDIN4(STATUS);SetLEDIN5(STATUS);SetLEDIN6(STATUS);\
                                 SetLEDIN7(STATUS);SetLEDIN8(STATUS);SetLEDIN9(STATUS);SetLEDIN10(STATUS);SetLEDIN11(STATUS);SetLEDIN12(STATUS);\
                                 SetLEDIN13(STATUS);SetLEDIN14(STATUS);SetLEDIN15(STATUS);SetLEDIN16(STATUS);
									


				
	T_KEY_id KB_ScanKey(void);
	
//	void InitGPIOKEY(void);

	void ScanLed(void);

	void UIFsm(void);
	void UICgh(void);
	void KB_ScanRemoteSwitch(void);
	void KB_ScanUartKey(void);
	void PowerOnSet(void);

	void DipUIFsm(void);
#endif
