/**********************************
作者：特纳斯电子
网站：https://www.mcude.com
联系方式：46580829(QQ)
淘宝店铺：特纳斯电子
**********************************/
//头文件
#include "./HAL/AliESP8266/AliESP8266.h"
#include "usart.h"
#include "./HAL/OLED/OLED_NEW.H"
unsigned char ESP8266_buf[1024];
unsigned short ESP8266_cnt,ESP8266_cntPre;
unsigned char USARTWIFI_TX_BUF[1024];
unsigned char uartwifi_value;				//串口2接收缓存变量
#define uwifi_printf(...)  HAL_UART_Transmit(&Huart_wifi,USARTWIFI_TX_BUF,sprintf((char *)USARTWIFI_TX_BUF,__VA_ARGS__),0xffff)

/*
************************************************************
*	函数名称：	Usart_SendString
*
*	函数功能：	串口数据发送
*
*	入口参数：	USARTx：串口组
*				str：要发送的数据
*				len：数据长度
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Usart_SendString(unsigned char *str, unsigned short len)
{
	unsigned short count = 0;
	for(; count < len; count++)
	{
		HAL_UART_Transmit(&Huart_wifi, str++, 1, 999);
	}
}
//==========================================================
//	函数名称：	ESP8266_Clear
//	函数功能：	清空缓存
//	入口参数：	无
//	返回参数：	无
//==========================================================
void ESP8266_Clear(void)
{
	memset(ESP8266_buf, 0, sizeof(ESP8266_buf));
	ESP8266_cnt = 0;
}

//==========================================================
//	函数名称：	ESP8266_WaitRecive
//	函数功能：	等待接收完成
//	入口参数：	无
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(ESP8266_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;

	if(ESP8266_cnt == ESP8266_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		ESP8266_cnt = 0;							//清0接收计数
		return REV_OK;								//返回接收完成标志
	}
	ESP8266_cntPre = ESP8266_cnt;					//置为相同
	return REV_WAIT;								//返回接收未完成标志

}

//==========================================================
//	函数名称：	ESP8266_SendCmd
//	函数功能：	发送命令
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//	返回参数：	0-成功	1-失败
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	unsigned short timeOut = 200;
	Usart_SendString((unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//如果收到数据
		{
			if(strstr((const char *)ESP8266_buf, res) != NULL)		//如果检索到关键词
			{
				ESP8266_Clear();									//清空缓存
				return 0;
			}
		}
		DelayXms(10);
	}
	return 1;
}


//==========================================================
//	函数名称：	ESP8266LinkAp
//	函数功能：	连接网络和设置格式
//	入口参数：	无
//	返回参数：	无
//==========================================================
void ESP8266LinkAp(void)
{
  char WIFI_buf[50];
	while(ESP8266_SendCmd("AT+RESTORE\r\n", ""))
    DelayXms(1000);
  while(ESP8266_SendCmd("AT+CIPSNTPCFG=1,8,\"pool.ntp.org\"\r\n", "OK"))
    DelayXms(1000);
  while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
    DelayXms(1000);
  while(ESP8266_SendCmd("AT+CIPSNTPCFG=1,8,\"ntp1.aliyun.com\"\r\n", "OK"))
    DelayXms(1000);

  sprintf(WIFI_buf,"AT+CWJAP=\"%s\",\"%s\"\r\n",SSID,PASS);										//发送客户端ID、用户名、密码
  while(ESP8266_SendCmd(WIFI_buf, "WIFI GOT IP"))
    DelayXms(1000);
  while(ESP8266_SendCmd("ATE0\r\n", "OK"))
    DelayXms(1000);  
	
     
}

//==========================================================
//	函数名称：	ESP8266LinkloTExplorer
//	函数功能：	连接阿里云，并且订阅主题
//	入口参数：	无
//	返回参数：	无
//==========================================================
void ESP8266LinkloT(void)
{
  char send_buf[512];
  //发送客户端ID、用户名、密码
  sprintf(send_buf,"AT+MQTTUSERCFG=0,1,\"NULL\",\"%s&%s\",\"%s\",0,0,\"\"\r\n",DeviceName,ProductKey,Password);			
	while(ESP8266_SendCmd(send_buf, "OK"))
	DelayXms(500);
  //发送客户端ID
  sprintf(send_buf,"AT+MQTTCLIENTID=0,\"%s\"\r\n",ClientId);			
	while(ESP8266_SendCmd(send_buf, "OK"))
	DelayXms(500);
  //连接MQTT阿里云服务器
	sprintf(send_buf,"AT+MQTTCONN=0,\"%s\",%s,1\r\n",mqttHostUrl,port);																			 
	while(ESP8266_SendCmd(send_buf, "OK"))
		DelayXms(500);
	//订阅主题
	sprintf(send_buf,"AT+MQTTSUB=0,\"/sys/%s/%s/thing/service/property/set\",1\r\n",ProductKey,DeviceName);	
	while(ESP8266_SendCmd(send_buf, "OK"))
		DelayXms(500);
	
}

//==========================================================
//	函数名称：	Ali_MQTT_Publish
//	函数功能：	向阿里云发布消息
//	入口参数：	无
//	返回参数：	无
//==========================================================
uint8_t test;
extern uint8_t relay_o;
extern uint16_t CO2,PM2_5,AIR;
extern uint16_t temp_max,AIR_max,CO2_max;                    
extern uint16_t temp,humi;                     //温湿度
void Ali_MQTT_Publish(void)
{
	char buf[200];
	char txt[512];

	memset(txt, 0, sizeof txt);

	/* 数据点整合 */
	sprintf(buf, "AT+MQTTPUB=0,\"/sys/%s/%s/thing/event/property/post\",",ProductKey,DeviceName);
	strcat(txt, buf);
	strcat(txt,"\"{\\\"method\\\":\\\"thing.service.property.set\\\"\\,\\\"id\\\":\\\"2012934115\\\"\\,\\\"params\\\":{");
  
  sprintf(buf,"\\\"CO2\\\":%d\\,", CO2);
	strcat(txt, buf);
  
  sprintf(buf,"\\\"PM2_5\\\":%d\\,",PM2_5);
	strcat(txt, buf);

  sprintf(buf, "\\\"AIR\\\":%d\\,",AIR);
	strcat(txt, buf);

  

  sprintf(buf, "\\\"humi\\\":%.1f",humi/10.0);
	strcat(txt, buf);


	strcat(txt, "}\\,\\\"version\\\":\\\"1.0.0\\\"}\",1,0\r\n");
	//发送数据
	uwifi_printf("%s",txt);
}
void Ali_MQTT_Publish1(void)
{
	char buf[200];
	char txt[512];

	memset(txt, 0, sizeof txt);

	/* 数据点整合 */
	sprintf(buf, "AT+MQTTPUB=0,\"/sys/%s/%s/thing/event/property/post\",",ProductKey,DeviceName);
	strcat(txt, buf);
	strcat(txt,"\"{\\\"method\\\":\\\"thing.service.property.set\\\"\\,\\\"id\\\":\\\"2012934115\\\"\\,\\\"params\\\":{");
  
  sprintf(buf,"\\\"temp\\\":%.1f\\,",temp/10.0);
	strcat(txt, buf);

  sprintf(buf, "\\\"temp_max\\\":%d\\,",temp_max);
	strcat(txt, buf);
  
  sprintf(buf, "\\\"CO2_max\\\":%d\\,",CO2_max);
	strcat(txt, buf);

  sprintf(buf, "\\\"AIR_max\\\":%d",AIR_max);
	strcat(txt, buf);


	strcat(txt, "}\\,\\\"version\\\":\\\"1.0.0\\\"}\",1,0\r\n");
	//发送数据
	uwifi_printf("%s",txt);
}


