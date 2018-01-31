/**
 * \file main.c
 */	

#include "RTL.h"

#include "lig_types.h"		  
#include "lig_platform.h"

#include "lpc17xx_timer.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_adc.h"

#include "Net_Config.h"					 
#include "i2c_inf.h"

#include "serial.h"
#include "debug_console.h"
			 
#include "sst25vf016b.h"
#include "flashdisk.h"

//#ifdef SUPPORT_CEC_APP
    #include "sii9575_lig_app_cec.h"
//#endif

#include "edid.h"
#include "diskio.h"	
#include "ff.h"
#include "key.h"

#include "main.h"
#include "sii9575_lig_app.h"
#include "command.h"

#ifdef SUPPORT_USB_COMM
	#include "cdcuser.h"
	#include "usb.h"
	#include "usbhw.h"
#endif

#ifdef SUPPORT_COMM_FORMAT_LIG_B
  #include "BFormat.h"
#endif

#include "krm3000.h"
  
#include "StepIn.h"
#include "pcm5142_drv_api.h"
#include "Wm8805_drv_api.h"
#include "Ir.h"
#include "vs100_m25_flash.h"
#include "vs100_flashdisk.h"
/*======================================
 * Local defines
 *=====================================*/
 
//=============================================================================================
//TCP,UDP����ͨѶ�������Լ�����
#define NUM_CONNECTIONS  (4)      
#define NUM_UDP_CONNECTIONS (1)

U8 tcp_soc[NUM_CONNECTIONS];   

U8 udp_soc[NUM_UDP_CONNECTIONS];  
U8 udp_remip[NUM_UDP_CONNECTIONS][4];
U16 udp_remport[NUM_UDP_CONNECTIONS];
//��ӵ�TCP�շ�����
void LIGWEB_TcpSendData(unsigned char ucSockIndex,unsigned char *pBuF, unsigned short usLen);

void LIGWEB_UdpSendData(unsigned char ucSockIndex,unsigned char *pBuF, unsigned short usLen);


//=============================================================================================
//WEB,IP�ȹ���ȫ�ֲ���
extern U8 lhost_name[];
static void timer_poll (void);					
void MAIN_EnableNetPolling(unsigned char EnDis);
unsigned char MAIN_IsNetPollingEnabled(void);
void MAIN_GetAutoIp (void);
    
//Ĭ�ϵ���������
UINT8  const DEF_NET_IP[]   = {192,168,1,39};
UINT8  const DEF_NET_MASK[] = {255,255,0,0};
UINT8  const DEF_NET_GATEWAY[] = {192,168,0,254};
UINT8  const DEF_NET_MAC[] = {0,1,2,50,60,70};
UINT16 const DEF_UDP_PORT  =50000;
UINT16 const DEF_TCP_PORT  =5000;
UINT8  const DEF_NET_DHCP  =0;
UINT8  const DEF_NET_SECURE=0;

//����LoopNetִ�зŵ���ʱ����,Ϊ���ȶ�,��Ҫȷ���Ƿ����ж�ʱ��ִ��
unsigned char NetPollFlag = 0;	

//Net�Ƿ���Ҫ��λ�ı�־
UINT16 net_reset_flag=0;

//IP���������޸ĵı�־
unsigned char CommNetFlag = 0;

//������DHCP=1��ʱ�������
unsigned char ucAutoIp[4]={192,168,1,39};

UINT8 net_dhcp_enable=DEF_NET_DHCP;

UINT8 NetSecureMode =DEF_NET_SECURE; //
UINT8 Ipdata[4]     = {192,168,1,39};
UINT8 GateWaydata[4]= {192,168,0,254};
UINT8 NetMaskdata[4]= {255,255,255,0};
UINT8 NetMacdata[6] = {0x1E,0x30,0x6C,0xA2,0x45,0x5E};
UINT16 tcpPort =DEF_TCP_PORT;  
UINT16 udpPort =DEF_UDP_PORT;


unsigned char uclastIp[4]={0xff,0xff,0xff,0xff};

#define MY_IP localm[NETIF_ETH].IpAdr

unsigned char ArgTBuF[16];
									

BOOL tick; 
U32  dhcp_tout;	
extern LOCALM localm[];      //Local Machine Settings     
#define DHCP_TOUT   700      //700 for DHCP function  // DHCP timeout 5 seconds      //

unsigned char DhcpChgFlag = 0;
unsigned char DhcpChgCommFlag=0;

//=============================================================================================
//���������ô����в��˷���
//#define NET_BUF_SIZE                20 //512 //1024//(512)//(256)               // serial buffer in bytes (power 2)
//#define NET_BUF_MASK               (NET_BUF_SIZE-1)  // buffer size mask

///* Buffer read / write macros */
//#define NET_BUF_RESET(idx,netBuf)      (netBuf[idx].rdIdx = netBuf[idx].wrIdx = 0)
//#define NET_BUF_WR(idx,netBuf, dataIn) (netBuf[idx].data[NET_BUF_MASK & netBuf[idx].wrIdx++] = (dataIn))
//#define NET_BUF_RD(idx,netBuf)         (netBuf[idx].data[NET_BUF_MASK & netBuf[idx].rdIdx++])
//#define NET_BUF_EMPTY(idx,netBuf)      (netBuf[idx].rdIdx == netBuf[idx].wrIdx)
//#define NET_BUF_FULL(idx,netBuf)       (netBuf[idx].rdIdx == netBuf[idx].wrIdx+1)
//#define NET_BUF_COUNT(idx,netBuf)      (NET_BUF_MASK & (netBuf[idx].wrIdx - netBuf[idx].rdIdx))

//// buffer type
//typedef struct __NET_BUF_T {
//  unsigned char data[NET_BUF_SIZE];
//  unsigned int wrIdx;
//  unsigned int rdIdx;
//} NET_BUF_T;

//unsigned long          net_txRestart[NUM_CONNECTIONS];                  // NZ if TX restart is required
//unsigned short         net_lineState[NUM_CONNECTIONS];                  // ((msr << 8) | (lsr))

//NET_BUF_T              net_out[NUM_CONNECTIONS];                        // Serial data buffers

//NET_BUF_T              udp_out[NUM_UDP_CONNECTIONS];



UINT8 usb_working_mode=1;//0 is CDC, 1 is massage

//����usb�õĶ���̫�̣��������⣬�������¶���usb����,���������ô����в��˷���
//#define USB_SER_BUF_SIZE              20 //(512)               // USB serial buffer in bytes (power 2)
//#define USB_SER_BUF_MASK               (USB_SER_BUF_SIZE-1ul)  // buffer size mask

///* Buffer read / write macros */
//#define USB_SER_BUF_RESET(usbserBuf)      (usbserBuf.rdIdx = usbserBuf.wrIdx = 0)
//#define USB_SER_BUF_WR(usbserBuf, dataIn) (usbserBuf.data[USB_SER_BUF_MASK & usbserBuf.wrIdx++] = (dataIn))
//#define USB_SER_BUF_RD(usbserBuf)         (usbserBuf.data[USB_SER_BUF_MASK & usbserBuf.rdIdx++])
//#define USB_SER_BUF_EMPTY(usbserBuf)      (usbserBuf.rdIdx == usbserBuf.wrIdx)
//#define USB_SER_BUF_FULL(usbserBuf)       (usbserBuf.rdIdx == usbserBuf.wrIdx+1)
//#define USB_SER_BUF_COUNT(usbserBuf)      (USB_SER_BUF_MASK & (usbserBuf.wrIdx - usbserBuf.rdIdx))


//// buffer type
//typedef struct __USB_SER_BUF_T {
//  unsigned char data[USB_SER_BUF_SIZE];
//  unsigned int wrIdx;
//  unsigned int rdIdx;
//	
//  unsigned int TempWrIdx;
//  unsigned int TempRrdIdx;
//	
//} USB_SER_BUF_T;

//void MAIN_ResetTxBuF(unsigned char ucEn);

//unsigned long          usbser_txRestart;                  // NZ if TX restart is required
//unsigned short         usbser_lineState;                  // ((msr << 8) | (lsr))
//USB_SER_BUF_T              usbser_out;                        // Serial data buffers


//add rcq                                                   
UINT8 Mtx_Mode=0;

#define GetSysState() 	MainDev.mStateD0	
#define GetRxState(a,b) 	MainDev.mStateD1[a][b]
#define GetTxState(a,b) 	MainDev.mStateD2[a][b]
#define GetOutState(a,b) 	MainDev.mStateD3[a][b]

/*======================================
 * File
 *=====================================*/
FATFS FatFs;				/* File system object for each logical drive */
FIL File;				/* File objects */
DIR Dir;					/* Directory object */

unsigned int byteW;

FIL FileUpload;/* File objects for upload*/

FILINFO Finfo;
//FILINFO Finfo;
FRESULT getdisk_status=FR_OK;
//static const char* Sys_log_path="\\system_log.log";

#define MAX_FILE_SIZE   2
//#define SYS_BUF_SIZE  (1024*MAX_FILE_SIZE) 
//__align(4) UNS_8  SYS_Buf[SYS_BUF_SIZE];  
////__align(4) UNS_8  SYS_RdBuf[SYS_BUF_SIZE];





/*======================================
 * Externals
 *=====================================*/
tagMainDev 	MainDev; 	
tagConDev 	ConDev;
tagInDev 		InDev[MAX_CHIP_INDEX][6];
tagOutDev 	OutDev[MAX_CHIP_INDEX][2];//OutDev[1];
tagMonDev 	MonDev[2];

tagAutoSwitch AutoSw;

