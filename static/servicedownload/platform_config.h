/**
 * @file platform_config.h
 * @author Guan dian
 * @date 24-Apr-2013
 * @brief Setup standard IO of Lib, functions of Lib, Macro and DBG.
 */

#ifndef _PLATFORM_CONFIG_H_
#define _PLATFORM_CONFIG_H_

//#define UART_DEBUG	1

//EXTERNAL void adi_printf (char *Strg, ...);

/*=======================================
 * Configuration switches
 *======================================*/
#include "stdio.h"

EXTERNAL void adi_printf (char *Strg, ...);

#if UART_DEBUG
    #define DBG_MSG         adi_printf
	#define DEBUG_PRINT     DBG_MSG			
#else
    #define DBG_MSG(...)
	#define DEBUG_PRINT(...)		
#endif

#if ASSERT_DISABLE || NDEBUG
#define ASSERT(...)
#else
#define ASSERT(condition)           adi_assert(__FILE__, __LINE__, __func__, (int)(condition), #condition)
#endif


/*=======================================
 * Library functions and macros
 *======================================*/
#define ARM_GPIO_DIR_OUT 	1	
#define ARM_GPIO_DIR_IN 	0

#define ARM_GPIO_PX00_DWORD 	0x00000001 
#define ARM_GPIO_PX01_DWORD 	0x00000002 	
#define ARM_GPIO_PX02_DWORD 	0x00000004	
#define ARM_GPIO_PX03_DWORD 	0x00000008	
#define ARM_GPIO_PX04_DWORD 	0x00000010	
#define ARM_GPIO_PX05_DWORD 	0x00000020	
#define ARM_GPIO_PX06_DWORD 	0x00000040	
#define ARM_GPIO_PX07_DWORD 	0x00000080	
#define ARM_GPIO_PX08_DWORD 	0x00000100	
#define ARM_GPIO_PX09_DWORD 	0x00000200	
#define ARM_GPIO_PX10_DWORD 	0x00000400	
#define ARM_GPIO_PX11_DWORD 	0x00000800	
#define ARM_GPIO_PX12_DWORD 	0x00001000	
#define ARM_GPIO_PX13_DWORD 	0x00002000	
#define ARM_GPIO_PX14_DWORD 	0x00004000	
#define ARM_GPIO_PX15_DWORD 	0x00008000	
#define ARM_GPIO_PX16_DWORD 	0x00010000	
#define ARM_GPIO_PX17_DWORD 	0x00020000	
#define ARM_GPIO_PX18_DWORD 	0x00040000	
#define ARM_GPIO_PX19_DWORD 	0x00080000		
#define ARM_GPIO_PX20_DWORD 	0x00100000	
#define ARM_GPIO_PX21_DWORD 	0x00200000	
#define ARM_GPIO_PX22_DWORD 	0x00400000	
#define ARM_GPIO_PX23_DWORD 	0x00800000	
#define ARM_GPIO_PX24_DWORD 	0x01000000	
#define ARM_GPIO_PX25_DWORD 	0x02000000	
#define ARM_GPIO_PX26_DWORD 	0x04000000	
#define ARM_GPIO_PX27_DWORD 	0x08000000	
#define ARM_GPIO_PX28_DWORD 	0x10000000	
#define ARM_GPIO_PX29_DWORD 	0x20000000	
#define ARM_GPIO_PX30_DWORD 	0x40000000	
#define ARM_GPIO_PX31_DWORD 	0x80000000	 