//==========================================================
//	函数名称：	Ali_MQTT_Recevie
//	函数功能：	接收阿里云下发的消息
//	入口参数：	无
//	返回参数：	无
//==========================================================

void Ali_MQTT_Recevie(void)
{
    char *Start;
    Start = strstr((char *)ESP8266_buf, "temp_max");
    if(Start != NULL)
    {
      
      Start = strstr(Start, ":") + 1;
      temp_max = atoi((const char *)Start);
      ESP8266_Clear();
    }
    
    Start = strstr((char *)ESP8266_buf, "CO2_max");
    if(Start != NULL)
    {
      Start = strstr(Start, ":") + 1;
      CO2_max = atoi((const char *)Start);
      ESP8266_Clear();
    }
    Start = strstr((char *)ESP8266_buf, "AIR_max");
    if(Start != NULL)
    {
      Start = strstr(Start, ":") + 1;
      AIR_max = atoi((const char *)Start);
      ESP8266_Clear();
    }
  
  
}
//==========================================================
//	函数名称：	ESP8266_Status
//	函数功能：	ESP8266状态判断
//	入口参数：	无
//	返回参数：	1-连接状态     0-断开状态
//==========================================================
_Bool ESP8266_Status(void)
{
	unsigned int timeOut;
	/* IP查询 */
	Usart_SendString((unsigned char *)"AT+CWSTATE?\r\n", strlen((const char *)"AT+CWSTATE?\r\n"));
	timeOut = 50;
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//如果收到数据
		{
			if(strstr((const char *)ESP8266_buf, "+CWSTATE") != NULL)		//如果检索到关键词
			{
				ESP8266_Clear();									//清空缓存
				return 1;	
			}
      else
      {
        ESP8266_Clear();									//清空缓存
        return 0;
			}
		}
		DelayXms(10);
	}
	ESP8266_Clear();
	return 1;
}
//==========================================================
//	函数名称：	ESP8266_init
//	函数功能：	主函数初始化函数
//	入口参数：	无
//	返回参数：	无
//==========================================================
void  ESP8266_init(void)
{
  HAL_UART_Receive_IT(&Huart_wifi, &uartwifi_value, 1);
  
  ESP8266LinkAp();
  ESP8266LinkloT();
}
//==========================================================
//	函数名称：	ESP8266_Get_Time
//	函数功能：	获取网络时间
//	入口参数：	无
//	返回参数：	无
//==========================================================
Time_Get ESP8266_Get_Time(void)
{
	unsigned short timeOut;
	char *Start;
	Time_Get Time;
	
	Usart_SendString((unsigned char *)"AT+CIPSNTPTIME?\r\n", strlen((const char *)"AT+CIPSNTPTIME?\r\n"));
	timeOut = 2000;
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							
		{
			if(strstr((const char *)ESP8266_buf, "OK") != NULL)		
			{
				Start = strstr((const char *)ESP8266_buf, "TIME:") + 5;
				
				if(strstr(Start, "Mon") != NULL)				
					Time.week = 1;
				else if(strstr(Start, "Tue") != NULL)
					Time.week = 2;
				else if(strstr(Start, "Wed") != NULL)
					Time.week = 3;
				else if(strstr(Start, "Thu") != NULL)
					Time.week = 4;
				else if(strstr(Start, "Fri") != NULL)
					Time.week = 5;
				else if(strstr(Start, "Sat") != NULL)
					Time.week = 6;
				else if(strstr(Start, "Sun") != NULL)
					Time.week = 7;
				
				if(strstr(Start, "Jan") != NULL)			
					Time.month = 1;
				else if(strstr(Start, "Feb") != NULL)
					Time.month = 2;
				else if(strstr(Start, "Mar") != NULL)
					Time.month = 3;
				else if(strstr(Start, "Apr") != NULL)
					Time.month = 4;
				else if(strstr(Start, "May") != NULL)
					Time.month = 5;
				else if(strstr(Start, "Jun") != NULL)
					Time.month = 6;
				else if(strstr(Start, "Jul") != NULL)
					Time.month = 7;
				else if(strstr(Start, "Aug") != NULL)
					Time.month = 8;
				else if(strstr(Start, "Sep") != NULL)
					Time.month = 9;
				else if(strstr(Start, "Oct") != NULL)
					Time.month = 10;
				else if(strstr(Start, "Nov") != NULL)
					Time.month = 11;
				else if(strstr(Start, "Dec") != NULL)
					Time.month = 12;
				
				Start = strstr(Start, " ") + 1;	
				Start = strstr(Start, " ") + 1;		
				if(*Start==' ')
				{
				  Start = strstr(Start, " ") + 1;		
				  Time.day = (*Start-'0');		
				}
				else
				{
				  Time.day = (*Start-'0')*10 + (*(Start+1)-'0');		
				}
				Start = strstr(Start, " ") + 1;		
				Time.hour = (*Start-'0')*10 + (*(Start+1)-'0');	
				Start = strstr(Start, ":") + 1;		
				Time.minute = (*Start-'0')*10 + (*(Start+1)-'0');		
				Start = strstr(Start, ":") + 1;		
				Time.second = (*Start-'0')*10 + (*(Start+1)-'0');		
				Start = strstr(Start, " ") + 1;		
				Time.year = (*Start-'0')*1000 + (*(Start+1)-'0')*100 + (*(Start+2)-'0')*10 + (*(Start+3)-'0');
				
				ESP8266_Clear();									
				break;
			}
		}
		
		DelayXms(10);
	}
	return Time;
}