const unsigned char  CHAR_BIT_TAB[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
		                         
const unsigned short SHORT_BIT_TAB[16]={
	                            0x0001,0x0002,0x0004,0x0008,
	                            0x0010,0x0020,0x0040,0x0080,
	                            0x0100,0x0200,0x0400,0x0800,
	                            0x1000,0x2000,0x4000,0x8000};	

unsigned char ucUIRefreshFlag = 0;
unsigned short usCountVisualIndcation=0;
unsigned char ucCommIDMark = UART_QUEUE;
unsigned short WaitStableCount = 0; 
unsigned char ucPowerUpSwFlag = 0;	//�����Զ��л���ǣ����ڵȴ����е����źż�����
unsigned char ucResetFlag=0;//ǿ�Ƹ�λ��ǣ�

unsigned short usTimeLevel = 0;	//�ر����5v��ʱ������

unsigned short ucConnectFlag = 0;	  //��Ϊ�Ƿ����ź����ӱ�ǣ�������ʾKRMҪ���

unsigned char UartChgFlag= 0;   
tagCasMode      sCascaded;      //cascaded mode ,����ģʽ
unsigned short  usCasWait50msCount;
unsigned short  usArcResetWaitCount=0;
unsigned char   ucArcResetFlag = 0;

	
unsigned char ucKeyReset = 0;

unsigned char ucMaxValidChipNum = 3;    //������ЧоƬ���������û���ϰ壬��ЧоƬΪ3��
unsigned char ucMaxValidPortNum = 16;   //������Ч�˿������������豸��ͬ��ͬ��������8��10��16

unsigned char ucMaxOutPortNum = 2;   //������Ч�������

//adc
unsigned short Design_Voltage[MAX_VOL_DOT_NUM];
unsigned long Detect_VolBuf[MAX_VOL_DOT_NUM];

/*======================================
 * Global declarations
 *=====================================*/                           
const unsigned char EDID_array[]={
    /*  0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07   0x08 0x09 0x0a 0x0b 0x0c 0x0d 0x0e 0x0f        */

    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x2D, 0xB2, 0x00, 0x12, 0x01, 0x01, 0x01, 0x01, 
    0xFF, 0x18, 0x01, 0x03, 0x80, 0x34, 0x20, 0x78, 0xE2, 0xB3, 0x25, 0xAC, 0x51, 0x30, 0xB4, 0x26, 
    0x10, 0x50, 0x54, 0xFF, 0xFF, 0x80, 0x81, 0x8F, 0x81, 0x99, 0xA9, 0x40, 0x61, 0x59, 0x45, 0x59, 
    0x31, 0x59, 0x71, 0x4A, 0x81, 0x40, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28, 
    0x55, 0x00, 0x07, 0x44, 0x21, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x32, 0x39, 0x35, 
    0x2D, 0x38, 0x38, 0x33, 0x34, 0x35, 0x30, 0x31, 0x30, 0x30, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x56, 
    0x4D, 0x2D, 0x32, 0x31, 0x34, 0x44, 0x54, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD, 
    0x00, 0x38, 0x4C, 0x1E, 0x53, 0x11, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0xF6, 

    0x02, 0x03, 0x1B, 0xC1, 0x23, 0x09, 0x07, 0x07, 0x48, 0x10, 0x05, 0x84, 0x03, 0x02, 0x07, 0x16, 
    0x01, 0x65, 0x03, 0x0C, 0x00, 0x10, 0x00, 0x83, 0x01, 0x00, 0x00, 0x02, 0x3A, 0x80, 0x18, 0x71, 
    0x38, 0x2D, 0x40, 0x58, 0x2C, 0x45, 0x00, 0x07, 0x44, 0x21, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x80, 
    0x18, 0x71, 0x1C, 0x16, 0x20, 0x58, 0x2C, 0x25, 0x00, 0x07, 0x44, 0x21, 0x00, 0x00, 0x9E, 0x01, 
    0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28, 0x55, 0x00, 0x07, 0x44, 0x21, 0x00, 0x00, 
    0x1E, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00, 0x07, 0x44, 0x21, 
    0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 

};//ע�⣬Si9575��Ĭ��EDIDֻ֧��RGB����֧��YUV,��Ƶͨ����Ϊ2 channel


unsigned char uc256Buf[256];
unsigned char uc16OfBuf[16];

unsigned char webEdidBuf[256];
unsigned char webEdidRequestFlag=0;//1 is input, 2 is output, 3 is Default
unsigned char webEdidRequestID=0;//0 is default, 1 is from 

unsigned  short NoSigWaitTime = 0;      //�ֶ�ģʽ��û�������ź��л���480p����

unsigned  short usSwAnalogOut50msCount = 0;      //�л���ģ��ֱͨ��Ƶ���

#define  CEC_DEV_ADD_DEF	0xff
unsigned char ucCECDevAdd=CEC_DEV_ADD_DEF;

unsigned char AutoSwitchList[COUNT_IN];			//�Զ��л��б�

unsigned char   Debug5vFlag = 0;
unsigned char   DebugErrFlag = 0;

void LoopNVRAM(void);

void Hardware_Init_Si9575_VS100TX(void);

void VS100TX_Init_Mode(unsigned char mode);
    
void CopyEDID2In(unsigned char ChipIndex,unsigned char reset);

void SetLCModePortList(unsigned char mport,unsigned char isValid);
void GetAutoModeChannel(void);
void GotoAutoSwitchChannel(void);
void PowerUpGetInSignal(void);

    unsigned short usDetectCount = 0;
#ifdef SUPPORT_VOLTAGE_DETECT
    unsigned char ADCNum = 0;

    uint32_t Get_Voltage(ADC_CHANNEL_SELECTION nADC);
    void Detect_Voltage(ADC_CHANNEL_SELECTION nADC);
    
    void VoltageDetect_Switch(unsigned char ucVolNum);
    void Handle_Voltage(void);
#endif

unsigned char K_Result=PASS_ONLY;
unsigned char CurrentCom=UART_QUEUE; //����SendByteû��ʹ�ö˿�,���ﱣ�浱ǰ���ڴ�����Ǹ��˿�




/*=========================================================================
 *������ת���������ϴ���upload��ִ��Reset�������Ҫ��������ת����ַ0��ʼ
 *
 *========================================================================*/                                     
__asm void upload_jump( uint32_t address ){
   LDR SP, [R0]		;Load new stack pointer address
   LDR PC, [R0, #4]	;Load new program counter address
}

void upload_user_start_code(uint32_t address)
{
    uint32_t i;
    #define SECTOR_0_START  0x00000000
	SysTick->VAL   =  (0x00);                                                              /* Load the SysTick Counter Value */
    SysTick->CTRL = (0 << 2) | (0<<0) | (0<<1); /* Enable SysTick IRQ and SysTick Timer */
	
	LPC_PINCON->PINSEL0=0x00000000;	 
	LPC_PINCON->PINSEL1=0x00000000;	  
	LPC_PINCON->PINSEL2=0x00000000; 
	LPC_PINCON->PINSEL3=0x00000000; 
	LPC_PINCON->PINSEL4=0x00000000; 
	LPC_PINCON->PINSEL5=0x00000000; 
	LPC_PINCON->PINSEL6=0x00000000; 
	LPC_PINCON->PINSEL7=0x00000000; 
	LPC_PINCON->PINSEL8=0x00000000;	 
	LPC_PINCON->PINSEL9=0x00000000;
	LPC_PINCON->PINSEL10=0x00000000; 
	
	LPC_PINCON->PINMODE0=0x00000000;	 
	LPC_PINCON->PINMODE1=0x00000000;	  
	LPC_PINCON->PINMODE2=0x00000000; 
	LPC_PINCON->PINMODE3=0x00000000; 
	LPC_PINCON->PINMODE4=0x00000000; 
	LPC_PINCON->PINMODE5=0x00000000; 
	LPC_PINCON->PINMODE6=0x00000000; 
	LPC_PINCON->PINMODE7=0x00000000; 
	LPC_PINCON->PINMODE8=0x00000000;	 
	LPC_PINCON->PINMODE9=0x00000000;
	
	LPC_PINCON->PINMODE_OD0=0x00000000;
	LPC_PINCON->PINMODE_OD1=0x00000000;
	LPC_PINCON->PINMODE_OD2=0x00000000;
	LPC_PINCON->PINMODE_OD3=0x00000000;
	LPC_PINCON->PINMODE_OD4=0x00000000;
	
	LPC_GPIO0->FIODIR=0x00000000;
	LPC_GPIO1->FIODIR=0x00000000;
	LPC_GPIO2->FIODIR=0x00000000;
	LPC_GPIO3->FIODIR=0x00000000;
	LPC_GPIO4->FIODIR=0x00000000;	
	
	NVIC_DisableIRQ(TIMER0_IRQn);
	NVIC_DisableIRQ(TIMER1_IRQn);
	NVIC_DisableIRQ(TIMER2_IRQn);
	NVIC_DisableIRQ(TIMER3_IRQn);
	
	NVIC_DisableIRQ(UART0_IRQn);
	NVIC_DisableIRQ(UART1_IRQn);
	NVIC_DisableIRQ(UART2_IRQn);
	NVIC_DisableIRQ(UART3_IRQn);
	
	NVIC_DisableIRQ(I2C0_IRQn);
	NVIC_DisableIRQ(I2C1_IRQn);
	NVIC_DisableIRQ(I2C2_IRQn);
	
	NVIC_DisableIRQ(SPI_IRQn);
	NVIC_DisableIRQ(SSP0_IRQn);
	NVIC_DisableIRQ(SSP0_IRQn);
	
	NVIC_DisableIRQ(RTC_IRQn);
	
	NVIC_DisableIRQ(EINT0_IRQn);
	NVIC_DisableIRQ(EINT1_IRQn);
	NVIC_DisableIRQ(EINT2_IRQn);
	NVIC_DisableIRQ(EINT3_IRQn);
	
	NVIC_DisableIRQ(USB_IRQn);
	NVIC_DisableIRQ(CAN_IRQn);
	NVIC_DisableIRQ(DMA_IRQn);
	
	NVIC_DisableIRQ(ENET_IRQn);
	
	NVIC_DisableIRQ(USBActivity_IRQn);
	
	for (i=0x1000;i!=0; i--);
	/* Change the Vector Table to the USER_FLASH_START 
	in case the user application uses interrupts */
	if (address==0)
		SCB->VTOR = SECTOR_0_START & 0x1FFFFF80;
	else
		SCB->VTOR = address & 0x1FFFFF80;

	upload_jump(SECTOR_0_START);
}


/*=========================================================================*/

//////////////////////////////////////////////////////////////////
//Copy From HITXYH                                              //
//////////////////////////////////////////////////////////////////
void MAIN_SetDataBackMode(unsigned char ucM)
{
	  if(ucM!=0)
	      K_Result=BROADCAST;
		else
	      K_Result=PASS_ONLY;
		
		return;
}
unsigned char MAIN_IsDataPassOnly(void)
{
	  if(K_Result==PASS_ONLY)
			  return 1;
		
		return 0;
}

unsigned char MAIN_GetCurrentCom(void)
{
	  return CurrentCom;
}
void MAIN_SetCurrentCom(unsigned char ucCom)
{
	  CurrentCom=ucCom;
}
//
void SEND_BYTE(unsigned char ucCh)
{	
			QUEU_TxPublicQueuBuFAdd(0,ucCh);
}
//
// -----------------------------------------
void QUEUE_ProgTxPublicQueu(void) 
{
		static uint8_t BoardCast=1;
		uint16_t Len;     
//    unsigned char n=0;                                  
	
		if(QUEU_IsPublicTxQueuBuFEmpty(0))
			  return;
		
		BoardCast=1;
		if(MAIN_IsDataPassOnly()) 
		    BoardCast=0;		
		
		QUEUE_ClrPublicTxQueuTxtail(0);
		Len=QUEU_PublicTxQueuDataLen(0);
		
		//
		if((BoardCast==1)||(UART_QUEUE ==MAIN_GetCurrentCom()))
				ser_Write(UART_QUEUE,(char*)PTQueuBuF[0].Ptx,Len);
		
		//
//		if((BoardCast==1)||(USB_QUEUE ==MAIN_GetCurrentCom()))
//		{
//				if(usb_working_mode==0)
//				{						
//						//USB_WriteEP (CDC_DEP_IN,PTQueuBuF[0].Ptx,Len);
//					
//						n=0;   
//						while(Len>32) //���в�Ϊ��
//						{
//								USB_WriteEP (CDC_DEP_IN,PTQueuBuF[n*32].Ptx,32);
//                HAL_DelayMs(5);
//							  Len-=32;	
//								n+=1;   			
//						}
//						if(Len!=0)
//						{
//								USB_WriteEP (CDC_DEP_IN,PTQueuBuF[n*32].Ptx,Len);
//                HAL_DelayMs(5); 			
//						}
//							
//				}
//		}
		
//		//
//		if((BoardCast==1)||(ucUartId==TX_WEB))
//		{
//		}
//		
		//
		if((BoardCast==1)||(NET_QUEUE_1==MAIN_GetCurrentCom()))
		{
				LIGWEB_TcpSendData(NET_QUEUE_1,PTQueuBuF[0].Ptx,Len);
		}
		if((BoardCast==1)||(NET_QUEUE_2==MAIN_GetCurrentCom()))
		{
				LIGWEB_TcpSendData(NET_QUEUE_2,PTQueuBuF[0].Ptx,Len);
		}
		if((BoardCast==1)||(NET_QUEUE_3==MAIN_GetCurrentCom()))
		{
				LIGWEB_TcpSendData(NET_QUEUE_3,PTQueuBuF[0].Ptx,Len);
		}
		if((BoardCast==1)||(NET_QUEUE_4==MAIN_GetCurrentCom()))
		{
				LIGWEB_TcpSendData(NET_QUEUE_4,PTQueuBuF[0].Ptx,Len);
		}		
		//
		if((BoardCast==1)||(UPD_QUEUE_1==MAIN_GetCurrentCom()))
		{
				LIGWEB_UdpSendData(UPD_QUEUE_1,PTQueuBuF[0].Ptx,Len);
		}
		
		//
		QUEU_TxPublicQueuBuFClr(0);
				
		return;
}

 /*=========================================================================
 *	UINT8 get_In_chip_ch(UINT8 uInNum)
 *	 �õ�����ڶ�Ӧ��оƬ�Լ���оƬ��Ӧ�Ķ˿ڣ����ص���ʽ�Ը�4bit��ʾоƬ����4bit��ʾ�˿�
 *========================================================================*/	
UINT8 get_In_chip_ch(UINT8 uInNum)
{
    unsigned char ChipSel;
    unsigned char PortSel;

    if(MainDev.mDeviceType == DEVICE_IS_16S1)
    {
        if(uInNum<6)
        {
            ChipSel = CHIP_IN1_IN6; 
            PortSel = uInNum;        
        }
        else if((uInNum>=6)&&(uInNum<12))
        {
            ChipSel = CHIP_IN7_IN12; 
            PortSel = uInNum-6;
            
        }
        else //if((uInNum>=12)&&(uInNum<16))
        {
            ChipSel = CHIP_IN13_IN16;        //IICSel = CHIP_IN13_IN16;
            PortSel = uInNum-12+2;        //�ӵ������ڿ�ʼ
        }
    }
    else if(MainDev.mDeviceType == DEVICE_IS_10S1)
    {
        if(uInNum<6)
        {
            ChipSel = CHIP_IN7_IN12; 
            PortSel = uInNum;
            
        }
        else //if((uInNum>=12)&&(uInNum<16))
        {
            ChipSel = CHIP_IN13_IN16;        //IICSel = CHIP_IN13_IN16;
            PortSel = uInNum-6+2;        //�ӵ������ڿ�ʼ
        }
        
    }
    else if(MainDev.mDeviceType == DEVICE_IS_8S1)
    {
        if(uInNum<4)
        {
            ChipSel = CHIP_IN7_IN12; 
            PortSel = uInNum+2;
            
        }
        else //if((uInNum>=12)&&(uInNum<16))
        {
            ChipSel = CHIP_IN13_IN16;        //IICSel = CHIP_IN13_IN16;
            PortSel = uInNum-4+2;        //�ӵ������ڿ�ʼ
        }
    }
    return ((ChipSel<<4)|PortSel);
}


 /*=========================================================================
 *	get_In_Num(UINT8 ChipSel,UINT8 PortSel)
 *	�õ��˿ڶ�Ӧ����ţ�ע�⣬����������0~15
 *========================================================================*/	
UINT8 get_In_Num(UINT8 ChipSel,UINT8 PortSel)
{
    unsigned char uInNum = 0;
    
    if(MainDev.mDeviceType == DEVICE_IS_16S1)
    {
        if(ChipSel==CHIP_IN1_IN6)    
            uInNum = PortSel;      
        else  if(ChipSel==CHIP_IN7_IN12)
            uInNum = 6+PortSel;         
        else if(ChipSel==CHIP_IN13_IN16)
        {
            if(PortSel>=2)
                uInNum = 12+PortSel-2;  //�ӵ������ڿ�ʼ
            else
                uInNum = 12+PortSel;             
        }
    }
    else if(MainDev.mDeviceType == DEVICE_IS_10S1)
    {
        if(ChipSel==CHIP_IN7_IN12)
            uInNum = PortSel;         
        else if(ChipSel==CHIP_IN13_IN16)
        {
            if(PortSel>=2)
                uInNum = 6+PortSel-2;  //�ӵ������ڿ�ʼ
            else
                uInNum = 6+PortSel;             
        }        
    }
    else if(MainDev.mDeviceType == DEVICE_IS_8S1)
    {
        if(ChipSel==CHIP_IN7_IN12)
            uInNum = PortSel - 2;         
        else if(ChipSel==CHIP_IN13_IN16)
        {
            if(PortSel>=2)
                uInNum = 4+PortSel-2;  //�ӵ������ڿ�ʼ
            else
                uInNum = 4+PortSel;             
        }        
    }
    
    return uInNum;
}


 /***************************************************************************
*��������:SaveNVRAM()
*����˵��:��ָ���ı仯�Ĳ����洢��E2PROM�С�
*��    �룺��
*��    ������
******************************************************************************/
void SaveNVRAM(unsigned char ucID)
{
    if (ucID==EEPROM_SWITCH_STATE)
    {
        MainDev.mEEPROMFlag1|=EEPROM_FLAG1_SWITCH_STATE;
    } 
    else if (ucID==EEPROM_OUT_POWER)
    {
        MainDev.mEEPROMFlag1|=EEPROM_FLAG1_OUT_POWER;
    }
    else if (ucID==EEPROM_FOLLOWENCRY)
        MainDev.mEEPROMFlag1|=EEPROM_FLAG1_FOLLOWENCRY;
    else if (ucID==EEPROM_DEVICE_ADD)
        MainDev.mEEPROMFlag1|=EEPROM_FLAG1_DEVICE_ADD;
    else if (ucID==EEPROM_FORMAT)
        MainDev.mEEPROMFlag1|=EEPROM_FLAG1_FORMAT;
    else if (ucID==EEPROM_SWITCH_DLY)
        MainDev.mEEPROMFlag1|=EEPROM_FLAG1_SWITCH_DLY;
    else if(ucID==EEPROM_UART_STATE)
        MainDev.mEEPROMFlag1 |= EEPROM_FLAG1_UART_STATE;
    else if(ucID==EEPROM_INHDCP_MODE)
        MainDev.mEEPROMFlag1 |= EEPROM_FLAG1_INHDCP_STATE;
    else if(ucID==EEPROM_AUDIO_MODE)
        MainDev.mEEPROMFlag1 |= EEPROM_FLAG1_AUDIO_STATE;
    else if(ucID==EEPROM_5V_TIME_LEVEL)
        MainDev.mEEPROMFlag1 |= EEPROM_FLAG1_5V_TIME;
        
    
    else if (ucID==EEPROM_SN_NUMBER)
        MainDev.mEEPROMFlag1|=EEPROM_FLAG1_SN_NUMBER;
    else if (ucID==EEPROM_MODEL_NAME)
        MainDev.mEEPROMFlag1|=EEPROM_FLAG1_MODEL_NAME;
    else if(ucID==EEPROM_PRIORITY_LIST)
        MainDev.mEEPROMFlag1 |= EEPROM_FLAG1_PRIORITY_LIST;


    //-----auto switch state
    else if(ucID==EEPROM_SWITCH_MODE)
        MainDev.mEEPROMFlag1|=EEPROM_Flag1_SWITCH_MODE;
    else if(ucID==EEPROM_AUTOSW_MODE)
        MainDev.mEEPROMFlag1|=EEPROM_Flag1_AUTOSW_MODE;
    
    else if(ucID==EEPROM_IP_SECURE)
        MainDev.mEEPROMFlag2|=EEPROM_FLAG2_IP_SECURE;
    else if(ucID==EEPROM_IP_NUMBER)
        MainDev.mEEPROMFlag2|=EEPROM_FLAG2_IP_NUMBER;
    else if(ucID==EEPROM_MASK_NUMBER)
        MainDev.mEEPROMFlag2|=EEPROM_FLAG2_MASK_NUMBER;
    else if(ucID==EEPROM_GATEWAY_NUMBER)
        MainDev.mEEPROMFlag2|=EEPROM_FLAG2_GATEWAY_NUMBER;
    else if(ucID==EEPROM_DHCP_ENBALE)
        MainDev.mEEPROMFlag2|=EEPROM_FLAG2_DHCP_ENABLE;
    else if(ucID==EEPROM_USB_PORT_MODE)
        MainDev.mEEPROMFlag2|=EEPROM_FLAG2_USB_MODE;
    else if(ucID==EEPROM_TCPPORT_NUMBER)
        MainDev.mEEPROMFlag2|=EEPROM_FLAG2_TCP_NUMBER;
    
    else if(ucID==EEPROM_UDPPORT_NUMBER)
        MainDev.mEEPROMFlag2|=EEPROM_FLAG2_UDP_NUMBER;
     
    else if(ucID==EEPROM_DNS_NAME)
        MainDev.mEEPROMFlag2|=EEPROM_FLAG2_DNS_NAME;
     
    else if(ucID==EEPROM_MAC_ADDRESS)
        MainDev.mEEPROMFlag2|=EEPROM_FLAG2_MAC_ADDRESS;
    
    else if(ucID==EEPROM_SW_SPEED)
        MainDev.mEEPROMFlag2|=EEPROM_FLAG2_SW_SPEED;
    
    else if(ucID==EEPROM_UART_BR)
        MainDev.mEEPROMFlag2|=EEPROM_FLAG2_UART_BAUD_STATE;
        //IR
    else if(ucID==EEPROM_IR_MATCH_TIME)
        MainDev.mEEPROMFlag2|=EEPROM_FLAG2_IR_TIM;
}


/***************************************************************************
*��������:LoopNVRAM()
*����˵��:���ݴ洢�����E2PROM�洢���ݣ�����һ�δ洢�������
*��    �룺��
*��    ������
******************************************************************************/
void LoopNVRAM(void)
{
    unsigned char n=0,m;
    if (MainDev.mEEPROMFlag1!=0)
    {
        if (MainDev.mEEPROMFlag1&EEPROM_FLAG1_DEVICE_ADD)
        {
            #ifdef MAIN_DEBUG_FLAG
            DEBUG_PRINT("EEPROM_FLAG1_DEVICE_ADD \n");
            #endif
            MainDev.mEEPROMFlag1&=~EEPROM_FLAG1_DEVICE_ADD;
            uc16OfBuf[0]=MainDev.mDevAdd;
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_DEVICE_ADD,uc16OfBuf,1);
             HAL_DelayMs(10);
        }
        if (MainDev.mEEPROMFlag1&EEPROM_FLAG1_FOLLOWENCRY)
        {
            #ifdef MAIN_DEBUG_FLAG
            DEBUG_PRINT("EEPROM_FLAG1_FOLLOWENCRY \n");
            #endif
            MainDev.mEEPROMFlag1&=~EEPROM_FLAG1_FOLLOWENCRY;
            uc16OfBuf[0]=ConDev.mConFollowEncrption[0];
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_FOLLOWENCRY,uc16OfBuf,1);
            HAL_DelayMs(10);
        }
              
        if (MainDev.mEEPROMFlag1&EEPROM_FLAG1_FORMAT)
        {
            #ifdef MAIN_DEBUG_FLAG
            DEBUG_PRINT("EEPROM_FLAG1_FORMAT \n");
            #endif
            MainDev.mEEPROMFlag1&=~EEPROM_FLAG1_FORMAT;
            if (MainDev.mComFormat==LIGUO_A || MainDev.mComFormat==LIGUO_B ||
                MainDev.mComFormat==KRM2000 || MainDev.mComFormat==KRM3000)
                uc16OfBuf[0]=MainDev.mComFormat;
            else 
                uc16OfBuf[0]=KRM2000;
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_FORMAT,uc16OfBuf,1);
            HAL_DelayMs(10);
        }
        
        if (MainDev.mEEPROMFlag1&EEPROM_FLAG1_SWITCH_DLY)
        {
            #ifdef MAIN_DEBUG_FLAG
            DEBUG_PRINT("EEPROM_FLAG1_SWITCH_DLY \n");
            #endif
            MainDev.mEEPROMFlag1&=~EEPROM_FLAG1_SWITCH_DLY;
            uc16OfBuf[0]=MainDev.mBlankDelayTx[0];//ֻ��һ�������
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_SWITCH_DLY,uc16OfBuf,1);
             HAL_DelayMs(10);
        }
        
        if (MainDev.mEEPROMFlag1&EEPROM_FLAG1_SWITCH_STATE)
        {
            #ifdef MAIN_DEBUG_FLAG
            DEBUG_PRINT("EEPROM_FLAG1_SWITCH_STATE \n");
            #endif
            MainDev.mEEPROMFlag1&=~EEPROM_FLAG1_SWITCH_STATE;
            uc16OfBuf[0]=MainDev.mNextSelectInPort;	//�˿�
            uc16OfBuf[1]=MainDev.mNextSelectChip;		//оƬ		
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_SWITCH_STATE,uc16OfBuf,2);
            HAL_DelayMs(10);
        }     
        
        if (MainDev.mEEPROMFlag1&EEPROM_FLAG1_OUT_POWER)
        {
            #ifdef MAIN_DEBUG_FLAG
            DEBUG_PRINT("EEPROM_FLAG1_OUT_POWER \n");
            #endif
            MainDev.mEEPROMFlag1&=~EEPROM_FLAG1_OUT_POWER;
            uc16OfBuf[0]=MainDev.mTxClose;//mOutPowerOn;
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_OUT_POWER,uc16OfBuf,1);  
             HAL_DelayMs(10);    
        }
      
        if (MainDev.mEEPROMFlag1&EEPROM_FLAG1_UART_STATE)
        {
            #ifdef MAIN_DEBUG_FLAG
            DEBUG_PRINT("EEPROM_FLAG1_UART_STATE \n");
            #endif
            MainDev.mEEPROMFlag1&=~EEPROM_FLAG1_UART_STATE;
            uc16OfBuf[0]=MainDev.mUartState;
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_UART_STATE,uc16OfBuf,1);
            HAL_DelayMs(10);
        }
        if (MainDev.mEEPROMFlag1&EEPROM_FLAG1_AUDIO_STATE)
        {
            #ifdef MAIN_DEBUG_FLAG
            DEBUG_PRINT("EEPROM_FLAG1_AUDIO_STATE \n");
            #endif
            MainDev.mEEPROMFlag1&=~EEPROM_FLAG1_AUDIO_STATE;
            uc16OfBuf[0]=MainDev.mAudioOutState;
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_AUDIO_MODE,uc16OfBuf,1);
            HAL_DelayMs(10);
        }
        if (MainDev.mEEPROMFlag1&EEPROM_FLAG1_INHDCP_STATE)
        {
            #ifdef MAIN_DEBUG_FLAG
            DEBUG_PRINT("EEPROM_FLAG1_INHDCP_STATE \n");
            #endif
            MainDev.mEEPROMFlag1&=~EEPROM_FLAG1_INHDCP_STATE;
           
            for(n=0;n<ucMaxValidPortNum;n++)
                uc16OfBuf[n]=MainDev.mInHDCPState[get_In_chip_ch(n)>>4][get_In_chip_ch(n)&0x0f];
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_INHDCP_MODE,uc16OfBuf,8);
            HAL_DelayMs(20);
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_INHDCP_MODE+8,uc16OfBuf+8,(ucMaxValidPortNum-8));
            HAL_DelayMs(20);
        }
        if (MainDev.mEEPROMFlag1&EEPROM_FLAG1_5V_TIME)
        {
            #ifdef MAIN_DEBUG_FLAG
            DEBUG_PRINT("EEPROM_FLAG1_AUDIO_STATE \n");
            #endif
            MainDev.mEEPROMFlag1&=~EEPROM_FLAG1_5V_TIME;         
            uc16OfBuf[0]=MainDev.uc5VCountLevel>>8;		//�ر�5Vʱ��
            uc16OfBuf[1]=(unsigned char)MainDev.uc5VCountLevel;
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_5V_TIME_LEVEL,uc16OfBuf,2);
            HAL_DelayMs(10);
        }

        if (MainDev.mEEPROMFlag1&EEPROM_FLAG1_SN_NUMBER)
        {
            #ifdef MAIN_DEBUG_FLAG
            DEBUG_PRINT("EEPROM_FLAG1_SN_NUMBER \n");
            #endif
            MainDev.mEEPROMFlag1&=~EEPROM_FLAG1_SN_NUMBER;

            //KRM3000	            
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_SN_NUMBER,SN_NUMBER,8);
            HAL_DelayMs(10);
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_SN_NUMBER+8,SN_NUMBER+8,6);
            HAL_DelayMs(10);
        }    
        
        if (MainDev.mEEPROMFlag1&EEPROM_FLAG1_MODEL_NAME)
        {
            #ifdef MAIN_DEBUG_FLAG
            DEBUG_PRINT("EEPROM_FLAG1_MODEL_NAME \n");
            #endif
            MainDev.mEEPROMFlag1&=~EEPROM_FLAG1_MODEL_NAME;

            //KRM3000	
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_MODEL_NAME,MODEL_NAME,8);
            HAL_DelayMs(10);
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_MODEL_NAME+8,MODEL_NAME+8,8);
            HAL_DelayMs(10);
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_MODEL_NAME+16,MODEL_NAME+16,3);
            HAL_DelayMs(10);

        }
		
        #ifdef SUPPORT_AUTO_SWITCH
        if(MainDev.mEEPROMFlag1&EEPROM_Flag1_SWITCH_MODE)
        {
            MainDev.mEEPROMFlag1&=~EEPROM_Flag1_SWITCH_MODE;
            uc16OfBuf[0] = (unsigned char)AutoSw.SwitchMode;	//�л�״̬            
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_SWITCH_MODE,uc16OfBuf,1);
            HAL_DelayMs(10);
        }
        if(MainDev.mEEPROMFlag1&EEPROM_Flag1_AUTOSW_MODE)
        {
            MainDev.mEEPROMFlag1&=~EEPROM_Flag1_AUTOSW_MODE;
            uc16OfBuf[0] = (unsigned char)AutoSw.AutoSwMode;	//�Զ��л�״̬            
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_AUTOSW_MODE,uc16OfBuf,1);
            HAL_DelayMs(10);
        }
        
        if(MainDev.mEEPROMFlag1&EEPROM_FLAG1_PRIORITY_LIST)
        {
            MainDev.mEEPROMFlag1&=~EEPROM_FLAG1_PRIORITY_LIST;           
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_PRIORITY_LIST,AutoSw.mPriority,ucMaxValidPortNum);
            HAL_DelayMs(10);
        } 
        #endif
        HAL_DelayMs(10);
    }  
		
    if (MainDev.mEEPROMFlag2!=0)
    {//IP ����
        if(MainDev.mEEPROMFlag2&EEPROM_FLAG2_IP_SECURE)
        {
            MainDev.mEEPROMFlag2&=~EEPROM_FLAG2_IP_SECURE;
            uc16OfBuf[0]= NetSecureMode;    
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_IP_SECURE,uc16OfBuf,1);
            HAL_DelayMs(10);
        }
        if(MainDev.mEEPROMFlag2&EEPROM_FLAG2_IP_NUMBER)
        {
            MainDev.mEEPROMFlag2&=~EEPROM_FLAG2_IP_NUMBER;
            uc16OfBuf[0]= Ipdata[0];
            uc16OfBuf[1]= Ipdata[1];
            uc16OfBuf[2]= Ipdata[2];
            uc16OfBuf[3]= Ipdata[3];    
    
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_IP_NUMBER,uc16OfBuf,4);
            HAL_DelayMs(10);
        }
				
        if(MainDev.mEEPROMFlag2&EEPROM_FLAG2_MASK_NUMBER)
        {
            MainDev.mEEPROMFlag2&=~EEPROM_FLAG2_MASK_NUMBER;
            uc16OfBuf[0]=NetMaskdata[0];
            uc16OfBuf[1]=NetMaskdata[1];
            uc16OfBuf[2]=NetMaskdata[2];
            uc16OfBuf[3]=NetMaskdata[3];     
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_MASK_NUMBER,uc16OfBuf,4);
            HAL_DelayMs(10);
        }
        
        if(MainDev.mEEPROMFlag2&EEPROM_FLAG2_GATEWAY_NUMBER)
        {
            MainDev.mEEPROMFlag2&=~EEPROM_FLAG2_GATEWAY_NUMBER;
            uc16OfBuf[0]= GateWaydata[0];
            uc16OfBuf[1]= GateWaydata[1];
            uc16OfBuf[2]= GateWaydata[2];
            uc16OfBuf[3]= GateWaydata[3];       
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_GATEWAY_NUMBER,uc16OfBuf,4);
            HAL_DelayMs(10);                    
        }           

        if(MainDev.mEEPROMFlag2&EEPROM_FLAG2_DHCP_ENABLE)
        {
            MainDev.mEEPROMFlag2&=~EEPROM_FLAG2_DHCP_ENABLE;
            uc16OfBuf[0]=net_dhcp_enable ;
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_DHCP_ENBALE,uc16OfBuf,1);
            HAL_DelayMs(10);
        }
        
        if(MainDev.mEEPROMFlag2&EEPROM_FLAG2_TCP_NUMBER)
        {
            MainDev.mEEPROMFlag2&=~EEPROM_FLAG2_TCP_NUMBER;
            uc16OfBuf[0]=(unsigned char)tcpPort ;
            uc16OfBuf[1]=(unsigned char)(tcpPort>>8 );
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_TCPPORT_NUMBER,uc16OfBuf,2);
        }
        if(MainDev.mEEPROMFlag2&EEPROM_FLAG2_UDP_NUMBER)
        {
            MainDev.mEEPROMFlag2&=~EEPROM_FLAG2_UDP_NUMBER;
            uc16OfBuf[0]=(unsigned char)udpPort ;
            uc16OfBuf[1]=(unsigned char)(udpPort>>8 );
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_UDPPORT_NUMBER,uc16OfBuf,2);
        }
        if(MainDev.mEEPROMFlag2&EEPROM_FLAG2_MAC_ADDRESS)
        {
            MainDev.mEEPROMFlag2&=~EEPROM_FLAG2_MAC_ADDRESS;

            uc16OfBuf[0]= NetMacdata[0];
            uc16OfBuf[1]= NetMacdata[1];
            uc16OfBuf[2]= NetMacdata[2];
            uc16OfBuf[3]= NetMacdata[3]; 
            uc16OfBuf[4]= NetMacdata[4];
            uc16OfBuf[5]= NetMacdata[5];             
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_MAC_ADDRESS,uc16OfBuf,6);
            HAL_DelayMs(10);           
            
        } 
				
				//
        if(MainDev.mEEPROMFlag2&EEPROM_FLAG2_USB_MODE)
        {
            MainDev.mEEPROMFlag2&=~EEPROM_FLAG2_USB_MODE;
        
            #ifdef SUPPORT_USB_COMM
            if(usb_working_mode==0)
                USB_SetType(USB_CDC_TYPE);
            else if(usb_working_mode==1)
                USB_SetType(USB_MSC_TYPE);
            USB_Init();         // USB Initialization	
            USB_Connect(TRUE);  // USB Connect	
            #endif
						//uc16OfBuf[0]=usb_working_mode ;
						//HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_USB_PORT_MODE,uc16OfBuf,1);//USB���ú󲻴洢
        }
				
        if(MainDev.mEEPROMFlag2&EEPROM_FLAG2_AUDIO_MUTE_MODE)
        {//
            MainDev.mEEPROMFlag2&=~EEPROM_FLAG2_AUDIO_MUTE_MODE;
    
            uc16OfBuf[0]=ConDev.mConAudioMute[0] ;
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_AUDIO_MUTE_MODE,uc16OfBuf,1);
            HAL_DelayMs(10);
        }
        if(MainDev.mEEPROMFlag2&EEPROM_FLAG2_VIDEO_MUTE_MODE)
        {//
            MainDev.mEEPROMFlag2&=~EEPROM_FLAG2_VIDEO_MUTE_MODE;
            uc16OfBuf[0]=ConDev.mConVideoMute[0] ;
            uc16OfBuf[1]=ConDev.mConVideoMute[1] ;
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_VIDEO_MUTE_MODE,uc16OfBuf,2);
            HAL_DelayMs(10);
        }
        
        if(MainDev.mEEPROMFlag2&EEPROM_FLAG2_UART_BAUD_STATE)
        {
            MainDev.mEEPROMFlag2&=~EEPROM_FLAG2_UART_BAUD_STATE;
            uc16OfBuf[0]=MainDev.mBR==115200?1:0;
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_UART_BR,uc16OfBuf,1);
        }        
                
        if (MainDev.mEEPROMFlag2&EEPROM_FLAG2_DNS_NAME)
        {
            MainDev.mEEPROMFlag2&=~EEPROM_FLAG2_DNS_NAME;
            //KRM3000	
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_DNS_NAME,DNS_NAME,8);
            HAL_DelayMs(20);
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_DNS_NAME+8,DNS_NAME+8,6);
            HAL_DelayMs(20);
        }   
        
        if(MainDev.mEEPROMFlag2&EEPROM_FLAG2_SW_SPEED)
        {
            MainDev.mEEPROMFlag2&=~EEPROM_FLAG2_SW_SPEED;
            uc16OfBuf[0]=MainDev.mSwtichSpeed;
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_SW_SPEED,uc16OfBuf,1);
            HAL_DelayMs(10);
        } 
        if(MainDev.mEEPROMFlag2&EEPROM_FLAG2_IR_TIM)
        {
            MainDev.mEEPROMFlag2&=~EEPROM_FLAG2_IR_TIM;                
            if( MainDev.mFactoryType == DEVICE_IS_LIG)
            {
                uc16OfBuf[0]=IRData.ucHeadWidth;
                uc16OfBuf[1]=IRData.ucEndAddressWidth;
                uc16OfBuf[2]=IRData.ucDataHeadWidth ;
                uc16OfBuf[3]=IRData.ucDataHigWidth;
                uc16OfBuf[4]=IRData.ucDataLowWidth;
                uc16OfBuf[5]=IRData.ucEndDataWidth ;
                uc16OfBuf[6]=IRData.ucEndHeadWidth ;
                uc16OfBuf[7] = IRData.MaxDataLenght ;           
                HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_IR_MATCH_TIME,uc16OfBuf,8);
                HAL_DelayMs(10);
                
                for(m=0;m<10;m++)
                {
                    for(n=0;n<4;n++)
                        uc16OfBuf[n]=(IrMatchNum[m]>>(24-8*n))&0xff;       //Hig byte
                    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_IR_MATCH_KEY0+m*4,uc16OfBuf,4);
                    HAL_DelayMs(10);
                }
            }
        }  
    }
    HAL_DelayMs(10);
}
//
unsigned char LoadNVRAM(void)
{
    #ifdef MAIN_DEBUG_FLAG
    DEBUG_PRINT("EEPROM: Load Nvram\n");
    #endif
    unsigned char n=0,m;
    
    MainDev.mEEPROMFlag1=0x0000;

    HAL_I2CReadBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_AA,uc16OfBuf,16);
    if((uc16OfBuf[EEPROM_AA-EEPROM_AA]!=0xaa)&&(uc16OfBuf[EEPROM_55-EEPROM_AA]!=0x55))
    {
        HAL_DelayMs(20);
        HAL_I2CReadBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_AA,uc16OfBuf,16);
    }
    if ((uc16OfBuf[EEPROM_AA-EEPROM_AA]!=0xaa)&&(uc16OfBuf[EEPROM_55-EEPROM_AA]!=0x55))
        return 1;

    
    MainDev.mDevAdd=uc16OfBuf[EEPROM_DEVICE_ADD-EEPROM_AA];
    if (MainDev.mDevAdd==0xeb)
        MainDev.mDevAdd=COMMAND_DEFAULT_ADD;

    ConDev.mConFollowEncrption[0] = (tagFollowEncrp)uc16OfBuf[EEPROM_FOLLOWENCRY-EEPROM_AA];

    if(MainDev.mFactoryType == DEVICE_IS_KRM)
        ConDev.mConFollowEncrption[0] = FE_FOLLOW_IN;


    MainDev.mComFormat=(tagComFormat)uc16OfBuf[EEPROM_FORMAT-EEPROM_AA];
    MainDev.mBlankDelayTx[0]=uc16OfBuf[EEPROM_SWITCH_DLY-EEPROM_AA];	
    if(MainDev.mBlankDelayTx[0]>0x0f)
        MainDev.mBlankDelayTx[0] = 0;    

    if(uc16OfBuf[EEPROM_SWITCH_STATE-EEPROM_AA]<=IN_PORT6)
        MainDev.mRxPortSel=uc16OfBuf[EEPROM_SWITCH_STATE-EEPROM_AA];	  
    else 
        MainDev.mRxPortSel=IN_PORT1;
    
    if(uc16OfBuf[EEPROM_SWITCH_STATE-EEPROM_AA+1]<ucMaxValidChipNum)
        MainDev.mRxChipSel=uc16OfBuf[EEPROM_SWITCH_STATE-EEPROM_AA+1];	  
    else
    {
        if(MainDev.mDeviceType==DEVICE_IS_16S1)
            MainDev.mRxChipSel=CHIP_IN1_IN6;
        else
            MainDev.mRxChipSel=CHIP_IN7_IN12;
    }
    
    MainDev.mTxClose=(tagTxClose)uc16OfBuf[EEPROM_OUT_POWER-EEPROM_AA];
    if((MainDev.mTxClose!=TX_ON)&&(MainDev.mTxClose!=TX_OFF))
        MainDev.mTxClose=TX_ON;
	
    MainDev.mUartState =(tagUart)uc16OfBuf[EEPROM_UART_STATE-EEPROM_AA];
    if((MainDev.mUartState!=UART_CPU)&&(MainDev.mUartState!=UART_HBT))
        MainDev.mUartState  = UART_CPU;
      
    if(MainDev.mFactoryType == DEVICE_IS_LIG)
    {
        if((MainDev.mComFormat != LIGUO_A)&&(MainDev.mComFormat!=LIGUO_B))
        {
            MainDev.mComFormat = LIGUO_A;
            MainDev.mEEPROMFlag1|=EEPROM_FLAG1_FORMAT;			
        }
    }
    else
    {
        if(MainDev.mComFormat != KRM3000)
        {
            MainDev.mComFormat = KRM3000;
            MainDev.mEEPROMFlag1|=EEPROM_FLAG1_FORMAT;			
        }
    }
    
    HAL_I2CReadBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_INHDCP_MODE,uc16OfBuf,16);
    for(n=0;n<ucMaxValidPortNum;n++)
    {
        if((uc16OfBuf[EEPROM_INHDCP_MODE-EEPROM_INHDCP_MODE+n]==0)||
            (uc16OfBuf[EEPROM_INHDCP_MODE-EEPROM_INHDCP_MODE+n]==1))
            MainDev.mInHDCPState[get_In_chip_ch(n)>>4][get_In_chip_ch(n)&0x0f]=(tagInHDCP)uc16OfBuf[EEPROM_INHDCP_MODE-EEPROM_INHDCP_MODE+n];
        else
            MainDev.mInHDCPState[get_In_chip_ch(n)>>4][get_In_chip_ch(n)&0x0f] = INHDCP_ON;
    }
	        

    //KRM3000
    HAL_I2CReadBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_SN_NUMBER,SN_NUMBER,14);

    HAL_I2CReadBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_MODEL_NAME,MODEL_NAME,19);
    
    HAL_I2CReadBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_DNS_NAME,DNS_NAME,14);

		
    HAL_I2CReadBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_SWITCH_MODE,uc16OfBuf,6);
    //select arc or Embed out
    MainDev.mAudioOutState =(tagAudio)uc16OfBuf[EEPROM_AUDIO_MODE-EEPROM_SWITCH_MODE];
    if((MainDev.mAudioOutState!= AUDIO_ARC)&&(MainDev.mAudioOutState!= AUDIO_DE_Embed))
        MainDev.mAudioOutState = AUDIO_DE_Embed;
    //�ر�5v�ȼ�
    MainDev.uc5VCountLevel= (uc16OfBuf[EEPROM_5V_TIME_LEVEL-EEPROM_SWITCH_MODE]<<8)
                            |uc16OfBuf[EEPROM_5V_TIME_LEVEL-EEPROM_SWITCH_MODE+1];
    if((MainDev.uc5VCountLevel>MAX_5VOFF_TIME)||(MainDev.uc5VCountLevel==0))
        MainDev.uc5VCountLevel	= DEFAULT_LEVEL;
    #ifdef SUPPORT_AUTO_SWITCH
    if(uc16OfBuf[EEPROM_SWITCH_MODE-EEPROM_SWITCH_MODE]>1)
        AutoSw.SwitchMode = AUTO_SWITCH_MODE;
    else
        AutoSw.SwitchMode =(tagSwitchMode)uc16OfBuf[EEPROM_SWITCH_MODE-EEPROM_SWITCH_MODE];	//�л�״̬    
    
    if(uc16OfBuf[EEPROM_AUTOSW_MODE-EEPROM_SWITCH_MODE]>1)
        AutoSw.AutoSwMode = PRIORITY_MODE;
    else
        AutoSw.AutoSwMode =(tagAutoMode) uc16OfBuf[EEPROM_AUTOSW_MODE-EEPROM_SWITCH_MODE];	//�Զ��л�״̬ 
    //USB ģʽ
    //USB���ú󲻴洢
