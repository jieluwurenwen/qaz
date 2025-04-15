/**********************************
���ߣ�����˹����
��վ��https://www.mcude.com
��ϵ��ʽ��46580829(QQ)
�Ա����̣�����˹����
**********************************/
#ifndef _ESP8266_H_
#define _ESP8266_H_

#include "main.h"
//C��
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

#define REV_OK		0	//������ɱ�־
#define REV_WAIT	1	//����δ��ɱ�־

#define DelayXms(x) HAL_Delay(x)

extern unsigned char ESP8266_buf[1024];
extern unsigned short ESP8266_cnt;

extern uint8_t uartwifi_value;				//����2���ջ������

typedef struct{			//ʱ��ṹ��
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t week;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}Time_Get;

void ESP8266_init(void);             //���������򲻼���������
void Ali_MQTT_Publish(void);		    //�ϱ���Ϣ ����1s�ϴ�һ������
void Ali_MQTT_Publish1(void);		    //�ϱ���Ϣ ����1s�ϴ�һ������
void Ali_MQTT_Recevie(void);        //������Ϣ 
_Bool ESP8266_Status(void);          //1-����״̬     0-�Ͽ�״̬
Time_Get ESP8266_Get_Time(void);

//���ڻص�����ʹ�÷���
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//  if(huart->Instance == Huart_wifi.Instance)//���ڴ����ж�
//	{
//    if(huart->Instance == Huart_wifi.Instance)//���ڴ���
//    {
//      HAL_UART_Receive_IT(&Huart_wifi, &uartwifi_value, 1);
//      if(ESP8266_cnt >= sizeof(ESP8266_buf))	ESP8266_cnt = 0; //��ֹ���ڱ�ˢ��
//      ESP8266_buf[ESP8266_cnt++] = uartwifi_value;	
//    }
//	}
//}

#endif
