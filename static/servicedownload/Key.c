/**--------------------------------------------------------------------------------------------------------
** Modified by: 	RCQ            
** Modified date:  	13June2013         
** Version:        	1.0         

*********************************************************************************************************/
#include "lig_types.h"		  
#include "lig_platform.h"

#include "lpc17xx_uart.h"
#include "LPC17xx.h"
#include "LPC17xx_gpio.h"
#include "LPC17xx_pinsel.h"

#include "IR.h"

#include "main.h"
#include "Key.h"
#include "i2c_inf.h" 

#ifdef SUPPORT_USB_COMM
	#include "cdcuser.h"
	#include "usb.h"
	#include "usbhw.h"
#endif

unsigned char twinkle_count =0;
unsigned char ucSelectEDIDState=0;      //0 no action,1 select out 1,2 select out2 3 select default
unsigned short usSelectEDIDCount=0;
static unsigned char ucCpyEDIDCount=0;
static tagAudio oldAudioSelect = AUDIO_DE_Embed;

void ScanLine(unsigned char ucScanPort);
void SetLed(unsigned char ucWhich,unsigned char state);


#define K_SC0	0
#define K_SC1	1
#define K_SC2	2
#define K_SC3	3
#define K_SC4	4
#define K_SC5	5
#define K_SC_NONE	6


//16keyboard bit0,1,2,3,4  p2.12,p2.13,p1.29, p1.28,p1.27,SEG p2.19-p2.26
#define GPIO_LED_BIT0		(1<<12)
#define GPIO_LED_BIT1		(1<<13)
#define GPIO_LED_BIT2		(1<<29)
#define GPIO_LED_BIT3		(1<<27)
#define GPIO_LED_BIT4		(1<<26)


/*-----------------------测试量-----------------------------------*/
unsigned char LED_Data[5]      = {0x00,0x00,0x00,0x00,0x00};
unsigned char LED_Dim[5]      = {0xff,0xff,0xff,0xff,0xff};
unsigned char LED_Blink[5]     = {0x00,0x00,0x00,0x00,0x00};
unsigned char Blink_State[5]     = {0xff,0xff,0xff,0x00,0x00};

unsigned char LED_Count;
unsigned char LED_Line;

unsigned char Temp0=0;


void ScanLine(unsigned char ucScanPort)
{    
		//Xieyh use it
		#define MAX_SCAN_PORT  7
	  const unsigned char EPLD18_ADD_TAB[MAX_SCAN_PORT]={0x01,0x02,0x04,0x08,0x00,0x00,0x00};
	  const unsigned char EPLD19_ADD_TAB[MAX_SCAN_PORT]={0x00,0x00,0x00,0x00,0x01,0x02,0x00};
		
		if(ucScanPort>=MAX_SCAN_PORT)
			  return;
		HAL_Cpld_Write(0x18,EPLD18_ADD_TAB[ucScanPort]);   
		HAL_Cpld_Write(0x19,EPLD19_ADD_TAB[ucScanPort]);
		return;
		//End Xieyh
		
//    if(ucScanPort == K_SC0 )
//    {
//        HAL_Cpld_Write(0x18,0x01);   
//        HAL_Cpld_Write(0x19,0x00);
//    }
//		else if(ucScanPort == K_SC1)
//    {
//				HAL_Cpld_Write(0x18,0x02);   
//        HAL_Cpld_Write(0x19,0x00);
//    }
//		else if(ucScanPort == K_SC2)
//    {
//				HAL_Cpld_Write(0x18,0x04);   
//        HAL_Cpld_Write(0x19,0x00);
//    }
//    else if(ucScanPort == K_SC3)
//    {
//				HAL_Cpld_Write(0x18,0x08);   
//        HAL_Cpld_Write(0x19,0x00);
//    }
//		else if(ucScanPort == K_SC4)
//    {
//				HAL_Cpld_Write(0x18,0x00);   
//        HAL_Cpld_Write(0x19,0x01);
//    }
//    else if(ucScanPort == K_SC5)
//    {
//				HAL_Cpld_Write(0x18,0x00);   
//        HAL_Cpld_Write(0x19,0x02);
//    }
//		else if(ucScanPort == K_SC_NONE)
//    {
//				HAL_Cpld_Write(0x18,0x00);   
//        HAL_Cpld_Write(0x19,0x00);
//    }

}

/***************************************************************************
*函数名称:ScanKey
*函数说明:按键扫描函数
*输    入：无
*输    出：None
******************************************************************************/											  
T_KEY_id ScanKey(void)
{
	static unsigned long ucKey  = 0xffffffff;
	unsigned long K_RD 	 = 0xffffffff;

	ucKey  = 0xffffffff;
	ScanLine(K_SC0);	

	K_RD  =  (HAL_Cpld_Read(0x04)<<4)|HAL_Cpld_Read(0x03);		
	ucKey = (ucKey<<5) | K_RD;
	
	ScanLine(K_SC1);			
	K_RD  =  (HAL_Cpld_Read(0x04)<<4)|HAL_Cpld_Read(0x03);		
	ucKey = (ucKey<<5) | K_RD;

	ScanLine(K_SC2);			
	K_RD  =  (HAL_Cpld_Read(0x04)<<4)|HAL_Cpld_Read(0x03);		
	ucKey = (ucKey<<5) | K_RD;
    
	ScanLine(K_SC3);			
	K_RD  =  (HAL_Cpld_Read(0x04)<<4)|HAL_Cpld_Read(0x03);		
	ucKey = (ucKey<<5) | K_RD;
    
	ScanLine(K_SC4);			
	K_RD  =  (HAL_Cpld_Read(0x04)<<4)|HAL_Cpld_Read(0x03);		
	ucKey = (ucKey<<5) | K_RD;
    
	ScanLine(K_SC5);			
	K_RD  =  (HAL_Cpld_Read(0x04)<<4)|HAL_Cpld_Read(0x03);		
	ucKey = (ucKey<<5) | K_RD;

	ScanLine(K_SC_NONE);
	
	switch(ucKey)
	{
			case 0xFFFFFFFF: return KEY_VACANCY;				
			case 0xFFEF7BDE: return KEY_1;		
			case 0xFFDEF7BD: return KEY_2;		
			case 0xFFBDEF7B: return KEY_3;

			case 0xFF7BDEF7: return KEY_4;		
			case 0xFDFF7BDE: return KEY_5;
			case 0xFBFEF7BD: return KEY_6;
					
			case 0xF7FDEF7B: return KEY_7;		
			case 0xEFFBDEF7: return KEY_8;		
			case 0xFDEFFBDE: return KEY_9;

			case 0xFBDFF7BD: return KEY_10;		
			case 0xF7BFEF7B: return KEY_11;
			case 0xEF7FDEF7: return KEY_12;
					
			case 0xDEFFBDEF: return KEY_13;		
			case 0xFDEF7FDE: return KEY_14;		
			case 0xFBDEFFBD: return KEY_15;

			case 0xF7BDFF7B: return KEY_16;	
			case 0xF7BDEFFB: return KEY_EDID;
				
			
			case 0xDAF6F5AD: return KEY_LOCK_IN6;		
			case 0xD6F5ED6B: return KEY_LOCK_IN7;        
			case 0xCEF3DCE7: return KEY_LOCK_IN8;		
			case 0xDCE77DCE: return KEY_LOCK_IN14;       
			case 0xDAD6FDAD: return KEY_LOCK_IN15;		
			case 0xD6B5FD6B: return KEY_LOCK_IN16;
					
			case 0xDEF7FDEF: return KEY_LOCK;
			case 0xEF7BFEF7: return KEY_OFF;

			case 0xDEF7BFEF: return KEY_A_M;
			case 0xDEF7BDFF: return KEY_LC_P;
			case 0xFF39CE73: return KEY_IN3IN4;

	//		case 0xDEE779CE: return KEY_LOCK_IN1;		//组合键LOCK + IN x
	//		case 0xDED6F5AD: return KEY_LOCK_IN2;
			case 0xDEB5ED6B: return KEY_LOCK_IN3;
	//		case 0xDE73DCE7: return KEY_LOCK_IN4;	   
	//		case 0xDCF779CE: return KEY_LOCK_IN5;


	//		case 0xEF6B7AD6: return KEY_OFF_IN1;
	//		case 0xEF5AF6B5: return KEY_OFF_IN2;
			case 0xEF39EE73: return KEY_OFF_IN3;	
	//		case 0xFF7BFEF7: return KEY_OFF_IN4;
	//		case 0xED7B7AD6: return KEY_OFF_IN5;	 
	//		case 0xEB7AF6B5: return KEY_OFF_IN6;
			case 0xCE73FCE7: return KEY_OFFLOCK;


	//		case 0xffff: return KEY_OFFLOCK;	  //??同一根回读线上，俩键同时按下，回读线被拉高
			default:return KEY_NONE;//Key_None
	} 
} 
  

/***************************************************************************
*函数名称:KB_ScanKey()
*函数说明:按键扫描函数
*输    入：无
*输    出：相应的键值
******************************************************************************/
T_KEY_id KB_ScanKey(void)
{
    static unsigned short usKeyCount = 0;
		static T_KEY_id  ucOldKey = KEY_NONE;
    static unsigned char ucDirthCount=0;
		T_KEY_id ucKeyVal = KEY_NONE;
    T_KEY_id ucVal;    
    
    if (usKeyCount!=0)
        usKeyCount++;
    
		ucKeyVal = ScanKey();
	
		if(ucKeyVal!=ucOldKey)					
		{
					if (ucDirthCount>=5)
					{           
							ucDirthCount=0;
							usKeyCount=1;
							ucVal=ucKeyVal;
							ucOldKey=ucKeyVal;
							return ucVal;
					}else 
							ucDirthCount++;
		}else 
    {
        ucDirthCount=0;
        if (ucKeyVal==KEY_VACANCY)
        {
            if (usKeyCount>=(1000/TIMER_BASE-TIMER_DIFF)*3)
            {
                usKeyCount=0;
                return KEY_10_SEC;
            }        
        }else if (ucKeyVal==KEY_LOCK)
        {
            if (usKeyCount>=(1000/TIMER_BASE-TIMER_DIFF))//*3/2)
            {
                usKeyCount=0;
                return KEY_LOCK_3_SEC;
            }           
        }
        else if (ucKeyVal==KEY_EDID)
        {
            if (usKeyCount>=(1000/TIMER_BASE-TIMER_DIFF))//*2)
            {
                usKeyCount=0;
                return KEY_EDID_3_SEC;
            }           
        }
        else if (ucKeyVal==KEY_LC_P)
        {
            if (usKeyCount>=(1000/TIMER_BASE-TIMER_DIFF))//*2)
            {
                usKeyCount=0;
                return KEY_LC_3_SEC;
            }
        }
         else if (ucKeyVal==KEY_OFF_IN3)
        {
            if (usKeyCount>=(1000/TIMER_BASE-TIMER_DIFF))
            {
                usKeyCount=0;
                return KEY_OFF_IN3_3SEC;
            }
        }
        else if (ucKeyVal==KEY_IN3IN4)
        {
            if (usKeyCount>=(1000/TIMER_BASE-TIMER_DIFF))
            {
                usKeyCount=0;
                return KEY_IN3IN4_3_SEC;
            }
        }
				else 
            usKeyCount=0;
    }
		return KEY_NONE;
}


//这里检测状态，状态变化才去选择通道
void KB_ScanRemoteSwitch(void)				   //远程控制
{
	static unsigned char ucKLinkDirthCount=0;
	static T_KEY_id	ucLastKeyVal = KEY_NONE;
	T_KEY_id ucKLinkKeyVal = KEY_NONE;
	unsigned short usTemp = 0;
    
	usTemp = HAL_Cpld_Read(0x08)<<12;
	usTemp =usTemp |(HAL_Cpld_Read(0x07)<<8);
	usTemp =usTemp|(HAL_Cpld_Read(0x06)<<4);
	usTemp =usTemp|HAL_Cpld_Read(0x05);	
	if(usTemp==0xFFFE)
	   ucKLinkKeyVal = KEY_1;
	else if(usTemp==0xFFFD)
	   ucKLinkKeyVal = KEY_2;
	else if(usTemp==0xFFFB)
	   ucKLinkKeyVal = KEY_3;
	else if(usTemp==0xFFF7)
	   ucKLinkKeyVal = KEY_4;
	else if(usTemp==0xFFEF)
	   ucKLinkKeyVal = KEY_5;
	else if(usTemp==0xFFDF)
	   ucKLinkKeyVal = KEY_6;
	else if(usTemp==0xFFBF)
	   ucKLinkKeyVal = KEY_7;
	else if(usTemp==0xFF7F)
	   ucKLinkKeyVal = KEY_8;
	else if(usTemp==0xFEFF)
	   ucKLinkKeyVal = KEY_9;
	else if(usTemp==0xFDFF)
	   ucKLinkKeyVal = KEY_10;
	else if(usTemp==0xFBFF)
	   ucKLinkKeyVal = KEY_11;
	else if(usTemp==0xF7FF)
	   ucKLinkKeyVal = KEY_12;
	else if(usTemp==0xEFFF)
	   ucKLinkKeyVal = KEY_13;
	else if(usTemp==0xDFFF)
	   ucKLinkKeyVal = KEY_14;
	else if(usTemp==0xBFFF)
	   ucKLinkKeyVal = KEY_15;
	else if(usTemp==0x7FFF)
	   ucKLinkKeyVal = KEY_16;
	else
	   ucKLinkKeyVal = KEY_NONE;

	if(ucKLinkKeyVal!=ucLastKeyVal)
	{
		ucKLinkDirthCount++; 
		if(ucKLinkDirthCount<=2)
			return; //KEY_NONE;
		else
		{
			ucKLinkDirthCount=0;
			ucLastKeyVal=ucKLinkKeyVal;

            if(ucKLinkKeyVal==KEY_NONE)
                return;
            if(MainDev.mDeviceType==DEVICE_IS_16S1)
            {
                MainDev.mNextSelectInPort = get_In_chip_ch(ucKLinkKeyVal)&0x0f;
                MainDev.mNextSelectChip = get_In_chip_ch(ucKLinkKeyVal)>>4;
            }
            else if(MainDev.mDeviceType==DEVICE_IS_10S1)
            {
                if((ucKLinkKeyVal>KEY_6)&&(ucKLinkKeyVal<=KEY_16))
                {
                    MainDev.mNextSelectInPort = get_In_chip_ch(ucKLinkKeyVal-KEY_6)&0x0f;
                    MainDev.mNextSelectChip = get_In_chip_ch(ucKLinkKeyVal-KEY_6)>>4;
                }
            }
            else if(MainDev.mDeviceType==DEVICE_IS_8S1)
            {
                if((ucKLinkKeyVal>KEY_8)&&(ucKLinkKeyVal<=KEY_16))
                {
                    MainDev.mNextSelectInPort = get_In_chip_ch(ucKLinkKeyVal-KEY_8)&0x0f;
                    MainDev.mNextSelectChip = get_In_chip_ch(ucKLinkKeyVal-KEY_8)>>4;
                }
            }
            
            if((MainDev.mNextSelectChip!=MainDev.mRxChipSel)||(MainDev.mNextSelectInPort!=MainDev.mRxPortSel))
                SET_EVENT(EV_CH_SWITCH);
			if(AutoSw.SwitchMode == AUTO_SWITCH_MODE)	   //只有在自动模式下存在覆盖状态
				MainDev.OverrideFlag = 1;

			AutoSw.AutoNoSig10sWait = 0;	//切换操作时，要重新计数
			MainDev.mTxClose=TX_ON;
			SaveNVRAM(EEPROM_SWITCH_STATE);
			SaveNVRAM(EEPROM_OUT_POWER);
			SET_EVENT(EV_SW_KEY);
			UICgh();
		}
	}
}

