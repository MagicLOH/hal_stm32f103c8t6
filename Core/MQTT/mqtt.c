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

//连接成功服务器回应 20 02 00 00
//客户端主动断开连接 e0 00
uint8_t PACKET_CONNECTACK[]    = {0x20, 0x02, 0x00, 0x00}; //确认连接请求
uint8_t PACKET_SUBACK[] = {0x90, 0x03};             //订阅/取消订阅确认
uint8_t PACKET_DISCONNECT[] = {0xE0, 0x00};           //断开连接
uint8_t PACKET_PINGREQ[]  = {0xC0, 0x00};             //心跳请求
uint8_t PACKET_PINGRESP[] = {0xD0, 0x00};             //心跳响应

/******************************************************************************\
 * 函数功能：判断服务端的响应结果是否正确
 * 形参说明：response - 正确的响应结果
 * 返 回 值：0 - 成功 / 1 - 失败
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
 * 函数功能：客户端连接服务端
 * 形参说明：[in]clientId - 客户端id
 *          [in]userName - 用户名
 *          [in]passWord - 密码
 * 返 回 值：0 - 成功 / 1 - 失败
\******************************************************************************/
uint8_t MQTT_Connect(char *clientId, char *userName, char *passWord)
{
    uint32_t RemainingLength = 0;
    uint32_t clientIdLen = strlen(clientId);
    uint32_t userNameLen = strlen(userName);
    uint32_t passWordLen = strlen(passWord);
    mqtt_txBufLen = 0;
    memset(mqtt_txBuf, 0, sizeof(mqtt_txBuf));
    //【1,固定报头】
    // 1.1 [7:4]报文类型 [3:0]报文标志
    mqtt_txBuf[mqtt_txBufLen++] = (T_CONNECT << 4) | F_CONNECT;
    // 1.2 整个报文剩余长度：可变报头+有效载荷
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
    //【2,可变报头】
    // 2.1 协议名长度
    mqtt_txBuf[mqtt_txBufLen++] = 0x00; //协议名长度MSB
    mqtt_txBuf[mqtt_txBufLen++] = 0x04; //协议名长度LSB
    // 2,2 协议名
    mqtt_txBuf[mqtt_txBufLen++] = 'M';
    mqtt_txBuf[mqtt_txBufLen++] = 'Q';
    mqtt_txBuf[mqtt_txBufLen++] = 'T';
    mqtt_txBuf[mqtt_txBufLen++] = 'T';
    // 2.3 协议级别
    mqtt_txBuf[mqtt_txBufLen++] = 0x04; // 3.1.1版本协议
    // 2.4 连接标志
    mqtt_txBuf[mqtt_txBufLen++] = 0xC2; //(1100 0010)用户名=1,密码=1,清理会话=1
    // 2.5 保持连接时间
    mqtt_txBuf[mqtt_txBufLen++] = 0;    //时间MSB
    mqtt_txBuf[mqtt_txBufLen++] = 100;  //时间LSB,100s
    //【3,有效载荷】
    // 3.1 客户端标识符(一定有)
    mqtt_txBuf[mqtt_txBufLen++] = clientIdLen >> 8;   //客户端标识符MSB
    mqtt_txBuf[mqtt_txBufLen++] = clientIdLen;        //客户端标识符LSB
    memcpy(&mqtt_txBuf[mqtt_txBufLen], clientId, clientIdLen); //拷贝标识符长度
    mqtt_txBufLen += clientIdLen;
    // 3.2 遗嘱主题
    // 3.3 遗嘱消息
    // 3.4 用户名
    if (userNameLen > 0)
    {
        mqtt_txBuf[mqtt_txBufLen++] = userNameLen >> 8; //用户名MSB
        mqtt_txBuf[mqtt_txBufLen++] = userNameLen;      //用户名LSB
        memcpy(&mqtt_txBuf[mqtt_txBufLen], userName, userNameLen); //拷贝标识符长度
        mqtt_txBufLen += userNameLen;
    }
    // 3.5 密码
    if (passWordLen > 0)
    {
        mqtt_txBuf[mqtt_txBufLen++] = passWordLen >> 8; //密码MSB
        mqtt_txBuf[mqtt_txBufLen++] = passWordLen;      //密码LSB
        memcpy(&mqtt_txBuf[mqtt_txBufLen], passWord, passWordLen); //拷贝标识符长度
        mqtt_txBufLen += passWordLen;
    }
    return MQTT_SendControlPackets(mqtt_txBuf, mqtt_txBufLen, PACKET_CONNECTACK);
}

