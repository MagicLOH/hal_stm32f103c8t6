#include <stdio.h>
#include <string.h>

#include "esp8266.h"
#include "stm32f103xb.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"

/**
  * @brief		ESP8266����ATָ��
  * @param		_pCmd - ATָ��
  * @param		_pAck - Ӧ��
  * @param		_Errtime - ��ʱʱ��
  * @retval		0-�ɹ� / 1-ʧ��
  */
uint8_t ESP8266_SendATCmd(char *_pCmd, char *_pAck, uint16_t _Errtime)
{
    uint8_t retry = 7;
    uint8_t count = 0;
    for (uint8_t i = 0; i < retry; i++)
    {
        usart3.rx_flag = 0;
        usart3.rx_size = 0;
        /* Send AT+XXX */
        HAL_UART_Transmit(&huart3, (uint8_t *)_pCmd, strlen(_pCmd), 0xFFFF);
        while (count != _Errtime)
        {
            delay_ms(1);
            count++;
            if (usart3.rx_flag)
            {
                usart3.rx_flag = 0; //��һ�ξ������־λ
                usart3.rx_size = 0;
                //printf("%s\n", usart3.rx_buffer);
                if (strstr((char *)usart3.rx_buffer, _pAck))
                {
                    printf("[OK] Receive Ack!\n");
                    memset((char *)usart3.rx_buffer, 0, RXBUFFERSIZE);
                    return 0;
                }
                memset((char *)usart3.rx_buffer, 0, RXBUFFERSIZE);
            }
        }
    }
    printf("[ERROR]!\n");
    usart3.rx_flag = 0;
    usart3.rx_size = 0;
    memset((char *)usart3.rx_buffer, 0, RXBUFFERSIZE);
    return 1;
}


/*
        �˳�͸��
*/
uint8_t ESP8266_ExtiSeriaNet(void)
{
    for (uint8_t i = 0; i < 3; i++)
    {
        USARTx_SendString(USART3, "+++");
        delay_ms(1000);
        if (ESP8266_SendATCmd("AT\r\n", "OK", 100) == 0)
            return 0;//success
    }
    return 1;//fail
}

/**
  * @brief		ESP8266�ͻ��˳�ʼ��
  * @param		_pSSID - �ȵ�����
  * @param		_pPwd - �ȵ�����
  * @retval		x-������ / 0-�ɹ�
  */
uint8_t ESP8266_TCPClient_Init(char *_pSSID, char *_pPwd, char *_ip, uint16_t _port)
{
    char buf[100];
    /* �˳�͸��ģʽ */
    printf("0.Exiting the SeriaNet mode.............");
    uint8_t ret = ESP8266_ExtiSeriaNet();
    if (ret != 0) return ret;
    delay_ms(300);
    printf("1.Device restart........................");
    /* ���Flash���й̼���Ϣ ��Ӧ��ΪWIFI GOT IP */
    if (ESP8266_SendATCmd("AT+RST\r\n", "ready", 1000)) return 2;
    printf("\n");
    delay_ms(500);
    /* ����ΪSTAģʽ */
    printf("2.Seting to STA mode....................");
    if (ESP8266_SendATCmd("AT+CWMODE=1\r\n", "OK", 200)) return 3;
    printf("\n");
    /* �����ֻ��ȵ� */
    printf("3.Connecting WIFI.......................");
    sprintf(buf, "AT+CWJAP_CUR=\"%s\",\"%s\"\r\n", _pSSID, _pPwd); // CUR - �����浽Flash
    if (ESP8266_SendATCmd(buf, "OK", 5000)) return 4;
    printf("\n");
    /* ����������ģʽ */
    printf("4.Turn on the single connections........");
    if (ESP8266_SendATCmd("AT+CIPMUX=0\r\n", "OK", 200)) return 5;
    printf("\n");
    /* ���ӵ������� */
    printf("5.Connecting the server.................");
    sprintf(buf, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", _ip, _port);
    if (ESP8266_SendATCmd(buf, "OK", 1000)) return 6;
    printf("\n");
    /* ʹ��͸��ģʽ */
    printf("6.Enable the SeriaNet mode..............");
    if (ESP8266_SendATCmd("AT+CIPMODE=1\r\n", "OK", 100)) return 7;
    printf("\n");
    /* ͸��ģʽ��ʼ�������� */
    printf("7.Sending data on the SeriaNet mode......");
    if (ESP8266_SendATCmd("AT+CIPSEND\r\n", ">", 200))   return 8;
    printf("\n");
    /* ȡ���ϵ��Զ�����AP */
#if 0
    printf("8.Connecting the server................."); // �����ñ��浽 Flash ϵͳ������
    if (ESP8266_SendATCmd("AT+CWAUTOCONN=0", "OK", 300)) return 9;
    printf("\n");
#endif
    printf("\t\t----------------------------------------\n");
    printf("\t\tWIFI Name 	: %s\n", _pSSID);
    printf("\t\tWIFI Password 	: %s\n", _pPwd);
    printf("\t\tServer IP 	: %s\n", _ip);
    printf("\t\tServer Port 	: %d\n", _port);
    printf("\t\t----------------------------------------\n");
    return 0;
}

