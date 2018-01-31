#ifndef __MAIN_H_
#define __MAIN_H_

    //#include "lig_types.h"
    #include "lig_platform.h"	
    #include "edid.h"	
    
    #define SUPPORT_AUTO_SWITCH
    #define SUPPORT_COMM_PROTOCOL
	#define SUPPORT_COMM_FORMAT_LIGUO	   //ע�⣬�����Ķ�Ӧ������web��ҳ����
	#define SUPPORT_COMM_FORMAT_LIG_B
    #define SUPPORT_COMM_FORMAT_KRAMER	   //KRM��Ӧ��KRM��web����,ֻ��Ҫ��Ӧ��Lig/Krm Web�滻Web�����ݼ���
    #define	SUPPORT_USB_COMM
     
//    #define SUPPORT_VOLTAGE_DETECT

    #define APP_DBG(a)  	DBG_MSG(a)
    
    #define CHIP_IN1_IN6        HAL_IIC_SII9575_C
    #define CHIP_IN7_IN12       HAL_IIC_SII9575_B
    #define CHIP_IN13_IN16      HAL_IIC_SII9575_A
    
    #define CHIP_OUT            HAL_IIC_SII9575_A
    #define CHIP_FOR_ARC        CHIP_IN7_IN12

    #define CHIP_TX_0		0
    #define CHIP_TX_1		1  //���TX1��
    

    #define CHIP_RX_0		0
    #define CHIP_RX_1		1 
    #define CHIP_RX_2		2
    #define CHIP_RX_3		3
    #define CHIP_RX_4		4
    #define CHIP_RX_5		5  

    #define IN_PORT1    CHIP_RX_0 
    #define IN_PORT2    CHIP_RX_1  
    #define IN_PORT3    CHIP_RX_2  
    #define IN_PORT4    CHIP_RX_3  
    #define IN_PORT5    CHIP_RX_4  
    #define IN_PORT6    CHIP_RX_5

    #define CHIP_RX_CLOSE	6
    #define CHIP_TPG		7

    #define IN_NONE		8	//�����Զ�ģʽ�¿������л�

    #define OFF_INPUT   6

    #define DEF_EDID_PORT   2
    #define WEB_UPLOAD_EDID		3
    #define COMMAND_UPLOAD_EDID		4

    #define COMMAND_DEFAULT_ADD		0x9b

    #define MAX_IN_PORT				6	
    #define MAX_COUNT_OUT    	1
    #define MAX_IN_CHIP      	3 
    
    #define COUNT_IN	16
    #define COUNT_OUT	(MAX_COUNT_OUT+1)  //COUNT_OUT
    
    #define OUT_PORT1 	CHIP_TX_1        //����HDMI��Ӧ��TX_1���߼���Ӧ�����1��HDBT��Ӧ��TX_0���߼���Ӧ�����2��
    #define OUT_PORT2 	CHIP_TX_0
    


    typedef enum {
        DDC_BROKEN=0,
        DDC_OK=1
    }tagValidDDC;

    typedef enum {
        FE_FOLLOW_IN=0,
        FE_OFF_IN_HDCP=1,
        FE_FORCE_DISCRP=2,
        FE_FORCE_ENCRP=3,
        FE_MIRROR_OUT=4   
    }tagFollowEncrp;
        
    typedef enum {
        OUT_ON=1,
        OUT_OFF=0
    }tagOutPower;

    typedef enum{
        TX_ON=1,
        TX_OFF=0
    }tagTxClose;

    typedef enum {
        HPD_HIGH=1,
        HPD_LOW=0
    }tagHPD;

    typedef enum {
        MSEN_HIGH=1,
        MSEN_LOW=0
    }tagMSEN;
	        
    typedef enum {	
        CH_NON_HDCP=0,
        CH_IS_HDCP=1, 
    }tagCapOfHDCP;

    typedef enum{
        UART_CPU=0,		  //���ڽӵ�CPU
        UART_HBT=1		  //����ͨ��HDBaseT����
    }tagUart;

    typedef enum{
        AUDIO_ARC=0,		  
        AUDIO_DE_Embed=1		   //��Ƕ����Ƶ���Ƕ������
    }tagAudio;

    typedef enum{
        INHDCP_ON=1,		  
        INHDCP_CLOSE=0		//�ر�����ڵ�HDCP����	  
    }tagInHDCP;
    
    typedef enum{
        EDID_DC_PASS=0,		//
        EDID_DC_24=1		   //
    }tagEDIDDC;
    
     typedef enum{
        EDID_CS_PASS=0,		//
        EDID_CS_RGB444=1		   //
    }tagEDIDCS;
		 
     typedef enum{
        EDID_AUD_PASS=0,		//
        EDID_AUD_2CH_LPCM=1		   //
    }tagEDID2CHLPCM;
     
     typedef enum{
        EDID_HDMI_HDMI=1,
        EDID_HDMI_DVI=0
    }tagEDIDTYPE;
     
    typedef enum {
        MIX_AUDIO_I2S = 0,
        MIX_AUDIO_SPDIF = 1,
        MIX_AUDIO_NONE  =2,
    }tagAudioMix;
    
    typedef enum {
        MIX_AUDIO_FORCE = 0,
        NO_MIX_AUDIO  =1,
        MIX_AUDIO_AUTO  =2
    }tagAudioMixMode;
      
    typedef enum {
        ANALOG_NONE = 0,
        ANALOG_CONNECTED = 1,
    }tagAnalogIn;
    
    typedef enum{
        DEVICE_IS_16S1 = 0,
        DEVICE_IS_10S1 = 1,
        DEVICE_IS_8S1 = 2,
    }tagDeviceType;
    
    typedef enum{
        DEVICE_IS_KRM = 0,
        DEVICE_IS_LIG = 1,
    }tagFactoryType;
    
    typedef enum{
        DEVICE_IS_DT = 0,
        DEVICE_IS_UHD = 1,
    }tagOutType;
    
    typedef enum{
        MUTE_IS_ON=1,		  
        MUTE_IS_OFF=0		 
    }tagMuteState;    //mute state
		/*--------------------------------------EDID E2PROM  -------------------------------------*/
    
    #define HAL_IIC_IN_DDC    HAL_IIC_EDID_AT24C02
    
    /*--------------------------------------- EEPROM -------------------------------------*/
    #define SAVE_EEPROMBUS			HAL_IIC_INF_AT24C02
    #define EEPROM_IIC_ADDRESS      0xa0
    
    //E2PROM�����ݴ洢��λ��
    #define EEPROM_AA				0x01
    #define EEPROM_55				0x02
    #define EEPROM_FOLLOWENCRY		0x03	//�������ģʽ
    #define EEPROM_DEVICE_ADD		0x04
    #define EEPROM_FORMAT			0x05
    #define EEPROM_SWITCH_DLY	0x06   //�ڳ�ʱ��洢
    #define EEPROM_SWITCH_STATE		0x07    //�洢״̬���洢ѡ��Ķ˿�
  //                                0x08    //�洢״̬���洢ѡ���оƬ
	
    #define EEPROM_UART_STATE		0x09	//KLink״̬����ʾRS232ѡ��CPU����HDBaseT����	
    #define EEPROM_OUT_POWER       	0x0A

    #define EEPROM_SWITCH_MODE		0x10
    #define EEPROM_AUTOSW_MODE		0x11
    #define EEPROM_AUDIO_MODE		0x12 	//ARC�ش�or��Ƕ��Ƶ���

    #define EEPROM_USB_PORT_MODE    0x13  
    #define EEPROM_5V_TIME_LEVEL	0x14    //0x15 2byte

    #define EEPROM_VIDEO_MUTE_MODE  0x16    //0x17 2byte
    #define EEPROM_UART_BR          0x18  //1 is 115200, else 9600
    #define EEPROM_SW_SPEED 				0x19
    #define EEPROM_AUDIO_MUTE_MODE  0x1A    //0x1B  2byte
    
    #define EEPROM_UDPPORT_NUMBER   0x1C    //0x1D 2byte
            
    #define EEPROM_SN_NUMBER				0x20	//SN��  14λ      Ĭ��0000 0000 001	
    #define EEPROM_MODEL_NAME				0x40	//MODEL NAME 19λ Ĭ��V', 'S', '-', '��', '��', 'H', 'D', 'M', 'I'	
    #define EEPROM_PRIORITY_LIST		0x60	//���ȼ��б�洢
    
    #define EEPROM_IP_NUMBER        0x70        
    #define EEPROM_MASK_NUMBER      0x74
    #define EEPROM_GATEWAY_NUMBER   0x78
    #define EEPROM_DHCP_ENBALE      0x7C
    #define EEPROM_TCPPORT_NUMBER   0x7D
     
    
    #define EEPROM_DNS_NAME         0x80    //MAC NAME 14Bytes Def: KRAMER_+SN LAST 4 
    
    #define EEPROM_INHDCP_MODE		0x90	
    
    #define EEPROM_MAC_ADDRESS		0xA0

    #define EEPROM_IR_MATCH_TIME    (0xA0+6)  //��Ҫ��8��byte 
		
    #define EEPROM_IP_SECURE      0xA8   
    
    #define EEPROM_IR_MATCH_KEY0	0xB0  //��Ҫ��20��byte +MatchKey 4byte*10  
    #define EEPROM_IR_MATCH_KEY1	0xB4 
    #define EEPROM_IR_MATCH_KEY2	0xB8  
    #define EEPROM_IR_MATCH_KEY3	0xBC 
    #define EEPROM_IR_MATCH_KEY4	0xC0  
    #define EEPROM_IR_MATCH_KEY5	0xC4  //��Ҫ��20��byte +MatchKey 4byte*5  
    #define EEPROM_IR_MATCH_KEY6	0xC8 
    #define EEPROM_IR_MATCH_KEY7	0xCC  
    #define EEPROM_IR_MATCH_KEY8	0xD0 
    #define EEPROM_IR_MATCH_KEY9	0xD4  
    
	  /*-------------------------------------- EVENT FLAG -------------------------------------*/
		//��Ҫ�洢�����ݱ��
    #define EEPROM_FLAG1_DEVICE_ADD      	0x0001
    #define EEPROM_FLAG1_FORMAT          	0x0002
    #define EEPROM_FLAG1_SWITCH_DLY     	0x0004
    #define EEPROM_FLAG1_SWITCH_STATE 	 	0x0008
		
    #define EEPROM_FLAG1_OUT_POWER       	0x0010  
    #define EEPROM_FLAG1_SN_NUMBER       	0x0020
    #define EEPROM_FLAG1_MODEL_NAME      	0x0040	
    #define EEPROM_FLAG1_PRIORITY_LIST	 	0x0080
		
    #define EEPROM_Flag1_SWITCH_MODE	 		0x0100
    #define EEPROM_Flag1_AUTOSW_MODE	 		0x0200  
    #define EEPROM_FLAG1_CEC_MODE        	0x0400
    #define EEPROM_FLAG1_FOLLOWENCRY     	0x0800

    #define EEPROM_FLAG1_UART_STATE		 		0x1000
    #define EEPROM_FLAG1_INHDCP_STATE	 		0x2000	 
    #define EEPROM_FLAG1_AUDIO_STATE	 		0x4000 
    #define EEPROM_FLAG1_5V_TIME		 			0x8000   		
    
    #define EEPROM_FLAG2_AUDIO_MUTE_MODE    0x0001  
    #define EEPROM_FLAG2_VIDEO_MUTE_MODE    0x0002  
    #define EEPROM_FLAG2_UART_BAUD_STATE    0x0004
    #define EEPROM_FLAG2_DNS_NAME           0x0008
    
    #define EEPROM_FLAG2_IR_TIM             0x0010
    #define EEPROM_FLAG2_SW_SPEED           0x0020 
    #define EEPROM_FLAG2_UDP_NUMBER         0x0040
    #define EEPROM_FLAG2_MAC_ADDRESS        0x0080   
    
    #define EEPROM_FLAG2_IP_SECURE       		0x0200
    #define EEPROM_FLAG2_IP_NUMBER       		0x0400
    #define EEPROM_FLAG2_MASK_NUMBER     		0x0800
		
    #define EEPROM_FLAG2_GATEWAY_NUMBER  		0x1000
    #define EEPROM_FLAG2_DHCP_ENABLE     		0x2000
    #define EEPROM_FLAG2_USB_MODE        		0x4000   
    #define EEPROM_FLAG2_TCP_NUMBER      		0x8000
		
    /*-------------------------------------- STATE FLAG -------------------------------------*/   
    #define ST_D0_POWER          0x01   
    #define ST_D0_RESET          0x02
    #define ST_D0_INIT           0x03 
    #define ST_D0_NONE           0x0f
    
    #define ST_D1_INIT           0x10
    #define ST_D1_SET_HPD        0x11 
    #define ST_D1_WAIT_SIG       0x12 
    #define ST_D1_WAIT_HD        0x13
    #define ST_D1_WAIT_PKT       0x14
    #define ST_D1_WAIT_AUD       0x15
    #define ST_D1_WAIT_HDCP      0x16
   
    #define ST_D1_NONE           0x1f
    
    #define ST_D2_WAIT_HPD       0x21   //and MSEN
    #define ST_D2_WAIT_DDC       0x22 
    #define ST_D2_GET_EDID       0x23 
    #define ST_D2_NONE           0x2f
    
    #define ST_D3_INIT           0x30 
    #define ST_D3_WAIT_DELAY     0x31  
    #define ST_D3_V_PRO       	 0x32  
    #define ST_D3_A_PRO       	 0x33 
    #define ST_D3_PKT_PRO      	 0x34
    #define ST_D3_HDCP_AUTH      0x35
    #define ST_D3_HDCP_SECRET    0x36 
    #define ST_D3_GEN			 			 0x37	
    #define ST_D3_NONE           0x3f   
        
    #define COUNT_BASE_WRONG         10000  
    #define COUNT_MAX_COUNT					 (COUNT_BASE_WRONG-1)
    #define COUNT_WATI_HPD					 200
    #define COUNT_NONE_WAIT					 10
    #define COUNT_BASE_CLEAR         0
    #define COUNT_BASE_HPD_CHECK     10
    #define COUNT_BASE_SIG_CHECK     10//20//40    //�����������źŶ����Ƚϴ�����ԼӴ������ֵ
    #define COUNT_BASE_DELAY         10
    #define COUNT_MAX_LINK_CHECK     20	 
    #define COUNT_WAIT_REREAD_EDID   25 
    #define COUNT_WAIT_DDC_CHECK     25
    #define COUNT_BLANK_COUNT_FACTOR 10
    #define COUNT_BASE_AUD_CHECK     50
 
    #define ST_D0 0
    #define ST_D1 1
    #define ST_D2 2
    #define ST_D3 3
 
    typedef enum{
	    LIGUO_A = 0,
	    LIGUO_B = 1,
	    KRM2000 = 2,
	    KRM3000 = 3
    }tagComFormat;

	 typedef enum {
        //CM_MODE_0=0,//
        CM_MODE_1,//��ֹʹ��CEC
        CM_MODE_2,//ģʽ2
        CM_MODE_3,//ģʽ3
        CM_MODE_4//,//�����֧��CEC���ܣ�������������LA��PA�����ڲ��ԣ�
        //CM_MODE_5,//�������֧��CEC�����ܣ�������������LA��PA�����ڲ��ԣ�
        //CM_MODE_6 //����ں�������Զ����������ͻ����������͵�������Ҫ���ã�����������������LA��PA��
    }tagCECMode;
    //HDMI Mode
    typedef enum {
        HDMI_HDMI=1,HDMI_DVI=0
    }tagHDMIMode;
    
    typedef enum {
        MON_EDID_OK=1,MON_EDID_ERR=0
    }tagEDIDFlag;

    typedef enum {
        HDMI_NON_HDCP=0,HDMI_IS_HDCP=1,HDMI_IS_MUTE=2
    }tagHDMIHDCPMode;
			 
    //deep color method
    typedef enum
    {
        HDMI_DC_TRUNCATE=0, HDMI_DC_ACTIVE_DITHER
    }tagHDMIDCMethod;
    //deep color
    typedef enum
    {
        HDMI_DC_24=0,HDMI_DC_30=1,HDMI_DC_36=2,HDMI_DC_48=4,HDMI_DC_YUV=8
    }tagHDMIDC;
    
    //color space
    typedef enum
    {
        HDMI_CS_RGB=0,HDMI_CS_YUV444=1,HDMI_CS_YUV422=2,HDMI_CS_YUV=3
    }tagHDMICS;
				 
    //Content Type
    typedef enum
    {
        HDMI_CT_NONE=0,HDMI_CT_TEXT=1,HDMI_CT_PHOTO=2,HDMI_CT_CINEMA=4,HDMI_CT_GAME=8
    }tagHDMICT;	  
	 
    //Colorimetry Support
    typedef enum
    {
        HDMI_CI_NONE=0,
        HDMI_CI_EXT=0x03,
        HDMI_CI_BT601=0x01,HDMI_CI_BT709=0x02,
        HDMI_CI_XVYCC601=0x04,HDMI_CI_XVYCC709=0x08,HDMI_CI_SYCC601=0x10,HDMI_CI_ADOBEYCC601=0x20,HDMI_CI_ADOBERGB=0x40
    }tagHDMICI;

    #define DynVESA	    0
    #define DynCEA	    1

    typedef enum
    {
        HDMI_VESA=0,HDMI_CEA=1
    }tagHDMIDynRange;
    
    typedef enum
    {
        HDMI_xvYCC601=0,HDMI_xvYCC709
    }tagHDMIExtColor;
        
    typedef enum 
    {
        HDMI_RGB_Default=0,HDMI_RGB_Range_Limited,HDMI_RGB_Range_Full
    }tagHDMIRGBRange;
        	
    typedef enum 
    {
        HDMI_PAR_NO_DATA=0,HDMI_PAR_4_3,HDMI_PAR_16_9,HDMI_PAR_Future
    }tagHDMIPictureAspectRatio; 
    
		typedef enum 
    {
        HDMI_AAR_AS_PAR=0,HDMI_AAR_4_3,HDMI_AAR_16_9,HDMI_AAR_14_9
    }tagHDMIActiveAspectRatio; 
	
    typedef enum 
    {
        HDMI_PS_NOKNOW=0,HDMI_PS_H_SCALER,HDMI_PS_V_SCALER,HDMI_PS_HV_SCALER
    }tagHDMIPictureScaler;
   
 
    #define NORMAL           0

    /*-------------------------------------- EVENT FLAG -------------------------------------*/
    #define EV_NONE			    0x00000000
    #define EV_RESET            0x00000002 
    #define EV_POWER_ON         0x00000004    

    #define EV_CHIP0_IN_0_SIG           0x00000010
    #define EV_CHIP0_IN_1_SIG       	0x00000020 
    #define EV_CHIP0_IN_2_SIG       	0x00000040 
    #define EV_CHIP0_IN_3_SIG       	0x00000080 
    #define EV_CHIP0_IN_4_SIG       	0x00000100 
    #define EV_CHIP0_IN_5_SIG       	0x00000200 

    #define EV_CHIP1_IN_0_SIG           0x00000400
    #define EV_CHIP1_IN_1_SIG       	0x00000800 
    #define EV_CHIP1_IN_2_SIG       	0x00001000 
    #define EV_CHIP1_IN_3_SIG       	0x00002000 
    #define EV_CHIP1_IN_4_SIG       	0x00004000 
    #define EV_CHIP1_IN_5_SIG       	0x00008000

    #define EV_CHIP2_IN_0_SIG 		0x00010000
    #define EV_CHIP2_IN_1_SIG		0x00020000 
    #define EV_CHIP2_IN_2_SIG 		0x00040000
    #define EV_CHIP2_IN_3_SIG		0x00080000
    #define EV_CHIP2_IN_4_SIG		0x00100000
    #define EV_CHIP2_IN_5_SIG		0x00200000
	   
    #define EV_OUT_0_MONITOR    0x00400000
    #define EV_OUT_0_AUTH       0x00800000 
    #define EV_OUT_0_SIG        0x01000000

    #define EV_SW_KEY           0x02000000  
    #define EV_PA1_UPDATA       0x04000000 	
    #define EV_CH_SWITCH        0x08000000 

    #define EV_EDID				0x10000000
    #define EV_AUTO_SWITCH		0x20000000	
    #define EV_OUT_1_MONITOR	0x40000000
    #define EV_CEC              0x80000000

	#define EV_IN_SIG_AUX        0x00000001
    #define EV_OUT_1_AUTH		0x00000008

		/*------------------------------------ UI FLAG -----------------------------------------*/
    #define UI_STATE_POWER_ON        0  //����У��
    #define UI_STATE_SET_DELAY       1  //�����л���ʱ�ڳ�ʱ��
    #define UI_STATE_SET_COM         2  //�л�ͨѶ�����
    #define UI_STATE_MATCH_IR        3  //Match IR Mode
    #define UI_STATE_SET_IN_HDCP     4  //�ر�����ڵ� HDCP ����
    #define UI_STATE_SET_OUT_HDCP    5  //�������ǿ�ƽ���
    #define UI_STATE_RESET           6  //�����豸��λ
    #define UI_STATE_SELECT_EDID    7  //ѡ��EDID�����������Ĭ�ϣ�����
    #define UI_STATE_SET_USB_MODE    8  //����USBģʽ
    
    #define UI_STATE_SWITCH          9  //�л�����
    #define UI_STATE_LOCK           10  //����
    #define UI_STATE_COPY_EDID      11  //EDID����
    #define UI_STATE_SET_ARC        12  //ARC����
    
    #define UI_STATE_WAIT 13

    #define COUNT_UI_BASE_FADE      200

    #define SET_EVENT(T) MainDev.mEV|=(T)
    #define CLR_EVENT(T) MainDev.mEV&=~(T)
    #define PRO_CASE_EVENT(T,SEQUENCE)          if((MainDev.mEV&T)&&(MainDev.mEvClip==SEQUENCE))
    
    #define START_PRO_EVENT_CLIP(MAX_SEQUENCE)
    
    #define END_PRO_EVENT_CLIP(MAX_SEQUENCE)    MainDev.mEvClip=(MainDev.mEvClip+1)%(MAX_SEQUENCE+1)

		/*------------------------ SYSTEM CLASS -----------------------------*/
    typedef struct {
        UINT16  mEEPROMFlag1; 
        UINT16  mEEPROMFlag2; 
        UINT8   mDevAdd; 
        UINT8   mUI;
        UINT16  sUICount;
			
			
        UINT8   DipStatu;
        
        BOOL    mDebugRunning;

        tagComFormat    mComFormat;
 
        UINT32  mEV;
        UINT8  	mEvClip;
        
        UINT32  mBR;  

        UINT8   mStateD0;//D0,D1,D2,D3
        UINT8   mStateD1[3][6];//D0,D1,D2,D3
        UINT8   mStateD2[3][2];//D0,D1,D2,D3	
        UINT8   mStateD3[3][2];//D0,D1,D2,D3
				
        UINT16  sStateD0Count;//D0,D1,D2,D3 
        UINT16  sStateD1Count[3][6];//D0,D1,D2,D3
        UINT16  sStateD2Count[3][2];//D0,D1,D2,D3
        UINT16  sStateD3Count[3][2];//D0,D1,D2,D3
       
        UINT8   mBlankDelayTx[2];			//�ڳ���ʱ�ȼ�  
        UINT16  sBlankTxCount50ms[2];	//50ms��ʱ�������ھ�ȷ�����ڳ���ʱ 
        
        UINT8		mRxChipSel;	   //video Receive port Select
        UINT8		mNextSelectChip;
	
        UINT8		mRxPortSel;	   //video Receive port Select
        UINT8		mNextSelectInPort;

        tagTxClose	mTxClose;//1 is turn off , 0 is turn on	

        UINT8	sWaitTxDDCCheck[2]; 
                                                         
        UINT8   sTxLinkSecretCount[2];
        UINT16  sCheckTxDDCCount50ms[2]; 
        
        unsigned short  mInPHY[COUNT_IN];
        unsigned short  mOutPHY;

        UINT8	OverrideFlag ;			//���Ǳ��
               
        tagAudioMix   		mAudioMixSrc;
        tagAudioMixMode   mAudioMixMode;
        tagAnalogIn   		mAudioAnalogIn;

        tagUart			mUartState;
        tagAudio  	mAudioOutState;
        tagInHDCP  	mInHDCPState[3][6];
								
        unsigned short 	uc5VCountLevel;
        unsigned short  sOutput5VCount50ms;
          
        tagEDIDDC  mEDIDDC;
        tagEDIDCS  mEDIDCS;
				
        tagEDID2CHLPCM mEDIDAUD;

        UINT8	mEDID[256];//from NVRAM of 9575
//        UINT8	mBOOT[64];//from BOOT
        BOOL	m5VOffFlag;
        
        UINT8	mInEDIDChFrom;		 //����Ĭ��or���ص�EDIDѡ��
        UINT16   mEDIDSelPort;   //����EDIDʱ��ѡ��EDID�˿ڣ�bit��ʾһ���˿�
        
        tagEDIDTYPE  mEDIDTYPE[3][6];
        BOOL    mInEDIDModify;     //�Ƿ���û����Ƶ��Ϣ��ʱ�򣬲�����Ƶ��Ϣ��
        
        tagDeviceType mDeviceType;
        
        UINT8   mSwtichSpeed;//0 is normal, 1 is fast
        
        tagFactoryType mFactoryType;
        
        tagOutType mDevOutType;
        
    } tagMainDev; 
			   
    typedef struct {	
        tagFollowEncrp  mConFollowEncrption[2]; 
											
        BOOL 			mConAutoTxDDCCheck[2];//�Ƿ���HPD��Ч������£��Զ�����DDC��� 
     
        tagMuteState 	mConAudioMute[1]; // 0 is Audio Mute OFF, 1 is Audio Mute ON
        
        tagMuteState 	mConVideoMute[2]; //  0 is Video Mute OFF, 1 is Audio Mute ON
        
//        BOOL            mConTxIsPattern;
				

    } tagConDev; 

    typedef struct {
        BOOL			    mInSig;	
        BOOL			    mInP5V;	
//		UINT8				mInFlag;//bit0:bksv ok	//???							
        tagHDMIMode 		mInHDMI;
//        tagHDMIDC 			mInDC; 	
//        tagHDMICS 			mInCS; 	
//        tagHDMICI 			mInCI;//601<->709
//        tagHDMIRGBRange 	mInRR;//Input Video Range.	

//        UINT8				mInPR;
//        UINT8				mInVIC;
        
//        UINT8			    mInChs[5];
//		UINT8			    mInAudSrc; 	//0 is LPCM SPDIF, 1 is LPCM I2S, 2 is HBR, 3 is DSD for debug 
//		UINT8			    mInAudFormat;//0 is LPCM, 1 is non-LPCM from audio channels	

        tagHDMIHDCPMode     mInHDCP; 
//        tagCapOfHDCP    	mInCapHDCP;   
        
//        BOOL                mGetInReady;    //??
        BOOL                 mGetInIsLPCM; //�����Ƿ���LPCM
        
        signed int          mGetInHer;
        signed int          mGetInVer;
        signed int          mGetInTmdsClk;
        bool_t              mGetInInterlaced;
        UINT8               mGetInVic;
        bool_t              mGetisHdmi;
     
 
    } tagInDev;  

    typedef struct {
		
        UINT8			mOutVidSrc;		

        tagHDMIMode 	mOutHDMI;								  

        tagHDMIHDCPMode     mOutHDCP; 


    } tagOutDev; 
	
    #define MON_DEV_HDMI_AUD_NUM_OF_FORMAT 10			   //CEA�涨�������10����
    
    typedef struct {	
        tagValidDDC     	mMonIsValidDDC;
        tagHPD         	 	mMonHPD;
//        tagMSEN         	mMonMSEN; 
									
        tagHDMIMode 		mMonHDMI;
        tagHDMIDC 			mMonDC; 
        tagHDMICS 			mMonCS; 

        BOOL				mMon3D;			
        tagCapOfHDCP    	mMonHDCP; 		
        BOOL    			mMonRepeater;  
						
        tagHDMIAudFormat    mMonAF[MON_DEV_HDMI_AUD_NUM_OF_FORMAT]; 				  
        tagHDMIAudFs 		mMonAS[MON_DEV_HDMI_AUD_NUM_OF_FORMAT];
        UINT16 				mMonAB[MON_DEV_HDMI_AUD_NUM_OF_FORMAT];
        UINT8				mMonAC[MON_DEV_HDMI_AUD_NUM_OF_FORMAT];	  
		 									 

        UINT8           	mMonEDIDBuf[256];
        UINT16  			mMonPA;   
        
        tagEDIDFlag 		mMonEDIDErrFlag; 
        
        BOOL                mGetMonIsNonLPCM;
        BOOL                mGetIsReady;
  //      UINT8 			    mAutoTxDDCCheck[2];
    } tagMonDev; 


    /*------------------------ Auto Switch -----------------------------*/
    #define BASE_1S_DELAY 20
    #define BASE_SW_AUTO_COUNT	1
    #define FIRST_POWER_ON_SWITCH		2  //�������3s

    #define OUTPUT5V_5MINCOUNT		   6000
    #define OUTPUT5V_1S_COUNT          20
    #define DEFAULT_LEVEL			   30	//Ĭ�ϵĵȼ�1��ÿһ��30S
    
    #define MAX_5VOFF_TIME              999

    typedef enum{
        MANUAL_SWITCH_MODE = 0,
        AUTO_SWITCH_MODE = 1
    }tagSwitchMode;
    
     typedef enum{
        NONE_CASCADED_MODE = 0,
        IS_CASCADED_MODE = 1
    }tagCasMode;
     
    
    typedef enum{
        UPBOARD_CONNECT_NONE = 0,
        UPBOARD_CONNECT_OK = 1
    }tagUpConnect;

    typedef enum{
        PRIORITY_MODE	= 0,
        LAST_CONNECTED_MODE	= 1,

    }tagAutoMode;

    typedef enum {
        SIG_EXIST=1,
        SIG_NONE=0
    }tagSig;

    typedef struct {
        tagSwitchMode 	SwitchMode;
        tagAutoMode	  	AutoSwMode;
        unsigned char 	AutoSWMark;	  //�Զ��л��ȴ����

        unsigned char 	ucChangePort;	//�����仯��ͨ��
        unsigned char 	ucSwitchPort;	//��仯�������л�ͨ��
        tagSig			AutoSigState[COUNT_IN];			//�Զ��л��¶˿ڵ�״̬���Զ��л�����Ҫ�ر�״̬
        tagSig			AutoSigLastState[COUNT_IN];	
        unsigned char 	mPriority[COUNT_IN];	//���ȼ��б�
        unsigned short 	AutoSw50msDelay;	//�źű仯�ĵȴ�ʱ��
        unsigned short 	AutoNoSig10sWait;
        unsigned char 	ucAutoSwFlag;	   //ֻ��ģʽ�����仯���
        //����ģʽ
//        tagCasMode      sCascaded;      //cascaded mode ,����ģʽ
//        unsigned short  sCasWait50msCount;

    } tagAutoSwitch;
        
    extern tagAutoSwitch AutoSw;
    
    extern unsigned char AutoSwitchList[COUNT_IN];	


		extern const unsigned char    CHAR_BIT_TAB[8];
		extern const unsigned short SHORT_BIT_TAB[16];
    
    #define MAX_CHIP_INDEX      3   
    extern unsigned char ucMaxValidChipNum;    //������ЧоƬ���������û���ϰ壬��ЧоƬΪ3��
    extern unsigned char ucMaxValidPortNum;    //������Ч�˿������������豸��ͬ��ͬ
    
    extern unsigned char ucMaxOutPortNum ;   //������Ч�������
    
    extern tagMainDev 	MainDev;
    extern tagConDev 	ConDev;
    extern tagInDev 	InDev[MAX_CHIP_INDEX][6];
    extern tagOutDev 	OutDev[MAX_CHIP_INDEX][2];
    extern tagMonDev 	MonDev[];	 

    extern const unsigned char EDID_array[];
    extern unsigned char uc16OfBuf[]; 
    extern unsigned char uc256Buf[];
    extern unsigned char ucUIRefreshFlag;
    
    extern unsigned char ucSwitchList[16][2];
    
    extern tagCasMode      sCascaded;      //cascaded mode ,����ģʽ
    extern unsigned short  usCasWait50msCount;
    
    extern unsigned char NetPollFlag;
    extern unsigned char ucKeyReset;
    extern unsigned char  UartChgFlag;
    
    #define MAX_VOL_DOT_NUM			7		 //���ѹ�����4����
	extern  unsigned short Design_Voltage[MAX_VOL_DOT_NUM];
	extern  unsigned long Detect_VolBuf[MAX_VOL_DOT_NUM];

	#define	DESIGN_VOLTAGE_SITE0 1300  // 
	#define	DESIGN_VOLTAGE_SITE1 1650  // 
	#define	DESIGN_VOLTAGE_SITE2 1300  // 
	#define	DESIGN_VOLTAGE_SITE3 1800  // 
    #define	DESIGN_VOLTAGE_SITE4 1050  // 
	#define	DESIGN_VOLTAGE_SITE5 3300  // 
	#define	DESIGN_VOLTAGE_SITE6 5000  // 
    
    extern UINT8 Mtx_Mode;
    extern UINT8 usb_working_mode;
    extern UINT16 udpPort;
    extern UINT16 tcpPort;
    extern UINT8 btn_WebSwitch[] ;
    extern UINT8 WebSwitchFlag[] ;
      
    extern UINT8 Ipdata[] ;
    extern UINT8 GateWaydata[] ;
    extern UINT8 NetMaskdata[];
		extern UINT8 NetMacdata[]; 
    extern UINT8 net_dhcp_enable;
    extern UINT16 net_reset_flag;
    extern unsigned char CommNetFlag;
    extern unsigned short ucConnectFlag;
    extern unsigned char uclastIp[];
		extern UINT8 NetSecureMode; //
    
    extern unsigned char webEdidBuf[];	//web���ϴ�����ҳ
    extern unsigned char webEdidRequestFlag;//1 is input, 2 is output, 3 is Default
    extern unsigned char webEdidRequestID;//0 is default, 1 is from 


    extern unsigned char ucCommIDMark;			//ѡ��ͨ�ŷ�ʽ��ǣ�
    extern unsigned short usCountVisualIndcation;
