/**
 * @file i2c_inf.c
 * @brief I2C_inf file for NXP LPC23xx/24xx  Family Microprocessors
 * 
 */
#include "lig_types.h"	 
#include "lig_platform.h"

#include "lpc17xx_i2c.h" 
#include "lpc17xx_pinsel.h"

#include "serial.h"
#include "i2c_inf.h"

extern UINT32  HAL_GetCurrentMsCount(void);

PINSEL_CFG_Type PinCfg_I2C; 

__IO FlagStatus complete_0_M;	 
__IO FlagStatus complete_1_M;	 
__IO FlagStatus complete_2_M;

I2C_M_SETUP_Type transferMCfg;

uint8_t Master_Buf[BUFFER_SIZE];

STATIC BOOL      I2CLogEnabled;

/*=============================================================================
 * Description: intializes all the global variables in i2c_inf function, and all 
 *              necessarily conditions to start the log
 *
 * Return: none
 *===========================================================================*/
void I2C_LogInit(void)
{
    I2CLogEnabled = TRUE;     
}

/*=============================================================================
 * Description: Stop logging I2C transactions
 *
 * Return: none
 *===========================================================================*/
void I2C_LogStop(void)
{
    I2CLogEnabled = FALSE;      
}

void I2C0_IRQHandler(void)
{
	I2C_MasterHandler(LPC_I2C0);
	if (I2C_MasterTransferComplete(LPC_I2C0)){
		complete_0_M = SET;
	}
}

void I2C1_IRQHandler(void)
{
	I2C_MasterHandler(LPC_I2C1);
	if (I2C_MasterTransferComplete(LPC_I2C1)){
		complete_1_M = SET;
	}
}

void I2C2_IRQHandler(void)
{
	I2C_MasterHandler(LPC_I2C2);
	if (I2C_MasterTransferComplete(LPC_I2C2)){
		complete_2_M = SET;
	}
}

/*!
 * @fn void I2cInit(UCHAR PortNum, UCHAR I2cMode, UINT16 ClockRate, UCHAR SlaveDeviceAddress)
 * @param PortNum IIC Index
 * @details Initialize I2C interface
 *
 * @param PortNum interface port number
 * @param I2cMode master or slave mode
 * @param ClockRate I2C clock frequency (master mode only)
 * @param SlaveDeviceAddress slave I2C device address (slave mode only)
 *
 * @return none
 *
 */
void I2cInit(UCHAR PortNum, UCHAR I2cMode, UINT16 ClockRate, UCHAR SlaveDeviceAddress)
{			  						  
	// Initialize Slave I2C peripheral
	unsigned long ulClk;

	I2CLogEnabled = FALSE; 

	ulClk=100000;
	
	/*
	 * Init I2C pin connect
	 */										
	PinCfg_I2C.OpenDrain = 0;//0--PINSEL_PINMODE_NORMAL
                            //1--PINSEL_PINMODE_OPENDRAIN
	
    PinCfg_I2C.Pinmode = 0;
			
    if (PortNum==0)
	{

        
		PinCfg_I2C.Funcnum = 1;

		PinCfg_I2C.Pinnum = 27; //sda
		PinCfg_I2C.Portnum = 0; 	

		PINSEL_ConfigPin(&PinCfg_I2C); 
		PinCfg_I2C.Pinnum = 28; //scl	
		PINSEL_ConfigPin(&PinCfg_I2C);  
								  
		I2C_Init(LPC_I2C0, ulClk);	

		/* Disable I2C0 interrupt */
		NVIC_DisableIRQ(I2C0_IRQn);	   
		NVIC_SetPriority(I2C0_IRQn, ((0x00<<3)|0x01));		
		/* Enable Slave I2C operation */
		I2C_Cmd(LPC_I2C0, ENABLE);
	}
    else if (PortNum==1)
	{
        //E2PROM 是慢速设备 ，写数据较慢
        ulClk=50000;
        
		PinCfg_I2C.Funcnum = 3;
									  
		PinCfg_I2C.Portnum = 0; 
			
		PinCfg_I2C.Pinnum = 19; //sda
		PINSEL_ConfigPin(&PinCfg_I2C); 
		PinCfg_I2C.Pinnum = 20; //scl	
		PINSEL_ConfigPin(&PinCfg_I2C);

		I2C_Init(LPC_I2C1, ulClk);		

		/* Disable I2C1 interrupt */
		NVIC_DisableIRQ(I2C1_IRQn);	   
		NVIC_SetPriority(I2C1_IRQn, ((0x00<<3)|0x01));

		/* Enable Slave I2C operation */
		I2C_Cmd(LPC_I2C1, ENABLE);	  
	}
    else if (PortNum==2)
	{
		PinCfg_I2C.Funcnum = 2;

		PinCfg_I2C.Portnum = 0; 	
									
		PinCfg_I2C.Pinnum = 10; //sda
		PINSEL_ConfigPin(&PinCfg_I2C); 
		PinCfg_I2C.Pinnum = 11; //scl	
		PINSEL_ConfigPin(&PinCfg_I2C);

		I2C_Init(LPC_I2C2, ulClk);		

		/* Disable I2C1 interrupt */
		NVIC_DisableIRQ(I2C2_IRQn);	   
		NVIC_SetPriority(I2C2_IRQn, ((0x00<<3)|0x01));

		/* Enable Slave I2C operation */
		I2C_Cmd(LPC_I2C2, ENABLE);	  
	}
}

