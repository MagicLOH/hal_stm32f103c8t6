#ifndef __DELAY_H_
#define __DELAY_H_

/*头文件引用*/
#include "sys.h"

/*宏定义*/

/*外部声明*/

/*函数声明*/
void delay_init(void);
void delay_us(uint32_t us);
void delay_ms(uint16_t ms);

#endif