//     usb_working_mode= uc16OfBuf[EEPROM_USB_PORT_MODE-EEPROM_SWITCH_MODE];
//    if(usb_working_mode>1)
//        usb_working_mode	= 0;
   
    //���ȼ��б�
    HAL_I2CReadBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_PRIORITY_LIST,uc16OfBuf,ucMaxValidPortNum);
    memcpy(AutoSw.mPriority,uc16OfBuf,ucMaxValidPortNum);
    #endif
    
		//IP ��������================================================================================
    HAL_I2CReadBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_IP_NUMBER,uc16OfBuf,15);
    
    Ipdata[0]=uc16OfBuf[0];
    Ipdata[1]=uc16OfBuf[1];
    Ipdata[2]=uc16OfBuf[2];
    Ipdata[3]=uc16OfBuf[3];
    
    NetMaskdata[0]=uc16OfBuf[4];
    NetMaskdata[1]=uc16OfBuf[5];
    NetMaskdata[2]=uc16OfBuf[6];
    NetMaskdata[3]=uc16OfBuf[7];
    
    GateWaydata[0]=uc16OfBuf[8];
    GateWaydata[1]=uc16OfBuf[9];
    GateWaydata[2]=uc16OfBuf[10];
    GateWaydata[3]=uc16OfBuf[11];

    
    net_dhcp_enable = uc16OfBuf[12];
        
    HAL_I2CReadBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_MAC_ADDRESS,uc16OfBuf,6);
    NetMacdata[0]=uc16OfBuf[0];
    NetMacdata[1]=uc16OfBuf[1];
    NetMacdata[2]=uc16OfBuf[2];
    NetMacdata[3]=uc16OfBuf[3];   
    NetMacdata[4]=uc16OfBuf[4];
    NetMacdata[5]=uc16OfBuf[5];
		
		//�õ�NET ��SECURE MODEģʽ
    HAL_I2CReadBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_IP_SECURE,uc16OfBuf,1);
    NetSecureMode=uc16OfBuf[0];		
    
		
		//��Ƶ��������================================================================================
    HAL_I2CReadBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_VIDEO_MUTE_MODE,uc16OfBuf,9);
    ConDev.mConAudioMute[0] = (tagMuteState)uc16OfBuf[EEPROM_AUDIO_MUTE_MODE-EEPROM_VIDEO_MUTE_MODE];
    if(ConDev.mConAudioMute[0]>1)
        ConDev.mConAudioMute[0]= (tagMuteState)0;
    ConDev.mConVideoMute[0] = (tagMuteState)uc16OfBuf[EEPROM_VIDEO_MUTE_MODE-EEPROM_VIDEO_MUTE_MODE];
    if(ConDev.mConVideoMute[0] >1)
        ConDev.mConVideoMute[0] = (tagMuteState)0;
    ConDev.mConVideoMute[1] = (tagMuteState)uc16OfBuf[EEPROM_VIDEO_MUTE_MODE-EEPROM_VIDEO_MUTE_MODE+1];
    if(ConDev.mConVideoMute[1] >1)
        ConDev.mConVideoMute[1] = (tagMuteState)0;
    
    if (uc16OfBuf[EEPROM_SW_SPEED-EEPROM_VIDEO_MUTE_MODE]==0)
    {
        MainDev.mSwtichSpeed=0;
    }else
    {
        MainDev.mSwtichSpeed=1;
    }
    
    if (uc16OfBuf[EEPROM_UART_BR-EEPROM_VIDEO_MUTE_MODE]==1)
    {
        MainDev.mBR=115200;
    }else
        MainDev.mBR=9600;
		
		//
    HAL_I2CReadBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_UDPPORT_NUMBER,uc16OfBuf,2);
    udpPort = (uc16OfBuf[1]<<8)|uc16OfBuf[0];
		//
    HAL_I2CReadBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_TCPPORT_NUMBER,uc16OfBuf,2);
    tcpPort = (uc16OfBuf[1]<<8)|uc16OfBuf[0];
		
		
    if(MainDev.mFactoryType == DEVICE_IS_LIG)
    {
        HAL_I2CReadBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_IR_MATCH_TIME,uc16OfBuf,15);
        IRData.ucHeadWidth = uc16OfBuf[0];
        IRData.ucEndAddressWidth = uc16OfBuf[1];
        IRData.ucDataHeadWidth = uc16OfBuf[2];
        IRData.ucDataHigWidth = uc16OfBuf[3];
        IRData.ucDataLowWidth = uc16OfBuf[4];
        IRData.ucEndDataWidth = uc16OfBuf[5];
        IRData.ucEndHeadWidth = uc16OfBuf[6];
        IRData.MaxDataLenght = uc16OfBuf[7];
        
        for(m=0;m<10;m++)
        {
            HAL_I2CReadBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_IR_MATCH_KEY0+m*4,uc16OfBuf,4);
            IrMatchNum[m] = uc16OfBuf[0]<<24|uc16OfBuf[1]<<16|uc16OfBuf[2]<<8|uc16OfBuf[3];       //Hig byte
        }
        
        if((IRData.ucHeadWidth==0)||(IRData.ucDataHigWidth==0)||
            (IRData.ucDataHigWidth==0)||(IRData.MaxDataLenght==0))
        {
                IRData.ucHeadWidth = LIG_IR_HEADER;
                IRData.ucEndAddressWidth = LIG_IR_END_DATA;   
                IRData.ucDataHeadWidth = LIG_IR_END_PACKET;
                IRData.ucDataHigWidth = LIG_IR_HIG_BIT;
                IRData.ucDataLowWidth = LIG_IR_LOW_BIT;
                
                IRData.ucEndDataWidth  = LIG_IR_END_DATA;
                IRData.ucEndHeadWidth  = LIG_IR_END_PACKET;
                
                IRData.MaxDataLenght = LIG_IR_PACKAGE_LENGTH;
                
                IrMatchNum[0] = LIG_DEFAULT_IR_NUM0;       
                IrMatchNum[1] = LIG_DEFAULT_IR_NUM1;
                IrMatchNum[2] = LIG_DEFAULT_IR_NUM2;
                IrMatchNum[3] = LIG_DEFAULT_IR_NUM3;
                IrMatchNum[4] = LIG_DEFAULT_IR_NUM4;
                IrMatchNum[5] = LIG_DEFAULT_IR_NUM5;       
                IrMatchNum[6] = LIG_DEFAULT_IR_NUM6;
                IrMatchNum[7] = LIG_DEFAULT_IR_NUM7;
                IrMatchNum[8] = LIG_DEFAULT_IR_NUM8;
                IrMatchNum[9] = LIG_DEFAULT_IR_NUM9;           
        }
    }
    
    //�������Զ����м���
    for(n=0;n<16;n++)
        MainDev.mInPHY[n] = 0xffff;
    
    return 0;
}
//
/***************************************************************************
*��������:LoopNVRAM()
*����˵��:��λ���ߵ�һ�δ洢Ĭ��ֵ
*��    �룺��
*��    ������
//��һ���ϵ�,AA-55״̬,ȫ��Ĭ�ϵĲ�������KMR Ҫ��
//���̸�λ��                   ModelName����,SN����,IP_MAC����,�����Ķ�Ĭ��
//ͨѶ��λFACTORY:ͨѶ��ʽ����,ModelName����,SN����,IP_MAC����,�����Ķ�Ĭ��
//���ڱ��豸,��Ϊֻ��K3000�ĸ�ʽ,���Լ��̸�λ��Factory��һ����
//ucResetMSN=0x04����һ���ϵ縴λ
//          =0x02: ���̸�λ
//          =0x01: ͨѶ��λ
******************************************************************************/
void InitNVRAM(unsigned char ucResetMSN)
{
    unsigned char m,n;
    #ifdef MAIN_DEBUG_FLAG
    DEBUG_PRINT("EEPROM: Init Nvram\n");
    #endif
        
    uc16OfBuf[0]=0xaa;
    uc16OfBuf[1]=0x55;
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_AA,uc16OfBuf,2);
    HAL_DelayMs(10);
	
    //Common parameters	
		//����A��ʽ��ַ
    uc16OfBuf[0]=COMMAND_DEFAULT_ADD;
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_DEVICE_ADD,uc16OfBuf,1);
    HAL_DelayMs(10);

    if(MainDev.mFactoryType == DEVICE_IS_LIG)
    {
        //if((MainDev.mComFormat!=LIGUO_A)&&(MainDev.mComFormat!=LIGUO_B))
        {
            uc16OfBuf[0]=LIGUO_A;
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_FORMAT,uc16OfBuf,1);
        }
    }
    else
    {
        //if((MainDev.mComFormat!=KRM3000)&&(MainDev.mComFormat!=KRM2000))
        {
            uc16OfBuf[0]=KRM3000;
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_FORMAT,uc16OfBuf,1);
        }
    }		
    HAL_DelayMs(10);
		
    //
    uc16OfBuf[0]=FE_FOLLOW_IN;
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_FOLLOWENCRY,uc16OfBuf,1);
    HAL_DelayMs(10);   
		
    //
    uc16OfBuf[0]=0; //DLY=0;
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_SWITCH_DLY,uc16OfBuf,1);
    HAL_DelayMs(10);
		
    //
    uc16OfBuf[0]=0; //EX-FAST
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_SW_SPEED,uc16OfBuf,1);
    HAL_DelayMs(10);
		
		//�л�״̬
    if(MainDev.mDeviceType==DEVICE_IS_16S1)
    {
        uc16OfBuf[0]=IN_PORT1;
        uc16OfBuf[1]=CHIP_IN1_IN6;
    }            
    else if(MainDev.mDeviceType==DEVICE_IS_10S1)
    {
        uc16OfBuf[0]=IN_PORT1;
        uc16OfBuf[1]=CHIP_IN7_IN12;
    }
    else if(MainDev.mDeviceType==DEVICE_IS_8S1)
    {
        uc16OfBuf[0]=IN_PORT3;
        uc16OfBuf[1]=CHIP_IN7_IN12;
    }
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_SWITCH_STATE,uc16OfBuf,2);
    HAL_DelayMs(10);
       
		//
    uc16OfBuf[0]=TX_ON;
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_OUT_POWER,uc16OfBuf,1);
    HAL_DelayMs(10);
		
		//
    uc16OfBuf[0]=UART_CPU;		//���ڵ�CPU
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_UART_STATE,uc16OfBuf,1);
		HAL_DelayMs(10);
		
		//
    uc16OfBuf[0]=DEFAULT_LEVEL>>8;	//�ر������5vʱ��ѡ��
    uc16OfBuf[1]=(unsigned char)DEFAULT_LEVEL;
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_5V_TIME_LEVEL,uc16OfBuf,2);    
    HAL_DelayMs(10);
		
		//
    uc16OfBuf[0]=AUDIO_DE_Embed;//Ĭ����Ƶ���
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_AUDIO_MODE,uc16OfBuf,1);
    HAL_DelayMs(10);
		
    //
    for(n=0;n<ucMaxValidPortNum;n++)//�����֧��HDCP ON
        uc16OfBuf[n]=INHDCP_ON;
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_INHDCP_MODE,uc16OfBuf,ucMaxValidPortNum);	    
    HAL_DelayMs(20);  
		
    //
    uc16OfBuf[0]=0 ;    //1==115200 else 9600
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_UART_BR,uc16OfBuf,1);
    HAL_DelayMs(10);
		
    //KRM3000	
    //SN��  11λ Ĭ��0000 0000 001
		//MODEL NAME Ĭ��VS-611DT
    if (ucResetMSN)
    {//ֻ�����豸��һ���ϵ縴λ�ų�ʼ�����ֵ��������λ���ܹ��޸����ֵ
				//SN
        memset(uc16OfBuf,0,16);
        memcpy(uc16OfBuf,"00000000000001",14);
        HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_SN_NUMBER,uc16OfBuf,8);
        HAL_DelayMs(20);
        HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_SN_NUMBER+8,uc16OfBuf+8,6);
        HAL_DelayMs(20);
			
        //Model Name
        memset(uc16OfBuf,0,16);
        if(MainDev.mDeviceType == DEVICE_IS_16S1)
        {
            if(MainDev.mDevOutType == DEVICE_IS_DT)
                memcpy(uc16OfBuf,"VS-1611DT",9);   //VS-1611DT
            else
                memcpy(uc16OfBuf,"VS-1611UHD",10); 
        }
        else if(MainDev.mDeviceType == DEVICE_IS_10S1)
        {
            if(MainDev.mDevOutType == DEVICE_IS_DT)
                memcpy(uc16OfBuf,"VS-1011DT",9);   //VS-1011DT
            else
                memcpy(uc16OfBuf,"VS-1011UHD",10); 
        }
        else if(MainDev.mDeviceType == DEVICE_IS_16S1)
        {
            if(MainDev.mDevOutType == DEVICE_IS_DT)
                memcpy(uc16OfBuf,"VS-811DT",8);   //VS-811DT
            else
                memcpy(uc16OfBuf,"VS-1011UHD",10); 
        }
        HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_MODEL_NAME,uc16OfBuf,8);
        HAL_DelayMs(20);
        HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_MODEL_NAME+8,uc16OfBuf+8,8);
        HAL_DelayMs(10);
        memset(uc16OfBuf,0,16);
        HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_MODEL_NAME+16,uc16OfBuf,3); 
        HAL_DelayMs(10);  
				
				//IP MAC
				uc16OfBuf[0] = DEF_NET_MAC[0];            
				uc16OfBuf[1] = DEF_NET_MAC[1];              
				uc16OfBuf[2] = DEF_NET_MAC[2];                 
				uc16OfBuf[3] = DEF_NET_MAC[3];                  
				uc16OfBuf[4] = DEF_NET_MAC[4];                 
				uc16OfBuf[5] = DEF_NET_MAC[5];                 
				HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_MAC_ADDRESS,uc16OfBuf,6);
				HAL_DelayMs(10);
    }
    
		//DNS Name
    HAL_I2CReadBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_SN_NUMBER,SN_NUMBER,14);
    memcpy(DNS_NAME,"KRAMER_",7);
    memcpy(DNS_NAME+7,SN_NUMBER+10,4);
    memcpy(DNS_NAME+7+4,"   ",3);
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_DNS_NAME,DNS_NAME,8);
    HAL_DelayMs(20);
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_DNS_NAME+8,DNS_NAME+8,6);
    HAL_DelayMs(20);

    //---------------------------auto switch ---------------------------------------//	
    #ifdef SUPPORT_AUTO_SWITCH
    uc16OfBuf[0] = AUTO_SWITCH_MODE;	//�л�״̬
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_SWITCH_MODE,uc16OfBuf,1);
    HAL_DelayMs(10);

    uc16OfBuf[0] = PRIORITY_MODE;			//�Զ��л�״̬
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_AUTOSW_MODE,uc16OfBuf,1);
    HAL_DelayMs(10);

    //�洢���ȼ��б�
    for(n=0;n<ucMaxValidPortNum;n++)
        uc16OfBuf[n]=n;
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_PRIORITY_LIST,uc16OfBuf,ucMaxValidPortNum);
    HAL_DelayMs(10);
    #endif
       
    //---------------------------IP configure ---------------------------------------//	
    uc16OfBuf[0] = DEF_NET_IP[0];            
    uc16OfBuf[1] = DEF_NET_IP[1];                
    uc16OfBuf[2] = DEF_NET_IP[2];                
    uc16OfBuf[3] = DEF_NET_IP[3];                 
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_IP_NUMBER,uc16OfBuf,4);
    HAL_DelayMs(10);
    
    uc16OfBuf[0] = DEF_NET_MASK[0];          
    uc16OfBuf[1] = DEF_NET_MASK[1];              
    uc16OfBuf[2] = DEF_NET_MASK[2];                
    uc16OfBuf[3] = DEF_NET_MASK[3];                  
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_MASK_NUMBER,uc16OfBuf,4);
    HAL_DelayMs(10);

    uc16OfBuf[0] = DEF_NET_GATEWAY[0];              
    uc16OfBuf[1] = DEF_NET_GATEWAY[1];                 
    uc16OfBuf[2] = DEF_NET_GATEWAY[2];                
    uc16OfBuf[3] = DEF_NET_GATEWAY[3];                  
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_GATEWAY_NUMBER,uc16OfBuf,4);
    HAL_DelayMs(10);
        
    uc16OfBuf[0] = DEF_NET_DHCP;
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_DHCP_ENBALE,uc16OfBuf,1);
    HAL_DelayMs(10);
    
    uc16OfBuf[0]=(unsigned char)DEF_TCP_PORT ;
    uc16OfBuf[1]=(unsigned char)(DEF_TCP_PORT>>8);
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_TCPPORT_NUMBER,uc16OfBuf,2);
    HAL_DelayMs(10);
    
    uc16OfBuf[0]=(unsigned char)DEF_UDP_PORT ;
    uc16OfBuf[1]=(unsigned char)(DEF_UDP_PORT>>8);
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_UDPPORT_NUMBER,uc16OfBuf,2);
    HAL_DelayMs(10);
    
    uc16OfBuf[0] = DEF_NET_MAC[0];            
    uc16OfBuf[1] = DEF_NET_MAC[1];              
    uc16OfBuf[2] = DEF_NET_MAC[2];                 
    uc16OfBuf[3] = DEF_NET_MAC[3];                  
    uc16OfBuf[4] = DEF_NET_MAC[4];                 
    uc16OfBuf[5] = DEF_NET_MAC[5];                 
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_MAC_ADDRESS,uc16OfBuf,6);
    HAL_DelayMs(10);
		              
    uc16OfBuf[0] = DEF_NET_SECURE;                 
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_IP_SECURE,uc16OfBuf,1);
    HAL_DelayMs(10);
    //---------------------------End of Net config ---------------------------------------//
    
    //Ĭ������£������⴮��
    //uc16OfBuf[0]=0 ;
    //HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_USB_PORT_MODE,uc16OfBuf,1);
    //HAL_DelayMs(10);

    uc16OfBuf[0]=0 ;    //MUTE_IS_OFF
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_AUDIO_MUTE_MODE,uc16OfBuf,1);
    HAL_DelayMs(10);
    uc16OfBuf[0]=0 ;    //MUTE_IS_OFF
    uc16OfBuf[1]=0 ; 
    HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_VIDEO_MUTE_MODE,uc16OfBuf,2);
    HAL_DelayMs(10);
    