/*!
 * @fn BOOL I2cReadRegister(UCHAR PortNum, UCHAR Device_addr, UCHAR Sub_addr, UCHAR *Data_buffer, UINT16 Data_len)
 *
 * @details Master read data from slave via I2C0 port.
 * @param PortNum IIC Index
 * @param Device_addr = slave device address
 * @param Sub_addr = slave device sub address for specific register
 * @param Data_buffer = read data buffer
 * @param Data_len = read data length.
 *
 * @return TURE or FALSE
 *
 */
BOOL I2cReadRegister(UCHAR PortNum, UCHAR Device_addr, UCHAR Sub_addr, UCHAR *Data_buffer, UINT16 Data_len)
{  		
    unsigned long	ulTempTimeoutCounter;
	/* Start I2C slave device first */	
	transferMCfg.sl_addr7bit = Device_addr>>1;	  
	transferMCfg.tx_data = &Sub_addr;
	transferMCfg.tx_length = 1;
	transferMCfg.rx_data = Data_buffer;
	transferMCfg.rx_length = Data_len;
	transferMCfg.retransmissions_max = 3;
    
    ulTempTimeoutCounter = 2000000 ;  //200ms

    if (PortNum==0)
    {		   	 
		complete_0_M =  RESET;
		if (I2C_MasterTransferData(LPC_I2C0, &transferMCfg, I2C_TRANSFER_INTERRUPT) != SUCCESS)
			return (FALSE);	
		/* Wait until both of them complete */
		while ((complete_0_M == RESET)&& ( -- ulTempTimeoutCounter ) )
		{
            ;
        }
		/* Process TimeOut */
    }else if (PortNum==1)
    {			   
		complete_1_M =  RESET;
		if (I2C_MasterTransferData(LPC_I2C1, &transferMCfg, I2C_TRANSFER_INTERRUPT) != SUCCESS)
			return (FALSE);	  
		/* Wait until both of them complete */
   		while ((complete_1_M == RESET)&& ( -- ulTempTimeoutCounter ) )
		{
            ;
        }
		/* Process TimeOut */
    }
	else if(PortNum==2)
	{
		complete_2_M =  RESET;
		if (I2C_MasterTransferData(LPC_I2C2, &transferMCfg, I2C_TRANSFER_INTERRUPT) != SUCCESS)
			return (FALSE);	  
		/* Wait until both of them complete */
		while ((complete_2_M == RESET)&& ( -- ulTempTimeoutCounter ) )
		{
            ;
        }
		/* Process TimeOut */
	}
	else 
        return (FALSE);

    return (TRUE);
}

