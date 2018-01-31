/******************************************************************************
 *
 * FILE: printf.h  
 * Author: Ken Fang
 * Date created: Jun, 2009
 * Description: StdIO Header file for NXP LPC23xx/24xx Family Microprocessors
 *
 * Copyright(C) 2009, Analog Devices
 *
 *****************************************************************************/
#include "lig_types.h"
#include "lig_platform.h"
#include "lpc17xx_uart.h"
#include "serial.h"

void adi_printf (char *FmtStrg, ...);
/*============================================================================
 *
 *
 *
 *===========================================================================*/
char *BinToAsciiDecimal (char *Buf, UINT32 Num)
{
    UINT32 Div = 1000000000;
    UINT32 Digit;
    UINT32 Found=0;
    
    while (Div)
    {
        Digit = Num / Div;
        Num-= (Digit * Div);
        Div = Div / 10;
        if (Digit)
        {
            Found = 1;
        }
        if (Found)
        {
            *Buf= ((char)Digit) + '0';
            Buf++;
        }
    }
    if (!Found)
    {
        *Buf='0';
        Buf++;
    }
    return (Buf);
}

/*============================================================================
 *
 *
 *
 *===========================================================================*/
void printhex (char *Buf, UINT32 Num, UINT32 Size)
{
    int i, j=0;
    UCHAR ch;

    if (Size > 8)
    {
        Size = 8;
    }
    Size = 8 - Size;
    for (i=28; i>=0; i-=4)
    {
        ch = (UCHAR)((((UINT32)Num)>>i) & 0xf);
        if (ch || (Size == 0))
        {
            j=1;
        }
        if (j || (i==0))
        {
            *Buf = ch>9? ch-10+'a': ch+'0';
            Buf++;
        }
        if (Size)
        {
            Size--;
        }
    }
    *Buf = 0;
}

/*============================================================================
 *
 *
 *
 *===========================================================================*/
char *CopyStrg (char *DstStrg, char *SrcStrg)
{
    while (*SrcStrg)
    {
        *DstStrg = *SrcStrg;
        DstStrg++;
        SrcStrg++;
    }
    *DstStrg = 0;
    return (DstStrg);
}

/*============================================================================
 *
 *
 *
 *===========================================================================*/
void generic_printf (char *StrgPtr, char *FmtStrg, va_list vl)
{
    UINT32 i, j, Wid;
    char Ch, *Cptr, *EndPtr, Buf[12];
    STATIC CONSTANT char Zeros[]="000000000";      /* Must be 9 zeros */

    /*=======================================================
     * FmtStrg is the last argument specified. All others 
     * must be accessed using the variable-argument macros.
     *=======================================================*/
    Cptr = FmtStrg;

    /*================================
     * Step through the list.
     *===============================*/
    for (i=0; *Cptr; i++)
    {
        Ch = *Cptr;
        Cptr++;
        if (Ch != '%')
        {
            if (!StrgPtr)
            {
                sendchar(Ch);
            }
            else
            {
                *StrgPtr = Ch;
                StrgPtr++;
            }
            continue;
        }
        /*========================
         * Found '%'
         *=======================*/
        Ch = *Cptr;
        Cptr++;
        if (!Ch)
        {
            break;
        }
        if (Ch == '0')
        {
            Ch = *Cptr;
            Cptr++;
            if (!Ch)
            {
                break;
            }
        }
        Wid = 0;
        if ((Ch >= '0') && (Ch <= '9'))
        {
            Wid = (UINT32)(Ch - '0') & 0xff;
            Ch = *Cptr;
            Cptr++;
        }
        if (Ch == 'l')
        {
            Ch = *Cptr;
            Cptr++;
        }
        switch (Ch)
        {
            case 'd':
            case 'u':
                EndPtr = BinToAsciiDecimal (Buf, (va_arg (vl, int)));  /*  *(UINT32 *)(va_arg (vl, int))); */
                *EndPtr = 0;
                j = (UINT32) (EndPtr - Buf);
                if (Wid > j)
                {
                    Wid-= j;
                    if (!StrgPtr)
                    {
                        ser_String ((const char *)(Zeros+9-Wid));
                    }
                    else
                    {
                        StrgPtr = CopyStrg(StrgPtr, (char *)(Zeros+9-Wid));
                    }
                }
                if (!StrgPtr)
                {
                    ser_String ((const char *)Buf);
                }
                else
                {
                    StrgPtr = CopyStrg(StrgPtr, (char *)Buf);
                }
                break;

            case 'x':
            case 'p':
                printhex (Buf, va_arg (vl, int), Wid);
                if (!StrgPtr)
                {
                    ser_String ((const char *)Buf);
                }
                else
                {
                    StrgPtr = CopyStrg(StrgPtr, (char *)Buf);
                }
                break;

            case 'c':
                if (!StrgPtr)
                {
                    sendchar (va_arg (vl, char));
                }
                else
                {
                    *StrgPtr = va_arg(vl, char);
                    StrgPtr++;
                }
                break;
            case 's':
                if (!StrgPtr)
                {
                    ser_String ((const char *)(va_arg (vl, char *)));
                }
                else
                {
                    StrgPtr = CopyStrg(StrgPtr, (char *)(va_arg (vl, char *)));
                }
                break;

            default:
                break;
        }
    }
    if (StrgPtr)
    {
        *StrgPtr = 0;
    }
    va_end (vl);
}

/*============================================================================
 *
 *
 *
 *===========================================================================*/
void adi_printf (char *FmtStrg, ...)
{
    va_list argp;
    va_start (argp, FmtStrg);
    generic_printf(NULL, FmtStrg, argp);
}

/*============================================================================
 *
 *
 *
 *===========================================================================*/
void adi_sprintf (char *StrgPtr, char *FmtStrg, ...)
{
    va_list argp;
    va_start (argp, FmtStrg);
    generic_printf(StrgPtr, FmtStrg, argp);
}

/*============================================================================
 *
 *
 *
 *===========================================================================*/
int adi_memcmp(const void * cs,const void * ct, unsigned int count)
{
    const unsigned char *su1, *su2;
    int res = 0;

    for(su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
    if ((res = *su1 - *su2) != 0)
    {
        break;
    }
    return (res);
}

/*============================================================================
 *
 *
 *
 *===========================================================================*/
int adi_memcpy(void *dst, void *src, UINT32 count)
{
    if (count)
    {
        ASSERT(dst);
        ASSERT(src);

        if (!dst || !src)
        {
            return FALSE;
        }

        while (count)
        {
            *(char *)dst = *(char *)src;
            dst = ((char *)dst)+1;
            src = ((char *)src)+1;
            count--;
        }
    }

    return (TRUE);
}

int adi_memset(void *dst, UINT8 num, UINT32 count)
{
    if (count)
    {
        ASSERT(dst);
        while (count)
        {
            *(char *)dst = num;
            dst = ((char *)dst)+1;
            count--;
        }
    }

    return (TRUE);
}
/*============================================================================
 *
 *
 *
 *===========================================================================*/
int adi_strcpy (char *dst, char *src)
{
    while (*src)
    {
        *dst = *src;
        dst++;
        src++;
    }
    *dst = 0;
    return (TRUE);
}

/*============================================================================
 *
 *
 *
 *===========================================================================*/
unsigned int adi_strlen (char *Strg)
{
    unsigned int i=0;

    while (*Strg)
    {
        Strg++;
        i++;
    }
    return (i);
}