//    if(MainDev.mFactoryType == DEVICE_IS_LIG)
//    {
//        uc16OfBuf[0] = LIG_IR_HEADER;
//        uc16OfBuf[1] = LIG_IR_END_DATA;   
//        uc16OfBuf[2] = LIG_IR_END_PACKET;
//        uc16OfBuf[3] = LIG_IR_HIG_BIT;
//        uc16OfBuf[4] = LIG_IR_LOW_BIT;        
//        uc16OfBuf[5] = LIG_IR_END_DATA;
//        uc16OfBuf[6] = LIG_IR_END_PACKET;     
//        uc16OfBuf[7] = LIG_IR_PACKAGE_LENGTH;    

//        HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_IR_MATCH_TIME,uc16OfBuf,8);
//        HAL_DelayMs(10);
//                          
//        for(m=0;m<10;m++)
//        {
//            for(n=0;n<4;n++)
//                uc16OfBuf[n]=(DEF_IR_NUM_BUF[m]>>(24-8*n))&0xff;       //Hig byte
//            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_IR_MATCH_KEY0+m*4,uc16OfBuf,4);
//            HAL_DelayMs(10);
//        }
//    }
}


#ifdef 	SUPPORT_AUTO_SWITCH
/********************************************************************************************
*��      ����GetListPort
*��      �ܣ��õ��б�����Ҫ�л��Ķ˿ڣ��б�������ȼ�����LC�б�
*��Ա������ListBuf   --     �����ȼ�����LC�б� 
********************************************************************************************/
void GetListPort(unsigned char* ListBuf)
{
    unsigned char i,n;
    for(n=0;n<ucMaxValidPortNum;n++)     
    {	
        for(i=0;i<ucMaxValidPortNum;i++)  //�˿�
        {   //��ѯ�˿ڣ��ҵ�������ȼ����������ȼ��Ƿ����źţ������������һ�����ȼ�
            if((ListBuf[i]==n)&&(InDev[get_In_chip_ch(i)>>4][get_In_chip_ch(i)&0x0f].mInSig == TRUE)) //�ҵ��б�����ȼ���Ӧ���л��Ķ˿�
            {                        						            
                AutoSw.ucSwitchPort = i;
                if (AutoSw.AutoSWMark== 0)                            
                    AutoSw.AutoSWMark = 1;      					
                return;                           
            }
        }
    }
}

/********************************************************************************************
*��      ����GetAutoModeChannel()
*��      �ܣ���Autoģʽ��,�õ��Զ��л���ͨ��
*��Ա��������        
********************************************************************************************/
void GetAutoModeChannel(void)
{
	unsigned char n = 0;//,i = 0;
	static unsigned char ucDelayCount = 0;
	if(AutoSw.SwitchMode != AUTO_SWITCH_MODE)
		return;
	else
	{
		//�����ȼ�ģʽ�£�ֻ�������ȼ��ź��Ƿ����źţ�����ͨ���ϵ��źű仯����
		if(AutoSw.ucAutoSwFlag  == 1)	//ֻ��ģʽ�����仯
		{
			AutoSw.ucAutoSwFlag  = 0;
			if(AutoSw.AutoSwMode == PRIORITY_MODE)
                GetListPort(AutoSw.mPriority);
		}else		//�ź�״̬�����仯��������Ҫȥ��
		{
			if(MainDev.OverrideFlag ==1)	//�����������ʱ��֧���Զ��л�������ͨ���ź���ʧ��Żָ�
				return;
			if(AutoSw.AutoSigState[AutoSw.ucChangePort] != AutoSw.AutoSigLastState[AutoSw.ucChangePort])
			{
				if(InDev[get_In_chip_ch(AutoSw.ucChangePort)>>4][get_In_chip_ch(AutoSw.ucChangePort)&0x0f].mInP5V==FALSE)//ע��˿ڵ���ʼλ��//���û��5v����û���źţ������л�
				{	
					//û��+5v���任��ͨ����û���źţ��������л����������������Ƿ�Ҫ����
					if(InDev[get_In_chip_ch(AutoSw.ucChangePort)>>4][get_In_chip_ch(AutoSw.ucChangePort)&0x0f].mInSig == FALSE)
						ucDelayCount = 0;
				}
				else
				{
					if(AutoSw.AutoSigLastState[AutoSw.ucChangePort]== SIG_EXIST)//��+5v�����б��ޣ���Ҫ�ϳ���ʱ��ȴ�
						ucDelayCount = 7;		 //7S
					else			//��+5v�����ޱ��У������л�
					{
						if(AutoSw.AutoSwMode == LAST_CONNECTED_MODE)
							ucDelayCount = 1;	   //��ֹ�źŲ��ȶ���ɵ�����
						else
							ucDelayCount = 0;
					}
				}

				if(AutoSw.AutoSw50msDelay==0)
					AutoSw.AutoSw50msDelay=1;	
													
				if(AutoSw.AutoSw50msDelay>=(BASE_1S_DELAY*ucDelayCount))	//�л��ȴ���ʱ��
				{			
					AutoSw.AutoSw50msDelay = 0;
					AutoSw.AutoSigLastState[AutoSw.ucChangePort] = AutoSw.AutoSigState[AutoSw.ucChangePort];					
					//�����ȼ�ģʽ�£������ź���α仯��ֻ�������ȼ�˳�����μ�⣬���ȼ��ߵ����л���
					if(AutoSw.AutoSwMode == PRIORITY_MODE)
					{
						for(n=0;n<ucMaxValidPortNum;n++)
						{	//����źű仯ʱ��������״̬û�и���,����ֻ����û�е��ź�״̬
							if(InDev[get_In_chip_ch(n)>>4][get_In_chip_ch(n)&0x0f].mInSig==FALSE)
							{
								if(AutoSw.AutoSigLastState[n]!=SIG_NONE)
									AutoSw.AutoSigLastState[n]=SIG_NONE;
							}
						}

						if(AutoSw.mPriority[AutoSw.ucChangePort]<=AutoSw.mPriority[get_In_Num(MainDev.mRxChipSel,MainDev.mRxPortSel)]) //�仯�Ķ˿����ȼ���
						{     
							if(InDev[get_In_chip_ch(AutoSw.mPriority[AutoSw.ucChangePort])>>4][get_In_chip_ch(AutoSw.mPriority[AutoSw.ucChangePort])&0x0f].mInSig == TRUE) //�����ȼ����ź�
							{
								AutoSw.ucSwitchPort = AutoSw.ucChangePort;
								if (AutoSw.AutoSWMark == 0)
									AutoSw.AutoSWMark = 1;
								return;
							}else		//�����ȼ��źű���
                            {
                                if(InDev[MainDev.mRxChipSel][MainDev.mRxPortSel].mInSig == FALSE)                                   
                                     GetListPort(AutoSw.mPriority);
                            }
						}
					}
					else //if(bAutoSwMode == LAST_CONNECTED_MODE)
					{
						if(AutoSw.AutoSigState[AutoSw.ucChangePort] == SIG_EXIST)//�仯ͨ���źŴ��ڵ������
						{							
							SetLCModePortList(AutoSw.ucChangePort,1);							
                            if(((get_In_chip_ch(AutoSw.ucChangePort)>>4)!=MainDev.mRxChipSel)
                                ||((get_In_chip_ch(AutoSw.ucChangePort)&0x0f)!=MainDev.mRxPortSel))
							{
								AutoSw.ucSwitchPort = AutoSw.ucChangePort;
								if (AutoSw.AutoSWMark == 0)
									AutoSw.AutoSWMark = 1;
								return;
							}
						}
						else if(AutoSw.AutoSigState[AutoSw.ucChangePort] == SIG_NONE)	//�仯��ͨ��û���ź�
						{
							SetLCModePortList(AutoSw.ucChangePort,0);	
                            GetListPort(AutoSwitchList);                            
						}
					}
				}
				else
				{
					SET_EVENT(EV_AUTO_SWITCH);
				}
			}
			else
			{
				AutoSw.AutoSw50msDelay = 0;
			} 
		}
	}
}