#define ARM_GPIO_PX00_WORD 		0x00000003 
#define ARM_GPIO_PX01_WORD 		0x0000000c 	
#define ARM_GPIO_PX02_WORD 		0x00000030	
#define ARM_GPIO_PX03_WORD 		0x000000c0	
#define ARM_GPIO_PX04_WORD 		0x00000300	
#define ARM_GPIO_PX05_WORD 		0x00000c00	
#define ARM_GPIO_PX06_WORD 		0x00003000	
#define ARM_GPIO_PX07_WORD 		0x0000c000	
#define ARM_GPIO_PX08_WORD 		0x00030000	
#define ARM_GPIO_PX09_WORD 		0x000c0000	
#define ARM_GPIO_PX10_WORD 		0x00300000	
#define ARM_GPIO_PX11_WORD 		0x00c00000	
#define ARM_GPIO_PX12_WORD 		0x03000000	
#define ARM_GPIO_PX13_WORD 		0x0c000000	
#define ARM_GPIO_PX14_WORD 		0x30000000	
#define ARM_GPIO_PX15_WORD 		0xc0000000	
#define ARM_GPIO_PX16_WORD 		0x00000003	
#define ARM_GPIO_PX17_WORD 		0x0000000c	
#define ARM_GPIO_PX18_WORD 		0x00000030	
#define ARM_GPIO_PX19_WORD 		0x000000c0		
#define ARM_GPIO_PX20_WORD 		0x00000300	
#define ARM_GPIO_PX21_WORD 		0x00000c00	
#define ARM_GPIO_PX22_WORD 		0x00003000	
#define ARM_GPIO_PX23_WORD 		0x0000c000	
#define ARM_GPIO_PX24_WORD 		0x00030000	
#define ARM_GPIO_PX25_WORD 		0x000c0000	
#define ARM_GPIO_PX26_WORD 		0x00300000	
#define ARM_GPIO_PX27_WORD 		0x00c00000	
#define ARM_GPIO_PX28_WORD 		0x03000000	
#define ARM_GPIO_PX29_WORD 		0x0c000000	
#define ARM_GPIO_PX30_WORD 		0x30000000	
#define ARM_GPIO_PX31_WORD 		0xc0000000		 

#define ARM_GPIO_PORT0 0						

//p0.21	 9575_RST
#define RST_9575_GPIO_PIN_DWORD 	ARM_GPIO_PX21_DWORD	
#define RST_9575_GPIO_PIN_WORD		ARM_GPIO_PX21_WORD	 //0x00000c00
#define RST_9575_GPIO_MODE_WORD		0x00000800			 //10,neither pull-up nor pull-down

//p0.22	 RESET_SPDIF0
#define RST_8416_0_GPIO_PIN_DWORD 	ARM_GPIO_PX22_DWORD	
#define RST_8416_0_GPIO_PIN_WORD		ARM_GPIO_PX22_WORD	 //0x00003000
#define RST_8416_0_GPIO_MODE_WORD		0x00002000			 //10,neither pull-up nor pull-down	 
						 
#define ARM_GPIO_PORT1 1
				
//p1.31 CPU Running
#define LED_RUN_GPIO_PIN_DWORD 		ARM_GPIO_PX31_DWORD	
#define LED_RUN_GPIO_PIN_WORD		ARM_GPIO_PX31_WORD	 //0xc0000000
#define LED_RUN_GPIO_MODE_WORD		0x80000000			 //10,neither pull-up nor pull-down

#define ARM_GPIO_PORT2 2  

#define ARM_GPIO_PORT3 3  

#define ARM_GPIO_PORT4 3  


//#define memcpy adi_memcpy
//#define memcmp adi_memcmp 
//#define memset adi_memset
//#define strcpy adi_strcpy
//#define strlen adi_strlen
//#define sprintf adi_sprintf

//void adi_sprintf (char *Strg, char *FrmtStrg, ...);
int adi_memcmp(const void * cs,const void * ct, unsigned int count);
int adi_memcpy(void *dst, void *src, UINT32 count);	  
int adi_memset(void *dst, UINT8 num, UINT32 count);
int adi_strcpy(char *dst, char *src);
unsigned int adi_strlen (char *Strg);
void adi_assert(const char *file, unsigned int line, const char *func, int condition, const char *description);

#endif