void KB_ScanUartKey(void)				   //Klink Key Detect
{
	static unsigned short usCount=0;
	static T_KLINK_KEY	ucLastVal = KLINK_KEY_NONE;

	T_KLINK_KEY  ucKlinkVal = KLINK_KEY_NONE;

	if(HAL_Cpld_Read(0x00)&0x01)		//Klink键按下，GPIO拉低	
			ucKlinkVal = KLINK_KEY_UP; 
	else
			ucKlinkVal = KLINK_KEY_DOWN; 
	if(ucKlinkVal!=ucLastVal)
	{
			usCount++;
			if(usCount>=3)
			{
					usCount = 0;
					ucLastVal = ucKlinkVal;
					if(ucKlinkVal==KLINK_KEY_UP)
							return;

					if(MainDev.mUartState==UART_CPU)
					{
							MainDev.mUartState = UART_HBT;	
					}
					else if(MainDev.mUartState==UART_HBT)	
					{
							MainDev.mUartState = UART_CPU;
					}
					SaveNVRAM(EEPROM_UART_STATE);  
			}
	}	
}

  
//灯正极须接高电平，负极需接低电平，即扫描线需置低，由SEGLED置高，形成电压差即可
void ScanLed(void)
{	
	static unsigned char ucLEDDimCount[5] = {0,0,0,0,0 };
	static unsigned char ucLEDBlinkCount = 0;									  //闪烁取反	
	static unsigned char flag_Blink = 1;					  //用于形成闪烁的时间段：300ms

	LED_BIT0_1;
	LED_BIT1_1;
	LED_BIT2_1;
	LED_BIT3_1;
	LED_BIT4_1;
	
	LED_SEG0_0;
	LED_SEG1_0;
	LED_SEG2_0;
	LED_SEG3_0;
	LED_SEG4_0;
	LED_SEG5_0;
	LED_SEG6_0;
	LED_SEG7_0;
	
	LED_Line++;
	if( LED_Line >= 5 )
	{
			LED_Line = 0;
	}

	if( LED_Line == 0 )
	{
			LED_BIT0_0;
			LED_BIT1_1;
			LED_BIT2_1;
			LED_BIT3_1;
			LED_BIT4_1;
	}
	else if( LED_Line == 1 )
	{
			LED_BIT0_1;
			LED_BIT1_0;
			LED_BIT2_1;
			LED_BIT3_1;
			LED_BIT4_1;
	}
	else if( LED_Line == 2 )
	{
			LED_BIT0_1;
			LED_BIT1_1;
			LED_BIT2_0;
			LED_BIT3_1;
			LED_BIT4_1;
	}
	else if( LED_Line == 3 )
	{
			LED_BIT0_1;
			LED_BIT1_1;
			LED_BIT2_1;
			LED_BIT3_0;
			LED_BIT4_1;
	}
	else if( LED_Line == 4 )
	{
			LED_BIT0_1;
			LED_BIT1_1;
			LED_BIT2_1;
			LED_BIT3_1;
			LED_BIT4_0;
	}

   	//设置的占空比,实现亮暗
	ucLEDDimCount[LED_Line] ++;
	if(ucLEDDimCount[LED_Line]>=(DEFAULT_DIM_COUNT))	  //*10)) //在中断MatchValue=10时效果不错
	{										  //*4)) //在中断MatchValue=25时效果不错
			Temp0 = LED_Data[LED_Line];
			ucLEDDimCount[LED_Line]= 0;
	}else
	{	   	   		
			Temp0 = (LED_Dim[LED_Line])&LED_Data[LED_Line];				
	}

	if(ucLEDBlinkCount>=DEFAULT_BLINK_COUNT)
	{
			// Temp0 = Temp0 & LED_Blink_Buf[LED_Line];
			if( flag_Blink == 1 )
			{
					flag_Blink = 0;
					Blink_State[0] = 0xff;
					Blink_State[1] = 0xff;
					Blink_State[2] = 0xff;
					Blink_State[3] = 0xff;
					Blink_State[4] = 0xff;
			}			
			else
			{
					flag_Blink = 1;
					Blink_State[0] = 0xff^LED_Blink[0];
					Blink_State[1] = 0xff^ LED_Blink[1];
					Blink_State[2] = 0xff^ LED_Blink[2];
					Blink_State[3] = 0xff^ LED_Blink[3];
					Blink_State[4] = 0xff^ LED_Blink[4];		
			}
			ucLEDBlinkCount=0;
	}
	else
	{
			ucLEDBlinkCount++;	
	}


	Temp0 = Temp0 & Blink_State[LED_Line];
	if( Temp0&0x01 )
	{
		LED_SEG0_1;
	}
	else
	{
		LED_SEG0_0;
	}

	if( Temp0&0x02 )
	{
		LED_SEG1_1;
	}
	else
	{
		LED_SEG1_0;
	}

	if( Temp0&0x04 )
	{
		LED_SEG2_1;
	}
	else
	{
		LED_SEG2_0;
	}
	if( Temp0&0x08 )
	{
		LED_SEG3_1;
	}
	else
	{
		LED_SEG3_0;
	}
	if( Temp0&0x10 )
	{
		LED_SEG4_1;
	}
	else
	{
		LED_SEG4_0;
	}
	if( Temp0&0x20 )
	{
		LED_SEG5_1;
	}
	else
	{
		LED_SEG5_0;
	}
	if( Temp0&0x40 )
	{
		LED_SEG6_1;
	}
	else
	{
		LED_SEG6_0;
	}
	if( Temp0&0x80 )
	{
		LED_SEG7_1;
	}
	else
	{
		LED_SEG7_0;
	}
}


////////////////////////////////////////////////////////////
void SetLEDOFF(unsigned char ucState)
{
	LED_Dim[4] |=0x01;
	if(ucState==LED_ON)
	{
		LED_Data[4]   |= 0x01;
		LED_Blink[4]	&= (~0x01);
	}		
	else if(ucState==LED_OFF)
	{
		LED_Data[4]   &= (~0x01);
		LED_Blink[4]	&= (~0x01);
	}		
	else if(ucState==LED_BLINK)					//这个程序可以暗闪，但是没有意义，闪定义为亮闪
	{
		LED_Data[4]   |= 0x01;
		LED_Dim[4]    |=0x01;
		LED_Blink[4]	|= 0x01;
	}			
}

void SetLEDLOCK(unsigned char ucState)
{
	LED_Dim[4] |=0x02;
	if(ucState==LED_ON)
	{
		LED_Data[4] |= 0x02;
		LED_Blink[4]	&= (~0x02);
	}		
	else if(ucState==LED_OFF)
	{
		LED_Data[4] &= (~0x02);
		LED_Blink[4]	&= (~0x02);
	}		
	else if(ucState==LED_BLINK)					//这个程序可以暗闪，但是没有意义，闪定义为亮闪
	{
		LED_Data[4] |= 0x02;
		LED_Dim[4] |=0x02;
		LED_Blink[4]	|= 0x02;
	}			
}

void SetLEDA_M(unsigned char ucState)
{
	LED_Dim[4] |=0x04;
	if(ucState==LED_ON)
	{
		LED_Data[4] |= 0x04;
		LED_Blink[4]	&= (~0x04);
	}		
	else if(ucState==LED_OFF)
	{
		LED_Data[4] &= (~0x04);
		LED_Blink[4]	&= (~0x04);
	}		
	else if(ucState==LED_BLINK)					//这个程序可以暗闪，但是没有意义，闪定义为亮闪
	{
		LED_Data[4] |= 0x04;
		LED_Dim[4] |=0x04;
		LED_Blink[4]	|= 0x04;
	}			
}

void SetLEDL_P(unsigned char ucState)
{
	LED_Dim[4] |=0x08;
	if(ucState==LED_ON)
	{
		LED_Data[4] |= 0x08;
		LED_Blink[4]	&= (~0x08);
	}		
	else if(ucState==LED_OFF)
	{
		LED_Data[4] &= (~0x08);
		LED_Blink[4]	&= (~0x08);
	}		
	else if(ucState==LED_BLINK)					//这个程序可以暗闪，但是没有意义，闪定义为亮闪
	{
		LED_Data[4] |= 0x08;
		LED_Dim[4] |=0x08;
		LED_Blink[4]	|= 0x08;
	}			
}

void SetLEDEDID(unsigned char ucState)
{
	if(ucState==LED_ON)
	{
		LED_Data[4] |= 0x20;
		LED_Dim[4] |=0x20;
		LED_Blink[4]	&= (~0x20);
	}		
	else if(ucState==LED_OFF)
		LED_Data[4] &= (~0x20);
	else if(ucState==LED_DIM)	//暗首先得亮，然后才能暗、
	{
		LED_Data[4] |= 0x20;
		LED_Dim[4] &=(~0x20);
		LED_Blink[4]	&= (~0x20);
	}
	else if(ucState==LED_BLINK)					
	{
		LED_Data[4] |= 0x20;
		LED_Dim[4] |=0x20;
		LED_Blink[4]	|= 0x20;
	}			
}

/////////

//
void SetLEDIN1(unsigned char ucState)
{
	if(ucState==LED_ON)
	{
		LED_Data[2] |= 0x01;
		LED_Dim[2] |=0x01;
		LED_Blink[2]	&= (~0x01);
	}		
	else if(ucState==LED_OFF)
		LED_Data[2] &= (~0x01);
	else if(ucState==LED_DIM)	//暗首先得亮，然后才能暗、
	{
		LED_Data[2] |= 0x01;
		LED_Dim[2] &=(~0x01);
		LED_Blink[2]	&= (~0x01);
	}
	else if(ucState==LED_BLINK)					//这个程序可以暗闪，但是没有意义，闪定义为亮闪
	{
		LED_Data[2] |= 0x01;
		LED_Dim[2] |=0x01;
		LED_Blink[2]	|= 0x01;
	}
		
}

void SetLEDIN2(unsigned char ucState)
{
	if(ucState==LED_ON)
	{
		LED_Data[2] |= 0x02;
		LED_Dim[2] |=0x02;
		LED_Blink[2]	&= (~0x02);
	}		
	else if(ucState==LED_OFF)
		LED_Data[2] &= (~0x02);
	else if(ucState==LED_DIM)	//暗首先得亮，然后才能暗、
	{
		LED_Data[2] |= 0x02;
		LED_Dim[2] &=(~0x02);
		LED_Blink[2]	&= (~0x02);
	}
	else if(ucState==LED_BLINK)					//这个程序可以暗闪，但是没有意义，闪定义为亮闪
	{
		LED_Data[2] |= 0x02;
		LED_Dim[2] |=0x02;
		LED_Blink[2]	|= 0x02;
	}
		
}
void SetLEDIN3(unsigned char ucState)
{
	if(ucState==LED_ON)
	{
		LED_Data[2] |= 0x04;
		LED_Dim[2] |=0x04;
		LED_Blink[2]	&= (~0x04);
	}		
	else if(ucState==LED_OFF)
		LED_Data[2] &= (~0x04);
	else if(ucState==LED_DIM)	//暗首先得亮，然后才能暗、
	{
		LED_Data[2] |= 0x04;
		LED_Dim[2] &=(~0x04);
		LED_Blink[2]	&= (~0x04);
	}
	else if(ucState==LED_BLINK)					//这个程序可以暗闪，但是没有意义，闪定义为亮闪
	{
		LED_Data[2] |= 0x04;
		LED_Dim[2] |=0x04;
		LED_Blink[2]	|= 0x04;
	}		
}

void SetLEDIN4(unsigned char ucState)
{
	if(ucState==LED_ON)
	{
		LED_Data[2] |= 0x08;
		LED_Dim[2] |=0x08;
		LED_Blink[2]	&= (~0x08);
	}		
	else if(ucState==LED_OFF)
		LED_Data[2] &= (~0x08);
	else if(ucState==LED_DIM)	//暗首先得亮，然后才能暗、
	{
		LED_Data[2] |= 0x08;
		LED_Dim[2] &=(~0x08);
		LED_Blink[2]	&= (~0x08);
	}
	else if(ucState==LED_BLINK)					//这个程序可以暗闪，但是没有意义，闪定义为亮闪
	{
		LED_Data[2] |= 0x08;
		LED_Dim[2] |=0x08;
		LED_Blink[2]	|= 0x08;
	}		
}


void SetLEDIN5(unsigned char ucState)
{
	if(ucState==LED_ON)
	{
		LED_Data[2] |= 0x10;
		LED_Dim[2] |=0x10;
		LED_Blink[2]	&= (~0x10);
	}		
	else if(ucState==LED_OFF)
		LED_Data[2] &= (~0x10);
	else if(ucState==LED_DIM)	//暗首先得亮，然后才能暗、
	{
		LED_Data[2] |= 0x10;
		LED_Dim[2] &=(~0x10);
		LED_Blink[2]	&= (~0x10);
	}
	else if(ucState==LED_BLINK)					//这个程序可以暗闪，但是没有意义，闪定义为亮闪
	{
		LED_Data[2] |= 0x10;
		LED_Dim[2] |=0x10;
		LED_Blink[2]	|= 0x10;
	}		
}

