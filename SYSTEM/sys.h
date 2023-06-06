#ifndef __SYS_H_
#define __SYS_H_

/*头文件引用*/
#include "stm32f1xx_hal.h"
#include "stm32f103xb.h"

/*宏定义*/
#define OS_SUPPORT 	0
#define SYS_CLK		100

/* 把"位带地址＋位序号"转换成别名地址的宏 */
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr & 0xFFFFF)<<5)+(bitnum<<2))
/* 把该地址转换成一个指针 */
#define MEM_ADDR(addr) *((volatile unsigned long *) (addr))
/* 位带操作 */
#define PAin(bitnum)    MEM_ADDR(BITBAND(GPIOA_BASE+0x08,bitnum))//GPIOA_IDR
#define PAout(bitnum)   MEM_ADDR(BITBAND(GPIOA_BASE+0x0C,bitnum))//GPIOA_ODR
#define PBin(bitnum)    MEM_ADDR(BITBAND(GPIOB_BASE+0x08,bitnum))//GPIOB_IDR
#define PBout(bitnum)   MEM_ADDR(BITBAND(GPIOB_BASE+0x0C,bitnum))//GPIOB_ODR
#define PCin(bitnum)    MEM_ADDR(BITBAND(GPIOC_BASE+0x08,bitnum))//GPIOC_IDR
#define PCout(bitnum)   MEM_ADDR(BITBAND(GPIOC_BASE+0x0C,bitnum))//GPIOC_ODR

typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  
typedef const int16_t sc16;  
typedef const int8_t sc8;  

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;  
typedef __I int16_t vsc16; 
typedef __I int8_t vsc8;   

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  
typedef const uint16_t uc16;  
typedef const uint8_t uc8; 

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;  
typedef __I uint16_t vuc16; 
typedef __I uint8_t vuc8; 




#endif
