#include "oled.h"
#include "i2c.h"
#include "delay.h"

#define OLED_CMD (0x00) // д����ģʽ
#define OLED_DAT (0x40) // д����ģʽ
#define OLED_SLAVE_ADDR (0x78) // Ĭ�ϴӻ���ַ
#define OLED_Width (128)
#define OLED_Height (64)

uint8_t OLED_GRAM[OLED_Height/8][OLED_Width]; // ����GRAM�Դ�
static void OLED_I2C_WriteCMD(uint8_t _ucCMD);
static void OLED_I2C_WriteDAT(uint8_t _ucDAT);
static void OLED_Clear(void);
/**
  * @brief		OLED_I2CӲ����ʼ��
  * @param		none
  * @retval		none
  * @hardware	PB6 - I2C_SCL - ͨ�ÿ�©���
  * @hardware	PB7 - I2C_SDA - ͨ�ÿ�©���
  */
void OLED_I2C_Init(void)
{
    /* I2C Ӳ����ʼ�� */
//    I2C_Init();
	delay_ms(200);
	
    /* 0.96��I2C�ӿ�OLED��ʼ�� */
    OLED_I2C_WriteCMD(0xAE);//--�ر���ʾ
	OLED_I2C_WriteCMD(0x00);//--set low column address
	OLED_I2C_WriteCMD(0x10);//--set high column address
	OLED_I2C_WriteCMD(0x40);//--set start line address
	OLED_I2C_WriteCMD(0x81);//--������Ļ�ԱȶȼĴ����Ĵ���
	OLED_I2C_WriteCMD(0xCF);//--���öԱȶ�[0x00~0xFF]
	OLED_I2C_WriteCMD(0xA1);//--Set SEG/Column Mapping 0xa0 ���ҷ��� 0xa1 ����
	OLED_I2C_WriteCMD(0xC8);//--Set COM/Row Scan Direction 0xC0 ���·��� 0xC8 ����
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
	OLED_I2C_WriteCMD(0x12);//--�������ҷ���
	OLED_I2C_WriteCMD(0xDB);//--set vcomh
	OLED_I2C_WriteCMD(0x40);//--Set VCOM Deselect Level
	OLED_I2C_WriteCMD(0x20);//--����Ѱַ��ʽ (0x00/0x01/0x02)
	OLED_I2C_WriteCMD(0x02);//--ҳ��Ѱַģʽ(ҳ���е�ַ����)
	OLED_I2C_WriteCMD(0x8D);//--set Charge Pump enable/disable
	OLED_I2C_WriteCMD(0x14);//--set(0x10) disable
	OLED_I2C_WriteCMD(0xA4);//--Disable Entire Display On (0xa4/0xa5)
	OLED_I2C_WriteCMD(0xA6);//--������ʾģʽ(0xA6 ����ģʽ��Ϊ 1/0xA7 ����ģʽ��Ϊ 0)
	OLED_I2C_WriteCMD(0xAF);//--������ʾ
	
	OLED_Clear();
}

/* OLEDд���� */
static void OLED_I2C_WriteCMD(uint8_t _ucCMD)
{
    I2C_Start();
    I2C_Write_Byte(OLED_SLAVE_ADDR); // д�ӻ���ַ
    I2C_Wait_Ack();
	
    I2C_Write_Byte(OLED_CMD); // д�������0x80 �����ֽڣ�Co=1,D/C#=0
    I2C_Wait_Ack();
	
    I2C_Write_Byte(_ucCMD); // д���������
    I2C_Wait_Ack();
	
    I2C_Stop();
}

/* OLEDд���� */
static void OLED_I2C_WriteDAT(uint8_t _ucDAT)
{
    I2C_Start();
    I2C_Write_Byte(OLED_SLAVE_ADDR); // д�ӻ���ַ
    I2C_Wait_Ack();
	
    I2C_Write_Byte(OLED_DAT); // д�������0x40 �����ֽڣ�Co=0,D/C#=1
    I2C_Wait_Ack();
	
    I2C_Write_Byte(_ucDAT); // д���������
    I2C_Wait_Ack();
	
    I2C_Stop();
}
/* OLEDд�������(�����ٶ�) */
static void OLED_I2C_WriteDatas(const uint8_t *_ucDAT, uint32_t _ulCnt)
{
    I2C_Start();
    I2C_Write_Byte(OLED_SLAVE_ADDR); // д�ӻ���ַ
    I2C_Wait_Ack();
	
    I2C_Write_Byte(OLED_DAT); // д�������0x40 �����ֽڣ�Co=0,D/C#=1
    I2C_Wait_Ack();
	
	for(uint32_t i=0;i<_ulCnt;i++) // д��������
	{
		I2C_Write_Byte(_ucDAT[i]); 
		I2C_Wait_Ack();
	}
	
    I2C_Stop();
}

/**
  * @brief		OLED������ʼ����
  * @param		_ucPage - ҳ��ַ [0~7]
  * @param		_ucCol - �е�ַ [0~127]
  */
static void OLED_SetPos(uint8_t _ucPage, uint8_t _ucCol)
{
	//xx & xx��ʾȡ����nλ,���ò�������
    OLED_I2C_WriteCMD(0xB0 + (_ucPage & 0x07));		// ����ҳ��
    OLED_I2C_WriteCMD(0x00 + (_ucCol & 0x0F));      // ����λ���õ���ʼ��ַ
    OLED_I2C_WriteCMD(0x10 + ((_ucCol & 0xF0)>>4)); // ����λ���ø���ʼ��ַ
}