void SetLEDIN6(unsigned char ucState)
{
	if(ucState==LED_ON)
	{
		LED_Data[2] |= 0x20;
		LED_Dim[2] |=0x20;
		LED_Blink[2]	&= (~0x20);
	}		
	else if(ucState==LED_OFF)
		LED_Data[2] &= (~0x20);
	else if(ucState==LED_DIM)	//暗首先得亮，然后才能暗、
	{
		LED_Data[2] |= 0x20;
		LED_Dim[2] &=(~0x20);
		LED_Blink[2]	&= (~0x20);
	}
	else if(ucState==LED_BLINK)					//这个程序可以暗闪，但是没有意义，闪定义为亮闪
	{
		LED_Data[2] |= 0x20;
		LED_Dim[2] |=0x20;
		LED_Blink[2]	|= 0x20;
	}			
}

void SetLEDIN7(unsigned char ucState)
{
	if(ucState==LED_ON)
	{
		LED_Data[2] |= 0x40;
		LED_Dim[2] |=0x40;
		LED_Blink[2]	&= (~0x40);
	}		
	else if(ucState==LED_OFF)
		LED_Data[2] &= (~0x40);
	else if(ucState==LED_DIM)	//暗首先得亮，然后才能暗、
	{
		LED_Data[2] |= 0x40;
		LED_Dim[2] &=(~0x40);
		LED_Blink[2]	&= (~0x40);
	}
	else if(ucState==LED_BLINK)					//这个程序可以暗闪，但是没有意义，闪定义为亮闪
	{
		LED_Data[2] |= 0x40;
		LED_Dim[2] |=0x40;
		LED_Blink[2]	|= 0x40;
	}	
}

void SetLEDIN8(unsigned char ucState)
{
	if(ucState==LED_ON)
	{
		LED_Data[2] |= 0x80;
		LED_Dim[2] |=0x80;
		LED_Blink[2]	&= (~0x80);
	}		
	else if(ucState==LED_OFF)
		LED_Data[2] &= (~0x80);
	else if(ucState==LED_DIM)	//暗首先得亮，然后才能暗、
	{
		LED_Data[2] |= 0x80;
		LED_Dim[2] &=(~0x80);
		LED_Blink[2]	&= (~0x80);
	}
	else if(ucState==LED_BLINK)					//这个程序可以暗闪，但是没有意义，闪定义为亮闪
	{
		LED_Data[2] |= 0x80;
		LED_Dim[2] |=0x80;
		LED_Blink[2]	|= 0x80;
	}		
}

///////////////////
void SetLEDIN9(unsigned char ucState)
{
	if(ucState==LED_ON)
	{
		LED_Data[3] |= 0x01;
		LED_Dim[3]  |=0x01;
		LED_Blink[3]&= (~0x01);
	}		
	else if(ucState==LED_OFF)
		LED_Data[3] &= (~0x01);
	else if(ucState==LED_DIM)	//暗首先得亮，然后才能暗、
	{
		LED_Data[3] |= 0x01;
		LED_Dim[3] &=(~0x01);
		LED_Blink[3]	&= (~0x01);
	}
	else if(ucState==LED_BLINK)					//这个程序可以暗闪，但是没有意义，闪定义为亮闪
	{
		LED_Data[3] |= 0x01;
		LED_Dim[3] |=0x01;
		LED_Blink[3]	|= 0x01;
	}		
}

void SetLEDIN10(unsigned char ucState)
{
	if(ucState==LED_ON)
	{
		LED_Data[3] |= 0x02;
		LED_Dim[3] |=0x02;
		LED_Blink[3]	&= (~0x02);
	}		
	else if(ucState==LED_OFF)
		LED_Data[3] &= (~0x02);
	else if(ucState==LED_DIM)	//暗首先得亮，然后才能暗、
	{
		LED_Data[3] |= 0x02;
		LED_Dim[3] &=(~0x02);
		LED_Blink[3]	&= (~0x02);
	}
	else if(ucState==LED_BLINK)					//这个程序可以暗闪，但是没有意义，闪定义为亮闪
	{
		LED_Data[3] |= 0x02;
		LED_Dim[3] |=0x02;
		LED_Blink[3]	|= 0x02;
	}		
}

void SetLEDIN11(unsigned char ucState)
{
	if(ucState==LED_ON)
	{
		LED_Data[3] |= 0x04;
		LED_Dim[3] |=0x04;
		LED_Blink[3]	&= (~0x04);
	}		
	else if(ucState==LED_OFF)
		LED_Data[3] &= (~0x04);
	else if(ucState==LED_DIM)	//暗首先得亮，然后才能暗、
	{
		LED_Data[3] |= 0x04;
		LED_Dim[3] &=(~0x04);
		LED_Blink[3]	&= (~0x04);
	}
	else if(ucState==LED_BLINK)					//这个程序可以暗闪，但是没有意义，闪定义为亮闪
	{
		LED_Data[3] |= 0x04;
		LED_Dim[3] |=0x04;
		LED_Blink[3]	|= 0x04;
	}	
}


void SetLEDIN12(unsigned char ucState)
{
	if(ucState==LED_ON)
	{
		LED_Data[3] |= 0x08;
		LED_Dim[3] |=0x08;
		LED_Blink[3]	&= (~0x08);
	}		
	else if(ucState==LED_OFF)
		LED_Data[3] &= (~0x08);
	else if(ucState==LED_DIM)	//暗首先得亮，然后才能暗、
	{
		LED_Data[3] |= 0x08;
		LED_Dim[3] &=(~0x08);
		LED_Blink[3]	&= (~0x08);
	}
	else if(ucState==LED_BLINK)					//这个程序可以暗闪，但是没有意义，闪定义为亮闪
	{
		LED_Data[3] |= 0x08;
		LED_Dim[3] |=0x08;
		LED_Blink[3]	|= 0x08;
	}		
}

void SetLEDIN13(unsigned char ucState)
{
	if(ucState==LED_ON)
	{
		LED_Data[3] |= 0x10;
		LED_Dim[3] |=0x10;
		LED_Blink[3]	&= (~0x10);
	}		
	else if(ucState==LED_OFF)
		LED_Data[3] &= (~0x10);
	else if(ucState==LED_DIM)	//暗首先得亮，然后才能暗、
	{
		LED_Data[3] |= 0x10;
		LED_Dim[3] &=(~0x10);
		LED_Blink[3]	&= (~0x10);
	}
	else if(ucState==LED_BLINK)					//这个程序可以暗闪，但是没有意义，闪定义为亮闪
	{
		LED_Data[3] |= 0x10;
		LED_Dim[3] |=0x10;
		LED_Blink[3]	|= 0x10;
	}		
}


void SetLEDIN14(unsigned char ucState)
{
	if(ucState==LED_ON)
	{
		LED_Data[3] |= 0x20;
		LED_Dim[3] |=0x20;
		LED_Blink[3]	&= (~0x20);
	}		
	else if(ucState==LED_OFF)
		LED_Data[3] &= (~0x20);
	else if(ucState==LED_DIM)	//暗首先得亮，然后才能暗、
	{
		LED_Data[3] |= 0x20;
		LED_Dim[3] &=(~0x20);
		LED_Blink[3]	&= (~0x20);
	}
	else if(ucState==LED_BLINK)					//这个程序可以暗闪，但是没有意义，闪定义为亮闪
	{
		LED_Data[3] |= 0x20;
		LED_Dim[3] |=0x20;
		LED_Blink[3]	|= 0x20;
	}			
}


void SetLEDIN15(unsigned char ucState)
{
	if(ucState==LED_ON)
	{
		LED_Data[3] |= 0x40;
		LED_Dim[3] |=0x40;
		LED_Blink[3]	&= (~0x40);
	}		
	else if(ucState==LED_OFF)
		LED_Data[3] &= (~0x40);
	else if(ucState==LED_DIM)	//暗首先得亮，然后才能暗、
	{
		LED_Data[3] |= 0x40;
		LED_Dim[3] &=(~0x40);
		LED_Blink[3]	&= (~0x40);
	}
	else if(ucState==LED_BLINK)					//这个程序可以暗闪，但是没有意义，闪定义为亮闪
	{
		LED_Data[3] |= 0x40;
		LED_Dim[3] |=0x40;
		LED_Blink[3]	|= 0x40;
	}
		
}

void SetLEDIN16(unsigned char ucState)
{
	if(ucState==LED_ON)
	{
		LED_Data[3] |= 0x80;
		LED_Dim[3] |=0x80;
		LED_Blink[3]	&= (~0x80);
	}		
	else if(ucState==LED_OFF)
		LED_Data[3] &= (~0x80);
	else if(ucState==LED_DIM)	//暗首先得亮，然后才能暗、
	{
		LED_Data[3] |= 0x80;
		LED_Dim[3] &=(~0x80);
		LED_Blink[3]	&= (~0x80);
	}
	else if(ucState==LED_BLINK)					//这个程序可以暗闪，但是没有意义，闪定义为亮闪
	{
		LED_Data[3] |= 0x80;
		LED_Dim[3] |=0x80;
		LED_Blink[3]	|= 0x80;
	}
		
}

void KeySetLedStatu(unsigned char ucLedIndex,unsigned char ucStatu)
{
		
		return;
}
/////////////////////////////////////////////////

void SetInLedDIM(void)
{
	unsigned char n;
	unsigned char ucChipSel,ucPortSel;

	for(n=0;n<ucMaxValidPortNum;n++)
	{
        ucChipSel = get_In_chip_ch(n)>>4;
        ucPortSel = get_In_chip_ch(n)&0x0f;
				if (MainDev.mTxClose!=TX_OFF)
				{
						if((ucChipSel==MainDev.mNextSelectChip)&&(ucPortSel==MainDev.mNextSelectInPort))
								continue;
				}
						
        if(MainDev.mDeviceType == DEVICE_IS_16S1)
        {
            switch(n)	
            {
                case 0:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN1(LED_DIM);
                        else
                            SetLEDIN1(LED_OFF);
                        break;
                case 1:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN2(LED_DIM);
                        else
                            SetLEDIN2(LED_OFF);
                        break;
                case 2:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN3(LED_DIM);
                        else
                            SetLEDIN3(LED_OFF);
                        break;
                case 3:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN4(LED_DIM);
                        else
                            SetLEDIN4(LED_OFF);
                        break;
                case 4:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN5(LED_DIM);
                        else
                            SetLEDIN5(LED_OFF);
                        break;
                case 5:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN6(LED_DIM);
                        else
                            SetLEDIN6(LED_OFF);
                        break;
                case 6:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN7(LED_DIM);
                        else
                            SetLEDIN7(LED_OFF);
                        break;
                case 7:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN8(LED_DIM);
                        else
                            SetLEDIN8(LED_OFF);
                        break;
                case 8:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN9(LED_DIM);
                        else
                            SetLEDIN9(LED_OFF);
                        break;
                case 9:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN10(LED_DIM);
                        else
                            SetLEDIN10(LED_OFF);
                        break;
                case 10:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN11(LED_DIM);
                        else
                            SetLEDIN11(LED_OFF);
                        break;
                case 11:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN12(LED_DIM);
                        else
                            SetLEDIN12(LED_OFF);  
                        break;
                case 12:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN13(LED_DIM);
                        else
                            SetLEDIN13(LED_OFF);
                        break;
                case 13:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN14(LED_DIM);
                        else
                            SetLEDIN14(LED_OFF);
                        break;
                case 14:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN15(LED_DIM);
                        else
                            SetLEDIN15(LED_OFF);
                        break;
                case 15:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN16(LED_DIM);
                        else
                            SetLEDIN16(LED_OFF);
                        break;                   
                default:
                    break;
            }
        }        
				if(MainDev.mDeviceType == DEVICE_IS_10S1)
        {
            switch(n)	
            {
                case 0:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN3(LED_DIM);
                        else
                            SetLEDIN3(LED_OFF);
                        break;
                case 1:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN4(LED_DIM);
                        else
                            SetLEDIN4(LED_OFF);
                        break;
                case 2:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN5(LED_DIM);
                        else
                            SetLEDIN5(LED_OFF);
                        break;
                case 3:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN6(LED_DIM);
                        else
                            SetLEDIN6(LED_OFF);
                        break;
                case 4:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN7(LED_DIM);
                        else
                            SetLEDIN7(LED_OFF);
                        break;
                case 5:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN11(LED_DIM);
                        else
                            SetLEDIN11(LED_OFF);
                        break;
                case 6:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN12(LED_DIM);
                        else
                            SetLEDIN12(LED_OFF);
                        break;
                case 7:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN13(LED_DIM);
                        else
                            SetLEDIN13(LED_OFF);
                        break;
                case 8:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN14(LED_DIM);
                        else
                            SetLEDIN14(LED_OFF);
                        break;
                case 9:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN15(LED_DIM);
                        else
                            SetLEDIN15(LED_OFF);
                        break;                
                default:
                    break;
            }
        } 
        if(MainDev.mDeviceType == DEVICE_IS_8S1)
        {
            switch(n)	
            {
                case 0:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN3(LED_DIM);
                        else
                            SetLEDIN3(LED_OFF);
                        break;
                case 1:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN4(LED_DIM);
                        else
                            SetLEDIN4(LED_OFF);
                        break;
                case 2:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN5(LED_DIM);
                        else
                            SetLEDIN5(LED_OFF);
                        break;
                case 3:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN6(LED_DIM);
                        else
                            SetLEDIN6(LED_OFF);
                        break;
                case 4:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN11(LED_DIM);
                        else
                            SetLEDIN11(LED_OFF);
                        break;
                case 5:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN12(LED_DIM);
                        else
                            SetLEDIN12(LED_OFF);
                        break;
                case 6:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN13(LED_DIM);
                        else
                            SetLEDIN13(LED_OFF);
                        break;
                case 7:	if(InDev[ucChipSel][ucPortSel].mInSig==TRUE)
                            SetLEDIN14(LED_DIM);
                        else
                            SetLEDIN14(LED_OFF);
                        break;                 
                default:
                    break;
            }
        }        
	}
} 

