/**********************************
���ߣ�����˹����
��վ��https://www.mcude.com
��ϵ��ʽ��46580829(QQ)
�Ա����̣�����˹����
**********************************/
#ifndef __ONEWIRE_DHT11_H__
#define	__ONEWIRE_DHT11_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/**********************************
���ߣ�����˹����
��վ��https://www.mcude.com
��ϵ��ʽ��46580829(QQ)
�Ա����̣�����˹����
**********************************/
/* ���Ͷ��� ------------------------------------------------------------------*/
/************************ DHT11 �������Ͷ���******************************/
typedef struct
{
	uint8_t  humi_high8bit;		//ԭʼ���ݣ�ʪ�ȸ�8λ
	uint8_t  humi_low8bit;	 	//ԭʼ���ݣ�ʪ�ȵ�8λ
	uint8_t  temp_high8bit;	 	//ԭʼ���ݣ��¶ȸ�8λ
	uint8_t  temp_low8bit;	 	//ԭʼ���ݣ��¶ȸ�8λ
	uint8_t  check_sum;	 	    //У���
  float    humidity;        //ʵ��ʪ��
  float    temperature;     //ʵ���¶�  
} DHT11_Data_TypeDef;
/**********************************
���ߣ�����˹����
��վ��https://www.mcude.com
��ϵ��ʽ��46580829(QQ)
�Ա����̣�����˹����
**********************************/
/* �궨�� -------------------------------------------------------------------*/
/***********************   DHT11 �������Ŷ���  **************************/
#define DHT11_Dout_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()
#define DHT11_Dout_PORT                           GPIOA
#define DHT11_Dout_PIN                            GPIO_PIN_7

/***********************   DHT11 �����궨��  ****************************/
#define DHT11_Dout_LOW()                          HAL_GPIO_WritePin(DHT11_Dout_PORT,DHT11_Dout_PIN,GPIO_PIN_RESET) 
#define DHT11_Dout_HIGH()                         HAL_GPIO_WritePin(DHT11_Dout_PORT,DHT11_Dout_PIN,GPIO_PIN_SET)
#define DHT11_Data_IN()	                          HAL_GPIO_ReadPin(DHT11_Dout_PORT,DHT11_Dout_PIN)
/**********************************
���ߣ�����˹����
��վ��https://www.mcude.com
��ϵ��ʽ��46580829(QQ)
�Ա����̣�����˹����
**********************************/
/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
void DHT11_GPIO_Init( void );
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef * DHT11_Data);

extern DHT11_Data_TypeDef DHT11_Data;
extern uint8_t Time3Count; //��ʱ�ȴ�ʱ��
//ʹ�÷���

//DHT11_Read_TempAndHumidity(&DHT11_Data);//���û�ȡ��ʪ�Ⱥ���
//		temp = DHT11_Data.temperature; //��ȡ�¶�
//		humi = DHT11_Data.humidity;    //��ȡʪ��




#endif /* __ONEWIRE_DHT11_H__ */