/********************************************************************************************
*��      ���� GotoAutoSwitchChannel()
*��      �ܣ���Autoģʽ�£��õ��Զ��л�ͨ�����л�����ͨ��
*��Ա��������        
********************************************************************************************/
void GotoAutoSwitchChannel(void)
{
	unsigned m = 0,i=0;
	if(AutoSw.AutoSWMark >= BASE_SW_AUTO_COUNT)
	{
		AutoSw.AutoSWMark = 0;
		//���ڵ�ǰ��ͨ���ϵ��ź���ʧ������������һ��ͨ���źţ�����ν�л�//������Ҫ�ж��źŵ���һ���ź�����
		if(AutoSw.AutoSigState[AutoSw.ucSwitchPort] == SIG_EXIST)
		{
             if(((get_In_chip_ch(AutoSw.ucSwitchPort)>>4)!=MainDev.mRxChipSel)
                ||((get_In_chip_ch(AutoSw.ucSwitchPort)&0x0f)!=MainDev.mRxPortSel))
			{
                MainDev.mNextSelectChip = get_In_chip_ch(AutoSw.ucSwitchPort)>>4;
				MainDev.mNextSelectInPort = get_In_chip_ch(AutoSw.ucSwitchPort)&0x0f;//AutoSw.ucSwitchPort;	
				SaveNVRAM(EEPROM_SWITCH_STATE);
			}
		}
		else
		{           
            if(AutoSw.AutoSwMode == PRIORITY_MODE)
			{	 //���Ѿ��õ��´��л��Ķ˿�����£�else��û��ʵ�����壬����˫�ر���
                for(m=0;m<ucMaxValidPortNum;m++)      //���ȼ�
                {	
                    for(i=0;i<ucMaxValidPortNum;i++)  //�˿�
                    {   //��ѯ�˿ڣ��ҵ�������ȼ����������ȼ��Ƿ����źţ������������һ�����ȼ�
                        if((AutoSw.mPriority[i]==m)&&(InDev[get_In_chip_ch(i)>>4][get_In_chip_ch(i)&0x0f].mInSig == TRUE)) //�ҵ��б�����ȼ���Ӧ���л��Ķ˿�
                        {                        						            
                            MainDev.mNextSelectChip = get_In_chip_ch(i)>>4;
                            MainDev.mNextSelectInPort = get_In_chip_ch(i)&0x0f;
                            SaveNVRAM(EEPROM_SWITCH_STATE);   					
                            if((MainDev.mRxPortSel!=MainDev.mNextSelectInPort)||(MainDev.mRxChipSel!=MainDev.mNextSelectChip))
                            {
                                SET_EVENT(EV_SW_KEY);
                                SET_EVENT(EV_CH_SWITCH);
                            }
                            if(MainDev.mUI!=UI_STATE_COPY_EDID)
                                UICgh();
                            return;                           
                        }
                    }
                }                
			}
			else if(AutoSw.AutoSwMode == LAST_CONNECTED_MODE)
			{
               for(m=0;m<ucMaxValidPortNum;m++)      // AutoSwitchList[m]
                {	
                    for(i=0;i<ucMaxValidPortNum;i++)  //�˿�
                    {   //��ѯ�˿ڣ��ҵ�������ȼ����������ȼ��Ƿ����źţ������������һ�����ȼ�
                        if((AutoSwitchList[i]==m)&&(InDev[get_In_chip_ch(i)>>4][get_In_chip_ch(i)&0x0f].mInSig == TRUE)) //�ҵ��б�����ȼ���Ӧ���л��Ķ˿�
                        {                        						            
                            MainDev.mNextSelectChip = get_In_chip_ch(i)>>4;
                            MainDev.mNextSelectInPort = get_In_chip_ch(i)&0x0f;
                            SaveNVRAM(EEPROM_SWITCH_STATE);    					
                            if((MainDev.mRxPortSel!=MainDev.mNextSelectInPort)||(MainDev.mRxChipSel!=MainDev.mNextSelectChip))
                            {
                                SET_EVENT(EV_SW_KEY);
                                SET_EVENT(EV_CH_SWITCH);
                            }
                            if(MainDev.mUI!=UI_STATE_COPY_EDID)
                                UICgh();
                            return;                            
                        }
                    }
                }
			}
									
		}						
		
		if((MainDev.mRxPortSel!=MainDev.mNextSelectInPort)||(MainDev.mRxChipSel!=MainDev.mNextSelectChip))
        {
			SET_EVENT(EV_SW_KEY);
            SET_EVENT(EV_CH_SWITCH);    //            SET_EVENT(EVENT_SIG_TABLELIST[MainDev.mNextSelectChip][MainDev.mNextSelectInPort]);
        }
		if(MainDev.mUI!=UI_STATE_COPY_EDID)
			UICgh();
	}
}


/********************************************************************************************
*��      ���� SetSinglePriortyPortList()
*��      �ܣ��������ȼ��б��У������˿ڵ����ȼ���
*���������mport	--ѡ��Ķ˿�
		  ucPriority	--���õ����ȼ�	
*�����������
********************************************************************************************/
void SetSinglePriorityPortList(unsigned char mport,unsigned char ucPriority)
{
	unsigned char n;

    
    if(mport<ucMaxValidPortNum)
    {
        //������õĸö˿ڵ����ȼ���ԭ����һ��
        if(AutoSw.mPriority[mport]!=ucPriority)            
        {           
            if(AutoSw.mPriority[mport]<ucPriority)  //���õ����ȼ�����ԭ���ö˿ڵ����ȼ�
            {                
                for(n=0;n<ucMaxValidPortNum;n++)
                {
                    if((AutoSw.mPriority[n]>AutoSw.mPriority[mport])&&(AutoSw.mPriority[n]<=ucPriority) )
                        AutoSw.mPriority[n] =  AutoSw.mPriority[n]-1; //ԭ�������ȼ����ڱ仯��Χ֮�ڣ��򲻱仯��������Ҫ��˳���1
                }
                AutoSw.mPriority[mport]=ucPriority;
            }
            else if(AutoSw.mPriority[mport]>ucPriority)   //���õ����ȼ�С��ԭ���ö˿ڵ����ȼ�
            {
                for(n=0;n<ucMaxValidPortNum;n++)
                {
                    if((AutoSw.mPriority[n]<AutoSw.mPriority[mport])&&(AutoSw.mPriority[n]>=ucPriority))
                        AutoSw.mPriority[n] =  AutoSw.mPriority[n]+1; //ԭ�������ȼ����ڱ仯��Χ֮�ڣ��򲻱仯��������Ҫ��˳���+1
                }               
            }
          
            AutoSw.mPriority[mport]=ucPriority;;
            HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_PRIORITY_LIST,AutoSw.mPriority,6); 
        }
    }
}


/********************************************************************************************
*��      ���� SetLCModePortList()
*��      �ܣ�����Auto LCģʽ�¸����źŵĽ��������ʧ���ö˿ڽ���˳��
*���������mport	--ѡ��Ķ˿�
		  isValid	--�仯�Ķ˿��ź��Ƿ���Ч	
*�����������
********************************************************************************************/
void SetLCModePortList(unsigned char mport,unsigned char isValid)
{
	unsigned char n;

	if(isValid ==1)
	{
        //�ҵ��˿ڶ�Ӧ��LCPortList��Ӧ���б�ֵ��ע�⣬�仯�ķ�Χ��Ȼ��ԭ���ö˿ڵĵȼ������һ�ȼ�֮��ı仯
                
        for(n=0;n<ucMaxValidPortNum;n++)
        {
            if(AutoSwitchList[n]<AutoSwitchList[mport])//ԭ�������ȼ����ڱ仯��Χ֮�ڣ��򲻱仯��������Ҫ��˳��+1                
                AutoSwitchList[n] =  AutoSwitchList[n]+1;                
        }
        AutoSwitchList[mport]=0;//������ȼ�                      
	}
	else
	{
        //�ҵ��˿ڶ�Ӧ��LCPortList��Ӧ���б�ֵ��ע�⣬�仯�ķ�Χ��Ȼ��ԭ���ö˿ڵĵȼ������һ�ȼ�֮��ı仯
        if(AutoSwitchList[mport]<(ucMaxValidPortNum-1))  //���õ����ȼ�����ԭ���ö˿ڵ����ȼ�
        {                
            for(n=0;n<ucMaxValidPortNum;n++)
            {
                if((AutoSwitchList[n]>AutoSwitchList[mport])&&(AutoSwitchList[n]<ucMaxValidPortNum) )               
                    AutoSwitchList[n] =  AutoSwitchList[n]-1; //ԭ�������ȼ����ڱ仯��Χ֮�ڣ��򲻱仯��������Ҫ��˳���1                
            }
            AutoSwitchList[mport]=ucMaxValidPortNum-1;//������ȼ�
        }
	}
}
//
void ShowAutoSwithcInfo(void)
{
	K3000_Back_Cmd_Name( PROT_COMMAND_DEBUG,' ');
	ser_String("Switch Info\0");
	if (AutoSw.SwitchMode != AUTO_SWITCH_MODE)
		ser_String(",Manual Switch Mode\0");
	else
	{
		ser_String(",Auto Switch Mode\0");
		if(AutoSw.AutoSwMode == PRIORITY_MODE )		
			ser_String(",Priority Mode\0");				
		else
			ser_String(",Last Connected Mode\0");
	}
	
	K3000_CR_LF();
}
#endif //SUPPORT_AUTO_SWITCH


/********************************************************************************************
*��      ����JudgeInputToPulldownOutput5V()
*��      �ܣ�����������źŵĽ���������������Ƿ�ر�����ڵ�
*��Ա��������        
********************************************************************************************/
void JudgeInputToPulldownOutput5V(void)
{	
	unsigned char InSigFlag = 0;

	//�ж������źŵ�������˴��ж������źź�+5V
     //��KRMҪ���ǵ�ǰѡ��Ķ˿�û���źţ��������������

    if(InDev[MainDev.mRxChipSel][MainDev.mRxPortSel].mInP5V	 == TRUE)
    {
        InSigFlag = 1;

    }
    if(InDev[MainDev.mRxChipSel][MainDev.mRxPortSel].mInSig	== TRUE)
    {
        InSigFlag = 1;

    }

		if(InSigFlag==0)		   //û���ź�
		{	
					if(MainDev.sOutput5VCount50ms==0)
					{
							usTimeLevel=0;
							MainDev.sOutput5VCount50ms = 1;		//��ʼ��ʱ
					}					
		}
		else
		{
					MainDev.sOutput5VCount50ms=0;			
					HAL_Cpld_Write(0x15,0x03);	//High
					if (MainDev.m5VOffFlag==TRUE)
					{
							MainDev.m5VOffFlag=FALSE;
					}
					usTimeLevel=0;
		}
			
		if(MainDev.sOutput5VCount50ms>=OUTPUT5V_1S_COUNT)
		{
			MainDev.sOutput5VCount50ms = 1;		//��ʱ30s,��ͷ�ټ�ʱ
			usTimeLevel++;

		}
	
    if(Debug5vFlag==1)
    {
        if(usTimeLevel>=1)//debug ����Ϊ1s	//��ʱ���
        {
            MainDev.sOutput5VCount50ms = 0;	//ֹͣ��ʱ
            HAL_Cpld_Write(0x15,0x00);	  //OUT_5V_LOW;
            MainDev.m5VOffFlag=TRUE;
            usTimeLevel = 0;//MainDev.uc5VCountLevel=0;
            //������ϣ��ص��趨��ʱ��
            Debug5vFlag = 0;
        }
    }
    else
    {
        if(MainDev.uc5VCountLevel!=0)
        {
            if(usTimeLevel>=MainDev.uc5VCountLevel)//DEFAULT_LEVEL)	//��ʱ���
            {
                MainDev.sOutput5VCount50ms = 0;	//ֹͣ��ʱ
                //Ϊ�˱�֤ARC������������ARCģʽ�²��ر�5V
                if(MainDev.mAudioOutState != AUDIO_ARC)
                {
                    HAL_Cpld_Write(0x15,0x00);	//OUT_5V_LOW;                                    
                    MainDev.m5VOffFlag=TRUE;
                }                
                usTimeLevel = 0;//MainDev.uc5VCountLevel=0;
            }
        }
        else
        {
            MainDev.sOutput5VCount50ms = 0;	
            usTimeLevel = 0;
            HAL_Cpld_Write(0x15,0x03);	//OUT_5V_HIG;   
            if (MainDev.m5VOffFlag==FALSE)
            {
                MainDev.m5VOffFlag=TRUE;
            }
        }       
    }
	return;

}


void Rst_Dp83848(void)
{
	PINSEL_CFG_Type  PinCfg_RST;

	//Pin1.18 ѡ��GPIO���ܣ�����Ϊ�����һֱ�ø�
	PinCfg_RST.Funcnum = 0;
	PinCfg_RST.Portnum = 1; 									
	PinCfg_RST.Pinnum = 18; //RST_RMII Pin1.18
	PINSEL_ConfigPin(&PinCfg_RST); 

	GPIO_SetDir(ARM_GPIO_PORT1,(1<<18),1);		 //Out

	GPIO_SetValue(ARM_GPIO_PORT1,(1<<18)); 		//High
	HAL_DelayMs(5);
	GPIO_ClearValue(ARM_GPIO_PORT1,(1<<18)); 	//Low
	HAL_DelayMs(10);
	GPIO_SetValue(ARM_GPIO_PORT1,(1<<18)); 	   //High

}


void VS100TX_Reset(void)
{   
    unsigned char ucVal  = 0;
    
    ucVal = HAL_Cpld_Read(0x16);
            
    HAL_Cpld_Write(0x16,ucVal|0x08);
    HAL_DelayMs(10);
    HAL_Cpld_Write(0x16,ucVal^0x08);
    HAL_DelayMs(10);
    HAL_Cpld_Write(0x16,ucVal|0x08);
}

/********************************************************************************************
*��      ����Si9575_Hardware_Reset()
*˵      ����Reset Si9575
             
*��Ա �� ����  
********************************************************************************************/
void Si9575_Hardware_Reset(unsigned char ChipIndex)
{
    unsigned char ucVal  = 0;
    
    ucVal = HAL_Cpld_Read(0x16);
    //Set Hig
    if(ChipIndex==0)
        HAL_Cpld_Write(0x16,ucVal|0x01); 
    else if(ChipIndex==1)
        HAL_Cpld_Write(0x16,ucVal|0x02); 
    else if(ChipIndex==2)
        HAL_Cpld_Write(0x16,ucVal|0x04);  
    HAL_DelayMs(50);
    //Set Low
    if(ChipIndex==0)
        HAL_Cpld_Write(0x16,ucVal^0x01); 
    else if(ChipIndex==1)
        HAL_Cpld_Write(0x16,ucVal^0x02); 
    else if(ChipIndex==2)
        HAL_Cpld_Write(0x16,ucVal^0x04);  
    
    //Set Hig
    if(ChipIndex==0)
        HAL_Cpld_Write(0x16,ucVal|0x01); 
    else if(ChipIndex==1)
        HAL_Cpld_Write(0x16,ucVal|0x02); 
    else if(ChipIndex==2)
        HAL_Cpld_Write(0x16,ucVal|0x04);
    
}

void Hardware_Init_Si9575_VS100TX(void)
{
    HAL_Cpld_Write(0x16,0x0f);
    HAL_DelayMs(50);
    HAL_Cpld_Write(0x16,0x00);
    HAL_DelayMs(50);
    HAL_Cpld_Write(0x16,0x0f); 
}


/********************************************************************************************
*��      ����TCP UDPͨѶ������()
*˵      ����NET IP
             
//ucSockIndex �Ǹ�TCP�˿ڿ����Ķ���������,
*��Ա �� ����  
********************************************************************************************/
void LIGWEB_TcpSendData(unsigned char ucSockIndex,unsigned char *pBuF, unsigned short usLen)
{
	  static unsigned char ucSocket=0;
	  unsigned char ucSoc=0;
	
    U8 *sendbuf;  
	  U16 maxlen,n,len=0;
												
	  if(ucSockIndex<NET_QUEUE_1)
			 ucSockIndex=NET_QUEUE_1;
		ucSocket=(ucSockIndex-NET_QUEUE_1);
    if(ucSocket>=NUM_CONNECTIONS)
        return;
		
    ucSoc=tcp_soc[ucSocket];
		
		switch (tcp_get_state(ucSoc)) 
		{
				case TCP_STATE_FREE:
				case TCP_STATE_CLOSED:
					len=0;
					break;
				case TCP_STATE_CONNECT:								
					if (tcp_check_send (ucSoc) == __TRUE) 
					{
							maxlen = tcp_max_dsize (ucSoc);
							if(sendbuf!=NULL)
							{
									sendbuf = tcp_get_buf(maxlen);						
									//����Ŀmaxlen=1460�϶������ǵ�usLen��
									for (n=0;n<usLen;n++)
									{		
											sendbuf[n]=pBuF[n];
									}
									
									len=n;
									tcp_send (ucSoc, sendbuf, len);
							}
					}
				  break;
		}
		
    //Tcp_SendData(ucSoc,pBuF,usLen);		
				
		return;		
}
//
U16 tcp_callback (U8 soc, U8 evt, U8 *ptr, U16 par) 
{
    unsigned char n;
    unsigned char soc_idx=0;
  /* This function is called by the TCP module on TCP event */
  /* Check the 'Net_Config.h' for possible events.          */
  par = par;
  
    for (soc_idx=0;soc_idx<NUM_CONNECTIONS;soc_idx++)
    {
        if (soc==tcp_soc[soc_idx])
            break;
    }
    if (soc_idx==NUM_CONNECTIONS)
        return (0);

  switch (evt) {
    case TCP_EVT_DATA:
      /* TCP data frame has arrived, data is located at *par1, */
      /* data length is par2. Allocate buffer to send reply.   */
        //���յ�TCP���ݣ��������
        n = 0; 
        do
        {
            Q_Add(NET_QUEUE_1+soc_idx,ptr[n]);
            n++;
        }
        while( n < par );
        
      break;

    case TCP_EVT_CONREQ:
      /* Remote peer requested connect, accept it */
      return (1);

    case TCP_EVT_CONNECT:
      /* The TCP socket is connected */
//       //TCP����ʱ����ð�TCP����֮ǰ����Ϣ���
//       memset(net_out[soc_idx].data,0xFF,NET_BUF_SIZE);
//       NET_BUF_RESET(soc_idx,net_out);
      return (1);
  }
  return (0);
}

