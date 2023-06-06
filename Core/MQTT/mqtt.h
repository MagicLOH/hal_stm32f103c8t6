#ifndef __MQTT_H__
#define __MQTT_H__

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

///////////////////////////////////////////////////////////////////////////////
// 控制报文类型
///////////////////////////////////////////////////////////////////////////////
typedef enum
{
    T_RESERVED1 = 0, // 保留位1
    T_CONNECT,       // 客户端到服务端,客户端请求连接服务端
    T_CONNACK,       // 服务端到客户端,连接报文确认
    T_PUBLISH,       // 两个方向都允许,发布消息
    T_PUBACK,        // 两个方向都允许,Qos1消息发布收到确认
    T_PUBREC,        // 两个方向都允许,发布收到(保证交互第一步)
    T_PUBREL,        // 两个方向都允许,发布释放(保证交互第二步)
    T_PUBCOMP,       // 两个方向都允许,Qos2消息发布完成(保证交互第三步)
    T_SUBSCRIBE,     // 客户端到服务端,客户端订阅请求
    T_SUBACK,        // 服务端到客户端,订阅请求报文确认
    T_UNSUBSCRIBE,   // 客户端到服务端,客户端取消订阅请求
    T_UNSUBACK,      // 服务端到客户端,取消订阅报文确认
    T_PINGREQ,       // 客户端到服务端,心跳请求
    T_PINGRESP,      // 服务端到客户端,心跳响应
    T_DISCONNECT,    // 客户端到服务端,客户端断开连接
    T_RESERVED2,     // 保留位2
} eMQTTControlPacketType;

///////////////////////////////////////////////////////////////////////////////
// 控制报文标志
///////////////////////////////////////////////////////////////////////////////
typedef enum
{
    F_CONNECT = 0,     // 客户端到服务端,客户端请求连接服务端
    F_CONNACK = 0,     // 服务端到客户端,连接报文确认
    F_PUBLISH = 0,     // 两个方向都允许,发布消息
    F_PUBACK = 0,      // 两个方向都允许,Qos1消息发布收到确认
    F_PUBREC = 0,      // 两个方向都允许,发布收到(保证交互第一步)
    F_PUBREL = 2,      // 两个方向都允许,发布释放(保证交互第二步)
    F_PUBCOMP = 0,     // 两个方向都允许,Qos2消息发布完成(保证交互第三步)
    F_SUBSCRIBE = 2,   // 客户端到服务端,客户端订阅请求
    F_SUBACK = 0,      // 服务端到客户端,订阅请求报文确认
    F_UNSUBSCRIBE = 2, // 客户端到服务端,客户端取消订阅请求
    F_UNSUBACK = 0,    // 服务端到客户端,取消订阅报文确认
    F_PINGREQ = 0,     // 客户端到服务端,心跳请求
    F_PINGRESP = 0,    // 服务端到客户端,心跳响应
    F_DISCONNECT = 0,  // 客户端到服务端,客户端断开连接
} eMQTTControlPacketFlag;

///////////////////////////////////////////////////////////////////////////////
// 核心功能函数
///////////////////////////////////////////////////////////////////////////////
uint8_t MQTT_Connect(char *clientId, char *userName, char *passWord);       //---连接
uint8_t MQTT_Publish(char *topic, char *message, unsigned char Qos);        //---推送
uint8_t MQTT_Subscribe(char *topic, unsigned char stat, unsigned char Qos); //---订阅/取消订阅
void MQTT_DisConnect(void);                                                 //---主动断开与服务器的连接
uint8_t MQTT_PingReqPer100s(void);                                          //---发送心跳包(保活)

#endif
