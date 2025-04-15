/**********************************
作者：特纳斯电子
网站：https://www.mcude.com
联系方式：46580829(QQ)
淘宝店铺：特纳斯电子
**********************************/
#ifndef _ESP8266_H_
#define _ESP8266_H_

#include "main.h"
//C库
#include <stdarg.h>
#include <stdlib.h>

#define SSID   "WIFI"
#define PASS   "123456789"

#define ProductKey    "a1AqLDOtnMT"
#define DeviceName    "komqi"
#define ClientId      "12345|securemode=3\\,signmethod=hmacsha1|"
#define Password      "94ADD9537EFC7523B87E0C7041CECE83A085CA72"
#define mqttHostUrl   "a1AqLDOtnMT.iot-as-mqtt.cn-shanghai.aliyuncs.com" 		
#define port          "1883" 	 

#define Huart_wifi   huart2

#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志

#define DelayXms(x) HAL_Delay(x)

extern unsigned char ESP8266_buf[1024];
extern unsigned short ESP8266_cnt;

extern uint8_t uartwifi_value;				//串口2接收缓存变量

typedef struct{			//时间结构体
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t week;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}Time_Get;

void ESP8266_init(void);             //连上网络则不继续连接了
void Ali_MQTT_Publish(void);		    //上报消息 建议1s上传一次数据
void Ali_MQTT_Publish1(void);		    //上报消息 建议1s上传一次数据
void Ali_MQTT_Recevie(void);        //接收消息 
_Bool ESP8266_Status(void);          //1-连接状态     0-断开状态
Time_Get ESP8266_Get_Time(void);

//串口回调函数使用方法
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//  if(huart->Instance == Huart_wifi.Instance)//串口触发中断
//	{
//    if(huart->Instance == Huart_wifi.Instance)//串口触发
//    {
//      HAL_UART_Receive_IT(&Huart_wifi, &uartwifi_value, 1);
//      if(ESP8266_cnt >= sizeof(ESP8266_buf))	ESP8266_cnt = 0; //防止串口被刷爆
//      ESP8266_buf[ESP8266_cnt++] = uartwifi_value;	
//    }
//	}
//}

#endif