/*!
 * @fn BOOL I2cWriteRegister(UCHAR PortNum, UCHAR Device_addr, UCHAR Sub_addr, UCHAR *Data_buffer, UINT16 Data_len)
 *
 * @details Master write data to slave via I2C0 port.
 * @param PortNum IIC Index
 * @param Device_addr = slave device address
 * @param Sub_addr = slave device sub address for specific register
 * @param Data_buffer = write data buffer
 * @param Data_len = write data length.
 *
 * @return TURE or FALSE
 *
 */
BOOL I2cWriteRegister(UCHAR PortNum, UCHAR Device_addr, UCHAR Sub_addr, UCHAR *Data_buffer, UINT16 Data_len)
{	
    unsigned long	ulTempTimeoutCounter;
	/* Start I2C slave device first */	
	transferMCfg.sl_addr7bit = Device_addr>>1;	  

	Master_Buf[0]=Sub_addr;
	memcpy(Master_Buf+1,Data_buffer,Data_len);

	transferMCfg.tx_data = Master_Buf;
	transferMCfg.tx_length = Data_len+1;
	transferMCfg.rx_data = NULL;
	transferMCfg.rx_length = 0;
	transferMCfg.retransmissions_max = 3;
    
    ulTempTimeoutCounter = 2000000 ;  //200ms

    if (PortNum==0)
    {		   				  
		complete_0_M =  RESET;
		if (I2C_MasterTransferData(LPC_I2C0, &transferMCfg, I2C_TRANSFER_INTERRUPT) != SUCCESS)
			return (FALSE);	
		/* Wait until both of them complete */
		while ((complete_0_M == RESET)&& ( -- ulTempTimeoutCounter ) )
		{
            ;
        }
		/* Process TimeOut */
    }else if (PortNum==1)
    {							
		complete_1_M =  RESET;
		if (I2C_MasterTransferData(LPC_I2C1, &transferMCfg, I2C_TRANSFER_INTERRUPT) != SUCCESS)
			return (FALSE);	
		/* Wait until both of them complete */
		while ((complete_1_M == RESET)&& ( -- ulTempTimeoutCounter ) )
		{
            ;
        }
		/* Process TimeOut */
    }
	else if (PortNum==2)
    {							
		complete_2_M =  RESET;
		if (I2C_MasterTransferData(LPC_I2C2, &transferMCfg, I2C_TRANSFER_INTERRUPT) != SUCCESS)
			return (FALSE);	
		/* Wait until both of them complete */
		while ((complete_2_M == RESET)&& ( -- ulTempTimeoutCounter ) )
		{
            ;
        }
		/* Process TimeOut */
    }
	else 
        return (FALSE);

    return (TRUE);
}

/*!
 * @fn BOOL I2cReadRegister2(UCHAR PortNum, UCHAR Device_addr, UINT16 Sub_addr, UCHAR *Data_buffer, UINT16 Data_len)
 *
 * @details Master read data from slave via I2C0 port.
 * @param PortNum IIC Index
 * @param Device_addr = slave device address
 * @param Sub_addr = slave device sub address for specific register
 * @param Data_buffer = read data buffer
 * @param Data_len = read data length.
 *
 * @return TURE or FALSE
 *
 */