//
void UICgh(void) 
{
    switch(MainDev.mUI)
    {
				case  UI_STATE_POWER_ON:
						SetAllLED(LED_ON);
						break;
				case  UI_STATE_SET_DELAY:
						SetAllLED(LED_OFF);    
						SetLEDLOCK(LED_BLINK);
            if(MainDev.mDeviceType==DEVICE_IS_16S1)
            {
                if (MainDev.mBlankDelayTx[0]==0)
                    SetLEDIN1(LED_ON);
                if (MainDev.mBlankDelayTx[0]==1)
                    SetLEDIN2(LED_ON);
                if (MainDev.mBlankDelayTx[0]==2)
                    SetLEDIN3(LED_ON);
                if (MainDev.mBlankDelayTx[0]==3)
                    SetLEDIN4(LED_ON);
                if (MainDev.mBlankDelayTx[0]==4)
                    SetLEDIN5(LED_ON);
                if (MainDev.mBlankDelayTx[0]==5)
                    SetLEDIN6(LED_ON);
                if (MainDev.mBlankDelayTx[0]==6)
                    SetLEDIN7(LED_ON);
                if (MainDev.mBlankDelayTx[0]==7)
                    SetLEDIN8(LED_ON);
                if (MainDev.mBlankDelayTx[0]==8)
                    SetLEDIN9(LED_ON);
                if (MainDev.mBlankDelayTx[0]==9)
                    SetLEDIN10(LED_ON);
                if (MainDev.mBlankDelayTx[0]==10)
                    SetLEDIN11(LED_ON);
                if (MainDev.mBlankDelayTx[0]==11)
                    SetLEDIN12(LED_ON);	
                if (MainDev.mBlankDelayTx[0]==12)
                    SetLEDIN13(LED_ON);
                if (MainDev.mBlankDelayTx[0]==13)
                    SetLEDIN14(LED_ON);
                if (MainDev.mBlankDelayTx[0]==14)
                    SetLEDIN15(LED_ON);
                if (MainDev.mBlankDelayTx[0]==15)
                    SetLEDIN16(LED_ON);	            
            }
            else if(MainDev.mDeviceType==DEVICE_IS_10S1)
            {
                if (MainDev.mBlankDelayTx[0]==0)
                    SetLEDIN3(LED_ON);
                if (MainDev.mBlankDelayTx[0]==1)
                    SetLEDIN4(LED_ON);
                if (MainDev.mBlankDelayTx[0]==2)
                    SetLEDIN5(LED_ON);
                if (MainDev.mBlankDelayTx[0]==3)
                    SetLEDIN6(LED_ON);
                if (MainDev.mBlankDelayTx[0]==4)
                    SetLEDIN7(LED_ON);
                if (MainDev.mBlankDelayTx[0]==5)
                    SetLEDIN11(LED_ON);
                if (MainDev.mBlankDelayTx[0]==6)
                    SetLEDIN12(LED_ON);
                if (MainDev.mBlankDelayTx[0]==7)
                    SetLEDIN13(LED_ON);
                if (MainDev.mBlankDelayTx[0]==8)
                    SetLEDIN14(LED_ON);
                if (MainDev.mBlankDelayTx[0]==9)
                    SetLEDIN15(LED_ON);
            }
            else if(MainDev.mDeviceType==DEVICE_IS_8S1)
            {
                if (MainDev.mBlankDelayTx[0]==0)
                    SetLEDIN3(LED_ON);
                if (MainDev.mBlankDelayTx[0]==1)
                    SetLEDIN4(LED_ON);
                if (MainDev.mBlankDelayTx[0]==2)
                    SetLEDIN5(LED_ON);
                if (MainDev.mBlankDelayTx[0]==3)
                    SetLEDIN6(LED_ON);
                if (MainDev.mBlankDelayTx[0]==4)
                    SetLEDIN11(LED_ON);
                if (MainDev.mBlankDelayTx[0]==5)
                    SetLEDIN12(LED_ON);
                if (MainDev.mBlankDelayTx[0]==6)
                    SetLEDIN13(LED_ON);
                if (MainDev.mBlankDelayTx[0]==7)
                    SetLEDIN14(LED_ON);
            }
						break;
				case UI_STATE_SET_USB_MODE:
            SetLEDOFF(LED_OFF);
            SetLEDEDID(LED_OFF);
						SetLEDLOCK(LED_BLINK);
            SetAllINLED(LED_OFF);	
            if(usb_working_mode==0)      //表示为VCOM
            {
                if(MainDev.mDeviceType==DEVICE_IS_16S1)
                    SetLEDIN16(LED_ON);
                else if(MainDev.mDeviceType==DEVICE_IS_10S1)
                    SetLEDIN15(LED_ON);
                else if(MainDev.mDeviceType==DEVICE_IS_8S1)
                    SetLEDIN14(LED_ON);
            }
            else if(usb_working_mode==1)//表示为Mass Storage Disk
            {
                if(MainDev.mDeviceType==DEVICE_IS_16S1)
                    SetLEDIN16(LED_BLINK);
                else if(MainDev.mDeviceType==DEVICE_IS_10S1)
                    SetLEDIN15(LED_BLINK);
                else if(MainDev.mDeviceType==DEVICE_IS_8S1)
                    SetLEDIN14(LED_BLINK);
            }
            break;
				case  UI_STATE_SET_IN_HDCP:
            SetLEDEDID(LED_OFF);
						SetLEDOFF(LED_OFF);
						SetLEDLOCK(LED_BLINK);
            if(MainDev.mDeviceType==DEVICE_IS_16S1)
            {
                if(MainDev.mInHDCPState[CHIP_IN1_IN6][0] == INHDCP_ON)
                {
                    SetLEDIN1(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN1_IN6][0] == INHDCP_CLOSE)
                {
                    SetLEDIN1(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN1_IN6][1] == INHDCP_ON)
                {
                    SetLEDIN2(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN1_IN6][1] == INHDCP_CLOSE)
                {
                    SetLEDIN2(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN1_IN6][2] == INHDCP_ON)
                {
                    SetLEDIN3(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN1_IN6][2] == INHDCP_CLOSE)
                {
                    SetLEDIN3(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN1_IN6][3] == INHDCP_ON)
                {
                    SetLEDIN4(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN1_IN6][3] == INHDCP_CLOSE)
                {
                    SetLEDIN4(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN1_IN6][4] == INHDCP_ON)
                {
                    SetLEDIN5(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN1_IN6][4] == INHDCP_CLOSE)
                {
                    SetLEDIN5(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN1_IN6][5] == INHDCP_ON)
                {
                    SetLEDIN6(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN1_IN6][5] == INHDCP_CLOSE)
                {
                    SetLEDIN6(LED_BLINK);	
                }

                if(MainDev.mInHDCPState[CHIP_IN7_IN12][0] == INHDCP_ON)
                {
                    SetLEDIN7(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN7_IN12][0] == INHDCP_CLOSE)
                {
                    SetLEDIN7(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN7_IN12][1] == INHDCP_ON)
                {
                    SetLEDIN8(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN7_IN12][1] == INHDCP_CLOSE)
                {
                    SetLEDIN8(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN7_IN12][2] == INHDCP_ON)
                {
                    SetLEDIN9(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN7_IN12][2] == INHDCP_CLOSE)
                {
                    SetLEDIN9(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN7_IN12][3] == INHDCP_ON)
                {
                    SetLEDIN10(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN7_IN12][3] == INHDCP_CLOSE)
                {
                    SetLEDIN10(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN7_IN12][4] == INHDCP_ON)
                {
                    SetLEDIN11(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN7_IN12][4] == INHDCP_CLOSE)
                {
                    SetLEDIN11(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN7_IN12][5] == INHDCP_ON)
                {
                    SetLEDIN12(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN7_IN12][5] == INHDCP_CLOSE)
                {
                    SetLEDIN12(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN13_IN16][2] == INHDCP_ON)
                {
                    SetLEDIN13(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN13_IN16][2] == INHDCP_CLOSE)
                {
                    SetLEDIN13(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN13_IN16][3] == INHDCP_ON)
                {
                    SetLEDIN14(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN13_IN16][3] == INHDCP_CLOSE)
                {
                    SetLEDIN14(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN13_IN16][4] == INHDCP_ON)
                {
                    SetLEDIN15(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN13_IN16][4] == INHDCP_CLOSE)
                {
                    SetLEDIN15(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN13_IN16][5] == INHDCP_ON)
                {
                    SetLEDIN16(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN13_IN16][5] == INHDCP_CLOSE)
                {
                    SetLEDIN16(LED_BLINK);	
                }
            }
            else if(MainDev.mDeviceType==DEVICE_IS_10S1)
            {
                if(MainDev.mInHDCPState[CHIP_IN7_IN12][0] == INHDCP_ON)
                {
                    SetLEDIN3(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN7_IN12][0] == INHDCP_CLOSE)
                {
                    SetLEDIN3(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN7_IN12][1] == INHDCP_ON)
                {
                    SetLEDIN4(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN7_IN12][1] == INHDCP_CLOSE)
                {
                    SetLEDIN4(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN7_IN12][2] == INHDCP_ON)
                {
                    SetLEDIN5(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN7_IN12][2] == INHDCP_CLOSE)
                {
                    SetLEDIN5(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN7_IN12][3] == INHDCP_ON)
                {
                    SetLEDIN6(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN7_IN12][3] == INHDCP_CLOSE)
                {
                    SetLEDIN6(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN7_IN12][4] == INHDCP_ON)
                {
                    SetLEDIN7(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN7_IN12][4] == INHDCP_CLOSE)
                {
                    SetLEDIN7(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN7_IN12][5] == INHDCP_ON)
                {
                    SetLEDIN11(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN7_IN12][5] == INHDCP_CLOSE)
                {
                    SetLEDIN11(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN13_IN16][2] == INHDCP_ON)
                {
                    SetLEDIN12(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN13_IN16][2] == INHDCP_CLOSE)
                {
                    SetLEDIN12(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN13_IN16][3] == INHDCP_ON)
                {
                    SetLEDIN13(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN13_IN16][3] == INHDCP_CLOSE)
                {
                    SetLEDIN13(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN13_IN16][4] == INHDCP_ON)
                {
                    SetLEDIN14(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN13_IN16][4] == INHDCP_CLOSE)
                {
                    SetLEDIN14(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN13_IN16][5] == INHDCP_ON)
                {
                    SetLEDIN15(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN13_IN16][5] == INHDCP_CLOSE)
                {
                    SetLEDIN15(LED_BLINK);	
                }
            }
            else if(MainDev.mDeviceType==DEVICE_IS_8S1)
            {
                if(MainDev.mInHDCPState[CHIP_IN7_IN12][2] == INHDCP_ON)
                {
                    SetLEDIN3(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN7_IN12][2] == INHDCP_CLOSE)
                {
                    SetLEDIN3(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN7_IN12][3] == INHDCP_ON)
                {
                    SetLEDIN4(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN7_IN12][3] == INHDCP_CLOSE)
                {
                    SetLEDIN4(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN7_IN12][4] == INHDCP_ON)
                {
                    SetLEDIN5(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN7_IN12][4] == INHDCP_CLOSE)
                {
                    SetLEDIN5(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN7_IN12][5] == INHDCP_ON)
                {
                    SetLEDIN6(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN7_IN12][5] == INHDCP_CLOSE)
                {
                    SetLEDIN6(LED_BLINK);	
                }
								
                if(MainDev.mInHDCPState[CHIP_IN13_IN16][2] == INHDCP_ON)
                {
                    SetLEDIN11(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN13_IN16][2] == INHDCP_CLOSE)
                {
                    SetLEDIN11(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN13_IN16][3] == INHDCP_ON)
                {
                    SetLEDIN12(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN13_IN16][3] == INHDCP_CLOSE)
                {
                    SetLEDIN12(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN13_IN16][4] == INHDCP_ON)
                {
                    SetLEDIN13(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN13_IN16][4] == INHDCP_CLOSE)
                {
                    SetLEDIN13(LED_BLINK);	
                }
                if(MainDev.mInHDCPState[CHIP_IN13_IN16][5] == INHDCP_ON)
                {
                    SetLEDIN14(LED_ON);	
                }
                else if(MainDev.mInHDCPState[CHIP_IN13_IN16][5] == INHDCP_CLOSE)
                {
                    SetLEDIN14(LED_BLINK);	
                }
            }

            break;
				case  UI_STATE_SET_OUT_HDCP:
						SetLEDEDID(LED_OFF);
						SetAllINLED(LED_OFF);
						SetLEDOFF(LED_OFF);
						SetLEDLOCK(LED_BLINK);
						if(ConDev.mConFollowEncrption==FE_FOLLOW_IN)
						{
								SetLEDIN3(LED_ON);
								
						}else if(ConDev.mConFollowEncrption[0]==FE_FORCE_DISCRP)
						{
								SetLEDIN3(LED_BLINK);						
						}
						break;
				case  UI_STATE_SET_ARC:
            SetAllINLED(LED_OFF);
						SetLEDOFF(LED_OFF);
						SetLEDLOCK(LED_BLINK);
            if(oldAudioSelect == AUDIO_ARC)//(MainDev.mAudioOutState == AUDIO_ARC)
            {
                SetLEDIN3(LED_BLINK);	
            }
						else if(oldAudioSelect == AUDIO_DE_Embed)//(MainDev.mAudioOutState == AUDIO_Embed)
						{
							SetLEDIN3(LED_ON);
						} 
            break;
				case  UI_STATE_RESET:
						SetAllLED(LED_BLINK);
						break;
				case  UI_STATE_SET_COM:				//注意复制EDID在灯自检完成之后
            SetLEDEDID(LED_OFF);
						SetAllINLED(LED_OFF);
						SetLEDOFF(LED_BLINK);
						SetLEDLOCK(LED_OFF);
            if(MainDev.mDeviceType==DEVICE_IS_16S1)
            {
                if (MainDev.mComFormat==LIGUO_A)
                    SetLEDIN1(LED_ON);
                if (MainDev.mComFormat==LIGUO_B)
                    SetLEDIN2(LED_ON);
                if (MainDev.mComFormat==KRM3000)
                    SetLEDIN3(LED_ON);
            }
            else 
            {
                if (MainDev.mComFormat==LIGUO_A)
                    SetLEDIN3(LED_ON);
                if (MainDev.mComFormat==LIGUO_B)
                    SetLEDIN4(LED_ON);
                if (MainDev.mComFormat==KRM3000)
                    SetLEDIN5(LED_ON);
            }
						break;
				case  UI_STATE_SWITCH:
						SetLEDLOCK(LED_OFF);
						SetLEDEDID(LED_OFF);
						if(MainDev.mTxClose==TX_OFF)
						{
							SetLEDOFF(LED_ON);
							SetInLedDIM();
						}else
						{
											SetLEDOFF(LED_OFF);

											if((MainDev.mNextSelectChip==CHIP_IN1_IN6) && (MainDev.mNextSelectInPort==IN_PORT1))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN1(LED_ON);				
													SetInLedDIM();
											}else if((MainDev.mNextSelectChip==CHIP_IN1_IN6) &&(MainDev.mNextSelectInPort==IN_PORT2))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN2(LED_ON);
													SetInLedDIM();
											}else if((MainDev.mNextSelectChip==CHIP_IN1_IN6) &&(MainDev.mNextSelectInPort==IN_PORT3))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN3(LED_ON);
													SetInLedDIM();
											}else if ((MainDev.mNextSelectChip==CHIP_IN1_IN6) &&(MainDev.mNextSelectInPort==IN_PORT4))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN4(LED_ON);
													SetInLedDIM();
											}else if((MainDev.mNextSelectChip==CHIP_IN1_IN6) &&(MainDev.mNextSelectInPort==IN_PORT5))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN5(LED_ON);
													SetInLedDIM();
											}else if ((MainDev.mNextSelectChip==CHIP_IN1_IN6) &&(MainDev.mNextSelectInPort==IN_PORT6))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN6(LED_ON);
													SetInLedDIM();
											}
											else if((MainDev.mNextSelectChip==CHIP_IN7_IN12) && (MainDev.mNextSelectInPort==IN_PORT1))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN7(LED_ON);			
													else if(MainDev.mDeviceType == DEVICE_IS_10S1)
															SetLEDIN3(LED_ON);	                        
															
													SetInLedDIM();
											}else if((MainDev.mNextSelectChip==CHIP_IN7_IN12) &&(MainDev.mNextSelectInPort==IN_PORT2))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN8(LED_ON);			
													else if(MainDev.mDeviceType == DEVICE_IS_10S1)
															SetLEDIN4(LED_ON);
													SetInLedDIM();
											}else if((MainDev.mNextSelectChip==CHIP_IN7_IN12) &&(MainDev.mNextSelectInPort==IN_PORT3))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN9(LED_ON);			
													else if(MainDev.mDeviceType == DEVICE_IS_10S1)
															SetLEDIN5(LED_ON);
													else if(MainDev.mDeviceType == DEVICE_IS_8S1)
															SetLEDIN3(LED_ON);
													SetInLedDIM();
											}else if ((MainDev.mNextSelectChip==CHIP_IN7_IN12) &&(MainDev.mNextSelectInPort==IN_PORT4))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN10(LED_ON);			
													else if(MainDev.mDeviceType == DEVICE_IS_10S1)
															SetLEDIN6(LED_ON);
													else if(MainDev.mDeviceType == DEVICE_IS_8S1)
															SetLEDIN4(LED_ON);
													SetInLedDIM();
											}else if((MainDev.mNextSelectChip==CHIP_IN7_IN12) &&(MainDev.mNextSelectInPort==IN_PORT5))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN11(LED_ON);			
													else if(MainDev.mDeviceType == DEVICE_IS_10S1)
															SetLEDIN7(LED_ON);
													else if(MainDev.mDeviceType == DEVICE_IS_8S1)
															SetLEDIN5(LED_ON);
													SetInLedDIM();
											}else if ((MainDev.mNextSelectChip==CHIP_IN7_IN12) &&(MainDev.mNextSelectInPort==IN_PORT6))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN12(LED_ON);			
													else if(MainDev.mDeviceType == DEVICE_IS_10S1)
															SetLEDIN11(LED_ON);
													else if(MainDev.mDeviceType == DEVICE_IS_8S1)
															SetLEDIN6(LED_ON);
													SetInLedDIM();
											}
											else if((MainDev.mNextSelectChip==CHIP_IN13_IN16) && (MainDev.mNextSelectInPort==IN_PORT3))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN13(LED_ON);			
													else if(MainDev.mDeviceType == DEVICE_IS_10S1)
															SetLEDIN12(LED_ON);
													else if(MainDev.mDeviceType == DEVICE_IS_8S1)
															SetLEDIN11(LED_ON);                        
													SetInLedDIM();
											}else if((MainDev.mNextSelectChip==CHIP_IN13_IN16) &&(MainDev.mNextSelectInPort==IN_PORT4))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN14(LED_ON);			
													else if(MainDev.mDeviceType == DEVICE_IS_10S1)
															SetLEDIN13(LED_ON);
													else if(MainDev.mDeviceType == DEVICE_IS_8S1)
															SetLEDIN12(LED_ON); 
													SetInLedDIM();
											}else if((MainDev.mNextSelectChip==CHIP_IN13_IN16) &&(MainDev.mNextSelectInPort==IN_PORT5))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN15(LED_ON);			
													else if(MainDev.mDeviceType == DEVICE_IS_10S1)
															SetLEDIN14(LED_ON);
													else if(MainDev.mDeviceType == DEVICE_IS_8S1)
															SetLEDIN13(LED_ON);
													SetInLedDIM();
											}else if ((MainDev.mNextSelectChip==CHIP_IN13_IN16) &&(MainDev.mNextSelectInPort==IN_PORT6))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN16(LED_ON);			
													else if(MainDev.mDeviceType == DEVICE_IS_10S1)
															SetLEDIN15(LED_ON);
													else if(MainDev.mDeviceType == DEVICE_IS_8S1)
															SetLEDIN14(LED_ON);
													SetInLedDIM();
											}
																	 
						}

						#ifdef 	SUPPORT_AUTO_SWITCH
						//置自动切换的A/M  LC/Pr灯
						if(AutoSw.SwitchMode == MANUAL_SWITCH_MODE)
						{
								SetLEDA_M(LED_OFF);
								SetLEDL_P(LED_OFF);	
						}	
						else
						{
								SetLEDA_M(LED_ON);
								if(AutoSw.AutoSwMode == PRIORITY_MODE)                
										SetLEDL_P(LED_OFF);
								else
								{
										if(sCascaded!=IS_CASCADED_MODE)
												SetLEDL_P(LED_ON);
										else
												SetLEDL_P(LED_BLINK);
								}
						}
						#endif
						//
						break;
				case  UI_STATE_LOCK:
						SetLEDLOCK(LED_ON);

						if(MainDev.mTxClose==TX_OFF)
						{
								SetLEDOFF(LED_ON);
								SetInLedDIM();
						}else
						{
											SetLEDOFF(LED_OFF);
											if((MainDev.mNextSelectChip==CHIP_IN1_IN6) && (MainDev.mNextSelectInPort==IN_PORT1))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN1(LED_ON);				
													SetInLedDIM();
											}else if((MainDev.mNextSelectChip==CHIP_IN1_IN6) &&(MainDev.mNextSelectInPort==IN_PORT2))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN2(LED_ON);
													SetInLedDIM();
											}else if((MainDev.mNextSelectChip==CHIP_IN1_IN6) &&(MainDev.mNextSelectInPort==IN_PORT3))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN3(LED_ON);
													SetInLedDIM();
											}else if ((MainDev.mNextSelectChip==CHIP_IN1_IN6) &&(MainDev.mNextSelectInPort==IN_PORT4))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN4(LED_ON);
													SetInLedDIM();
											}else if((MainDev.mNextSelectChip==CHIP_IN1_IN6) &&(MainDev.mNextSelectInPort==IN_PORT5))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN5(LED_ON);
													SetInLedDIM();
											}else if ((MainDev.mNextSelectChip==CHIP_IN1_IN6) &&(MainDev.mNextSelectInPort==IN_PORT6))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN6(LED_ON);
													SetInLedDIM();
											}
											else if((MainDev.mNextSelectChip==CHIP_IN7_IN12) && (MainDev.mNextSelectInPort==IN_PORT1))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN7(LED_ON);			
													else if(MainDev.mDeviceType == DEVICE_IS_10S1)
															SetLEDIN3(LED_ON);	                        
															
													SetInLedDIM();
											}else if((MainDev.mNextSelectChip==CHIP_IN7_IN12) &&(MainDev.mNextSelectInPort==IN_PORT2))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN8(LED_ON);			
													else if(MainDev.mDeviceType == DEVICE_IS_10S1)
															SetLEDIN4(LED_ON);
													SetInLedDIM();
											}else if((MainDev.mNextSelectChip==CHIP_IN7_IN12) &&(MainDev.mNextSelectInPort==IN_PORT3))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN9(LED_ON);			
													else if(MainDev.mDeviceType == DEVICE_IS_10S1)
															SetLEDIN5(LED_ON);
													else if(MainDev.mDeviceType == DEVICE_IS_8S1)
															SetLEDIN3(LED_ON);
													SetInLedDIM();
											}else if ((MainDev.mNextSelectChip==CHIP_IN7_IN12) &&(MainDev.mNextSelectInPort==IN_PORT4))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN10(LED_ON);			
													else if(MainDev.mDeviceType == DEVICE_IS_10S1)
															SetLEDIN6(LED_ON);
													else if(MainDev.mDeviceType == DEVICE_IS_8S1)
															SetLEDIN4(LED_ON);
													SetInLedDIM();
											}else if((MainDev.mNextSelectChip==CHIP_IN7_IN12) &&(MainDev.mNextSelectInPort==IN_PORT5))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN11(LED_ON);			
													else if(MainDev.mDeviceType == DEVICE_IS_10S1)
															SetLEDIN7(LED_ON);
													else if(MainDev.mDeviceType == DEVICE_IS_8S1)
															SetLEDIN5(LED_ON);
													SetInLedDIM();
											}else if ((MainDev.mNextSelectChip==CHIP_IN7_IN12) &&(MainDev.mNextSelectInPort==IN_PORT6))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN12(LED_ON);			
													else if(MainDev.mDeviceType == DEVICE_IS_10S1)
															SetLEDIN11(LED_ON);
													else if(MainDev.mDeviceType == DEVICE_IS_8S1)
															SetLEDIN6(LED_ON);
													SetInLedDIM();
											}
											else if((MainDev.mNextSelectChip==CHIP_IN13_IN16) && (MainDev.mNextSelectInPort==IN_PORT3))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN13(LED_ON);			
													else if(MainDev.mDeviceType == DEVICE_IS_10S1)
															SetLEDIN12(LED_ON);
													else if(MainDev.mDeviceType == DEVICE_IS_8S1)
															SetLEDIN11(LED_ON);                        
													SetInLedDIM();
											}else if((MainDev.mNextSelectChip==CHIP_IN13_IN16) &&(MainDev.mNextSelectInPort==IN_PORT4))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN14(LED_ON);			
													else if(MainDev.mDeviceType == DEVICE_IS_10S1)
															SetLEDIN13(LED_ON);
													else if(MainDev.mDeviceType == DEVICE_IS_8S1)
															SetLEDIN12(LED_ON); 
													SetInLedDIM();
											}else if((MainDev.mNextSelectChip==CHIP_IN13_IN16) &&(MainDev.mNextSelectInPort==IN_PORT5))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN15(LED_ON);			
													else if(MainDev.mDeviceType == DEVICE_IS_10S1)
															SetLEDIN14(LED_ON);
													else if(MainDev.mDeviceType == DEVICE_IS_8S1)
															SetLEDIN13(LED_ON);
													SetInLedDIM();
											}else if ((MainDev.mNextSelectChip==CHIP_IN13_IN16) &&(MainDev.mNextSelectInPort==IN_PORT6))
											{
													if(MainDev.mDeviceType == DEVICE_IS_16S1)
															SetLEDIN16(LED_ON);			
													else if(MainDev.mDeviceType == DEVICE_IS_10S1)
															SetLEDIN15(LED_ON);
													else if(MainDev.mDeviceType == DEVICE_IS_8S1)
															SetLEDIN14(LED_ON);
													SetInLedDIM();
											}			
						}

						#ifdef 	SUPPORT_AUTO_SWITCH
						//置自动切换的A/M  LC/Pr灯
						if(AutoSw.SwitchMode == MANUAL_SWITCH_MODE)
						{
									SetLEDA_M(LED_OFF);
									SetLEDL_P(LED_OFF);	
						}	
						else
						{
									SetLEDA_M(LED_ON);
									if(AutoSw.AutoSwMode == PRIORITY_MODE)                
											SetLEDL_P(LED_OFF);
									else
									{
											if(sCascaded!=IS_CASCADED_MODE)
													SetLEDL_P(LED_ON);
											else
													SetLEDL_P(LED_BLINK);
									}
						}
						#endif
						break;
				case  UI_STATE_COPY_EDID:    
						SetAllLED(LED_OFF);
            if(MainDev.mDeviceType==DEVICE_IS_16S1)
            {
                if (ucCpyEDIDCount==0)
                {
                    ucCpyEDIDCount=1;
                    SetLEDIN1(LED_ON);
                }else if (ucCpyEDIDCount==1)
                {
                    ucCpyEDIDCount=2;
                    SetLEDIN2(LED_ON);
                }else if (ucCpyEDIDCount==2)
                {
                    ucCpyEDIDCount=3;
                    SetLEDIN3(LED_ON);
                }else if (ucCpyEDIDCount==3)
                {
                    ucCpyEDIDCount=4;
                    SetLEDIN4(LED_ON);
                }else if (ucCpyEDIDCount==4)
                {
                    ucCpyEDIDCount=5;
                    SetLEDIN5(LED_ON);
                }else if (ucCpyEDIDCount==5)
                {
                     ucCpyEDIDCount=6;
                    SetLEDIN6(LED_ON);
                }
                else if (ucCpyEDIDCount==6)
                {
                    ucCpyEDIDCount=7;
                    SetLEDIN7(LED_ON);
                }else if (ucCpyEDIDCount==7)
                {
                    ucCpyEDIDCount=8;
                    SetLEDIN8(LED_ON);
                }else if (ucCpyEDIDCount==8)
                {
                    ucCpyEDIDCount=9;
                    SetLEDIN9(LED_ON);
                }else if (ucCpyEDIDCount==9)
                {
                    ucCpyEDIDCount=10;
                    SetLEDIN10(LED_ON);
                }else if (ucCpyEDIDCount==10)
                {
                    ucCpyEDIDCount=11;
                    SetLEDIN11(LED_ON);
                }else if (ucCpyEDIDCount==11)
                {
                     ucCpyEDIDCount=12;
                    SetLEDIN12(LED_ON);
                }else if (ucCpyEDIDCount==12)
                {
                    ucCpyEDIDCount=13;
                    SetLEDIN13(LED_ON);
                }else if (ucCpyEDIDCount==13)
                {
                    ucCpyEDIDCount=14;
                    SetLEDIN14(LED_ON);
                }else if (ucCpyEDIDCount==14)
                {
                    ucCpyEDIDCount=15;
                    SetLEDIN15(LED_ON);
                }else if (ucCpyEDIDCount==15)
                {
                    ucCpyEDIDCount=16;
                    SetLEDIN16(LED_ON);
                }
                else 
                   ucCpyEDIDCount++;
            }
            if(MainDev.mDeviceType==DEVICE_IS_10S1)
            {
                if (ucCpyEDIDCount==0)
                {
                    ucCpyEDIDCount=1;
                    SetLEDIN3(LED_ON);
                }else if (ucCpyEDIDCount==1)
                {
                    ucCpyEDIDCount=2;
                    SetLEDIN4(LED_ON);
                }else if (ucCpyEDIDCount==2)
                {
                    ucCpyEDIDCount=3;
                    SetLEDIN5(LED_ON);
                }else if (ucCpyEDIDCount==3)
                {
                    ucCpyEDIDCount=4;
                    SetLEDIN6(LED_ON);
                }else if (ucCpyEDIDCount==4)
                {
                    ucCpyEDIDCount=5;
                    SetLEDIN7(LED_ON);
                }else if (ucCpyEDIDCount==5)
                {
                     ucCpyEDIDCount=6;
                    SetLEDIN11(LED_ON);
                }
                else if (ucCpyEDIDCount==6)
                {
                    ucCpyEDIDCount=7;
                    SetLEDIN12(LED_ON);
                }else if (ucCpyEDIDCount==7)
                {
                    ucCpyEDIDCount=8;
                    SetLEDIN13(LED_ON);
                }else if (ucCpyEDIDCount==8)
                {
                    ucCpyEDIDCount=9;
                    SetLEDIN14(LED_ON);
                }else if (ucCpyEDIDCount==9)
                {
                    ucCpyEDIDCount=10;
                    SetLEDIN15(LED_ON);
                }
                else 
                    ucCpyEDIDCount++;
            }
            if(MainDev.mDeviceType==DEVICE_IS_8S1)
            {
                if (ucCpyEDIDCount==0)
                {
                    ucCpyEDIDCount=1;
                    SetLEDIN3(LED_ON);
                }else if (ucCpyEDIDCount==1)
                {
                    ucCpyEDIDCount=2;
                    SetLEDIN4(LED_ON);
                }else if (ucCpyEDIDCount==2)
                {
                    ucCpyEDIDCount=3;
                    SetLEDIN5(LED_ON);
                }else if (ucCpyEDIDCount==3)
                {
                    ucCpyEDIDCount=4;
                    SetLEDIN6(LED_ON);
                }else if (ucCpyEDIDCount==4)
                {
                    ucCpyEDIDCount=5;
                    SetLEDIN11(LED_ON);
                }else if (ucCpyEDIDCount==5)
                {
                     ucCpyEDIDCount=6;
                    SetLEDIN12(LED_ON);
                }
                else if (ucCpyEDIDCount==6)
                {
                    ucCpyEDIDCount=7;
                    SetLEDIN13(LED_ON);
                }else if (ucCpyEDIDCount==7)
                {
                    ucCpyEDIDCount=8;
                    SetLEDIN14(LED_ON);
                }
                else 
                    ucCpyEDIDCount++;
            }
						break;   
				case UI_STATE_SELECT_EDID:
            SetLEDIN1(LED_BLINK);
            if(usSelectEDIDCount!=0)
            {              
                if(ucSelectEDIDState==0)
                {   
                    SetAllINLED(LED_ON);
                }
                else
                {    
                    SetAllINLED(LED_OFF);
                }
            }
            else
            {
                if (MainDev.mEDIDSelPort&(1<<0))                
                    SetLEDIN1(LED_ON);
                else 
                    SetLEDIN1(LED_OFF);
                
                if (MainDev.mEDIDSelPort&(1<<1))
                    SetLEDIN2(LED_ON);
                else 
                    SetLEDIN2(LED_OFF);
                
                if (MainDev.mEDIDSelPort&(1<<2))
                    SetLEDIN3(LED_ON);
                else 
                    SetLEDIN3(LED_OFF);
                
                if (MainDev.mEDIDSelPort&(1<<3))
                    SetLEDIN4(LED_ON);
                else 
                    SetLEDIN4(LED_OFF);
                
                if (MainDev.mEDIDSelPort&(1<<4))
                    SetLEDIN5(LED_ON);
                else 
                    SetLEDIN5(LED_OFF);
                
                if (MainDev.mEDIDSelPort&(1<<5))
                    SetLEDIN6(LED_ON);
                else 
                    SetLEDIN6(LED_OFF);
                
                if (MainDev.mEDIDSelPort&(1<<6))
                    SetLEDIN7(LED_ON);
                else 
                    SetLEDIN7(LED_OFF);
                
                if (MainDev.mEDIDSelPort&(1<<7))
                    SetLEDIN8(LED_ON);
                else 
                    SetLEDIN8(LED_OFF);
                
                if (MainDev.mEDIDSelPort&(1<<8))
                    SetLEDIN9(LED_ON);
                else 
                    SetLEDIN9(LED_OFF);
                
                if (MainDev.mEDIDSelPort&(1<<9))
                    SetLEDIN10(LED_ON);
                else 
                    SetLEDIN10(LED_OFF);
                
                if (MainDev.mEDIDSelPort&(1<<10))
                    SetLEDIN11(LED_ON);
                else 
                    SetLEDIN11(LED_OFF);
                if (MainDev.mEDIDSelPort&(1<<11))
                    SetLEDIN12(LED_ON);
                else 
                    SetLEDIN12(LED_OFF);
                
                if (MainDev.mEDIDSelPort&(1<<12))
                    SetLEDIN13(LED_ON);
                else 
                    SetLEDIN13(LED_OFF);
                if (MainDev.mEDIDSelPort&(1<<13))
                    SetLEDIN14(LED_ON);
                else 
                    SetLEDIN14(LED_OFF);
                
                if (MainDev.mEDIDSelPort&(1<<14))
                    SetLEDIN15(LED_ON);
                else 
                    SetLEDIN15(LED_OFF);
                
                if (MainDev.mEDIDSelPort&(1<<15))
                    SetLEDIN16(LED_ON);
                else 
                   SetLEDIN16(LED_OFF);
            }
            break;  
				case UI_STATE_MATCH_IR:
