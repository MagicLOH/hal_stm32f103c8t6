#include "i2c.h"
#include "delay.h"
#include "sys.h"

#define I2C_SCL PBout(6)
#define I2C_SDA PBout(7)
#define READ_SDA PBin(7)

void I2C_Delay(void)
{
	delay_us(2);
}

void I2C_OLED_SCL_Level(_bool_t level)
{
	I2C_SCL = level;
}

void I2C_OLED_SDA_Level(_bool_t level)
{
	I2C_SDA = level;
}

/*起始信号:在时钟线高电平时，数据线从高电平跳变到低电平*/
void I2C_Start(void)
{
	I2C_SCL = 1;
	I2C_SDA = 1;
	I2C_Delay();
	
	I2C_SDA = 0;//触发下降沿
	I2C_Delay();
	
	I2C_SCL = 0;//钳住总线
	I2C_Delay();
}

/*结束信号:在时钟线高电平时，数据线从低电平跳变到高电平*/
void I2C_Stop(void)
{
	I2C_SCL = 1;
	I2C_SDA = 0;
	I2C_Delay();
	
	I2C_SDA = 1;//触发上升沿
//	I2C_Delay();
//	
//	I2C_SCL = 0;//钳住总线
//	I2C_Delay();
	
}

/*应答信号*/
void I2C_Ack(void)
{
	//I2C_SCL = 0;//时钟线低电平才能修改数据
	I2C_SDA = 0;
	I2C_Delay();
	
	I2C_SCL = 1;//(时钟线高电平 数据线低电平)
	I2C_Delay();
	
	I2C_SCL = 0;//钳住总线
	I2C_Delay();
	
}

/*非应答信号*/
void I2C_NAck(void)
{
	//I2C_SCL = 0;//时钟线低电平才能修改数据
	I2C_SDA = 1;
	I2C_Delay();
	
	I2C_SCL = 1;//(时钟线低电平 数据线高电平)
	I2C_Delay();
	
	I2C_SCL = 0;//钳住总线
	I2C_Delay();
	
}

/*等待应答:主机发送数据时使用 0成功/1失败*/
uint8_t I2C_Wait_Ack(void)
{
	uint8_t ucErrtime = 0;
	/*此时SDA为输入模式*/
	I2C_SDA = 1;delay_us(1);//释放数据线,从机才能修改数据
	I2C_SCL = 1;delay_us(1);//锁存数据 
	while(READ_SDA)
	{
		ucErrtime++;
		if(ucErrtime>233)
		{
			I2C_Stop();
			return 1;
		}	
	}
	I2C_SCL = 0;//钳住总线
	I2C_Delay();
	return 0;
}

/*开始写字节*/
void I2C_Write_Byte(uint8_t _ucTxd)
{
	for(uint8_t i=0;i<8;i++)
	{
		I2C_SCL = 0;//主机开始写
		I2C_SDA = (_ucTxd<<i & 0x80)? 1:0;
		I2C_Delay();
		
		I2C_SCL = 1;//从设备开始读
		I2C_Delay();
	}
	
	I2C_SCL = 0;//钳住总线
	I2C_Delay();
	
}

/*开始读字节*/
uint8_t I2C_Read_Byte(void)
{
	uint8_t ucRxd = 0;
	
	I2C_SDA = 1;//释放数据线让从设备修改数据
	for(uint8_t i=0;i<8;i++)
	{
		I2C_SCL = 0;//从设备开始修改数据
		I2C_Delay();
				
		I2C_SCL = 1;
		if(READ_SDA)
//			ucRxd |= I2C_SDA<<(7-i); 
			ucRxd |= 1<<(7-i); 
		I2C_Delay();
	}
	
	I2C_SCL = 0;//钳住总线
	I2C_Delay();
	
	return ucRxd;
}





