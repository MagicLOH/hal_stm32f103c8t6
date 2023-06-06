#ifndef __I2C_H_
#define __I2C_H_

/*头文件引用*/
#include "stm32f103xb.h"

typedef enum {
	False = 0,
	True = 1,
}_bool_t;

/*宏定义*/

/*外部声明*/

/*函数声明*/
void I2C_Init(void);
uint8_t I2C_Read_Byte(void);
void I2C_Write_Byte(uint8_t _ucTxd);
uint8_t I2C_Wait_Ack(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NAck(void);
void I2C_OLED_SCL_Level(_bool_t level);
void I2C_OLED_SDA_Level(_bool_t level);

#endif
