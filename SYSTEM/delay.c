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
	SysTick->CTRL &= ~(1<<2);   	//选择外部时钟源 HCLK/8 = 9MHz VAL+1 耗时1/9us
}

void delay_us(u32 nus)
{		
	SysTick->LOAD = 9 * nus; 	// 预装载值 9MHz = 1/9 us
	SysTick->VAL = 0; 			// 从0开始计数
	SysTick->CTRL |= 1<<0;      // 启动定时器
    while (!(SysTick->CTRL & 1 << 16)){}  // 等待标志位成立，读取刚好清零
    SysTick->CTRL &= ~(1<<0);   // 关闭定时器
}

void delay_ms(u16 nms)
{
    SysTick->LOAD = 9000 * nms;    	//自动重装载值
    SysTick->VAL = 0;           	//清空计数器
    SysTick->CTRL |= 1<<0;      	//启动定时器
    while (!(SysTick->CTRL & 1<<16)){}  //等待标志位成立，读取刚好清零
    SysTick->CTRL &= ~(1<<0);   	//关闭定时器
}

#endif