BOOL I2cReadRegister2(UCHAR PortNum, UCHAR Device_addr, UINT16 Sub_addr, UCHAR *Data_buffer, UINT16 Data_len)
{
    unsigned long	ulTempTimeoutCounter;
	int n; 		
	/* Start I2C slave device first */	
	transferMCfg.sl_addr7bit = Device_addr>>1;	  

	Master_Buf[0]=(Sub_addr&0xff00)>>8;	 
	Master_Buf[1]=Sub_addr&0x00ff;

	transferMCfg.tx_data = Master_Buf;
	transferMCfg.tx_length = 2;
	transferMCfg.rx_data = Data_buffer;
	transferMCfg.rx_length = Data_len;
	transferMCfg.retransmissions_max = 3;
    
    ulTempTimeoutCounter = 2000000 ;  //200ms
	  
	/* Force complete flag for the first time of running */
    if (PortNum==0)
    {		   			
		complete_0_M =  RESET;
		if (I2C_MasterTransferData(LPC_I2C0, &transferMCfg, I2C_TRANSFER_INTERRUPT) != SUCCESS)
			return (FALSE);	 
		/* Wait until both of them complete */
		while ((complete_0_M == RESET)&& ( -- ulTempTimeoutCounter ) )
		{
            ;
        }
		/* Process TimeOut */
    }else if (PortNum==1)
    {					 
		complete_1_M =  RESET;
		if (I2C_MasterTransferData(LPC_I2C1, &transferMCfg, I2C_TRANSFER_INTERRUPT) != SUCCESS)
			return (FALSE);		 
		/* Wait until both of them complete */
		while ((complete_1_M == RESET)&& ( -- ulTempTimeoutCounter ) )
		{
            ;
        }
		/* Process TimeOut */
    }
//	else if (PortNum==2)
//    {					 
//		complete_2_M =  RESET;
//		if (I2C_MasterTransferData(LPC_I2C2, &transferMCfg, I2C_TRANSFER_INTERRUPT) != SUCCESS)
//			return (FALSE);		 
//		/* Wait until both of them complete */
//		while ((complete_2_M == RESET)&& ( -- ulTempTimeoutCounter ) )
//		{
//            ;
//        }
//		/* Process TimeOut */
//    }
	else 
        return (FALSE);

	/**/
	if (I2CLogEnabled==TRUE)
	{		
		DBG_MSG("I2C R REG=0x%x",(UCHAR)(Sub_addr>>8));
		DBG_MSG("%x, ",(UCHAR)Sub_addr);
		for (n=0;n<Data_len;n++)
		{
			DBG_MSG("VAL=0x%x",Data_buffer[n]);
		}			   
		DBG_MSG("\n");
		HAL_DelayMs(5);
	}
    return (TRUE);
}

/*!
 * @fn BOOL I2cWriteRegister2(UCHAR PortNum, UCHAR Device_addr, UINT16 Sub_addr, UCHAR *Data_buffer, UINT16 Data_len)
 *
 * @details Master write data to slave via I2C0 port.
 * @param PortNum IIC Index
 * @param Device_addr = slave device address
 * @param Sub_addr = slave device sub address for specific register
 * @param Data_buffer = write data buffer
 * @param Data_len = write data length.
 *
 * @return TURE or FALSE
 *
 */
BOOL I2cWriteRegister2(UCHAR PortNum, UCHAR Device_addr, UINT16 Sub_addr, UCHAR *Data_buffer, UINT16 Data_len)
{
    unsigned long	ulTempTimeoutCounter;
	int n;
	/* Start I2C slave device first */	
	transferMCfg.sl_addr7bit = Device_addr>>1;	  

	Master_Buf[0]=(Sub_addr&0xff00)>>8;	 
	Master_Buf[1]=Sub_addr&0x00ff;	

	memcpy(Master_Buf+2,Data_buffer,Data_len);

	transferMCfg.tx_data = Master_Buf;
	transferMCfg.tx_length = Data_len+2;
	transferMCfg.rx_data = NULL;
	transferMCfg.rx_length = 0;
	transferMCfg.retransmissions_max = 3;
    
    ulTempTimeoutCounter = 2000000 ;  //200ms

    if (PortNum==0)
    {		   		   
		complete_0_M =  RESET;
		if (I2C_MasterTransferData(LPC_I2C0, &transferMCfg, I2C_TRANSFER_INTERRUPT) != SUCCESS)
			return (FALSE);	 
		/* Wait until both of them complete */
		while ((complete_0_M == RESET)&& ( -- ulTempTimeoutCounter ) )
		{
            ;
        }
		/* Process TimeOut */
    }else if (PortNum==1)
    {		   
		complete_1_M =  RESET;
		if (I2C_MasterTransferData(LPC_I2C1, &transferMCfg, I2C_TRANSFER_INTERRUPT) != SUCCESS)
			return (FALSE);	 
		/* Wait until both of them complete */
		while ((complete_1_M == RESET)&& ( -- ulTempTimeoutCounter ) )
		{
            ;
        }
		/* Process TimeOut */
    }
//	else if (PortNum==2)
//    {		   
//		complete_2_M =  RESET;
//		if (I2C_MasterTransferData(LPC_I2C2, &transferMCfg, I2C_TRANSFER_INTERRUPT) != SUCCESS)
//			return (FALSE);	 
//		/* Wait until both of them complete */
//		while ((complete_2_M == RESET)&& ( -- ulTempTimeoutCounter ) )
//		{
//            ;
//        }
//		/* Process TimeOut */
//    }
	else 
        return (FALSE);

	/**/   
	if (I2CLogEnabled==TRUE)
	{
		DBG_MSG("I2C W REG=0x%x",(UCHAR)(Sub_addr>>8));
		DBG_MSG("%x, ",(UCHAR)Sub_addr);
		for (n=0;n<Data_len;n++)
		{
			DBG_MSG("VAL=0x%x",Data_buffer[n]);
		} 
		DBG_MSG("\n");  
		HAL_DelayMs(5);
	}	 
    return (TRUE);
}

