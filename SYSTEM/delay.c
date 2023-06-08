#include "delay.h"
#include "stm32f1xx_hal_cortex.h"

#if OS_SUPPORT

void delay_us(uint32_t us)
{
	int i, j, k;
	for (i = 0; i < 1000; i++)
		for (j = 0; j < 1000; j++)
			for(k = 0; k < 1000; k++);
}

void delay_ms(uint16_t ms)
{
	int i;
	for (i = 0; i < ms; i++)
		delay_us(1000);
}

#else
void delay_init(void)
{
	SysTick->CTRL &= ~(1<<2);   	//ѡ���ⲿʱ��Դ HCLK/8 = 9MHz VAL+1 ��ʱ1/9us
}

void delay_us(u32 nus)
{		
	SysTick->LOAD = 9 * nus; 	// Ԥװ��ֵ 9MHz = 1/9 us
	SysTick->VAL = 0; 			// ��0��ʼ����
	SysTick->CTRL |= 1<<0;      // ������ʱ��
    while (!(SysTick->CTRL & 1 << 16)){}  // �ȴ���־λ��������ȡ�պ�����
    SysTick->CTRL &= ~(1<<0);   // �رն�ʱ��
}

void delay_ms(u16 nms)
{
    SysTick->LOAD = 9000 * nms;    	//�Զ���װ��ֵ
    SysTick->VAL = 0;           	//��ռ�����
    SysTick->CTRL |= 1<<0;      	//������ʱ��
    while (!(SysTick->CTRL & 1<<16)){}  //�ȴ���־λ��������ȡ�պ�����
    SysTick->CTRL &= ~(1<<0);   	//�رն�ʱ��
}

#endif