//
void LIGWEB_UdpSendData(unsigned char ucSockIndex,unsigned char *pBuF, unsigned short usLen)
{	
	  static unsigned char ucSocket=0;
	  unsigned char ucSoc=0;
											
    U8 *sendbuf;
    U16 n,maxlen;    
    U8 soc_idx=0;

	
	  if(ucSockIndex<UPD_QUEUE_1)
			  return;
		ucSocket=(ucSockIndex-UPD_QUEUE_1);
		
    if(ucSocket>=NUM_UDP_CONNECTIONS)
        return;
		
    ucSoc=udp_soc[ucSocket];		
	  soc_idx=ucSocket;
		
    //    
    maxlen=maxlen<512?maxlen:512;
		
		maxlen=usLen;
    sendbuf = udp_get_buf (maxlen);
    
    for (n=0;n<maxlen;n++)
    {
        sendbuf[n]=pBuF[n];
    }

    // Send 'Hello World!' to remote peer //
    udp_send (udp_soc[soc_idx], udp_remip[soc_idx], udp_remport[soc_idx], sendbuf, maxlen);	
}
//
U16 udp_callback (U8 socket, U8 *remip, U16 remport, U8 *buf, U16 len) 
{
    U8 n;
    U8 soc_idx=0;
		//This function is called when UDP data is received //
    for (soc_idx=0;soc_idx<NUM_UDP_CONNECTIONS;soc_idx++)
    {
        if (socket==udp_soc[soc_idx])
            break;
    }
    if (soc_idx==NUM_UDP_CONNECTIONS)
        return (0);
    
    udp_remip[soc_idx][0]=remip[0];
    udp_remip[soc_idx][1]=remip[1];
    udp_remip[soc_idx][2]=remip[2];
    udp_remip[soc_idx][3]=remip[3];
    
    udp_remport[soc_idx]=remport;
    
		// Process received data from 'buf' //
    n = 0; 
    do
    {
        Q_Add(UPD_QUEUE_1+soc_idx,buf[n]);
        n++;
    }
    while( n < len );
    
    return (0);
}
//
void loop_net(void)
{
		if(!MAIN_IsNetPollingEnabled())
				return;
		
		timer_poll (); 
    main_TcpNet ();
}
//
void MAIN_EnableNetPolling(unsigned char EnDis)
{//Ping,Web�ȷŵ���ʱ����,��
		NetPollFlag=EnDis;
		return;
}
//
unsigned char MAIN_IsNetPollingEnabled(void)
{
	  if(NetPollFlag!=0)
				return 1;
		
		return 0;
}
//
void reset_net(void)
{
    unsigned char soc_idx=0;
	
	
//		NetMacdata[0]=0x00;
//		NetMacdata[1]=0x01;
//		NetMacdata[2]=0x02;
//		NetMacdata[3]=0x32;
//		NetMacdata[4]=0x3C;
//		NetMacdata[5]=0x36;		
//	  memcpy(own_hw_adr,NetMacdata,6);		
//	
		init_TcpNet();
    
    //Change the host name //
    memset(lhost_name,0,16);
    memcpy (lhost_name, (U8 *)DNS_NAME,14);
    
			
		
		localm[NETIF_ETH].NetMask[0]=255;//NetMaskdata[0];
		localm[NETIF_ETH].NetMask[1]=255;//NetMaskdata[1];
		localm[NETIF_ETH].NetMask[2]=0;//NetMaskdata[2];
		localm[NETIF_ETH].NetMask[3]=0;//NetMaskdata[3];
				
		localm[NETIF_ETH].DefGW[0]=192;//GateWaydata[0];
		localm[NETIF_ETH].DefGW[1]=168;//GateWaydata[1];
		localm[NETIF_ETH].DefGW[2]=0;//GateWaydata[2];
		localm[NETIF_ETH].DefGW[3]=1;//GateWaydata[3];
		
		
//    if (CommNetFlag&0x04)
//    {
//        CommNetFlag &= ~0x04;
//        ip_config.IpAdr[0]  =   Ipdata[0] ;
//        ip_config.IpAdr[1]  =   Ipdata[1] ;
//        ip_config.IpAdr[2]  =   Ipdata[2] ;
//        ip_config.IpAdr[3]  =   Ipdata[3] ;
//    }
//    if (CommNetFlag&0x01)
//    {
//        CommNetFlag &= ~0x01;
//        ip_config.NetMask[0]  =   NetMaskdata[0] ;
//        ip_config.NetMask[1]  =   NetMaskdata[1] ;
//        ip_config.NetMask[2]  =   NetMaskdata[2] ;
//        ip_config.NetMask[3]  =   NetMaskdata[3] ;
//    }
//    if (CommNetFlag&0x01)
//    {
//        CommNetFlag &= ~0x01;
//        ip_config.NetMask[0]  =   NetMaskdata[0] ;
//        ip_config.NetMask[1]  =   NetMaskdata[1] ;
//        ip_config.NetMask[2]  =   NetMaskdata[2] ;
//        ip_config.NetMask[3]  =   NetMaskdata[3] ;
//    }
//    	
//    localm[NETIF_ETH].NetMask[0]=ip_config.NetMask[0];
//    localm[NETIF_ETH].NetMask[1]=ip_config.NetMask[1];
//    localm[NETIF_ETH].NetMask[2]=ip_config.NetMask[2];
//    localm[NETIF_ETH].NetMask[3]=ip_config.NetMask[3];
//        
//    localm[NETIF_ETH].DefGW[0]=ip_config.DefGW[0];
//    localm[NETIF_ETH].DefGW[1]=ip_config.DefGW[1];
//    localm[NETIF_ETH].DefGW[2]=ip_config.DefGW[2];
//    localm[NETIF_ETH].DefGW[3]=ip_config.DefGW[3];
    
    if (net_dhcp_enable==0)
    {
        dhcp_tout = 0;  
        dhcp_disable ();
        //IP��ַ������,��÷ŵ�dhcp_disable ();֮��
				localm[NETIF_ETH].IpAdr[0]= 192;//Ipdata[0];
				localm[NETIF_ETH].IpAdr[1]= 168;//Ipdata[1];
				localm[NETIF_ETH].IpAdr[2]= 20;//Ipdata[2];
				localm[NETIF_ETH].IpAdr[3]= 186;//Ipdata[3];
//        localm[NETIF_ETH].IpAdr[0]=ip_config.IpAdr[0];
//        localm[NETIF_ETH].IpAdr[1]=ip_config.IpAdr[1];
//        localm[NETIF_ETH].IpAdr[2]=ip_config.IpAdr[2];
//        localm[NETIF_ETH].IpAdr[3]=ip_config.IpAdr[3];
    }else
    {
        dhcp_tout = DHCP_TOUT;  
    }
    
    // Initialize TCP Socket and start listening //
    for (soc_idx=0;soc_idx<NUM_CONNECTIONS;soc_idx++)
    {
        tcp_soc[soc_idx] = tcp_get_socket (TCP_TYPE_SERVER|TCP_TYPE_KEEP_ALIVE, 0, 20, tcp_callback);
        if (tcp_soc[soc_idx] != 0) 
        {
            tcp_listen (tcp_soc[soc_idx], tcpPort);
        }
    }	

    for (soc_idx=0;soc_idx<NUM_UDP_CONNECTIONS;soc_idx++)
    {
        udp_soc[soc_idx] = udp_get_socket (0,UDP_OPT_SEND_CS|UDP_OPT_CHK_CS, udp_callback);
        if (udp_soc[soc_idx] != 0) 
        {
            udp_open(udp_soc[soc_idx], udpPort);
        }
    }
}


//
void MAIN_GetAutoIp (void)
{    	
	  net_dhcp_enable=0;
	  if(net_dhcp_enable==0)
			  return;
				
		//
		if (mem_test (&MY_IP, 0, IP_ADRLEN) == __FALSE ) 
		{
				//Success, DHCP has already got the IP address. //			
				ArgTBuF[0]=localm[NETIF_ETH].IpAdr[0];
				ArgTBuF[1]=localm[NETIF_ETH].IpAdr[1];
				ArgTBuF[2]=localm[NETIF_ETH].IpAdr[2];
				ArgTBuF[3]=localm[NETIF_ETH].IpAdr[3];
			  if((ArgTBuF[0]!=ucAutoIp[0])||(ArgTBuF[1]!=ucAutoIp[1])||
					 (ArgTBuF[2]!=ucAutoIp[2])||(ArgTBuF[3]!=ucAutoIp[3]))
				{
						ucAutoIp[0]=ArgTBuF[0];
						ucAutoIp[1]=ArgTBuF[1];
						ucAutoIp[2]=ArgTBuF[2];
						ucAutoIp[3]=ArgTBuF[3];
					
					  if(ucAutoIp[1]!=0xFE)
						{//�Զ�IP�ɹ��Ҳ��Ǿ�̬IP		
								if((ucAutoIp[0]!=Ipdata[0])||
									 (ucAutoIp[1]!=Ipdata[1])||
									 (ucAutoIp[2]!=Ipdata[2])||
									 (ucAutoIp[3]!=Ipdata[3]))
								{//��ѯ�����µ�IP,��������							  
										Ipdata[0]=ucAutoIp[0];
										Ipdata[1]=ucAutoIp[1];
										Ipdata[2]=ucAutoIp[2];
										Ipdata[3]=ucAutoIp[3];
									
										//֪ͨ�����˿�
										//K3000_SetDhcpIp(ArgTBuF);
										
										//Ҫ�����Զ���λ������:��Ҫ��?
								}		
						}
				}
								
        tick = __FALSE;
				return; 
		}
				
		return;
}



////////////////////////////////////////////////////////////////////////////////////////////
//Hitxyh:�޸���IP_ADD,IP_MASK,IP_GNY,IP_MAC, IP_DNS��ʱ��,����Ҫ��λ											//
//       �޸���TCP_PORT, UDP_PORT, DHCP, SECURE_MODE ����Ҫ��λ														//
////////////////////////////////////////////////////////////////////////////////////////////
void MAIN_ActionOnEthernet(void)
{
    unsigned char n=0;
	
		MAIN_GetAutoIp();
	   
    if(net_reset_flag==0)
			  return;
		    
		//���ж��Ƿ�����Ҫ����Ĳ���
		//�ȱ��浽EEPRM��
		MAIN_EnableNetPolling(0);
		
		HAL_I2CReadBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_IP_NUMBER,uc16OfBuf,15);

		if(net_dhcp_enable==0)
		{		
				Ipdata[0]=uc16OfBuf[0];
				Ipdata[1]=uc16OfBuf[1];
				Ipdata[2]=uc16OfBuf[2];
				Ipdata[3]=uc16OfBuf[3];
		}
		else
		{
				Ipdata[0]=ucAutoIp[0];
				Ipdata[1]=ucAutoIp[1];;
				Ipdata[2]=ucAutoIp[2];;
				Ipdata[3]=ucAutoIp[3];;
		}
		
		NetMaskdata[0]=uc16OfBuf[4];
		NetMaskdata[1]=uc16OfBuf[5];
		NetMaskdata[2]=uc16OfBuf[6];
		NetMaskdata[3]=uc16OfBuf[7];
		
		GateWaydata[0]=uc16OfBuf[8];
		GateWaydata[1]=uc16OfBuf[9];
		GateWaydata[2]=uc16OfBuf[10];
		GateWaydata[3]=uc16OfBuf[11];
		
		
		net_reset_flag = 0;		
		
		reset_net();
		
		MAIN_EnableNetPolling(1);
    
		return;
}
//

//void HTTP_LoopSendData2Web(void)
//{
//    if (webEdidRequestFlag!=0)
//    {
//        if (webEdidRequestFlag==1)
//        {
//            HAL_DelayMs(20);
//            Back_Command2Web(PROT_SET_HTTP_DOWNLOAD);
//        }else if (webEdidRequestFlag==2)
//        {
//            HAL_DelayMs(20);
//            Back_Command2Web(PROT_SET_HTTP_DOWNLOAD);
//        }else if (webEdidRequestFlag==3)
//        {
//            HAL_DelayMs(20);
//            Back_Command2Web(PROT_SET_HTTP_DOWNLOAD);
//        }
//        webEdidRequestFlag=0;
//    }
//}


#define LONG_REACH_MODE			0
#define HDBT_MODE						1 
#define HDBT_AUTO_MODE			2 
//���Դ���I2C���ò���ģʽ��ʧ��,����GPIO����
void VS100TX_Init_Mode(unsigned char mode)
{  
		if(mode==LONG_REACH_MODE)
				HAL_Cpld_Write(0x11,0x00);   //low
		else if(mode==HDBT_MODE)
        HAL_Cpld_Write(0x11,0x01);   //High
    else if(mode==HDBT_AUTO_MODE)
    {
        #define VS100TX_ADD       0x50
        #define VS100TX_CMD_FB    0xFB
        #define VS100TX_CMD_F1    0xF1
        #define VS100TX_CMD_F5    0xF5
        #define VS100TX_CMD_F6    0xF6
        
        const unsigned char AutoModeTab[] = {0x01,0x00,0x07};
//        const unsigned char HDBTModeTab[] = {0x01,0x01,0x07};
//        const unsigned char LongReachModeTab[] = {0x01,0x01,0x06};
        unsigned char ucBUF[5]; 
        unsigned char vs100buf[3];
 
/*     test       
        ucBUF[0]=0x00;
        ucBUF[1]=0x00;
        ucBUF[2]=0x00;
        ucBUF[3]=0x00;
        ucBUF[4]=0x00;
        HAL_I2C16ReadBlock(HAL_IIC_VS100TX1,VS100TX_ADD,(VS100TX_CMD_F5<<8)|0x02,ucBUF,5); //test,Read DEVICE��DATE 
        

        ucBUF[0]=0x00;
        ucBUF[1]=0x00;
        ucBUF[2]=0x00;
        ucBUF[3]=0x00;
        ucBUF[4]=0x00;
        //VX100_ReadIicByte(portIndex,VS100TX_ADD,VS100TX_CMD_F5,0x04,ucBUF,5);
        HAL_I2C16ReadBlock(HAL_IIC_VS100TX1,VS100TX_ADD,(VS100TX_CMD_F5<<8)|0x04,ucBUF,5);//test,Read DEVICE��TYPE

        //Get Operation Mode
        ucBUF[0]=0x00;
        ucBUF[1]=0x00;
        HAL_I2C16ReadBlock(HAL_IIC_VS100TX1,VS100TX_ADD,(VS100TX_CMD_F6<<8)|0x05,ucBUF,2);//test,Read DEVICE��TYPE

        ucBUF[0]=0x00;
        ucBUF[1]=0x00;
        ucBUF[2]=0x00;
        ucBUF[3]=0x00;
        ucBUF[4]=0x00;
        //Read Length
        HAL_I2C16ReadBlock(HAL_IIC_VS100TX1,VS100TX_ADD,(VS100TX_CMD_F6<<8)|0x07,ucBUF,5);
 */       
        //Set Mode Capability
        //ʹ�� Long Reach Mode
        ucBUF[0]=0x06;        //06
        ucBUF[1]=0x01;        //01
        HAL_I2CWriteBlock (HAL_IIC_VS100TX1, VS100TX_ADD, 0xF1 ,ucBUF, 2);
        
        //Get  Mode Capability
        ucBUF[0]=0x00;
        ucBUF[1]=0x00;
        HAL_I2C16ReadBlock(HAL_IIC_VS100TX1,VS100TX_ADD,(0xF2<<8)|0x06,ucBUF,5);
        //ʹ�� HDBaseT Mode

        ucBUF[0]=0x07;        //07
        ucBUF[1]=0x01;        //01
        HAL_I2CWriteBlock (HAL_IIC_VS100TX1, VS100TX_ADD, 0xF1 ,ucBUF, 2);
         
        //Get  Mode Capability
        ucBUF[0]=0x00;
        ucBUF[1]=0x00;
        HAL_I2C16ReadBlock(HAL_IIC_VS100TX1,VS100TX_ADD,(0xF2<<8)|0x07,ucBUF,2);
        
        //ʹ�� AUTO Mode ʱ���� LPPF1��2ģʽ
        ucBUF[0]=0x04;        //04
        ucBUF[1]=0x01;        //01
        HAL_I2CWriteBlock (HAL_IIC_VS100TX1, VS100TX_ADD, 0xF8 ,ucBUF, 2);
        
        memcpy(vs100buf,(void*)&AutoModeTab,3);

        HAL_I2CWriteBlock(HAL_IIC_VS100TX1,VS100TX_ADD,VS100TX_CMD_FB,vs100buf,sizeof(vs100buf));
    }
    return ;
}


void Hardware_Init_Wm8805_PCM5142_CS5340(void)
{   
    
    HAL_Cpld_Write(0x17,0x03);
    HAL_DelayMs(100);
    HAL_Cpld_Write(0x17,0x00);
    HAL_DelayMs(100);
    HAL_Cpld_Write(0x17,0x03);
    
    PCM5142DrvDeviceInitialize ();
    
    Wm8805DrvDeviceInitialize(false);
    
}

/********************************************************************************************
*��      ����AnalogAudioDetect()
*��      �ܣ����ģ��������Ƿ�������Ƶ�������ж��Ƿ���Ҫ�Զ�Ƕ����Ƶ
*��Ա��������        
********************************************************************************************/
void AnalogAudioDetect(void)
{
    unsigned long ulTemp = 0;
    
	ulTemp = GPIO_ReadValue(ARM_GPIO_PORT0);   
   	if(ulTemp&(1<<22))
	{        
		if(MainDev.mAudioAnalogIn!= ANALOG_CONNECTED)
        {    
            MainDev.mAudioAnalogIn= ANALOG_CONNECTED;  
                   
            if(MainDev.mAudioMixMode==MIX_AUDIO_AUTO)    //�Ƿ������Ƶֻ�����Զ�Ƕ��ģʽ�²�������
            {
                SET_EVENT(EV_OUT_0_SIG);
            }
        }
	}		
	else
	{
        if(MainDev.mAudioAnalogIn!= ANALOG_NONE)
        {    
            MainDev.mAudioAnalogIn= ANALOG_NONE;     
            if(MainDev.mAudioMixMode==MIX_AUDIO_AUTO)     //�Ƿ������Ƶֻ�����Զ�Ƕ��ģʽ�²������ì





            {
                SET_EVENT(EV_OUT_0_SIG);
            }
        }
	}	
}

/********************************************************************************************
*��      ����SelectAnalogAudioOut()
*��      �ܣ�ѡ�����ƽ����Ƶ��Ӧ����Ƶ��Դ
*��Ա������ ucWhichAudio = 0---��Ƕ��Ƶ        
            ucWhichAudio = 1---ģ��ֱͨ��Ƶ
********************************************************************************************/
void SelectAnalogAudioOut(unsigned char ucWhichAudio)
{
    if(ConDev.mConAudioMute[0] == MUTE_IS_ON)
        HAL_Cpld_Write(0x09,0x00);
    else
    {
        if(MainDev.mAudioOutState==AUDIO_ARC)	
        {       
            HAL_Cpld_Write(0x09,0x01);
            HAL_Cpld_Write(0x1A,0x01);        
        }    
        else //if(MainDev.mAudioOutState==AUDIO_De_Embed)	
        {
            if(ucWhichAudio==AUD_OUT_FROM_DE_EMBED)
            {
                HAL_Cpld_Write(0x09,0x01);  
                HAL_Cpld_Write(0x1A,0x01);	//��Ƕ��Ƶ
            }
            else if(ucWhichAudio==AUD_OUT_FROM_ANALOG)
            {
                HAL_Cpld_Write(0x09,0x01);               
                HAL_Cpld_Write(0x1A,0x00);		//ģ����Ƶֱͨ
            }
            else if(ucWhichAudio==AUD_OUT_FROM_NONE)
            {
                HAL_Cpld_Write(0x09,0x00);
            }
        }
    }
}

void Reset_MRA_Aud_Mute(BOOL isMute)
{
    if(isMute==TRUE)
        HAL_Cpld_Write(0x09,0x00);
    else
        HAL_Cpld_Write(0x09,0x01);
}