/*!
 * @fn BOOL I2cReadRegister8(UCHAR PortNum, UCHAR Device_addr, UCHAR Sub_addr, UCHAR *Data_buffer)
 *
 * @details Master read a 8-bit data from slave via I2C0 port.
 * @param PortNum IIC Index
 * @param Device_addr = slave device address
 * @param Sub_addr = slave device sub address for specific register
 * @param Data_buffer = read data buffer
 *
 * @return TURE or FALSE
 *
 */
BOOL I2cReadRegister8(UCHAR PortNum, UCHAR Device_addr, UCHAR Sub_addr, UCHAR *Data_buffer)
{
    return (I2cReadRegister(PortNum, Device_addr, Sub_addr, Data_buffer, 1));
}

/*!
 * @fn BOOL I2cWriteRegister8(UCHAR PortNum, UCHAR Device_addr, UCHAR Sub_addr, UCHAR val)
 *
 * Description: Master write a 8-bit data to slave via I2C0 port.
 * @param PortNum IIC Index
 * @param Device_addr = slave device address
 * @param Sub_addr = slave device sub address for specific register
 * @param val = byte written
 *
 * @return TURE or FALSE
 *
 */
BOOL I2cWriteRegister8(UCHAR PortNum, UCHAR Device_addr, UCHAR Sub_addr, UCHAR val)
{
    return (I2cWriteRegister(PortNum, Device_addr, Sub_addr, &val, 1));
}

/*!
 * @fn BOOL I2cReadRegister16(UCHAR PortNum, UCHAR Device_addr, UINT16 Sub_addr, UCHAR *Data_buffer)
 *
 * @details Master read a 8-bit data from slave via I2C0 port.
 * @param PortNum IIC Index
 * @param Device_addr = slave device address
 * @param Sub_addr = slave device sub address for specific register
 * @param Data_buffer = read data buffer
 *
 * @return TURE or FALSE
 *
 */
BOOL I2cReadRegister16(UCHAR PortNum, UCHAR Device_addr, UINT16 Sub_addr, UCHAR *Data_buffer)
{
    return (I2cReadRegister2(PortNum, Device_addr, Sub_addr, Data_buffer, 1));
}

/*!
 * @fn BOOL I2cWriteRegister16(UCHAR PortNum, UCHAR Device_addr, UINT16 Sub_addr, UCHAR val)
 *
 * @details Master write a 8-bit data to slave via I2C0 port.
 * @param PortNum IIC Index
 * @param Device_addr = slave device address
 * @param Sub_addr = slave device sub address for specific register
 * @param val = byte written
 *
 * @return TURE or FALSE
 *
 */
BOOL I2cWriteRegister16(UCHAR PortNum, UCHAR Device_addr, UINT16 Sub_addr, UCHAR val)
{
    return (I2cWriteRegister2(PortNum, Device_addr, Sub_addr, &val, 1));
}
 