//            if((MainDev.mDeviceType==DEVICE_IS_8S1)&&
            if(IRMatchMode != STATE_MATCH_NUM0)
                SetLEDOFF(LED_OFF);
            SetAllINLED(LED_OFF);
            if(IRMatchMode == STATE_MATCH_TIM_START)  //得到时序                   
                SetLEDLOCK(LED_BLINK);
            else
                SetLEDLOCK(LED_OFF);
            if(MainDev.mDeviceType==DEVICE_IS_16S1)
            {
                if(IRMatchMode == STATE_MATCH_NUM1)                     //得到匹配值1	
                    SetLEDIN1(LED_BLINK);	                
                else if(IRMatchMode == STATE_MATCH_NUM2)                   //得到匹配值2
                    SetLEDIN2(LED_BLINK);
                else if(IRMatchMode == STATE_MATCH_NUM3)                   //得到匹配值3	
                    SetLEDIN3(LED_BLINK);	
                else if(IRMatchMode == STATE_MATCH_NUM4)                   //得到匹配值4
                    SetLEDIN4(LED_BLINK);            
                else if(IRMatchMode == STATE_MATCH_NUM5)                     //得到匹配值5	
                    SetLEDIN5(LED_BLINK);	                
                else if(IRMatchMode == STATE_MATCH_NUM6)                   //得到匹配值6
                    SetLEDIN6(LED_BLINK);
                else if(IRMatchMode == STATE_MATCH_NUM7)                   //得到匹配值7	
                    SetLEDIN7(LED_BLINK);	
                else if(IRMatchMode == STATE_MATCH_NUM8)                   //得到匹配值8
                    SetLEDIN8(LED_BLINK); 
                else if(IRMatchMode == STATE_MATCH_NUM9)                   //得到匹配值9
                    SetLEDIN9(LED_BLINK);                                 
                else if(IRMatchMode == STATE_MATCH_NUM0)                   //得到匹配值0
                    SetLEDOFF(LED_BLINK);
            }
            if(MainDev.mDeviceType==DEVICE_IS_10S1)
            {
                if(IRMatchMode == STATE_MATCH_NUM1)                     //得到匹配值1	
                    SetLEDIN3(LED_BLINK);	                
                else if(IRMatchMode == STATE_MATCH_NUM2)                   //得到匹配值2
                    SetLEDIN4(LED_BLINK);
                else if(IRMatchMode == STATE_MATCH_NUM3)                   //得到匹配值3	
                    SetLEDIN5(LED_BLINK);	
                else if(IRMatchMode == STATE_MATCH_NUM4)                   //得到匹配值4
                    SetLEDIN6(LED_BLINK);            
                else if(IRMatchMode == STATE_MATCH_NUM5)                     //得到匹配值5	
                    SetLEDIN7(LED_BLINK);	                
                else if(IRMatchMode == STATE_MATCH_NUM6)                   //得到匹配值6
                    SetLEDIN11(LED_BLINK);
                else if(IRMatchMode == STATE_MATCH_NUM7)                   //得到匹配值7	
                    SetLEDIN12(LED_BLINK);	
                else if(IRMatchMode == STATE_MATCH_NUM8)                   //得到匹配值8
                    SetLEDIN13(LED_BLINK); 
                else if(IRMatchMode == STATE_MATCH_NUM9)                   //得到匹配值9
                    SetLEDIN14(LED_BLINK);                                 
                else if(IRMatchMode == STATE_MATCH_NUM0)                   //得到匹配值0
                    SetLEDOFF(LED_BLINK);
            } 
            if(MainDev.mDeviceType==DEVICE_IS_8S1)
            {
                if(IRMatchMode == STATE_MATCH_NUM1)                     //得到匹配值1	
                    SetLEDIN3(LED_BLINK);	                
                else if(IRMatchMode == STATE_MATCH_NUM2)                   //得到匹配值2
                    SetLEDIN4(LED_BLINK);
                else if(IRMatchMode == STATE_MATCH_NUM3)                   //得到匹配值3	
                    SetLEDIN5(LED_BLINK);	
                else if(IRMatchMode == STATE_MATCH_NUM4)                   //得到匹配值4
                    SetLEDIN6(LED_BLINK);            
                else if(IRMatchMode == STATE_MATCH_NUM5)                     //得到匹配值5	
                    SetLEDIN11(LED_BLINK);	                
                else if(IRMatchMode == STATE_MATCH_NUM6)                   //得到匹配值6
                    SetLEDIN12(LED_BLINK);
                else if(IRMatchMode == STATE_MATCH_NUM7)                   //得到匹配值7	
                    SetLEDIN13(LED_BLINK);	
                else if(IRMatchMode == STATE_MATCH_NUM8)                   //得到匹配值8
                    SetLEDIN14(LED_BLINK);                              
                else if(IRMatchMode == STATE_MATCH_NUM0)                   //得到匹配值0
                    SetLEDOFF(LED_BLINK);
            }            
            break;            
	}
}