void Detect_Board_Type(void)
{
    unsigned char ucTemp =0;
    
    ucTemp = HAL_Cpld_Read(0x02);
    	
		MainDev.mDeviceType = DEVICE_IS_16S1;//Ĭ��
		if((ucTemp&0x08)==0x00)		//��������	
    {
        MainDev.mDeviceType = DEVICE_IS_16S1;
    }
		else
		{
				if((ucTemp&0x04)==0x04)
				{
						MainDev.mDeviceType = DEVICE_IS_8S1;
				}
				else		
				{
						MainDev.mDeviceType = DEVICE_IS_10S1;               //VS-1611DT
				}
    }
    
    //
    if((ucTemp&0x02)==0x02)		
        MainDev.mDevOutType = DEVICE_IS_DT;
    else 
				MainDev.mDevOutType = DEVICE_IS_UHD;
    
		//
    if((ucTemp&0x01)==0x01)		
        MainDev.mFactoryType = DEVICE_IS_KRM;
    else 
				MainDev.mFactoryType = DEVICE_IS_LIG;
    
        
    
    if(MainDev.mDeviceType==DEVICE_IS_16S1)
    {
        ucMaxValidChipNum = 3;    
        ucMaxValidPortNum = 16;
    }
    else if(MainDev.mDeviceType==DEVICE_IS_10S1)
    {
        ucMaxValidChipNum = 2;    
        ucMaxValidPortNum = 10;
    }
    else if(MainDev.mDeviceType==DEVICE_IS_8S1)
    {
        ucMaxValidChipNum = 2;    
        ucMaxValidPortNum = 8;
    }

    
    if(MainDev.mDevOutType == DEVICE_IS_UHD)
        ucMaxOutPortNum = 1;
    else 
        ucMaxOutPortNum =2;    
    
//		
//    if(MainDev.mFactoryType==DEVICE_IS_LIG)
//    {
//        memcpy(DEF_IP,LIG_DEF_IP,4);
//        memcpy(DEF_GATEWAY,LIG_DEF_GATEWAY,4);
//        memcpy(DEF_NETMASK,LIG_DEF_NETMASK,4);
//        memcpy(DEF_MAC,LIG_DEF_MAC,6);
//        
//        DEF_UDP_PORT =LIG_DEF_UDP_PORT;
//        DEF_TCP_PORT=LIG_DEF_TCP_PORT;   
//    }
//    else
//    {
//        memcpy(DEF_IP,KRM_DEF_IP,4);
//        memcpy(DEF_GATEWAY,KRM_DEF_GATEWAY,4);
//        memcpy(DEF_NETMASK,KRM_DEF_NETMASK,4);
//        memcpy(DEF_MAC,KRM_DEF_MAC,6);
//        
//        DEF_UDP_PORT =KRM_DEF_UDP_PORT;
//        DEF_TCP_PORT=KRM_DEF_TCP_PORT;   
//    }
//		
//    InitIR();
//    USB_SER_BUF_RESET(usbser_out);
}
//
//

void DeviceSeekFsm(void)
{
    static unsigned char iRunCount=0;
    			
    //Run LED,��GPIO�ٶ���
		iRunCount++;	
		if (iRunCount>=20)
        iRunCount=0; 
    if (iRunCount==0)
        HAL_Cpld_Write(0x12,0x01);
		if (iRunCount==10)
        HAL_Cpld_Write(0x12,0x00);

 
    //Ӳ����Ƶ�ԭ��֧��ARCʱ��������֧��StepIn//Ӳ����Ƶ�ԭ��ARC�����������
    if(MainDev.mAudioOutState==AUDIO_ARC)	
    {       
        HAL_Cpld_Write(0x14,0x00);		    //=1��Ƕ��Ƶ��=0 ѡ��ARC    
        HAL_Cpld_Write(0x15,0x03);		//��ARCģʽ�£����ر����5V                 
    }    
    else if(MainDev.mAudioOutState==AUDIO_DE_Embed)	
    {
        HAL_Cpld_Write(0x14,0x01);		//=1��Ƕ��Ƶ��=0 ѡ��ARC   
        if(MainDev.mTxClose==TX_OFF)
            Sii9575_DeviceEmbedAudioEnable(CHIP_OUT,false);
        else
            Sii9575_DeviceEmbedAudioEnable(CHIP_OUT,true );
    }
    
    if(ser_isBufEmpty())    //����������
    {
        if(MainDev.mUartState==UART_CPU)		 //uart to CPU
        {	
            HAL_Cpld_Write(0x1d,0x01);//(LED_ON);
            GPIO_SetValue(ARM_GPIO_PORT3,(1<<26));  //��������״̬���÷���                      
        }
        else if(MainDev.mUartState==UART_HBT)	   //uart to  HDBasT
        {
            HAL_Cpld_Write(0x1d,0x00);//(LED_OFF);
            GPIO_ClearValue(ARM_GPIO_PORT3,(1<<26)); 
        }
        if(UartChgFlag)
        {
        
           //�ȴ����������ɺ��ȥ�Ĳ�����
            UartChgFlag = 0;
            ser_ClosePort(0);
            ser_InitPort0(MainDev.mBR);
            ser_OpenPort(0);
        }
    }

    #ifdef SUPPORT_VOLTAGE_DETECT
    Handle_Voltage();
    #endif
    DipUIFsm();
    AnalogAudioDetect();        
    #ifdef SUPPORT_STEPIN_FUN        
    STP_Detect(0);
    STP_SetStepInBtnLed();
    #endif

    KB_ScanRemoteSwitch();
    KB_ScanUartKey();
    
    LoopNVRAM();

    JudgeInputToPulldownOutput5V();
    IR_ControlSwitch();
}

void DeviceRovingARCFsm(void)
{
    Sii9575AppCecRoving(SII_CPI_RX_A0);     //ARC #0
    Sii9575AppCecRoving(SII_CPI_TX_A1);     //ARC #1
   
    if(ucArcResetFlag)
    {
				if(usArcResetWaitCount==0)
				{
						Sii9575AppInstanceSet(CHIP_FOR_ARC); 
						Sii9575_CEC_Init(SII_CPI_TX_A1,CEC_LOGADDR_UNREGORBC,0x0000);//��ʼ������������豸���ɹ����豸
						Sii9575_CEC_Init(SII_CPI_RX_A0,CEC_LOGADDR_UNREGORBC,0x0000);
						usArcResetWaitCount=1;   
				}
				if(usArcResetWaitCount>=100)
				{
					usArcResetWaitCount = 0;
					ucArcResetFlag = 0;            
					Sii9575AppCecRestart(CHIP_FOR_ARC);
				}
    }
}

/*!
 * @fn int main (void)
 * @details This is the application main entry point
 */
int main (void)
{		 	
		unsigned char ChipCount =0;
    int format_flag=0;
    unsigned char n;     
            
    MAIN_EnableNetPolling(0);
	
		HAL_PlatformInit();
	
		//close the annlog audio 
    Reset_MRA_Aud_Mute(TRUE); 
	
    SPI1_FLASH_Init();
    
    SetAllLED(LED_ON);   
         	         
    disk_initialize(0);
    
    SSTF016B_RdID(Manu_ID,&flashid);
    SSTF016B_RdID(Dev_ID,&flashid);
		//0x00BF258E
    SSTF016B_RdID(Jedec_ID,&flashid);
         
		if (format_flag==1)
		{ 
        SSTF016B_Erase(0,255);
		} 
    
    getdisk_status=f_mount(&FatFs, "", 1);
    
    if ((getdisk_status==FR_NO_FILESYSTEM)||
        (getdisk_status==FR_DISK_ERR)||
        (getdisk_status==FR_INVALID_PARAMETER))
    {
        getdisk_status=f_mkfs("",1,MAX_ADDR);
        f_setlabel("lOS");
    }
    //дlog--������ʱ����Ҫ����log�ļ�
    FDISK_CreateFile2Flash(Sys_log_path);
    FDISK_CloseFile2Flash();
    
		//Note:USB_UP_LED Pin�������RST_RMII Pin����,
		//������USBʱ,USB_UP_LED�ᱻ����,���RST_RMIIһֱ��λ
		Rst_Dp83848();
        
		//�����ô���ָ��CPU
    GPIO_SetValue(ARM_GPIO_PORT3,(1<<26));  

    Detect_Board_Type();
    
    //Read M25 for test
    M25_SPI1_FLASH_Init();
    GPIO_SetDir(ARM_GPIO_PORT0,(1<<25),1);		 //Out
    GPIO_ClearValue(ARM_GPIO_PORT0,(1<<25)); //cpu spi1 to vs100 flash
    M25_Flash_RdID(Manu_ID,&m25_flashid);
    M25_Flash_RdID(Dev_ID,&m25_flashid);
    M25_Flash_RdID(Jedec_ID,&m25_flashid);
    GPIO_SetValue(ARM_GPIO_PORT0,(1<<25)); //cpu spi1 to vs100 flash
          
		#ifdef SUPPORT_COMM_PROTOCOL
		InitQueue();    
		#endif
    
    #ifdef SUPPORT_STEPIN_FUN
    STP_Init();
    #endif
    
		//��ʼ�����漰�ĸ��ֲ���
    Sii9575AppSysCgh(ST_D0_INIT); 
		
    Sii9575AppSysCgh(ST_D0_POWER);

    usDetectCount = 1;
    MainDev.mEV=EV_NONE;
		
    
		#ifdef SUPPORT_COMM_PROTOCOL
		COMMAND_Init();
		#endif
		
		
		MAIN_EnableNetPolling(0);
		net_reset_flag = 1;		
		for (;;)
		{	
				if (SysTickCnt>100000000)
						SysTickCnt=0;
                       
				SeekQueue();				
				QUEUE_ProgTxPublicQueu() ;
				
				//
				LoopNVRAM();				
				
				if (KR3000_CommandMODE==FILE_UPLOADMODE)
						continue;          
				
				MAIN_ActionOnEthernet();
        //loop_net();
				
				
        DeviceSeekFsm();        
                
        if (MainDev.mDebugRunning==TRUE)
            continue; 	
				
        
        Si9575_SpdifAudOutOnlyPCM();    //���÷�PCM��ģ������
        
        //HTTP_LoopSendData2Web();
        
        AutoSwStateFsm();               

        Sii9575AppSysFsm();
       
        DeviceRovingARCFsm();
       
        Sii9575AppOutputLoop(CHIP_OUT);         

        START_PRO_EVENT_CLIP(0)	 
        
				PRO_CASE_EVENT(EV_RESET,0)
				{
						CLR_EVENT(EV_RESET);
						Sii9575AppSysCgh(ST_D0_RESET);
				}
				//
				//
				PRO_CASE_EVENT(EV_POWER_ON,0)
				{
						CLR_EVENT(EV_POWER_ON);
						Sii9575AppSysCgh(ST_D0_INIT);
				}
        //
				//
				PRO_CASE_EVENT(EV_CH_SWITCH,2)            
				{
						CLR_EVENT(EV_CH_SWITCH);
						if((MainDev.mNextSelectChip!=MainDev.mRxChipSel)||(MainDev.mNextSelectInPort!=MainDev.mRxPortSel))
						{
								if(MainDev.mAudioMixMode!=MIX_AUDIO_FORCE)
										SelectAnalogAudioOut(AUD_OUT_FROM_NONE);//�л��ر�ƽ�������
								switchch(MainDev.mNextSelectChip,MainDev.mNextSelectInPort);
								MainDev.mRxPortSel = MainDev.mNextSelectInPort;
								MainDev.mRxChipSel = MainDev.mNextSelectChip;
								
								Sii9575AppOutCgh(CHIP_OUT,CHIP_TX_0,ST_D3_WAIT_DELAY);
								Sii9575AppOutCgh(CHIP_OUT,CHIP_TX_1,ST_D3_WAIT_DELAY);
									
						} 						
				}
				//
				//
				PRO_CASE_EVENT(EV_SW_KEY,3)
				{
						CLR_EVENT(EV_SW_KEY);
						#ifdef SUPPORT_COMM_PROTOCOL
						if(MainDev.mComFormat == KRM3000)
						{
								if(MainDev.mTxClose==TX_ON)//if (MainDev.mOutPowerOn==OUT_ON)
										K3000_KsSwitchSendCom(0,1,get_In_Num(MainDev.mNextSelectChip,MainDev.mNextSelectInPort));
								else 
										K3000_KsSwitchSendCom(0,1,ucMaxValidPortNum);                   
						}
						#endif
				}
				//
				//
				PRO_CASE_EVENT(EV_OUT_0_SIG,4)	   //����¼�����Ϊ����Main Pipe��ΪԴ���
				{//���¼���������ͨ���źŸı�������ģ������¼����Թ���
						CLR_EVENT(EV_OUT_0_SIG);
						//HDMI Output
						Sii9575AppOutCgh(CHIP_OUT,CHIP_TX_0, ST_D3_WAIT_DELAY);
						//HDBaseT OutPut
						Sii9575AppOutCgh(CHIP_OUT,CHIP_TX_1, ST_D3_WAIT_DELAY);
				}
				//
				//
				PRO_CASE_EVENT(EV_EDID,5)
				{
                CLR_EVENT(EV_EDID);
                memset(uc256Buf,0xff,256);

                if(MainDev.mInEDIDChFrom==CHIP_TX_0)
                {
                    memcpy(uc256Buf,MonDev[0].mMonEDIDBuf,256);
                    //����Kramer��Ҫ�󣬸���DVI��EDIDʱ����Ҫ��DVI��EDID��ΪHDMI��EDID����,û����Ƶ��ʱ�����пռ������£�Ҳ��Ҫ�����Ƶ��
                    if((uc256Buf[126]==0)||(uc256Buf[126]>0x03))//��չ�������3
                    {  
                        uc256Buf[126] = 0x01;
                      //���¼�������
                        uc256Buf[127]=EDID_GetBufDataChecksum(uc256Buf);	  //ǰ128byte�������
                        memcpy(uc256Buf+128,(void *)&(EDID_array[128]),128);
                        uc256Buf[255]=EDID_GetBufDataChecksum(uc256Buf+128);
                    }else if (uc256Buf[128]!=2)
                    {
                        memcpy(uc256Buf+128,(void *)&(EDID_array[128]),128);
                        uc256Buf[255]=EDID_GetBufDataChecksum(uc256Buf+128);
                    }   
                }
                else if(MainDev.mInEDIDChFrom==CHIP_TX_1)
                {
                    memcpy(uc256Buf,MonDev[1].mMonEDIDBuf,256);
                    MainDev.mOutPHY =MonDev[1].mMonPA;//����PA��������ARC����������HDMI�ڵ�PA
                    //����Kramer��Ҫ�󣬸���DVI��EDIDʱ����Ҫ��DVI��EDID��ΪHDMI��EDID����,û����Ƶ��ʱ�����пռ������£�Ҳ��Ҫ�����Ƶ��
                    if((uc256Buf[126]==0)||(uc256Buf[126]>0x03))//��չ�������3
                    {  
                        uc256Buf[126] = 0x01;
                      //���¼�������
                        uc256Buf[127]=EDID_GetBufDataChecksum(uc256Buf);	  //ǰ128byte�������
                        memcpy(uc256Buf+128,(void *)&(EDID_array[128]),128);
                        uc256Buf[255]=EDID_GetBufDataChecksum(uc256Buf+128);
                    }else if (uc256Buf[128]!=2)
                    {
                        memcpy(uc256Buf+128,(void *)&(EDID_array[128]),128);
                        uc256Buf[255]=EDID_GetBufDataChecksum(uc256Buf+128);
                    }
                }
                else if(MainDev.mInEDIDChFrom==WEB_UPLOAD_EDID)
                {
                    memcpy(uc256Buf,webEdidBuf,256);//ͨ���������ҳ���������۲�������Ӧ���޸���Ƶ��Ȳ���
										MainDev.mOutPHY = MonDev[1].mMonPA;	//����PA��������ARC����������HDMI�ڵ�PA
                }
                else if(MainDev.mInEDIDChFrom==COMMAND_UPLOAD_EDID)
                {
                }
                else			
                    memcpy(uc256Buf,(void*)&EDID_array,256);  
            
                if (EDID_GetBufDataValid(uc256Buf)==0)//���Ƶ�Buf�е�EDID��Ч
                {			   
                        //����ĳ�����Ҫuc256DataBuf�������װ���������������EDID���ݣ�			        
                    memcpy(uc256Buf,(void*)&EDID_array,256);     
                    MainDev.mInEDIDChFrom=DEF_EDID_PORT;  							        	
                }
                     
                //���ﲻ���֣�ֻҪ��ҳ������DC��CS�������κθ���EDID��ʱ�򶼻ᰴ���ø���
                if(MainDev.mEDIDDC == EDID_DC_24)
                {
                    EDID_SetEDIDPar2Buf(uc256Buf+128,PARA_DC_SET,SET_DC_24);
                    uc256Buf[255]=EDID_GetBufDataChecksum(uc256Buf+128);	  //��128byte�������				 
                }
                    
                if(MainDev.mEDIDCS == EDID_CS_RGB444)
                {
                    EDID_SetEDIDPar2Buf(uc256Buf+128,PARA_CS_SET,SET_CS_RGB);
                    uc256Buf[255]=EDID_GetBufDataChecksum(uc256Buf+128);	  //��128byte�������				
                } 
                if(MainDev.mEDIDAUD == EDID_AUD_2CH_LPCM)
                {
                    EDID_SetEDIDPar2Buf(uc256Buf+128,PARA_AUD_CHAN_SET,SET_AUD_CHAN_2);
                    EDID_SetEDIDPar2Buf(uc256Buf+128,PARA_SPEAKER_SET,SET_FL_FR_SPEAKER);
                    EDID_SetEDIDPar2Buf(uc256Buf+128,PARA_AUD_FORMAT_SET,SET_ONLY_LPCM);
                    uc256Buf[255]=EDID_GetBufDataChecksum(uc256Buf+128);	  //��128byte�������				
                } 
                
                for(n=0;n<16;n++)
                {
                    if ((MainDev.mEDIDSelPort&(1<<n))!=0)
                    {
                        CopyEDID2InEEPROM( get_In_chip_ch(n)>>4,get_In_chip_ch(n)&0x0f, 0);
                        //CopyPA2In(get_In_chip_ch(n)>>4);
                    }                   
                }
//                CopyPA2In(CHIP_IN7_IN12);
                //�����������ѡ��״̬
                MainDev.mEDIDSelPort = 0;
                if((MainDev.mInEDIDChFrom==WEB_UPLOAD_EDID)||(MainDev.mInEDIDChFrom==COMMAND_UPLOAD_EDID))
                    MainDev.mInEDIDChFrom=DEF_EDID_PORT;
     
				}            
        // 
				//
				PRO_CASE_EVENT(EV_AUTO_SWITCH,6)
				{
						CLR_EVENT(EV_AUTO_SWITCH);	
						#ifdef 	SUPPORT_AUTO_SWITCH                
						//������Ҫ�ȴ����е������źż����Ϻ󣬸���Ҫ���л�
						if(ucPowerUpSwFlag==1)
						{
								if(WaitStableCount==0)
										WaitStableCount = 1;					
								if(WaitStableCount >= (BASE_1S_DELAY*FIRST_POWER_ON_SWITCH))
								{
										WaitStableCount = 0;
										ucPowerUpSwFlag = 0;
										CLR_EVENT(EV_AUTO_SWITCH);
										PowerUpGetInSignal();			
										SET_EVENT(EV_OUT_0_SIG);
								}
								else
										SET_EVENT(EV_AUTO_SWITCH);					
						}
						else
						{                    
								GetAutoModeChannel();
								GotoAutoSwitchChannel();	                    
						}  
						#endif
				}
 			
        //
				END_PRO_EVENT_CLIP(6);	  

				ChipCount++;
				if(ChipCount%3==0)
				{    
						if(MainDev.mDeviceType==DEVICE_IS_16S1) 
						{
								Sii9575AppIntRxLoop(CHIP_IN1_IN6);
						
								Sii9575AppInFsm(CHIP_IN1_IN6,CHIP_RX_0); 						 	
								Sii9575AppInFsm(CHIP_IN1_IN6,CHIP_RX_1); 
								Sii9575AppInFsm(CHIP_IN1_IN6,CHIP_RX_2); 						 	
								Sii9575AppInFsm(CHIP_IN1_IN6,CHIP_RX_3); 
								Sii9575AppInFsm(CHIP_IN1_IN6,CHIP_RX_4); 						 	
								Sii9575AppInFsm(CHIP_IN1_IN6,CHIP_RX_5); 
						}						
				}        
				else if(ChipCount%3==1)
				{
						Sii9575AppIntRxLoop(CHIP_IN7_IN12);	
						Sii9575AppInFsm(CHIP_IN7_IN12,CHIP_RX_0); 						 	
						Sii9575AppInFsm(CHIP_IN7_IN12,CHIP_RX_1); 
						Sii9575AppInFsm(CHIP_IN7_IN12,CHIP_RX_2); 						 	
						Sii9575AppInFsm(CHIP_IN7_IN12,CHIP_RX_3); 
						Sii9575AppInFsm(CHIP_IN7_IN12,CHIP_RX_4); 						 	
						Sii9575AppInFsm(CHIP_IN7_IN12,CHIP_RX_5); 
				}
				else if(ChipCount%3==2)
				{
						Sii9575AppIntRxLoop(CHIP_IN13_IN16);
																
						Sii9575AppInFsm(CHIP_IN13_IN16,CHIP_RX_2); 	
						Sii9575AppInFsm(CHIP_IN13_IN16,CHIP_RX_3); 						 
						Sii9575AppInFsm(CHIP_IN13_IN16,CHIP_RX_4);  	
						Sii9575AppInFsm(CHIP_IN13_IN16,CHIP_RX_5);             
				}      
        //
				
        Sii9575AppInstanceSet(CHIP_OUT);

        if(ChipCount%2==0)
        {
            //TX0 for HDBT
            Sii9575AppTxFsm(CHIP_OUT,CHIP_TX_0);           
            Sii9575AppOutFsm(CHIP_OUT,CHIP_TX_0); 
        }  
        else if(ChipCount%2==1)
        { 
            //TX1 for HDMI      	
            Sii9575AppTxFsm(CHIP_OUT,CHIP_TX_1);  
            Sii9575AppOutFsm(CHIP_OUT,CHIP_TX_1);  
        }
				//
	}
    
}

