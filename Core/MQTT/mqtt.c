#include <stdio.h>
#include <string.h>

#include "mqtt.h"
#include "delay.h"
#include "usart.h"
#include "led.h"

uint8_t mqtt_txBufLen;
uint8_t mqtt_rxBufLen;
uint8_t mqtt_txBuf[256];
uint8_t mqtt_rxBuf[256];

//���ӳɹ���������Ӧ 20 02 00 00
//�ͻ��������Ͽ����� e0 00
uint8_t PACKET_CONNECTACK[]    = {0x20, 0x02, 0x00, 0x00}; //ȷ����������
uint8_t PACKET_SUBACK[] = {0x90, 0x03};             //����/ȡ������ȷ��
uint8_t PACKET_DISCONNECT[] = {0xE0, 0x00};           //�Ͽ�����
uint8_t PACKET_PINGREQ[]  = {0xC0, 0x00};             //��������
uint8_t PACKET_PINGRESP[] = {0xD0, 0x00};             //������Ӧ

/******************************************************************************\
 * �������ܣ��жϷ���˵���Ӧ����Ƿ���ȷ
 * �β�˵����response - ��ȷ����Ӧ���
 * �� �� ֵ��0 - �ɹ� / 1 - ʧ��
\******************************************************************************/
static uint8_t MQTT_SendControlPackets(uint8_t *buf, uint8_t len, uint8_t *response)
{
    uint16_t i, j;
    usart3.rx_size = 0;
    usart3.rx_flag = 0;
    for (i = 0; i < 10; i++)
    {
        mqtt_rxBufLen = 0;
        memset(mqtt_rxBuf, 0, sizeof(mqtt_rxBuf));
        USART3_SendDataPack(buf, len);
        for (j = 0; j < 1000*60; j++)
        {
            delay_ms(1);
            if (j % 100 == 0)
                LED1 = !LED1;
            if (usart3.rx_flag)
            {
                memcpy(mqtt_rxBuf, usart3.rx_buffer, usart3.rx_size);
//                //////////////////////////////////////////////////////
//                printf("ServerResponse:[ ");
//                for(uint8_t k=0; k<g_ucUsart3RxCnt; k++)
//                {
//                    printf("%02x ", g_ucUsart3RxBuf[k]);
//                }
//                printf("]\r\n");
//                //////////////////////////////////////////////////////
                usart3.rx_size = 0;
                usart3.rx_flag = 0;
				memset(usart3.rx_buffer, 0, RXBUFFERSIZE);
            }
            if ((mqtt_rxBuf[0] == response[0]) && (mqtt_rxBuf[1] == response[1]))
            {
                printf("[OK]\n");
                return 0;
            }
        }
    }
    printf("[ERROR]\n");
    return 1;
}

