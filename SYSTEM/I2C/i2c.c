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

/*��ʼ�ź�:��ʱ���߸ߵ�ƽʱ�������ߴӸߵ�ƽ���䵽�͵�ƽ*/
void I2C_Start(void)
{
	I2C_SCL = 1;
	I2C_SDA = 1;
	I2C_Delay();
	
	I2C_SDA = 0;//�����½���
	I2C_Delay();
	
	I2C_SCL = 0;//ǯס����
	I2C_Delay();
}

/*�����ź�:��ʱ���߸ߵ�ƽʱ�������ߴӵ͵�ƽ���䵽�ߵ�ƽ*/
void I2C_Stop(void)
{
	I2C_SCL = 1;
	I2C_SDA = 0;
	I2C_Delay();
	
	I2C_SDA = 1;//����������
//	I2C_Delay();
//	
//	I2C_SCL = 0;//ǯס����
//	I2C_Delay();
	
}

/*Ӧ���ź�*/
void I2C_Ack(void)
{
	//I2C_SCL = 0;//ʱ���ߵ͵�ƽ�����޸�����
	I2C_SDA = 0;
	I2C_Delay();
	
	I2C_SCL = 1;//(ʱ���߸ߵ�ƽ �����ߵ͵�ƽ)
	I2C_Delay();
	
	I2C_SCL = 0;//ǯס����
	I2C_Delay();
	
}

/*��Ӧ���ź�*/
void I2C_NAck(void)
{
	//I2C_SCL = 0;//ʱ���ߵ͵�ƽ�����޸�����
	I2C_SDA = 1;
	I2C_Delay();
	
	I2C_SCL = 1;//(ʱ���ߵ͵�ƽ �����߸ߵ�ƽ)
	I2C_Delay();
	
	I2C_SCL = 0;//ǯס����
	I2C_Delay();
	
}

/*�ȴ�Ӧ��:������������ʱʹ�� 0�ɹ�/1ʧ��*/
uint8_t I2C_Wait_Ack(void)
{
	uint8_t ucErrtime = 0;
	/*��ʱSDAΪ����ģʽ*/
	I2C_SDA = 1;delay_us(1);//�ͷ�������,�ӻ������޸�����
	I2C_SCL = 1;delay_us(1);//�������� 
	while(READ_SDA)
	{
		ucErrtime++;
		if(ucErrtime>233)
		{
			I2C_Stop();
			return 1;
		}	
	}
	I2C_SCL = 0;//ǯס����
	I2C_Delay();
	return 0;
}

/*��ʼд�ֽ�*/
void I2C_Write_Byte(uint8_t _ucTxd)
{
	for(uint8_t i=0;i<8;i++)
	{
		I2C_SCL = 0;//������ʼд
		I2C_SDA = (_ucTxd<<i & 0x80)? 1:0;
		I2C_Delay();
		
		I2C_SCL = 1;//���豸��ʼ��
		I2C_Delay();
	}
	
	I2C_SCL = 0;//ǯס����
	I2C_Delay();
	
}

/*��ʼ���ֽ�*/
uint8_t I2C_Read_Byte(void)
{
	uint8_t ucRxd = 0;
	
	I2C_SDA = 1;//�ͷ��������ô��豸�޸�����
	for(uint8_t i=0;i<8;i++)
	{
		I2C_SCL = 0;//���豸��ʼ�޸�����
		I2C_Delay();
				
		I2C_SCL = 1;
		if(READ_SDA)
//			ucRxd |= I2C_SDA<<(7-i); 
			ucRxd |= 1<<(7-i); 
		I2C_Delay();
	}
	
	I2C_SCL = 0;//ǯס����
	I2C_Delay();
	
	return ucRxd;
}