/* OLEDӲ������ */
static void OLED_Clear(void)
{
	for(uint8_t i=0;i<8;i++)
    {
        OLED_SetPos(i,0);
        for(uint8_t j=0;j<128;j++) //�е�ַ������
        {
            OLED_I2C_WriteDAT(0x00);
        }
    }
}

/* OLED���㺯�� */
static void OLED_DispPoint(u8 _ucX,u8 _ucY,u8 _ucState)
{
    if(_ucX < OLED_Width && _ucY < OLED_Height) //line-[0~63] column-[0-127]
    {
        uint8_t ucPage = _ucY / 8;	//�ڼ�ҳ
        uint8_t ucCol = _ucX;		//�ڼ���
        uint8_t ucPos = _ucY % 8; 	//�ڼ��������ص�
		
		/* LSB��λ��ǰ */
        if(_ucState) 
            OLED_GRAM[ucPage][ucCol] |= 1<<ucPos;  //����ģʽ
        else 
            OLED_GRAM[ucPage][ucCol] &= ~(1<<ucPos);  //����ģʽ
    }
}

/* OLED�Դ�ˢ�º��� */
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
/* OLED�Դ渴λ */
void OLED_GRAM_Reset(void)
{
    for(uint8_t i=0; i<OLED_Height/8; i++)
    {
        for(uint8_t j=0; j<OLED_Width; j++)
        {
            OLED_GRAM[i][j] = 0x00;  //1Ϊ�� 0Ϊ��
        }
    }
}

/* ˮƽ������ʾ */
void OLED_HorizontalShift(uint32_t start, uint32_t end, _OLEDScrollDir direct)
{
	OLED_I2C_WriteCMD(0x2E); 	// ֹͣ����
	OLED_GRAM_Refresh();
	OLED_I2C_WriteCMD(direct); 	// ��������
	OLED_I2C_WriteCMD(0x00); 	// A:�����ֽ�
	OLED_I2C_WriteCMD(start); 	// B:��ʼҳ��ַ
	OLED_I2C_WriteCMD(0x07); 	// C:����ʱ����
	OLED_I2C_WriteCMD(end); 	// D:����ҳ�ڵ�ַ
	OLED_I2C_WriteCMD(0x00);	// E:�����ֽ�
	OLED_I2C_WriteCMD(0xFF); 	// F:�����ֽ�
	OLED_I2C_WriteCMD(0x2F); 	// ��ʼ����
}

/* ��ֱˮƽ������ʾ */
void OLED_VerticalHorizontalShift(uint32_t start, uint32_t end, uint32_t offset, _OLEDScrollDir direct)
{
	OLED_I2C_WriteCMD(0x2E); 	// ֹͣ����
	OLED_GRAM_Refresh();
	OLED_I2C_WriteCMD(direct); 	// ��������
	OLED_I2C_WriteCMD(0x00); 	// A:�����ֽ�
	OLED_I2C_WriteCMD(start); 	// B:��ʼҳ��ַ
	OLED_I2C_WriteCMD(0x07); 	// C:����ʱ����
	OLED_I2C_WriteCMD(end); 	// D:����ҳ�ڵ�ַ
	OLED_I2C_WriteCMD(offset);	// E:��ֱƫ����
	OLED_I2C_WriteCMD(0x2F); 	// ��ʼ����
}

/* ��ֱ���� */
void OLED_VerticalShift(uint32_t start, uint32_t LineNum)
{
	if ((start + LineNum) > 64) return;
	OLED_I2C_WriteCMD(0x2E); 	// ֹͣ����
	OLED_GRAM_Refresh();
	OLED_I2C_WriteCMD(0xA3);
	OLED_I2C_WriteCMD(start); 	// A:��ʼ��
	OLED_I2C_WriteCMD(LineNum); // B:��������
	OLED_I2C_WriteCMD(0x2F); 	// ��ʼ����
}

/* ����ʽ��ʾ */
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
        for(j=0; j<8; j++) // дһ���ֽ�����
        {
            if(!_ucMode) // ����ģʽ
                (ucByte & 0x01) ? OLED_DispPoint(_ucX,_ucY++,1):OLED_DispPoint(_ucX,_ucY++,0); 
            else // ����ģʽ
                (ucByte & 0x01) ? OLED_DispPoint(_ucX,_ucY++,0):OLED_DispPoint(_ucX,_ucY++,1); 
            //��������
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
  * @brief		��ʾ�ַ���
  * @param		_ucMode - 0����1����
  * @retval		none
  */
void OLED_DispSrting
(
	uint8_t _ucX,uint8_t _ucY,uint8_t _ucWidth,uint8_t _ucHeight,const char *_pStr,u8 _ucMode
)
{
    uint8_t ASCIIPos; // ��¼����ascii���ֿ�λ��
    uint8_t ucX0 = _ucX;
    while(*_pStr != '\0')
    {
        /*�Զ�����*/
        if( _ucX > (OLED_Width-1))//���ʲ���ʾ��ͬһ��
        {
            _ucY += _ucHeight;
            _ucX = ucX0;//ͬ��������
            //_ucX = 0;//��������뻻��
        }
		if ((*_pStr >= ' ') && (*_pStr <= '~'))
			ASCIIPos = *_pStr++ - ' ';
		else
			ASCIIPos = '?' - ' '; // ������ascii�ַ�
		/* ��ʾһ���ַ� */
        OLED_DrawData_by_ColLn(_ucX,_ucY,_ucWidth,_ucHeight,Ascii8x16[ASCIIPos],_ucMode);
        _ucX += _ucWidth; // ƫ��һ���ַ�
    }
}

/**
  * @brief		��ʾ����
  * @param		_ucMode - 0����1����
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