//    extern const UINT32 EVENT_SIG_TABLE[]; 
    extern const UINT32 EVENT_SIG_TABLELIST[3][6];
    extern unsigned short WaitStableCount;
    extern unsigned char ucPowerUpSwFlag;	//�����Զ��л���ǣ����ڵȴ����е����źż�����
    extern const unsigned char BIT_8BITS_TAB[];
    extern unsigned char ucCECDevAdd;
    extern unsigned char ucResetFlag;

    extern unsigned short usTimeLevel;
    
    extern unsigned  short NoSigWaitTime ;  
    extern unsigned  short usSwAnalogOut50msCount;
    extern unsigned short  usArcResetWaitCount;
    extern unsigned char  ucArcResetFlag ;
    extern unsigned short usDetectCount;

    extern unsigned char DhcpChgFlag;
    extern unsigned char own_hw_adr[];

    UINT8 get_switch_in_ch(UINT8 uIn, UINT8 *uOut);	 
    UINT8 get_In_chip_ch(UINT8 uInNum);
    UINT8 get_In_Num(UINT8 ChipSel,UINT8 PortSel);

    void SaveNVRAM(unsigned char ucID);
    void InitNVRAM(unsigned char ucResetMSN);
    unsigned char LoadNVRAM(void);
    void LoopNVRAM(void);
    
    void SEND_BYTE(unsigned char ucCh);