/******************************************************************************\
 * �������ܣ��ͻ������ӷ����
 * �β�˵����[in]clientId - �ͻ���id
 *          [in]userName - �û���
 *          [in]passWord - ����
 * �� �� ֵ��0 - �ɹ� / 1 - ʧ��
\******************************************************************************/
uint8_t MQTT_Connect(char *clientId, char *userName, char *passWord)
{
    uint32_t RemainingLength = 0;
    uint32_t clientIdLen = strlen(clientId);
    uint32_t userNameLen = strlen(userName);
    uint32_t passWordLen = strlen(passWord);
    mqtt_txBufLen = 0;
    memset(mqtt_txBuf, 0, sizeof(mqtt_txBuf));
    //��1,�̶���ͷ��
    // 1.1 [7:4]�������� [3:0]���ı�־
    mqtt_txBuf[mqtt_txBufLen++] = (T_CONNECT << 4) | F_CONNECT;
    // 1.2 ��������ʣ�೤�ȣ��ɱ䱨ͷ+��Ч�غ�
    RemainingLength = 10 + (2 + clientIdLen) + (2 + userNameLen) + (2 + passWordLen);
    do
    {
        uint8_t encodedByte = RemainingLength % 128;
        RemainingLength = RemainingLength / 128;
        // if there are more data to encode, set the top bit of this byte
        if (RemainingLength > 0)
            encodedByte = encodedByte | 128;
        mqtt_txBuf[mqtt_txBufLen++] = encodedByte;
    }
    while (RemainingLength > 0);
    //��2,�ɱ䱨ͷ��
    // 2.1 Э��������
    mqtt_txBuf[mqtt_txBufLen++] = 0x00; //Э��������MSB
    mqtt_txBuf[mqtt_txBufLen++] = 0x04; //Э��������LSB
    // 2,2 Э����
    mqtt_txBuf[mqtt_txBufLen++] = 'M';
    mqtt_txBuf[mqtt_txBufLen++] = 'Q';
    mqtt_txBuf[mqtt_txBufLen++] = 'T';
    mqtt_txBuf[mqtt_txBufLen++] = 'T';
    // 2.3 Э�鼶��
    mqtt_txBuf[mqtt_txBufLen++] = 0x04; // 3.1.1�汾Э��
    // 2.4 ���ӱ�־
    mqtt_txBuf[mqtt_txBufLen++] = 0xC2; //(1100 0010)�û���=1,����=1,����Ự=1
    // 2.5 ��������ʱ��
    mqtt_txBuf[mqtt_txBufLen++] = 0;    //ʱ��MSB
    mqtt_txBuf[mqtt_txBufLen++] = 100;  //ʱ��LSB,100s
    //��3,��Ч�غɡ�
    // 3.1 �ͻ��˱�ʶ��(һ����)
    mqtt_txBuf[mqtt_txBufLen++] = clientIdLen >> 8;   //�ͻ��˱�ʶ��MSB
    mqtt_txBuf[mqtt_txBufLen++] = clientIdLen;        //�ͻ��˱�ʶ��LSB
    memcpy(&mqtt_txBuf[mqtt_txBufLen], clientId, clientIdLen); //������ʶ������
    mqtt_txBufLen += clientIdLen;
    // 3.2 ��������
    // 3.3 ������Ϣ
    // 3.4 �û���
    if (userNameLen > 0)
    {
        mqtt_txBuf[mqtt_txBufLen++] = userNameLen >> 8; //�û���MSB
        mqtt_txBuf[mqtt_txBufLen++] = userNameLen;      //�û���LSB
        memcpy(&mqtt_txBuf[mqtt_txBufLen], userName, userNameLen); //������ʶ������
        mqtt_txBufLen += userNameLen;
    }
    // 3.5 ����
    if (passWordLen > 0)
    {
        mqtt_txBuf[mqtt_txBufLen++] = passWordLen >> 8; //����MSB
        mqtt_txBuf[mqtt_txBufLen++] = passWordLen;      //����LSB
        memcpy(&mqtt_txBuf[mqtt_txBufLen], passWord, passWordLen); //������ʶ������
        mqtt_txBufLen += passWordLen;
    }
    return MQTT_SendControlPackets(mqtt_txBuf, mqtt_txBufLen, PACKET_CONNECTACK);
}

/******************************************************************************\
 * �������ܣ��ͻ��� �� ����� ������Ϣ
 * �β�˵����[in]topic - ������
 *          [in]message - Ҫ���͵���Ϣ
 *          [in]Qos - ���������ȼ�
 *          [in]identifier - ��ʶ��
 * �� �� ֵ�����ĳ���
\******************************************************************************/
uint8_t MQTT_Publish(char *topic, char *message, uint8_t Qos)
{
    uint32_t RemainingLength = 0;
    uint16_t topicLen = strlen(topic);
    uint32_t messageLen = strlen(message);
    uint16_t id = 0;
    mqtt_txBufLen = 0;
    memset(mqtt_txBuf, 0, sizeof(mqtt_txBuf));
    //��1,�̶���ͷ��
    // 1.1 [7:4]�������� [3:0]���ı�־
    mqtt_txBuf[mqtt_txBufLen++] = (T_PUBLISH << 4) | F_PUBLISH;
    // 1.2 ��������ʣ�೤�ȣ��ɱ䱨ͷ(������ + ���ı�ʶ��) + ��Ч�غ�
    RemainingLength = (2 + topicLen) + (Qos ? 2 : 0) + messageLen;
    do
    {
        uint8_t encodedByte = RemainingLength % 128;
        RemainingLength = RemainingLength / 128;
        // if there are more data to encode, set the top bit of this byte
        if (RemainingLength > 0)
            encodedByte = encodedByte | 128;
        mqtt_txBuf[mqtt_txBufLen++] = encodedByte;
    }
    while (RemainingLength > 0);
    //��2,�ɱ䱨ͷ��
    // 2.1 ����������+������
    mqtt_txBuf[mqtt_txBufLen++] = topicLen >> 8;  //����������MSB
    mqtt_txBuf[mqtt_txBufLen++] = topicLen;       //����������LSB
    memcpy(&mqtt_txBuf[mqtt_txBufLen], topic, topicLen); //������
    mqtt_txBufLen += topicLen;
    // 2.2 ���ı�ʶ��(Qos>0ʱ�Ŵ���)
    if (Qos)
    {
        mqtt_txBuf[mqtt_txBufLen++] = F_PUBLISH >> 8; //���ı�ʶ��MSB
        mqtt_txBuf[mqtt_txBufLen++] = F_PUBLISH;      //���ı�ʶ��LSB
        id++;
    }
    //��3,��Ч�غɡ�
    memcpy(&mqtt_txBuf[mqtt_txBufLen], message, messageLen);
    mqtt_txBufLen += messageLen;
    //��4,��Ӧ��
    //��5,������
    //���Ϳ��Ʊ���
    USART3_SendData(mqtt_txBuf, mqtt_txBufLen);
    return mqtt_txBufLen;
}

