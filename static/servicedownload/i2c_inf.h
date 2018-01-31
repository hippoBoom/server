/**
 * @file i2c_inf.h
 * @author Guan dian
 * @date 26-Apr-2013
 * @brief header file I2C Hardware Abstraction Layer library
 */

#ifndef _I2C_INF_H_
#define _I2C_INF_H_

#define I2CMASTER   0x0
/*#define I2CSLAVE    0x1*/

/*** PROTOTYPES ***/

/** Max buffer length */
#define BUFFER_SIZE		(256+2)	//at 256 due to NVRAM of IIC (48+2)	

void I2C_LogInit(void);
void I2C_LogStop(void);

void I2cInit(UCHAR PortNum, UCHAR I2cMode, UINT16 ClockRate, UCHAR SlaveDeviceAddress);

/* I2C0 interface */
BOOL I2cReadRegister(UCHAR PortNum, UCHAR, UCHAR, UCHAR*, UINT16);
BOOL I2cWriteRegister(UCHAR PortNum, UCHAR, UCHAR, UCHAR*, UINT16);

BOOL I2cReadRegister2(UCHAR PortNum, UCHAR, UINT16, UCHAR*, UINT16);
BOOL I2cWriteRegister2(UCHAR PortNum, UCHAR, UINT16, UCHAR*, UINT16);

BOOL I2cReadRegister8(UCHAR PortNum, UCHAR, UCHAR, UCHAR*);
BOOL I2cWriteRegister8(UCHAR PortNum, UCHAR, UCHAR, UCHAR);

BOOL I2cReadRegister16(UCHAR PortNum, UCHAR, UINT16, UCHAR*);
BOOL I2cWriteRegister16(UCHAR PortNum, UCHAR, UINT16, UCHAR);

#endif
