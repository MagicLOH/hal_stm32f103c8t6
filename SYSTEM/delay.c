#include "delay.h"

void delay_init(void)
{
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	/* Configure the SysTick to have interrupt in 1ms time basis*/
	HAL_SYSTICK_Config(SystemCoreClock / (1000U / uwTickFreq));
}

#if OS_SUPPORT

void delay_us(uint32_t us)
{
	SysTick->CTRL &= ~(1<<2);	//选择外部时钟源；8MHz VAL+1 耗时1/8us
	SysTick->LOAD = 8 * us; 	// 预装载值 8MHz = 1/8 us
	SysTick->VAL = 0; 			// 从0开始计数
	SysTick->CTRL |= 1<<0;      //启动定时器
    while (!(SysTick->CTRL & 1<<16)){}  //等待标志位成立，读取刚好清零
    SysTick->CTRL &= ~(1<<0);   //关闭定时器
}

void delay_ms(uint16_t ms)
{
	SysTick->CTRL &= ~(1<<2);   //选择外部时钟源；8MHz VAL+1 耗时1/8us
    SysTick->LOAD = 8000*ms;    //自动重装载值
    SysTick->VAL = 0;           //清空计数器
    SysTick->CTRL |= 1<<0;      //启动定时器
    while (!(SysTick->CTRL & 1<<16)){}  //等待标志位成立，读取刚好清零
    SysTick->CTRL &= ~(1<<0);   //关闭定时器
}

#else

void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				    	 
	ticks=nus*SYS_CLK; 						
	told=SysTick->VAL;        				
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			
		}  
	}
}

void delay_ms(u16 nms)
{
	u32 i;
	for(i=0;i<nms;i++) delay_us(1000);
}

#endif