#ifdef SUPPORT_VOLTAGE_DETECT
uint32_t Get_Voltage(ADC_CHANNEL_SELECTION nADC)
 {
	return ( ADC_ChannelGetData(LPC_ADC,nADC));
 }


void VoltageDetect_Switch(unsigned char ucVolNum)
{
        if(ucVolNum==0)
        {    
            SEL_A0_0;
            SEL_A1_0;
            SEL_A2_0;
        }
        if(ucVolNum==1)
        {               
            SEL_A0_1;
            SEL_A1_0;
            SEL_A2_0;
        }
        if(ucVolNum==2)
        {
            SEL_A0_0;
            SEL_A1_1;
            SEL_A2_0;
        }
        if(ucVolNum==3)
        {
            SEL_A0_1;
            SEL_A1_1;
            SEL_A2_0;
        }
        if(ucVolNum==4)
        {               
            SEL_A0_0;
            SEL_A1_0;
            SEL_A2_1;
        }
        if(ucVolNum==5)
        {
            SEL_A0_1;
            SEL_A1_0;
            SEL_A2_1;
        }
        if(ucVolNum==6)
        {
            SEL_A0_0;
            SEL_A1_1;
            SEL_A2_1;
        }
}

void Detect_Voltage(ADC_CHANNEL_SELECTION ADChannel)
{ 
 		unsigned char i;
		volatile uint32_t ADC_Buf = 0;

		uint32_t adc_data;

        HAL_DelayMs(5);     //��֪��Ϊʲô������������ʱ���ݾͳ���

        adc_data = 0;
        ADC_Buf = 0;
        
        for(i = 0;i < 8; i++) 
        {
            ADC_Buf   = Get_Voltage(ADChannel);
            adc_data += ADC_Buf;
        }

        adc_data = (adc_data / 8);                    /* ����8�ν����ǲ�����          */
        adc_data = (adc_data * 3300)/4096;
            
        Detect_VolBuf[ADCNum]   = adc_data;
		
		return;
 }

 
void Handle_Voltage(void)
{ 
    if(ADCNum>=MAX_VOL_DOT_NUM)  
        ADCNum = 0;
    VoltageDetect_Switch(ADCNum);        


    if(usDetectCount>5)     //4*50ms = 200ms
    {
        Detect_Voltage(ADC_CHANNEL_1);                
        ADCNum ++;
        usDetectCount = 1;
    }
    
}

void ClrADStatu(void)
{
	unsigned char n;

	for(n=0;n<MAX_VOL_DOT_NUM;n++)
		Detect_VolBuf[n] = 0;
    
    //��Ƶ�ѹ
	Design_Voltage[0]	= DESIGN_VOLTAGE_SITE0;
	Design_Voltage[1]	= DESIGN_VOLTAGE_SITE1;
	Design_Voltage[2]	= DESIGN_VOLTAGE_SITE2;
	Design_Voltage[3]	= DESIGN_VOLTAGE_SITE3;
    Design_Voltage[4]	= DESIGN_VOLTAGE_SITE4;
	Design_Voltage[5]	= DESIGN_VOLTAGE_SITE5;
	Design_Voltage[6]	= DESIGN_VOLTAGE_SITE6;
}
#endif

//K3000����ʾ��Ϣ����
//void ShowDetectVoltageInfo(void)
//{
//    unsigned char n;
//	if(MainDev.mComFormat == KRM3000)
//    {          
//        ser_String("Design Voltage Info:\0");
//        K3000_SendNum_SHORT(Design_Voltage[0]);
//        ser_String(",");
//        K3000_SendNum_SHORT(Design_Voltage[1]);
//        ser_String(",");
//        K3000_SendNum_SHORT(Design_Voltage[2]);
//        ser_String(",");
//        K3000_SendNum_SHORT(Design_Voltage[3]);
//        ser_String(",");
//        K3000_SendNum_SHORT(Design_Voltage[4]);
//        ser_String(",");
//        K3000_SendNum_SHORT(Design_Voltage[5]);
//        ser_String(",");
//        K3000_SendNum_SHORT(Design_Voltage[6]);
//        K3000_CR_LF();
//        ser_String("Detect Voltage Info:\0");
//        K3000_SendNum_SHORT(Detect_VolBuf[0]);
//        ser_String(",");
//        K3000_SendNum_SHORT(Detect_VolBuf[1]);
//        ser_String(",");
//        K3000_SendNum_SHORT(Detect_VolBuf[2]);
//        ser_String(",");
//        K3000_SendNum_SHORT(Detect_VolBuf[3]);
//        ser_String(",");
//        K3000_SendNum_SHORT(Detect_VolBuf[4]);
//        ser_String(",");
//        K3000_SendNum_SHORT(Detect_VolBuf[5]*2);
//        ser_String(",");
//        K3000_SendNum_SHORT(Detect_VolBuf[6]*3);
//        K3000_CR_LF();
//        
//            //�жϵ�ѹ�Ƿ�����
//        for(n=0;n<MAX_VOL_DOT_NUM;n++) 
//        {
//            if(n==5)
//            {
//                if(((Detect_VolBuf[n]*2*100)>(Design_Voltage[n]*105))||
//                  ((Detect_VolBuf[n]*2*100)<(Design_Voltage[n]*95)))  
//                {   //�����ѹ���ԣ�ֱ���˳�
//                    K3000_CR_LF();
//                    return;
//                }
//            }
//            else if(n==6)
//            {
//                if(((Detect_VolBuf[n]*3*100)>(Design_Voltage[n]*105))||
//                  ((Detect_VolBuf[n]*3*100)<(Design_Voltage[n]*95)))  
//                {   //�����ѹ���ԣ�ֱ���˳�
//                    K3000_CR_LF();
//                    return;
//                }
//            }
//            else
//            {
//                if(((Detect_VolBuf[n]*100)>(Design_Voltage[n]*105))||
//                  ((Detect_VolBuf[n]*100)<(Design_Voltage[n]*95)))  
//                {   //�����ѹ���ԣ�ֱ���˳�
//                    K3000_CR_LF();
//                    return;
//                }
//            }
//        }
//        ser_String("OK!");
//        K3000_CR_LF();
//    }
//}


//void ShowFlashIDInfo(void)
//{
//	if(MainDev.mComFormat == KRM3000)
//    {
//        ser_String("Flash ID:\0");
//        K3000_SendNum_XSHORT(flashid>>16);
//        K3000_SendNum_XSHORT(flashid);
//        K3000_CR_LF();
//        if(flashid==0x00BF258E)
//        {
//            ser_String("OK!");
//            K3000_CR_LF();
//        }
//    }
//}

//void ShowE2promFlag(void)
//{
//	if(MainDev.mComFormat == KRM3000)
//    {
//        ser_String("E2prom state:\0");
//        HAL_I2CReadBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_AA,uc16OfBuf,2);
//        K3000_SendNum_XCHAR(uc16OfBuf[0]);
//        ser_String(",");
//        K3000_SendNum_XCHAR(uc16OfBuf[1]);
//        K3000_CR_LF();
//        if((uc16OfBuf[0]==0xaa)&&(uc16OfBuf[1]==0x55))
//            ser_String("OK!");
//        K3000_CR_LF();
//    }
//}

//void EraseEEPROMFlag(void)
//{
//    if(MainDev.mComFormat == KRM3000)
//    {
//        uc16OfBuf[0] = 0xff;
//        uc16OfBuf[1] = 0xff;
//        HAL_I2CWriteBlock(SAVE_EEPROMBUS,EEPROM_IIC_ADDRESS,EEPROM_AA,uc16OfBuf,2);
//        HAL_DelayMs(20); 
//        ser_String("Erase finished,Please check it");
//        K3000_CR_LF();
//    }
//}

#ifdef SUPPORT_STEPIN_FUN
void ShowStepInConnectInfo(void)
{
	if(MainDev.mComFormat == KRM3000)
    {
        ser_String("StepIN connect state:\0");
        K3000_SendNum_SHORT(STP_GetClientConnectState(0));
        ser_String(",");
        K3000_SendNum_SHORT(STP_GetClientConnectState(1));
        ser_String(",");
        K3000_SendNum_SHORT(STP_GetClientConnectState(2));
        ser_String(",");
        K3000_SendNum_SHORT(STP_GetClientConnectState(3));
        ser_String(",");
        K3000_SendNum_SHORT(STP_GetClientConnectState(4));
        ser_String(",");
        K3000_SendNum_SHORT(STP_GetClientConnectState(5));
        K3000_CR_LF();
	}
}
#endif

//void ShowPriorityList(void)
//{
//	if(MainDev.mComFormat == KRM3000)
//    {
//        ser_String("Priority List:\0");
//        K3000_SendNum_SHORT(AutoSw.mPriority[0]);
//        ser_String(",");
//        K3000_SendNum_SHORT(AutoSw.mPriority[1]);
//        ser_String(",");
//        K3000_SendNum_SHORT(AutoSw.mPriority[2]);
//        ser_String(",");
//        K3000_SendNum_SHORT(AutoSw.mPriority[3]);
//        ser_String(",");
//        K3000_SendNum_SHORT(AutoSw.mPriority[4]);
//        ser_String(",");
//        K3000_SendNum_SHORT(AutoSw.mPriority[5]);
//        K3000_CR_LF();
//	}
//}

void SetOutput5VOffTime(void)
{
	if(MainDev.mComFormat == KRM3000)
    {
        ser_String("Set output off time 1s, \0");
        Debug5vFlag = 1;
        ser_String("Finished back to 30s  \0");
        K3000_CR_LF();
	}
}

//void ShowInputSignalInfor(void)
//{
//    if(MainDev.mComFormat == KRM3000)
//    {
//        unsigned char n;
//        for (n=0;n<ucMaxValidPortNum;n++)
//        {
//            if (InDev[get_In_chip_ch(n)>>4][get_In_chip_ch(n)&0x0f].mInSig==SIG_EXIST)
//                ser_String("SIG_EXIST\0");
//            else if (InDev[get_In_chip_ch(n)>>4][get_In_chip_ch(n)&0x0f].mInSig==SIG_NONE)
//                ser_String("SIG_NONE\0");
//            else 
//                ser_String(",SIG Wrong\0");
//                
//            if (InDev[get_In_chip_ch(n)>>4][get_In_chip_ch(n)&0x0f].mInP5V==1)
//                ser_String(",5V_HIGH\0");
//            else if (InDev[get_In_chip_ch(n)>>4][get_In_chip_ch(n)&0x0f].mInP5V==0)
//                ser_String(",5V_LOW\0");
//            else 
//                ser_String(",5V Wrong\0");
//            
//            if (InDev[get_In_chip_ch(n)>>4][get_In_chip_ch(n)&0x0f].mInHDMI==HDMI_HDMI)
//                ser_String(",HDMI\0");
//            else if (InDev[get_In_chip_ch(n)>>4][get_In_chip_ch(n)&0x0f].mInHDMI==HDMI_DVI)
//                ser_String(",DVI\0");
//            else 
//                ser_String(",TYPE Wrong\0");
//                
//            if (InDev[get_In_chip_ch(n)>>4][get_In_chip_ch(n)&0x0f].mInHDCP==HDMI_IS_HDCP)
//                ser_String(",IS_HDCP\0");
//            else if (InDev[get_In_chip_ch(n)>>4][get_In_chip_ch(n)&0x0f].mInHDCP==HDMI_NON_HDCP) 
//                ser_String(",NON_HDCP\0");
//            else 
//                ser_String(",HDCP Wrong\0n");
//                
//            K3000_CR_LF();
//        }
//    }
//}

//void ShowOutSignalInfor(void)
//{
//   if(MainDev.mComFormat == KRM3000)
//   {
//        unsigned char n;
//       
//        for (n=0;n<ucMaxOutPortNum;n++)
//        {
//            ser_String("OUT\0");
//            if(n==0)
//                ser_String("0\0");
//            else
//                ser_String("1\0");
//            if (OutDev[CHIP_OUT][n].mOutHDCP==CH_IS_HDCP)
//                ser_String(",With HDCP\0");
//            else
//                ser_String(",Without HDCP\0");
//            
//            if (OutDev[CHIP_OUT][n].mOutHDMI==HDMI_HDMI)

//                ser_String(",HDMI\0");
//            else 
//                ser_String(",DVI\0");
//           K3000_CR_LF();
//        }
//    }
//}

//void ShowMonitorInfor(void)
//{
//    if(MainDev.mComFormat == KRM3000)
//    {
//        unsigned char n;
//       
//        for (n=0;n<ucMaxOutPortNum;n++)
//        {
//            ser_String("MONITOR\0");
//            if(n==0)
//                ser_String("0\0");
//            else
//                ser_String("1\0");
//            if (MonDev[n].mMonIsValidDDC==DDC_BROKEN)
//                ser_String(",DDC be broden\0");
//            else
//                ser_String(",DDC be right\0");
//            if (MonDev[n].mMonHPD==HPD_HIGH)
//                ser_String(",HPD High\0");
//            else
//                ser_String(",HPD Low\0");
//            
//            if (MonDev[n].mMonHDCP==CH_IS_HDCP)
//                ser_String(",With HDCP\0");
//            else
//                ser_String(",Without HDCP\0");
//           
//            
//           K3000_CR_LF();   
//        }
//    }
//}

//void ShowMonitorHDCPInfor(void)
//{
//    if(MainDev.mComFormat == KRM3000)
//    {
//        unsigned char n;
//       
//        for (n=0;n<ucMaxOutPortNum;n++)
//        {  
//            ser_String("Get MONITOR TX\0");
//            if(n==0)
//                ser_String("0\0");
//            else
//                ser_String("1\0");
//         
//            if (MonDev[n].mMonHDCP==CH_IS_HDCP)
//            {
//                ser_String("HDCP DDC OK\0");
//            }else 
//            {
//                ser_String("HDCP DDC Err\0");
//            }  
//            K3000_CR_LF();       
//        }
//    }
//}



//�δ��ʱ��������
static void timer_poll (void) 
{
  	/* System tick timer running in poll mode */

	static unsigned long lstTime=0;
	unsigned long s; 
  	//if (SysTick->CTRL & 0x10000)
	s=SysTickCnt-lstTime;
	if (s>10)
	{
		/* Timer tick every 1 ms */
		timer_tick ();
		tick = __TRUE; 
		lstTime=SysTickCnt;
	}
}
//File Disk����ز�������
void FDISK_CreateFile2Flash(const char* logPath)
{
    //FIL File;/* File objects */
    //char *pCh;
    static int n;
    if (f_open(&FileUpload,logPath,FA_READ|FA_WRITE)==FR_OK)
    {
        n=1;
    }
    else if (f_open(&FileUpload,logPath,FA_READ|FA_WRITE|FA_CREATE_NEW)==FR_OK)
    {   n=2;
    }else
    {
        n=0;
        return ;
    }
    if (n==1)
        return;

}

void FDISK_CloseFile2Flash(void)
{ 
    f_close(&FileUpload);             //�ر��ļ�
}

void FDISK_OpenFile2Flash(const unsigned char *logStr,int n)
{
    //char *pCh;
    unsigned int m;

    f_write(&FileUpload,logStr,n,&m);
}


unsigned long FDISK_GetFlashFileSize(void)
{ 
    return f_size(&FileUpload);           
}

unsigned char FDISK_GetFlashFileErr(void)
{ 
    return f_error(&FileUpload);            
}

unsigned char FDISK_CheckFlashFileCRC(const char* logPath,unsigned short UpLoadChecksum)
{ 
    #define ReadbufSize1  4096
    #define ReadbufSize2  256
    
    unsigned long n = 0,SizeCount = 0;
    unsigned short usFileRedun = 0;
    unsigned int i,Remainder = 0;    //ȡ��֮���ٿ�ȡ���
    unsigned long getfilesize = 0;
    unsigned char ucGetbuf1[ReadbufSize1] = {0};


    getfilesize =  f_size(&FileUpload);
    if(getfilesize>ReadbufSize1)
    {
        SizeCount = getfilesize/ReadbufSize1;
        Remainder = getfilesize%ReadbufSize1;
        if(f_open(&FileUpload,logPath,FA_READ|FA_WRITE)==FR_OK)
        {
            for(n=0;n<SizeCount;n++)
            {
                f_read(&FileUpload,ucGetbuf1,ReadbufSize1,&byteW);
                //����У���
                for(i=0;i<ReadbufSize1;i++)              
                    usFileRedun +=ucGetbuf1[i];
                
            }
            f_read(&FileUpload,ucGetbuf1,Remainder,&byteW);
            for(i=0;i<Remainder;i++)
                usFileRedun +=ucGetbuf1[i];
            
        }
        f_close(&FileUpload);  
    }
    else if((getfilesize>ReadbufSize2)&&(getfilesize<=ReadbufSize1))
    {
        SizeCount = getfilesize/ReadbufSize2;
        Remainder = getfilesize%ReadbufSize2;
        if(f_open(&FileUpload,logPath,FA_READ|FA_WRITE)==FR_OK)
        {
            for(n=0;n<SizeCount;n++)
            {
                f_read(&FileUpload,ucGetbuf1,ReadbufSize2,&byteW);
                //����У���
                for(i=0;i<ReadbufSize2;i++)
                    usFileRedun +=ucGetbuf1[i];
            }
            f_read(&FileUpload,ucGetbuf1,Remainder,&byteW);
            //����У���
            for(i=0;i<Remainder;i++)
                usFileRedun +=ucGetbuf1[i];
        }
        f_close(&FileUpload);
    }
    else
    {
        if(f_open(&FileUpload,logPath,FA_READ|FA_WRITE)==FR_OK)
        {
            f_read(&FileUpload,ucGetbuf1,getfilesize,&byteW);
            //����У���
            for(i=0;i<getfilesize;i++)
                usFileRedun +=ucGetbuf1[i];
        }
        f_close(&FileUpload);  
        
    }
   
    if(usFileRedun!=UpLoadChecksum)    
        return 1;
    else
        return 0;
}

void FDISK_Record2Log(const char* logPath,char *logStr)
{
    FIL File;/* File objects */
    //char *pCh;
    unsigned int n;
    unsigned char logBuf[8];
    
    if (f_open(&File,logPath,FA_READ|FA_WRITE)==FR_OK)
    {
        if (f_size(&File)>(1024*MAX_FILE_SIZE))
        {
            f_unlink(logPath);
            //f_open(&File,logPath,FA_READ|FA_WRITE|FA_CREATE_NEW);   //���½�������
            return;  
        }
        else
        {
            f_read(&File,logBuf,8,&n);
            while(n!=0)     //�ļ�û������β
            {
                f_read(&File,logBuf,8,&n);
            }
            f_printf(&File,"\r\n<Err>%ldms,%s</Err>\r\n",SysTickCnt,logStr);      
        }
        f_close(&File);
        return;
    }
    else //if (f_open(&File,logPath,FA_READ|FA_WRITE|FA_CREATE_NEW)==FR_OK)
    {
        if (f_open(&File,logPath,FA_READ|FA_WRITE|FA_CREATE_NEW)==FR_OK)
        {
             f_printf(&File,"\r\n<Err>%ldms,%s err</Err>\r\n",SysTickCnt,logStr); 
             f_close(&File);
            return;
        }
        else
            return;
    }
   // f_close(&File);  
}


