#ifndef __OLED_H_
#define __OLED_H_

/*ͷ�ļ�����*/
#include "sys.h"

typedef enum
{
    DIRECT_UP_RIGHT		= (u8)0x29, // ��ֱ���ҹ���
    DIRECT_DOWN_LEFT	= (u8)0x2A, // ��ֱ�������
    DIRECT_RIGHT		= (u8)0x26, // ���ҹ���
	DIRECT_LEFT			= (u8)0x27, // �������
}_OLEDScrollDir;


/*�궨��*/

/*�ⲿ����*/
extern const u8 Ascii8x16[][16*8/8];
extern const u8 CHN_16x16[][16*16/8];

/*��������*/
void OLED_I2C_Init(void);
void OLED_GRAM_Reset(void);
void OLED_GRAM_Refresh(void);
void OLED_HorizontalShift(uint32_t start, uint32_t end, _OLEDScrollDir direct);
void OLED_VerticalHorizontalShift(uint32_t start, uint32_t end, uint32_t offset, _OLEDScrollDir direct);
void OLED_VerticalShift(uint32_t start, uint32_t LineNum);
void OLED_DispSrting(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const char *_pStr,u8 _ucMode);
void OLED_DrawChinese(u8 _ucX,u8 _ucY,u8 _ucSize,u8 _ucNum,u8 _ucMode);

#endif
