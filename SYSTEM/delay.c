#include "delay.h"


#if OS_SUPPORT

void delay_us(uint32_t us)
{
	SysTick->CTRL &= ~(1<<2);	// ѡ���ⲿʱ��Դ��8MHz VAL+1 ��ʱ1/8us
	SysTick->LOAD = 8 * us; 	// Ԥװ��ֵ 8MHz = 1/8 us
	SysTick->VAL = 0; 			// ��0��ʼ����
	SysTick->CTRL |= 1<<0;      // ������ʱ��
    while (!(SysTick->CTRL & 1 << 16)){}  // �ȴ���־λ��������ȡ�պ�����
    SysTick->CTRL &= ~(1<<0);   // �رն�ʱ��
}

void delay_ms(uint16_t ms)
{
	SysTick->CTRL &= ~(1<<2);   //ѡ���ⲿʱ��Դ��8MHz VAL+1 ��ʱ1/8us
    SysTick->LOAD = 8000*ms;    //�Զ���װ��ֵ
    SysTick->VAL = 0;           //��ռ�����
    SysTick->CTRL |= 1<<0;      //������ʱ��
    while (!(SysTick->CTRL & 1<<16)){}  //�ȴ���־λ��������ȡ�պ�����
    SysTick->CTRL &= ~(1<<0);   //�رն�ʱ��
}

#else
void delay_us(u32 nus)
{		
//	u32 ticks;
//	u32 told,tnow,tcnt=0;
//	u32 reload=SysTick->LOAD;				    	 
//	ticks=nus*SYS_CLK; 						
//	told=SysTick->VAL;        				
//	while(1)
//	{
//		tnow=SysTick->VAL;	
//		if(tnow!=told)
//		{	    
//			if(tnow<told)tcnt+=told-tnow;	
//			else tcnt+=reload-tnow+told;	    
//			told=tnow;
//			if(tcnt>=ticks)break;			
//		}  
//	}
	SysTick->CTRL &= ~(1<<2);	// ѡ���ⲿʱ��Դ��8MHz VAL+1 ��ʱ1/8us
	SysTick->LOAD = 8 * nus; 	// Ԥװ��ֵ 8MHz = 1/8 us
	SysTick->VAL = 0; 			// ��0��ʼ����
	SysTick->CTRL |= 1<<0;      // ������ʱ��
    while (!(SysTick->CTRL & 1 << 16)){}  // �ȴ���־λ��������ȡ�պ�����
    SysTick->CTRL &= ~(1<<0);   // �رն�ʱ��
}

void delay_ms(u16 nms)
{
	SysTick->CTRL &= ~(1<<2);   	//ѡ���ⲿʱ��Դ��8MHz VAL+1 ��ʱ1/8us
    SysTick->LOAD = 8000 * nms;    	//�Զ���װ��ֵ
    SysTick->VAL = 0;           	//��ռ�����
    SysTick->CTRL |= 1<<0;      	//������ʱ��
    while (!(SysTick->CTRL & 1<<16)){}  //�ȴ���־λ��������ȡ�պ�����
    SysTick->CTRL &= ~(1<<0);   	//�رն�ʱ��
}

#endif