/******************************************************************************\
 * �������ܣ��ͻ��� ����/ȡ������ ����
 * �β�˵����
*            [in]topic - ����
 *           [in]stat - ״̬(1-����,0-ȡ������)
 *           [in]Qos - ���������ȼ�
 *           [in]identifier - ��ʶ��
 * �� �� ֵ��0 - �ɹ� / 1 - ʧ��
\******************************************************************************/
uint8_t MQTT_Subscribe(char *topic, uint8_t stat, uint8_t Qos)
{
    uint8_t identifier;                //---���ı�ʶ��
    uint32_t RemainingLength = 0;      //---ʣ�೤��
    uint32_t topicLen = strlen(topic); //---���ⳤ��
    mqtt_txBufLen = 0;
    memset(mqtt_txBuf, 0, sizeof(mqtt_txBuf));
    //��1,�̶���ͷ��
    // 1.1 [7:4]�������� [3:0]���ı�־
    if (stat) //---����ģʽ
        mqtt_txBuf[mqtt_txBufLen++] = (T_SUBSCRIBE << 4) | F_SUBSCRIBE;
    else //---ȡ������ģʽ
        mqtt_txBuf[mqtt_txBufLen++] = (T_UNSUBSCRIBE << 4) | F_UNSUBSCRIBE;
    // 1.2 ��������ʣ�೤�ȣ��ɱ䱨ͷ(���ı�ʶ��) + ��Ч�غ�(���������)
    RemainingLength = 2 + (2 + topicLen) + (stat ? 1 : 0);
    do
    {
        uint32_t encodedByte = RemainingLength % 128;
        RemainingLength = RemainingLength / 128;
        // if there are more data to encode, set the top bit of this byte
        if (RemainingLength > 0)
            encodedByte = encodedByte | 128;
        mqtt_txBuf[mqtt_txBufLen++] = encodedByte;
    }
    while (RemainingLength > 0);
    //��2,�ɱ䱨ͷ��
    // 2.1 ���ı�ʶ�� --- ����/ȡ���������� ʹ�õı��ı�ʶ��Ҫһ��
    if (stat)    identifier = F_SUBSCRIBE;
    else identifier = F_UNSUBSCRIBE;
    mqtt_txBuf[mqtt_txBufLen++] = identifier >> 8; //���ı�ʶ��MSB
    mqtt_txBuf[mqtt_txBufLen++] = identifier;      //���ı�ʶ��LSB
    //��3,��Ч�غɡ�
    // 3.1 ���������
    mqtt_txBuf[mqtt_txBufLen++] = topicLen >> 8;   //���ⳤ��MSB
    mqtt_txBuf[mqtt_txBufLen++] = topicLen;        //���ⳤ��LSB
    memcpy(&mqtt_txBuf[mqtt_txBufLen], topic, topicLen); //��������
    mqtt_txBufLen += topicLen;
    // 3.2 ���������ȼ� --- ����ʱ�Ŵ���
    if (stat)
        mqtt_txBuf[mqtt_txBufLen++] = Qos;
    //���Ϳ��Ʊ���
    return MQTT_SendControlPackets(mqtt_txBuf, mqtt_txBufLen, PACKET_SUBACK);
}

/******************************************************************************\
 * �������ܣ��ͻ��� �� ����� ������������,�Ա�����������100s��Ҫ�����ط�
 * �β�˵����None
 * �� �� ֵ��None
\******************************************************************************/
uint8_t MQTT_PingReqPer100s(void)
{
    return MQTT_SendControlPackets(PACKET_PINGREQ, sizeof(PACKET_PINGREQ), PACKET_PINGRESP);
}

/******************************************************************************\
 * �������ܣ��ͻ��� ���� �Ͽ�����
 * �β�˵����None
 * �� �� ֵ��None
\******************************************************************************/
void MQTT_DisConnect(void)
{
    USART3_SendData(PACKET_DISCONNECT, sizeof(PACKET_DISCONNECT));
}

