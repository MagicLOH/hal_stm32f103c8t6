#ifndef __ESP8266_H_
#define __ESP8266_H_

/*ͷ�ļ�����*/
#include "stm32f1xx_hal.h"

/*�궨��*/

/*�ⲿ����*/

/*��������*/
//uint8_t ESP8266_ExtiSeriaNet(void);
//uint8_t ESP8266_SendATCmd(char *_pCmd,char *_pAck,uint16_t _usOvertime);
uint8_t ESP8266_TCPClient_Init(char *_pSSID,char *_pPwd, char *_ip, uint16_t _port);

#endif