/****************************************************************************
*函数名称:UIFsm()
*函数说明:在各种状态下的指示灯的状况变化
*输    入：无
*输    出：无
******************************************************************************/
void UIFsm(void)
{
    static unsigned char ucLastVal=0;
		static unsigned short usEscCount = 0;
    T_KEY_id tKey;
    
    unsigned char chip=0,port=0;
    
    static unsigned short usEDIDKeyCount = 0;
    static unsigned char SelectEDIDFlag = 0;
    const unsigned char twinkle_count_buf[3]={1,0,2};

    if (usCountVisualIndcation!=0)		//K3000中键灯可视显示，表示所有的灯都亮一下
    {
        usCountVisualIndcation++;
        if(usCountVisualIndcation>=200)
        {
            usCountVisualIndcation=0;
            ucUIRefreshFlag=1;
        }        
    } 
  
    if (ucUIRefreshFlag)
    {
        ucUIRefreshFlag=0;
        UICgh();
    }   

    if (MainDev.mUI==UI_STATE_SWITCH    || MainDev.mUI==UI_STATE_LOCK         || 
 //       MainDev.mUI==UI_STATE_COPY_EDID || 
        MainDev.mUI==UI_STATE_SET_ARC   ||MainDev.mUI==UI_STATE_SET_IN_HDCP   ||
        MainDev.mUI==UI_STATE_SET_COM   || MainDev.mUI==UI_STATE_SET_USB_MODE ||
        MainDev.mUI==UI_STATE_SET_OUT_HDCP||MainDev.mUI==UI_STATE_SET_DELAY)
        tKey=KB_ScanKey();			//得到键值
    else 
    {	//扫描按键，主要在开机设置阶段	
        tKey = ScanKey();
    }
 
    switch(MainDev.mUI)
    { 
        case  UI_STATE_WAIT:
            break;
        
        case  UI_STATE_POWER_ON:		//开机设置
            switch(tKey)
            {
                case KEY_1:		//按IN1键开机，复位	,16S1和10S1，和8S1的输入IN1键位置不一样                    
                case KEY_3:	                    
                    if((MainDev.mDeviceType==DEVICE_IS_16S1)&&(tKey==KEY_1))
                    {
                         MainDev.sUICount=COUNT_UI_BASE_FADE/2;//计数等待，方便计数，没有用延时
                         MainDev.mUI=UI_STATE_RESET;
                         UICgh();
                    }
                    else 
                    {
                        if(tKey==KEY_3)
                        {
                            MainDev.sUICount=COUNT_UI_BASE_FADE/2;//计数等待，方便计数，没有用延时
                            MainDev.mUI=UI_STATE_RESET;
                            UICgh();
                        }
                    }
                    break;
                case KEY_OFFLOCK:	//按OFF+LOCK开机，进入到黑场时间设定		
                    if(MainDev.mFactoryType != DEVICE_IS_LIG)
                        break;
                    MainDev.sUICount=COUNT_UI_BASE_FADE*5;//计数等待
                    MainDev.mUI=UI_STATE_SET_DELAY;
                    UICgh();
                    break;            
                case KEY_OFF:		//按OFF键开机，进入到通讯格式修改 //如果是KRM设备，不应该有修改		
                    if(MainDev.mFactoryType != DEVICE_IS_LIG)
                        break;
                    if (MainDev.mComFormat==KRM3000)
                        MainDev.mComFormat=LIGUO_A;                     
                    ucLastVal = MainDev.mComFormat;
                    MainDev.sUICount=0;
                    MainDev.mUI=UI_STATE_SET_COM;
                    UICgh();
                    break;							
                case KEY_OFF_IN3:
                    if(MainDev.mFactoryType != DEVICE_IS_LIG)
                        break;
                    MainDev.sUICount=COUNT_UI_BASE_FADE;
                    MainDev.mUI=UI_STATE_SET_OUT_HDCP;
                    UICgh();
                    break;
                case KEY_LOCK:
                    MainDev.sUICount=0;
                    MainDev.mUI=UI_STATE_SET_IN_HDCP;		
                    UICgh();
                    break;
                case KEY_16:
                case KEY_15:
                case KEY_14:                   
                    if(((MainDev.mDeviceType==DEVICE_IS_16S1)&&(tKey==KEY_16))||
                       ((MainDev.mDeviceType==DEVICE_IS_10S1)&&(tKey==KEY_15))||
                        ((MainDev.mDeviceType==DEVICE_IS_8S1)&&(tKey==KEY_14))
                    )
                    {
                        ucLastVal = usb_working_mode;
                        MainDev.sUICount=COUNT_BASE_DELAY;                   
                        MainDev.mUI=UI_STATE_SET_USB_MODE;
                        UICgh();                      
                    }
                    else
                        MainDev.mUI=UI_STATE_SWITCH;    //都不满足时需要退出
                    break;
                default:
                    MainDev.sUICount=COUNT_BASE_DELAY;   
                    MainDev.mUI=UI_STATE_SWITCH;
                    break;
            }
            break;
		 
        case  UI_STATE_SET_DELAY:	//设置黑场时间
            switch(tKey)
            {
                case KEY_1:
                case KEY_2:
                case KEY_3:
                case KEY_4:
								case KEY_5:
                case KEY_6:   
                case KEY_7:
                case KEY_8:
                case KEY_9:
                case KEY_10:
								case KEY_11:
                case KEY_12:    
                case KEY_13:
                case KEY_14:
                case KEY_15:
                case KEY_16:                       
                    if(MainDev.mDeviceType==DEVICE_IS_16S1)
                    {
                        MainDev.mBlankDelayTx[0]= tKey-KEY_1;
                        MainDev.mBlankDelayTx[1]= tKey-KEY_1;
                    }
                    else if(MainDev.mDeviceType==DEVICE_IS_10S1)
                    {
                        if((tKey>KEY_2)&&(tKey<KEY_8))
                        {                           
                            MainDev.mBlankDelayTx[0]= tKey-KEY_3;
                            MainDev.mBlankDelayTx[1]= tKey-KEY_3;
                        }
                        else if((tKey>KEY_10)&&(tKey<KEY_16))
                        {
                            MainDev.mBlankDelayTx[0]= tKey-KEY_11+5;
                            MainDev.mBlankDelayTx[1]= tKey-KEY_11+5;
                        }
                    }
                    else if(MainDev.mDeviceType==DEVICE_IS_8S1)
                    {
                        if((tKey>KEY_2)&&(tKey<KEY_7))
                        {
                            MainDev.mBlankDelayTx[0]= tKey-KEY_3;
                            MainDev.mBlankDelayTx[1]= tKey-KEY_3;
                        }
                        else if((tKey>KEY_10)&&(tKey<KEY_15))
                        {
                            MainDev.mBlankDelayTx[0]= tKey-KEY_10+4;
                            MainDev.mBlankDelayTx[1]= tKey-KEY_10+4;
                        }
                    }
                    UICgh();
                    break;
                case KEY_LOCK:
                    //Press and not release.
                    SaveNVRAM(EEPROM_SWITCH_DLY);
                    MainDev.sUICount=COUNT_UI_BASE_FADE;
                    MainDev.mUI=UI_STATE_SWITCH;
                    break;
                case KEY_10_SEC: 
                case KEY_OFF:
                    MainDev.sUICount=0;
                    MainDev.mUI=UI_STATE_SWITCH;
                    UICgh();
                    break;
                default:
					//UICgh();
                    break;
            }
            break;

        case  UI_STATE_SET_COM:
            switch(tKey)
            {
                case KEY_OFF:		//在OFF键改通讯格式的只有在利国设备上有
                    SetAllINLED(LED_OFF);
                    if ((MainDev.mComFormat!=LIGUO_A)&&(MainDev.mComFormat!=LIGUO_B))
                        MainDev.mComFormat=LIGUO_A;  
                    //调试时，可以设置3种格式                            
                    if (MainDev.mComFormat==LIGUO_A)
                        MainDev.mComFormat=LIGUO_B;
                    else if (MainDev.mComFormat==LIGUO_B)
                        MainDev.mComFormat=LIGUO_A;     
                           
                    UICgh();     
                    break;
                case KEY_LOCK:
                    //save com mode
                    SaveNVRAM(EEPROM_FORMAT);
                    MainDev.sUICount=0;
                    MainDev.mUI=UI_STATE_SWITCH;
                    UICgh();
                    break;
                case KEY_10_SEC:
                    MainDev.mComFormat =(tagComFormat) ucLastVal;
                    MainDev.sUICount=0;
                    MainDev.mUI=UI_STATE_SWITCH;
                    UICgh();
                    break;
                default:
                    break;
            }
            break;
           
        case  UI_STATE_SET_USB_MODE:
            if (MainDev.sUICount==0)
            {
                switch(tKey)
                {
                    case KEY_14:
                    case KEY_15:
                    case KEY_16:
                        if(((MainDev.mDeviceType==DEVICE_IS_16S1)&&(tKey==KEY_16))||
                            ((MainDev.mDeviceType==DEVICE_IS_10S1)&&(tKey==KEY_15))||
                            ((MainDev.mDeviceType==DEVICE_IS_8S1)&&(tKey==KEY_14))
                        )
                        {
                             if(usb_working_mode==0) //0 is CDC, 1 is massage
                                usb_working_mode=1;
                             else if (usb_working_mode==1)
                                usb_working_mode=0; 
                             UICgh();                        
                        }
                        break;
                    case KEY_LOCK:
                        MainDev.mUI=UI_STATE_SWITCH;
                        UICgh();
                        //SaveNVRAM(EEPROM_USB_PORT_MODE);   //USB功能不保存
                        break;
                    case KEY_10_SEC:
                    case KEY_OFF:
                        usb_working_mode =ucLastVal;
                        MainDev.sUICount=0;
                        MainDev.mUI=UI_STATE_SWITCH;
                        UICgh();
                        break;
                    default:
                        break;
                }
            }
            break;

        case  UI_STATE_SET_IN_HDCP:
            switch(tKey)
            {
                case KEY_1:
                case KEY_2:
                case KEY_3:
                case KEY_4:
                case KEY_5:
                case KEY_6:  
                case KEY_7:
                case KEY_8:
                case KEY_9:
                case KEY_10:
                case KEY_11:
                case KEY_12:    
                case KEY_13:
                case KEY_14:
                case KEY_15:
                case KEY_16: 
                    if(MainDev.mDeviceType==DEVICE_IS_16S1)
                    {
                        chip = get_In_chip_ch(tKey)>>4;
                        port = get_In_chip_ch(tKey)&0x0f;
                    }
                    else if(MainDev.mDeviceType==DEVICE_IS_10S1)
                    {
                        if((tKey>KEY_2)&&(tKey<KEY_8))
                        {
                            chip = get_In_chip_ch(tKey-KEY_3)>>4;
                            port = get_In_chip_ch(tKey-KEY_3)&0x0f;
                        }
                        else if((tKey>KEY_10)&&(tKey<KEY_16))
                        {
                            chip = get_In_chip_ch(tKey-KEY_11+5)>>4 ;
                            port = get_In_chip_ch(tKey-KEY_11+5)&0x0f;
                        }
                    }
                    else if(MainDev.mDeviceType==DEVICE_IS_8S1)
                    {
                        if((tKey>KEY_2)&&(tKey<KEY_7))
                        {
                            chip = get_In_chip_ch(tKey-KEY_3)>>4;
                            port = get_In_chip_ch(tKey-KEY_3)&0x0f;
                        }
                        else if((tKey>KEY_10)&&(tKey<KEY_15))
                        {
                            //chip= get_In_chip_ch(tKey-KEY_10+4)>>4;
                            //port = get_In_chip_ch(tKey-KEY_10+4)&0x0f;
													
                            chip = get_In_chip_ch(tKey-KEY_11+4)>>4 ;
                            port = get_In_chip_ch(tKey-KEY_11+4)&0x0f;
                        }
                    }
                    if (MainDev.mInHDCPState[chip][port]==INHDCP_ON)
                        MainDev.mInHDCPState[chip][port]=INHDCP_CLOSE;
                    else if (MainDev.mInHDCPState[chip][port]==INHDCP_CLOSE)
                        MainDev.mInHDCPState[chip][port]=INHDCP_ON; 
                    UICgh();
                    break;
                case KEY_LOCK_3_SEC://、、KEY_LOCK:
                    MainDev.mUI=UI_STATE_SWITCH;                   
                    SaveNVRAM(EEPROM_INHDCP_MODE);
                    UICgh();
                    break;
                case KEY_OFF:
                case KEY_10_SEC:
                    MainDev.mUI=UI_STATE_SWITCH;
                    UICgh();
                    break;
                default:
                    break;
            }
            break;
        
        case  UI_STATE_SET_OUT_HDCP:
            switch(tKey)
            {
                case KEY_3:
                    if (MainDev.sUICount==0)
                    {
                        if (ConDev.mConFollowEncrption==FE_FOLLOW_IN)//(MainDev.mFollowEncrption==FE_FOLLOW)
                        {
                            ConDev.mConFollowEncrption[0]=FE_FORCE_DISCRP;
                        }else if (ConDev.mConFollowEncrption[0]==FE_FORCE_DISCRP)//(MainDev.mFollowEncrption==FE_FORCE_DISCRP)
                        {
                            ConDev.mConFollowEncrption[0]=FE_FOLLOW_IN;
                        }
                        UICgh();
                    }
                    break;
                case KEY_LOCK:
                    //save follow
                    SaveNVRAM(EEPROM_FOLLOWENCRY);
                    MainDev.sUICount=0;
                    MainDev.mUI=UI_STATE_SWITCH;
                    UICgh();                   
                    break;
                case KEY_OFF:
                case KEY_10_SEC:
                    MainDev.mUI=UI_STATE_SWITCH;
                    UICgh();
                    break;
                default:
                    break;
            }   
            break;
            
        case  UI_STATE_RESET:	//复位操作
//            switch(tKey)
//            {
                //case KEY_1:
                    if (MainDev.sUICount==0)
                    {//Reset
                      //  SET_EVENT(EV_RESET);	
						ucKeyReset = 1;
                        MainDev.sUICount=0;
                        MainDev.mUI=UI_STATE_SWITCH;
						//MainDev.mUI=UI_STATE_POWER_ON;
                        //UICgh();
                    }
//                    break;
//                default:
//                    MainDev.sUICount=0;
//                    MainDev.mUI=UI_STATE_SWITCH;
//                    UICgh();
//                    break;
          //  }      
            break;   
        case  UI_STATE_LOCK:
            if (MainDev.sUICount==0) //&& ucLEDCount==0)			
                SetLEDLOCK(LED_ON);
            switch(tKey)
            {
                case KEY_1:
                case KEY_2:
                case KEY_3:
                case KEY_4:
                case KEY_5:
                case KEY_6:
                case KEY_7:
                case KEY_8:
                case KEY_9:
                case KEY_10:
                case KEY_11:
                case KEY_12:
                case KEY_13:
                case KEY_14:
                case KEY_15:
                case KEY_16:
                case KEY_OFF:
                case KEY_EDID:
                case KEY_LOCK:
		#ifdef 	SUPPORT_AUTO_SWITCH
                case KEY_A_M:
                case KEY_LC_P:
                case KEY_LC_3_SEC:    
		#endif
                    MainDev.sUICount=(COUNT_UI_BASE_FADE/TIMER_BASE)*1;
                    SetLEDLOCK(LED_BLINK);	
                    break;
                case KEY_LOCK_3_SEC:
                    MainDev.sUICount=0;
                    MainDev.mUI=UI_STATE_SWITCH;
                    UICgh();
                    break;
                default:
                    break;
            }
            break;

        case  UI_STATE_COPY_EDID:		//按KRM需求，设计按键弹起实现改功能
            if (MainDev.sUICount==0)
            {
                MainDev.sUICount=COUNT_UI_BASE_FADE%20; 
                UICgh();  
                if (ucCpyEDIDCount>=ucMaxValidPortNum)
                {
                    MainDev.sUICount=0;
                    usSelectEDIDCount = 0;
                    MainDev.mUI=UI_STATE_SWITCH;
                    UICgh();
                    SET_EVENT(EV_EDID);
                }
            }
            break;
        case  UI_STATE_SELECT_EDID:		//按KRM需求，设计按键弹起实现改功能
            if (MainDev.sUICount==0)
            {
                switch(tKey)
                {
                    case KEY_1:
                    case KEY_2:
                    case KEY_3:
                    case KEY_4:
                    case KEY_5:
                    case KEY_6:  
                    case KEY_7:
                    case KEY_8:
                    case KEY_9:
                    case KEY_10:
                    case KEY_11:
                    case KEY_12:    
                    case KEY_13:
                    case KEY_14:
                    case KEY_15:
                    case KEY_16:
                         if (ucLastVal==tKey)
                            break;
                         ucLastVal=tKey;
                         MainDev.mEDIDSelPort =  (MainDev.mEDIDSelPort^(1<<tKey));    
                         usEscCount = 0;
                         UICgh();
                         break;
                    case KEY_EDID:
                        if(ucLastVal!=KEY_EDID)
                        {
                            ucLastVal=KEY_EDID;
                            usSelectEDIDCount=1;
                            usEscCount = 0;
                            
                            SelectEDIDFlag = 1;
                        }
                        else if(ucLastVal==KEY_EDID)
                        {
                            usEDIDKeyCount++ ;
                            if(usEDIDKeyCount>120)
                            {
                                usEDIDKeyCount = 0;
                                usEscCount = 0;
                                SelectEDIDFlag = 0;
                                MainDev.mUI=UI_STATE_COPY_EDID;
                                ucCpyEDIDCount =0;
                                UICgh();
                            }
                        }
                        break;
                    case KEY_OFF:
                        ucLastVal=KEY_NONE;
                        usEscCount = 0;
                        MainDev.sUICount=0;
                        MainDev.mUI=UI_STATE_SWITCH;
                        UICgh();
                        break;
                    default:
                       //超时退出当前状态
                        ucLastVal=KEY_NONE;
                        usEDIDKeyCount = 0;
                        if(SelectEDIDFlag==1)
                        {
                            SelectEDIDFlag  = 0;
                            if(MainDev.mInEDIDChFrom ==  OUT_PORT1)
                                MainDev.mInEDIDChFrom =  OUT_PORT2;
                            else if(MainDev.mInEDIDChFrom ==  OUT_PORT2)
                                MainDev.mInEDIDChFrom =  DEF_EDID_PORT;
                            else if(MainDev.mInEDIDChFrom ==  DEF_EDID_PORT)
                                MainDev.mInEDIDChFrom =  OUT_PORT1;
                        }

                        if(usSelectEDIDCount!=0)
                        {
                            usSelectEDIDCount++;
                            if(usSelectEDIDCount==8) 
                            {
                                ucSelectEDIDState = 1;  //LED OFF
                                UICgh();
                            }
                            if(usSelectEDIDCount==14)
                            {
                                ucSelectEDIDState = 0;  //LED ON
                                UICgh();
                            }
                            if(usSelectEDIDCount>20)
                            {
                                ucSelectEDIDState = 1;  //LED OFF
                                UICgh();
                                if(twinkle_count<twinkle_count_buf[MainDev.mInEDIDChFrom])
                                {
                                    twinkle_count++;
                                    usSelectEDIDCount = 1;
                                }
                                else
                                {
                                    usSelectEDIDCount =0;
                                    twinkle_count =0;
                                }
                            }
                        }
                        else
                        {
                            usEscCount++;
                            if(usEscCount>=600)			//1000 约为20s
                            {
                                usEscCount = 0;
                                MainDev.sUICount=0;
                                MainDev.mUI=UI_STATE_SWITCH;
                                UICgh();
                            }
                        }
                        break;
                }
            }
            break;
      case  UI_STATE_SET_ARC:		//设置音频输出是ARC输出还是解嵌音频输出
            if (MainDev.sUICount==0)
            {
                switch(tKey)
                {
                    case KEY_3:                    
                        if(oldAudioSelect == AUDIO_ARC)
                        {
                            oldAudioSelect = AUDIO_DE_Embed;	
                        }					
                        else if(oldAudioSelect == AUDIO_DE_Embed)
                        {
                            oldAudioSelect = AUDIO_ARC;
                        }                         
                        usEscCount = 0;
                  
                        MainDev.sUICount=0;
                        MainDev.mUI=UI_STATE_SET_ARC;
                        UICgh();
                        break;
                   case KEY_LOCK:   
                        if((MainDev.mAudioOutState!=AUDIO_ARC)&&(oldAudioSelect == AUDIO_ARC))
                            ucArcResetFlag = 1;
                        MainDev.mAudioOutState = oldAudioSelect;
                        //save follow
                        SaveNVRAM(EEPROM_AUDIO_MODE);
                        usEscCount = 0;
                        MainDev.sUICount=0;                
                        MainDev.mUI=UI_STATE_SWITCH;
                        UICgh();
                        
                        break;
                   case KEY_OFF:
                        usEscCount = 0;
                        MainDev.sUICount=0;
                        MainDev.mUI=UI_STATE_SWITCH;
                        UICgh();
                        break;
                   default:
                       //超时退出当前状态
                        usEscCount++;
                        if(usEscCount>=600)			//1000 约为20s
                        {
                            usEscCount = 0;
                            MainDev.sUICount=0;
                            MainDev.mUI=UI_STATE_SWITCH;
                            UICgh();
                        }
                       break;
                }
            }
            break;
   
        case  UI_STATE_SWITCH:
            if (MainDev.sUICount==0)
            {
                switch(tKey)
                {
                    case KEY_1:
                    case KEY_2:
                    case KEY_3:
                    case KEY_4:
                    case KEY_5:
                    case KEY_6:  
                    case KEY_7:
                    case KEY_8:
                    case KEY_9:
                    case KEY_10:
                    case KEY_11:
                    case KEY_12:    
                    case KEY_13:
                    case KEY_14:
                    case KEY_15:
                    case KEY_16:                         

                            if(MainDev.mTxClose==TX_OFF)
                                SET_EVENT(EV_OUT_0_SIG);  //这地方应该先后顺序问题会造成切换有错
                            MainDev.mTxClose=TX_ON;
                            if(MainDev.mDeviceType==DEVICE_IS_16S1)
                            {
                                MainDev.mNextSelectChip = get_In_chip_ch(tKey)>>4;
                                MainDev.mNextSelectInPort = get_In_chip_ch(tKey)&0x0f;
                            }
                            else if(MainDev.mDeviceType==DEVICE_IS_10S1)
                            {
                                if((tKey>KEY_2)&&(tKey<KEY_8))
                                {
                                    MainDev.mNextSelectChip = get_In_chip_ch(tKey-KEY_3)>>4;
                                    MainDev.mNextSelectInPort = get_In_chip_ch(tKey-KEY_3)&0x0f;
                                }
                                else if((tKey>KEY_10)&&(tKey<KEY_16))
                                {
                                    MainDev.mNextSelectChip = get_In_chip_ch(tKey-KEY_11+5)>>4 ;
                                    MainDev.mNextSelectInPort = get_In_chip_ch(tKey-KEY_11+5)&0x0f;
                                }
                            }
                            else if(MainDev.mDeviceType==DEVICE_IS_8S1)
                            {
                                if((tKey>KEY_2)&&(tKey<KEY_7))
                                {
                                    MainDev.mNextSelectChip = get_In_chip_ch(tKey-KEY_3)>>4;
                                    MainDev.mNextSelectInPort = get_In_chip_ch(tKey-KEY_3)&0x0f;
                                }
                                else if((tKey>KEY_10)&&(tKey<KEY_15))
                                {
                                    //MainDev.mNextSelectChip = get_In_chip_ch(tKey-KEY_10+4)>>4;
                                    //MainDev.mNextSelectInPort = get_In_chip_ch(tKey-KEY_10+4)&0x0f;
																		//Xiey use the following
                                    MainDev.mNextSelectChip = get_In_chip_ch(tKey-KEY_11+4)>>4 ;
                                    MainDev.mNextSelectInPort = get_In_chip_ch(tKey-KEY_11+4)&0x0f;
                                }
                            }
                            if((MainDev.mNextSelectChip!=MainDev.mRxChipSel)||(MainDev.mNextSelectInPort!=MainDev.mRxPortSel))
                                SET_EVENT(EV_CH_SWITCH);
                            
                            if(AutoSw.SwitchMode == AUTO_SWITCH_MODE)	   //只有在自动模式下存在覆盖状态
                                MainDev.OverrideFlag = 1;

                            AutoSw.AutoNoSig10sWait = 0;	//切换操作时，要重新计数
                            SaveNVRAM(EEPROM_SWITCH_STATE);
                            SaveNVRAM(EEPROM_OUT_POWER);                       
                            SET_EVENT(EV_SW_KEY);
                            UICgh();
                            break;
                    case KEY_OFF:
                            if(MainDev.mTxClose==TX_OFF)//if (OutDev[0].mOutPowerOn==OUT_ON)
                                MainDev.mTxClose=TX_ON;//OutDev[0].mOutPowerOn=OUT_OFF;
                            else 
                                MainDev.mTxClose=TX_OFF;//OutDev[0].mOutPowerOn=OUT_ON;  
                            SaveNVRAM(EEPROM_OUT_POWER);         
                        //  SET_EVENT(EV_IN_SIG);      
                            SET_EVENT(EV_OUT_0_SIG);
                            SET_EVENT(EV_SW_KEY);                        
                            UICgh();                           
                            break;
                    #ifdef 	 SUPPORT_AUTO_SWITCH
                    case KEY_A_M:
                    case KEY_LC_P:  
                    case KEY_LC_3_SEC: 
                        if(tKey == KEY_A_M)
                        {
                            if(AutoSw.SwitchMode == MANUAL_SWITCH_MODE)
                            {
                                AutoSw.SwitchMode = AUTO_SWITCH_MODE;
                                SaveNVRAM(EEPROM_SWITCH_MODE);	
                                if(AutoSw.AutoSwMode == PRIORITY_MODE)
                                {
                                    AutoSw.ucAutoSwFlag = 1;
                                    SET_EVENT(EV_AUTO_SWITCH);
                                }					
                            }				
                            else if(AutoSw.SwitchMode == AUTO_SWITCH_MODE)
                            {
                                AutoSw.SwitchMode = MANUAL_SWITCH_MODE;	   
                                SaveNVRAM(EEPROM_SWITCH_MODE);	
                                //只有在自动模式下存在覆盖状态
                                MainDev.OverrideFlag = 0;
                            }				
                        }
                        
                        if(tKey == KEY_LC_P)
                        {
                            if(AutoSw.SwitchMode == AUTO_SWITCH_MODE)
                            {
                                if(AutoSw.AutoSwMode == PRIORITY_MODE)
                                {
                                    AutoSw.AutoSwMode = LAST_CONNECTED_MODE;
                                    SaveNVRAM(EEPROM_AUTOSW_MODE);	
                                    //只有在自动模式状态变化下取消覆盖状态
                                    MainDev.OverrideFlag = 0;
                                }					
                                else if(AutoSw.AutoSwMode == LAST_CONNECTED_MODE)
                                {
                                    AutoSw.AutoSwMode = PRIORITY_MODE;
                                    SaveNVRAM(EEPROM_AUTOSW_MODE);	
                                    AutoSw.ucAutoSwFlag = 1;
                                    SET_EVENT(EV_AUTO_SWITCH);
                                    //只有在自动模式状态变化下取消覆盖状态
                                    MainDev.OverrideFlag = 0;
                                }		
                            }							
                        }
                        if(tKey ==KEY_LC_3_SEC)        
                        {
                            if(AutoSw.SwitchMode == AUTO_SWITCH_MODE)
                            {   //长按都是LC模式
                                AutoSw.AutoSwMode = LAST_CONNECTED_MODE;
//                                if(AutoSw.AutoSwMode == LAST_CONNECTED_MODE)
//                                {   //开机是正常模式，不存储级联状态
                                    if(sCascaded==IS_CASCADED_MODE)                                    
                                        sCascaded=NONE_CASCADED_MODE;                                                                          
                                    else if(sCascaded==NONE_CASCADED_MODE)                                    
                                        sCascaded=IS_CASCADED_MODE;                                    
//                                }
                            }   
                        }                            
                        UICgh();
                        break;														
                        #endif
                    case KEY_LOCK_3_SEC:
                        MainDev.sUICount=0;
                        MainDev.mUI=UI_STATE_LOCK;
                        UICgh();
                        break;
                    case KEY_OFF_IN3_3SEC:                      //设置ARC输出还是解嵌音频
                        MainDev.sUICount=0;
                        usEscCount = 0;
                        MainDev.mUI=UI_STATE_SET_ARC;                        
                        oldAudioSelect = MainDev.mAudioOutState;
                        UICgh();
                        break;
                
                    case KEY_EDID:	
                        usEscCount = 0;
                        MainDev.mEDIDSelPort  = 0x0000;
                        MainDev.sUICount=0;
                        MainDev.mUI=UI_STATE_SELECT_EDID;
                        usSelectEDIDCount =1;
                        MainDev.mInEDIDChFrom =  OUT_PORT1;//DEF_EDID_PORT;
												SetLEDEDID(LED_BLINK);
                        SetAllINLED(LED_OFF);
                        twinkle_count =0;
                        break;
                    case KEY_IN3IN4_3_SEC:
                        if(IRMatchMode==NO_MATCH_STATE)
                            IRMatchMode = STATE_MATCH_TIM_START;
                        MainDev.mUI = UI_STATE_MATCH_IR;
                        usIRMatchEscCount = 1;//开始计时
                        UICgh();
                	default:
                        break;
                }
            }            
            break;
        case  UI_STATE_MATCH_IR:
            if (MainDev.sUICount==0)
            {
                switch(tKey)
                {
                    case KEY_1:
                    case KEY_2:
                    case KEY_3:                        
                    case KEY_4:
                    case KEY_5:   
                    case KEY_6:
                    case KEY_7:                        
                    case KEY_8:
                    case KEY_9: 
                    case KEY_10:     
                        usIRMatchEscCount = 1;	//重新计时
                        break;
                    case KEY_LOCK:
                        SaveNVRAM(EEPROM_IR_MATCH_TIME);
                        MainDev.sUICount=0;
                        MainDev.mUI=UI_STATE_SWITCH;
                        IRMatchMode=NO_MATCH_STATE;
                        UICgh();
                        break;
                    default:
                        usIRMatchEscCount++;
                        if(usIRMatchEscCount>=600)			//1000 约为20s
                        {
                            usIRMatchEscCount = 0;
                            MainDev.sUICount=0;
                            MainDev.mUI=UI_STATE_SWITCH;
                            IRMatchMode=NO_MATCH_STATE;
                            UICgh();
                        }
                        break;
                }

            }
     
        default:
            break;
    } 

}


/****************************************************************************
*函数名称:DipUIFsm()
*函数说明:在DIP拨码的状态
*输    入：无
*输    出：无
******************************************************************************/
void DipUIFsm(void)
{
	  MainDev.DipStatu=HAL_Cpld_Read(0x01);
	  MainDev.DipStatu &=0x03;
	
    //if(HAL_Cpld_Read(0x01)&(1<<0)) //DIP 1 -->SW1 
    if(MainDev.DipStatu&0x01) //DIP 1 -->SW1 
    {    
        if(MainDev.mAudioMixMode!=MIX_AUDIO_AUTO)
        {
           MainDev.mAudioMixMode=MIX_AUDIO_AUTO;        
           SET_EVENT(EV_OUT_0_SIG);
        }

    }
    else
    {    
        if(MainDev.mAudioMixMode!=MIX_AUDIO_FORCE)
        {
           MainDev.mAudioMixMode=MIX_AUDIO_FORCE;         //模拟音频为I2S输入
           SET_EVENT(EV_OUT_0_SIG);
        }
    }          
}