//    void SEND_NET_BYTE(unsigned char ucCh);
    
    void  CopyEDID2In(unsigned char appIdx,unsigned char reset);
    void CopyPA2In(unsigned char appIdx);
    void ShowAutoSwithcInfo(void);
//    void ShowDetectVoltageInfo(void);
//    void ShowFlashIDInfo(void);
		
#ifdef SUPPORT_STEPIN_FUN
    void ShowStepInConnectInfo(void);
#endif
    
    void SetOutput5VOffTime(void);
    void reset_net(void);
//    void ShowE2promFlag(void);
//    void EraseEEPROMFlag(void);
//    void ShowPriorityList(void);
    
    void SetSinglePriorityPortList(unsigned char mport,unsigned char ucPriority);
    
//    void ShowInputSignalInfor(void);
//    void ShowOutSignalInfor(void);
//    void ShowMonitorInfor(void);
//    void ShowMonitorHDCPInfor(void);
    
    void upload_user_start_code(uint32_t address);
    
    void FDISK_OpenFile2Flash(const unsigned char *logStr,int n);
    void FDISK_CloseFile2Flash(void);
    void FDISK_CreateFile2Flash(const char* logPath);
    
    void FDISK_Record2Log(const char* logPath,char *logStr);
    
    #define LONG_REACH_MODE			0
    #define HDBT_MODE				1 
    void VS100TX_Init_Mode(unsigned char mode);
    void Hardware_Init_Si9575_VS100TX(void);
    void Si9575_Hardware_Reset(unsigned char ChipIndex);
    void VS100TX_Reset(void);
    void Rst_Dp83848(void);
    
    void Hardware_Init_Wm8805_PCM5142_CS5340(void);

    #ifdef SUPPORT_VOLTAGE_DETECT
    void ClrADStatu(void);
    #endif
    
//    extern void Back_Command2Web(unsigned char idx);
    
    void Reset_MRA_Aud_Mute(BOOL isMute);
    
    #define AUD_OUT_FROM_DE_EMBED      0
    #define AUD_OUT_FROM_ANALOG        1
    #define AUD_OUT_FROM_NONE          2
    
    void SelectAnalogAudioOut(unsigned char ucWhichAudio);
    
    void loop_net(void);
		
		
		//
		#define PASS_ONLY    0
		#define BROADCAST    1
		//K_Result=0:ֻ��һ��һ�Ķ˿ڻ���
		//K_Result=1:����һ��һ�Ķ˿ڻ�����,��Ҫ�������˿ڻ���
		void MAIN_SetDataBackMode(unsigned char ucM);   
		unsigned char MAIN_IsDataPassOnly(void);		
		unsigned char MAIN_GetCurrentCom(void);
		void MAIN_SetCurrentCom(unsigned char ucCom);
#endif




