#include "oled.h"
#include "i2c.h"
#include "delay.h"

#define OLED_CMD (0x00) // 写命令模式
#define OLED_DAT (0x40) // 写数据模式
#define OLED_SLAVE_ADDR (0x78) // 默认从机地址
#define OLED_Width (128)
#define OLED_Height (64)

uint8_t OLED_GRAM[OLED_Height/8][OLED_Width]; // 建立GRAM显存
static void OLED_I2C_WriteCMD(uint8_t _ucCMD);
static void OLED_I2C_WriteDAT(uint8_t _ucDAT);
static void OLED_Clear(void);
/**
  * @brief		OLED_I2C硬件初始化
  * @param		none
  * @retval		none
  * @hardware	PB6 - I2C_SCL - 通用开漏输出
  * @hardware	PB7 - I2C_SDA - 通用开漏输出
  */
void OLED_I2C_Init(void)
{
    /* I2C 硬件初始化 */
//    I2C_Init();
	delay_ms(200);
	
    /* 0.96寸I2C接口OLED初始化 */
    OLED_I2C_WriteCMD(0xAE);//--关闭显示
	OLED_I2C_WriteCMD(0x00);//--set low column address
	OLED_I2C_WriteCMD(0x10);//--set high column address
	OLED_I2C_WriteCMD(0x40);//--set start line address
	OLED_I2C_WriteCMD(0x81);//--设置屏幕对比度寄存器寄存器
	OLED_I2C_WriteCMD(0xCF);//--设置对比度[0x00~0xFF]
	OLED_I2C_WriteCMD(0xA1);//--Set SEG/Column Mapping 0xa0 左右反置 0xa1 正常
	OLED_I2C_WriteCMD(0xC8);//--Set COM/Row Scan Direction 0xC0 上下反置 0xC8 正常
	OLED_I2C_WriteCMD(0xA6);//--set normal display
	OLED_I2C_WriteCMD(0xA8);//--set multiplex ratio(1 to 64)
	OLED_I2C_WriteCMD(0x3F);//--1/64 duty
	OLED_I2C_WriteCMD(0xD3);//--set display offset
	OLED_I2C_WriteCMD(0x00);//--not offset
	OLED_I2C_WriteCMD(0xD5);//--set display clock divide ratio/oscillatorfrequency
	OLED_I2C_WriteCMD(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_I2C_WriteCMD(0xD9);//--set pre-charge period
	OLED_I2C_WriteCMD(0xF1);//--Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_I2C_WriteCMD(0xDA);//--set com pins hardware configuration
	OLED_I2C_WriteCMD(0x12);//--启用左右反置
	OLED_I2C_WriteCMD(0xDB);//--set vcomh
	OLED_I2C_WriteCMD(0x40);//--Set VCOM Deselect Level
	OLED_I2C_WriteCMD(0x20);//--设置寻址方式 (0x00/0x01/0x02)
	OLED_I2C_WriteCMD(0x02);//--页面寻址模式(页内列地址自增)
	OLED_I2C_WriteCMD(0x8D);//--set Charge Pump enable/disable
	OLED_I2C_WriteCMD(0x14);//--set(0x10) disable
	OLED_I2C_WriteCMD(0xA4);//--Disable Entire Display On (0xa4/0xa5)
	OLED_I2C_WriteCMD(0xA6);//--设置显示模式(0xA6 正常模式亮为 1/0xA7 反显模式亮为 0)
	OLED_I2C_WriteCMD(0xAF);//--开启显示
	
	OLED_Clear();
}

/* OLED写命令 */
static void OLED_I2C_WriteCMD(uint8_t _ucCMD)
{
    I2C_Start();
    I2C_Write_Byte(OLED_SLAVE_ADDR); // 写从机地址
    I2C_Wait_Ack();
	
    I2C_Write_Byte(OLED_CMD); // 写入控制字0x80 控制字节，Co=1,D/C#=0
    I2C_Wait_Ack();
	
    I2C_Write_Byte(_ucCMD); // 写入具体命令
    I2C_Wait_Ack();
	
    I2C_Stop();
}

/* OLED写数据 */
static void OLED_I2C_WriteDAT(uint8_t _ucDAT)
{
    I2C_Start();
    I2C_Write_Byte(OLED_SLAVE_ADDR); // 写从机地址
    I2C_Wait_Ack();
	
    I2C_Write_Byte(OLED_DAT); // 写入控制字0x40 控制字节，Co=0,D/C#=1
    I2C_Wait_Ack();
	
    I2C_Write_Byte(_ucDAT); // 写入具体命令
    I2C_Wait_Ack();
	
    I2C_Stop();
}
/* OLED写多个数据(提升速度) */
static void OLED_I2C_WriteDatas(const uint8_t *_ucDAT, uint32_t _ulCnt)
{
    I2C_Start();
    I2C_Write_Byte(OLED_SLAVE_ADDR); // 写从机地址
    I2C_Wait_Ack();
	
    I2C_Write_Byte(OLED_DAT); // 写入控制字0x40 控制字节，Co=0,D/C#=1
    I2C_Wait_Ack();
	
	for(uint32_t i=0;i<_ulCnt;i++) // 写入多个数据
	{
		I2C_Write_Byte(_ucDAT[i]); 
		I2C_Wait_Ack();
	}
	
    I2C_Stop();
}

/**
  * @brief		OLED设置起始坐标
  * @param		_ucPage - 页地址 [0~7]
  * @param		_ucCol - 列地址 [0~127]
  */
static void OLED_SetPos(uint8_t _ucPage, uint8_t _ucCol)
{
	//xx & xx表示取出第n位,设置参数保护
    OLED_I2C_WriteCMD(0xB0 + (_ucPage & 0x07));		// 设置页数
    OLED_I2C_WriteCMD(0x00 + (_ucCol & 0x0F));      // 低四位设置低起始地址
    OLED_I2C_WriteCMD(0x10 + ((_ucCol & 0xF0)>>4)); // 高四位设置高起始地址
}

/* OLED硬件清屏 */
static void OLED_Clear(void)
{
	for(uint8_t i=0;i<8;i++)
    {
        OLED_SetPos(i,0);
        for(uint8_t j=0;j<128;j++) //列地址会自增
        {
            OLED_I2C_WriteDAT(0x00);
        }
    }
}

/* OLED画点函数 */
static void OLED_DispPoint(u8 _ucX,u8 _ucY,u8 _ucState)
{
    if(_ucX < OLED_Width && _ucY < OLED_Height) //line-[0~63] column-[0-127]
    {
        uint8_t ucPage = _ucY / 8;	//第几页
        uint8_t ucCol = _ucX;		//第几列
        uint8_t ucPos = _ucY % 8; 	//第几个列像素点
		
		/* LSB低位在前 */
        if(_ucState) 
            OLED_GRAM[ucPage][ucCol] |= 1<<ucPos;  //正常模式
        else 
            OLED_GRAM[ucPage][ucCol] &= ~(1<<ucPos);  //反显模式
    }
}

/* OLED显存刷新函数 */
void OLED_GRAM_Refresh(void)
{
    for(uint8_t i=0;i<OLED_Height/8;i++)
    {
        OLED_SetPos(i,0);
//        for(uint8_t j=0;j<OLED_Width;j++)
//        {
//            OLED_I2C_WriteDAT(OLED_GRAM[i][j]);
//        }
		OLED_I2C_WriteDatas(OLED_GRAM[i], OLED_Width);
    }
}
/* OLED显存复位 */
void OLED_GRAM_Reset(void)
{
    for(uint8_t i=0; i<OLED_Height/8; i++)
    {
        for(uint8_t j=0; j<OLED_Width; j++)
        {
            OLED_GRAM[i][j] = 0x00;  //1为亮 0为灭
        }
    }
}

/* 水平滚动显示 */
void OLED_HorizontalShift(uint32_t start, uint32_t end, _OLEDScrollDir direct)
{
	OLED_I2C_WriteCMD(0x2E); 	// 停止滚动
	OLED_GRAM_Refresh();
	OLED_I2C_WriteCMD(direct); 	// 滚动方向
	OLED_I2C_WriteCMD(0x00); 	// A:虚拟字节
	OLED_I2C_WriteCMD(start); 	// B:起始页地址
	OLED_I2C_WriteCMD(0x07); 	// C:滚动时间间隔
	OLED_I2C_WriteCMD(end); 	// D:结束页第地址
	OLED_I2C_WriteCMD(0x00);	// E:虚拟字节
	OLED_I2C_WriteCMD(0xFF); 	// F:虚拟字节
	OLED_I2C_WriteCMD(0x2F); 	// 开始滚动
}

/* 垂直水平滚动显示 */
void OLED_VerticalHorizontalShift(uint32_t start, uint32_t end, uint32_t offset, _OLEDScrollDir direct)
{
	OLED_I2C_WriteCMD(0x2E); 	// 停止滚动
	OLED_GRAM_Refresh();
	OLED_I2C_WriteCMD(direct); 	// 滚动方向
	OLED_I2C_WriteCMD(0x00); 	// A:虚拟字节
	OLED_I2C_WriteCMD(start); 	// B:起始页地址
	OLED_I2C_WriteCMD(0x07); 	// C:滚动时间间隔
	OLED_I2C_WriteCMD(end); 	// D:结束页第地址
	OLED_I2C_WriteCMD(offset);	// E:垂直偏移量
	OLED_I2C_WriteCMD(0x2F); 	// 开始滚动
}

/* 垂直滚动 */
void OLED_VerticalShift(uint32_t start, uint32_t LineNum)
{
	if ((start + LineNum) > 64) return;
	OLED_I2C_WriteCMD(0x2E); 	// 停止滚动
	OLED_GRAM_Refresh();
	OLED_I2C_WriteCMD(0xA3);
	OLED_I2C_WriteCMD(start); 	// A:起始行
	OLED_I2C_WriteCMD(LineNum); // B:滚动行数
	OLED_I2C_WriteCMD(0x2F); 	// 开始滚动
}

/* 列行式显示 */
static void OLED_DrawData_by_ColLn
(
	uint8_t _ucX,uint8_t _ucY,uint8_t _ucWidth,uint8_t _ucHeight,const uint8_t *_pData,uint8_t _ucMode
)
{
    uint8_t i,j;
    uint8_t ucY0 = _ucY;
    uint8_t ucX0 = _ucX;
    while(_ucHeight % 8) _ucHeight++;
    for(i=0; i<(_ucWidth*_ucHeight/8); i++)
    {
        uint8_t ucByte = *_pData++;
        for(j=0; j<8; j++) // 写一个字节数据
        {
            if(!_ucMode) // 正常模式
                (ucByte & 0x01) ? OLED_DispPoint(_ucX,_ucY++,1):OLED_DispPoint(_ucX,_ucY++,0); 
            else // 反显模式
                (ucByte & 0x01) ? OLED_DispPoint(_ucX,_ucY++,0):OLED_DispPoint(_ucX,_ucY++,1); 
            //更新数据
            ucByte >>= 1 ;
        }
        if(!(j % 8))
        {
            _ucX++;
            _ucY = ucY0;
        }
        if(_ucX - ucX0 == _ucWidth) 
        {
            _ucX = ucX0;
            ucY0 += 8;
            _ucY = ucY0;
        }
    }
}

/**
  * @brief		显示字符串
  * @param		_ucMode - 0正常1反显
  * @retval		none
  */
void OLED_DispSrting
(
	uint8_t _ucX,uint8_t _ucY,uint8_t _ucWidth,uint8_t _ucHeight,const char *_pStr,u8 _ucMode
)
{
    uint8_t ASCIIPos; // 记录所在ascii码字库位置
    uint8_t ucX0 = _ucX;
    while(*_pStr != '\0')
    {
        /*自动换行*/
        if( _ucX > (OLED_Width-1))//单词不显示在同一行
        {
            _ucY += _ucHeight;
            _ucX = ucX0;//同缩进换行
            //_ucX = 0;//顶格左对齐换行
        }
		if ((*_pStr >= ' ') && (*_pStr <= '~'))
			ASCIIPos = *_pStr++ - ' ';
		else
			ASCIIPos = '?' - ' '; // 不属于ascii字符
		/* 显示一个字符 */
        OLED_DrawData_by_ColLn(_ucX,_ucY,_ucWidth,_ucHeight,Ascii8x16[ASCIIPos],_ucMode);
        _ucX += _ucWidth; // 偏移一个字符
    }
}

/**
  * @brief		显示汉字
  * @param		_ucMode - 0正常1反显
  * @retval		none
  */
void OLED_DrawChinese
(
	uint8_t _ucX,uint8_t _ucY,uint8_t _ucSize,uint8_t _ucNum,uint8_t _ucMode
)
{
    OLED_DrawData_by_ColLn(_ucX,_ucY,_ucSize,_ucSize,CHN_16x16[_ucNum],_ucMode);
	return;
}

