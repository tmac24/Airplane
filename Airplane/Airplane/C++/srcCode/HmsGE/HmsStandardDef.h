/****************************************************************************/
/*                                                                          */
/*              Hermes system：HmsStandardDef.h                             */
/*              the standard type define                                    */
/*                                                                          */
/*              Copyright 2019 Hermes system All rights reserved            */
/*                                                                          */
/****************************************************************************/
#ifndef __HERMES_STANDARD_H__
#define __HERMES_STANDARD_H__

//**********************宏定义区****************************
#ifdef SYS_BIG_ENDIAN
#define BigLittleSwap16(A) 		((((u16)(A) & 0xff00) >> 8) | \ 
 								(((u16)(A) & 0x00ff) << 8))

#define BigLittleSwap32(A) 		((((u32)(A) & 0xff000000) >> 24) | \ 
 								(((u32)(A) & 0x00ff0000) >> 8) | \ 
								(((u32)(A) & 0x0000ff00) << 8) | \ 
								(((u32)(A) & 0x000000ff) << 24))
#else
#define BigLittleSwap16(A)		(A)	
#define BigLittleSwap32(A)		(A)	
#endif

#define HMS_OK					(0)
#define HMS_ERROR				(-1)

#define HMS_NULL				((void*)0)

#define HMS_UNUSE
//**********************数据类型定义区**************************
typedef unsigned char		HmsBool8;
typedef unsigned char		HmsUint8;	
typedef signed char			HmsInt8;
typedef unsigned short		HmsUint16;
typedef signed short 		HmsInt16;
typedef unsigned int		HmsUint32;	
typedef signed int			HmsInt32;
typedef unsigned long long	HmsUint64;	
typedef long long			HmsInt64;

typedef float				HmsFloat32;
typedef double				HmsFloat64;

typedef unsigned long       HmsDWORD;

//**********************全局变量声明**************************


#define _NAV_SVS_LOGIC_JUDGE 1

#endif