/******************************************************************************\
 * 函数功能：客户端 向 服务端 推送信息
 * 形参说明：[in]topic - 主题名
 *          [in]message - 要推送的信息
 *          [in]Qos - 服务质量等级
 *          [in]identifier - 标识符
 * 返 回 值：报文长度
\******************************************************************************/
uint8_t MQTT_Publish(char *topic, char *message, uint8_t Qos)
{
    uint32_t RemainingLength = 0;
    uint16_t topicLen = strlen(topic);
    uint32_t messageLen = strlen(message);
    uint16_t id = 0;
    mqtt_txBufLen = 0;
    memset(mqtt_txBuf, 0, sizeof(mqtt_txBuf));
    //【1,固定报头】
    // 1.1 [7:4]报文类型 [3:0]报文标志
    mqtt_txBuf[mqtt_txBufLen++] = (T_PUBLISH << 4) | F_PUBLISH;
    // 1.2 整个报文剩余长度：可变报头(主题名 + 报文标识符) + 有效载荷
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
    //【2,可变报头】
    // 2.1 主题名长度+主题名
    mqtt_txBuf[mqtt_txBufLen++] = topicLen >> 8;  //主题名长度MSB
    mqtt_txBuf[mqtt_txBufLen++] = topicLen;       //主题名长度LSB
    memcpy(&mqtt_txBuf[mqtt_txBufLen], topic, topicLen); //主题名
    mqtt_txBufLen += topicLen;
    // 2.2 报文标识符(Qos>0时才存在)
    if (Qos)
    {
        mqtt_txBuf[mqtt_txBufLen++] = F_PUBLISH >> 8; //报文标识符MSB
        mqtt_txBuf[mqtt_txBufLen++] = F_PUBLISH;      //报文标识符LSB
        id++;
    }
    //【3,有效载荷】
    memcpy(&mqtt_txBuf[mqtt_txBufLen], message, messageLen);
    mqtt_txBufLen += messageLen;
    //【4,响应】
    //【5,动作】
    //发送控制报文
    USART3_SendData(mqtt_txBuf, mqtt_txBufLen);
    return mqtt_txBufLen;
}

/******************************************************************************\
 * 函数功能：客户端 订阅/取消订阅 主题
 * 形参说明：
*            [in]topic - 主题
 *           [in]stat - 状态(1-订阅,0-取消订阅)
 *           [in]Qos - 服务质量等级
 *           [in]identifier - 标识符
 * 返 回 值：0 - 成功 / 1 - 失败
\******************************************************************************/
uint8_t MQTT_Subscribe(char *topic, uint8_t stat, uint8_t Qos)
{
    uint8_t identifier;                //---报文标识符
    uint32_t RemainingLength = 0;      //---剩余长度
    uint32_t topicLen = strlen(topic); //---主题长度
    mqtt_txBufLen = 0;
    memset(mqtt_txBuf, 0, sizeof(mqtt_txBuf));
    //【1,固定报头】
    // 1.1 [7:4]报文类型 [3:0]报文标志
    if (stat) //---订阅模式
        mqtt_txBuf[mqtt_txBufLen++] = (T_SUBSCRIBE << 4) | F_SUBSCRIBE;
    else //---取消订阅模式
        mqtt_txBuf[mqtt_txBufLen++] = (T_UNSUBSCRIBE << 4) | F_UNSUBSCRIBE;
    // 1.2 整个报文剩余长度：可变报头(报文标识符) + 有效载荷(主题过滤器)
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
    //【2,可变报头】
    // 2.1 报文标识符 --- 订阅/取消订阅主题 使用的报文标识符要一致
    if (stat)    identifier = F_SUBSCRIBE;
    else identifier = F_UNSUBSCRIBE;
    mqtt_txBuf[mqtt_txBufLen++] = identifier >> 8; //报文标识符MSB
    mqtt_txBuf[mqtt_txBufLen++] = identifier;      //报文标识符LSB
    //【3,有效载荷】
    // 3.1 主题过滤器
    mqtt_txBuf[mqtt_txBufLen++] = topicLen >> 8;   //主题长度MSB
    mqtt_txBuf[mqtt_txBufLen++] = topicLen;        //主题长度LSB
    memcpy(&mqtt_txBuf[mqtt_txBufLen], topic, topicLen); //拷贝主题
    mqtt_txBufLen += topicLen;
    // 3.2 服务质量等级 --- 订阅时才存在
    if (stat)
        mqtt_txBuf[mqtt_txBufLen++] = Qos;
    //发送控制报文
    return MQTT_SendControlPackets(mqtt_txBuf, mqtt_txBufLen, PACKET_SUBACK);
}

/******************************************************************************\
 * 函数功能：客户端 向 服务端 发送心跳请求,以保持网络连接100s内要不断重发
 * 形参说明：None
 * 返 回 值：None
\******************************************************************************/
uint8_t MQTT_PingReqPer100s(void)
{
    return MQTT_SendControlPackets(PACKET_PINGREQ, sizeof(PACKET_PINGREQ), PACKET_PINGRESP);
}

/******************************************************************************\
 * 函数功能：客户端 主动 断开连接
 * 形参说明：None
 * 返 回 值：None
\******************************************************************************/
void MQTT_DisConnect(void)
{
    USART3_SendData(PACKET_DISCONNECT, sizeof(PACKET_DISCONNECT));
}

