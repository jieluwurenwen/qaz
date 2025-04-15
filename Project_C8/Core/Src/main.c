/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "./HAL/key/key.h"
#include "./HAL/OLED/OLED_NEW.H"
#include "./HAL/delay/delay.h"
#include "./HAL/dht11/dht11.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
void Key_function(void);								//��������
void Monitor_function(void);						//��⺯��
void Display_function(void);						//��ʾ����
void Manage_function(void);							//��������
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define BEEP(a) (a?HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET)) 

#define P_LED(a) (a?HAL_GPIO_WritePin(P_LED_GPIO_Port, P_LED_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(P_LED_GPIO_Port, P_LED_Pin, GPIO_PIN_RESET)) 

                                                                                                                                                                        
uint8_t USART2_TX_BUF[200];
#define u2_printf(...)  HAL_UART_Transmit(&huart2,USART2_TX_BUF,sprintf((char *)USART2_TX_BUF,__VA_ARGS__),0xffff)
  

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t key_num,flag_display;         //��������ʾ����
uint16_t time_1ms,time_500ms;         //��ʱ����1ms,500ms
uint16_t time_1,flag_1,time_2,flag_2;   //��ʱ����

//����2�����ݻ�ȡ
uint8_t uart2_value;      //���ڴ��ĵ�������
//���ڵĴ������飬���ڵĽ���ʱ�䣬���ڴ�ֵ������
uint8_t uart2_buf[36],uart2_time,uart2_num;
uint8_t uart2_rx_flag;//���ڵĻ�ȡֵ�ı�־λ
//��ͨ�����ݻ�ȡ
uint8_t adc_ch;   //adc�ĸ���
uint32_t adc_buf[3];//adc��ֵ�Ĵ洢����
uint16_t CO2,PM2_5,AIR;               //���飬PM2.5,ˮλ����
uint16_t temp,humi;                     //��ʪ��
uint16_t temp_max=30,AIR_max=20,CO2_max=20;
uint16_t SG90_1=500;                                                      //����Ƕȿ��Ʊ�����500-2500 0-180�㣩

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint16_t dong_get_adc(){
    //����ADC1
  HAL_ADC_Start(&hadc1);
    //�ȴ�ADCת����ɣ���ʱΪ100ms
    HAL_ADC_PollForConversion(&hadc1,100);
    //�ж�ADC�Ƿ�ת���ɹ�
    if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1),HAL_ADC_STATE_REG_EOC)){
         //��ȡֵ
       return HAL_ADC_GetValue(&hadc1);
    }
    return 0;
}
/****
*******�������ú���
*****/
void Key_function(void)
{
	key_num = Chiclet_Keyboard_Scan();	//����ɨ��
	if(key_num != 0)										//�а�������
	{
		switch(key_num)
		{
			case 1:													//����1���л�ģʽ
				flag_display++;
				if(flag_display >= 4)					//һ��4��ģʽ
					flag_display = 0;
				OLED_Clear();									//��һ�£�����һ��
			break;

			case 2:													//����2��ÿ��һ����ֵ��һ
			if(flag_display==0)        
        SG90_1=500;
			if(flag_display==1)
        temp_max++;
			if(flag_display==2)
        CO2_max++;
			if(flag_display==3)
        AIR_max++;
			break;

			case 3:													//����3��ÿ��һ����ֵ��һ
			if(flag_display==1)
        temp_max--;
			if(flag_display==2)
        CO2_max--;
			if(flag_display==3)
        AIR_max--;				
			break;	
			case 4:													//����4���л�ģʽ
				BEEP(0);
			break;

			default:
			break;
		}
	}
}

/****
*******��⺯��
*****/
void Monitor_function(void)
{
    DHT11_Read_TempAndHumidity(&DHT11_Data);//���û�ȡ��ʪ�Ⱥ���
		temp = DHT11_Data.temperature; //��ȡ�¶�
		humi = DHT11_Data.humidity;    //��ȡʪ��
		  for(adc_ch=0;adc_ch<3;adc_ch++)
      {
      //�ֱ���ͨ��1��2��3��ADCֵ
      adc_buf[adc_ch]=dong_get_adc();
      }
      
      PM2_5 = ((adc_buf[0]*3.3/4096.0)*1000)/12;  //ÿ500ms��ȡһ��PM2_5Ũ��
      AIR= adc_buf[1]/4095.0*100;                         //ÿ500ms����������
      CO2=adc_buf[2]/4095.0*100;                             //ÿ500ms��ȡһ�ζ�����̼Ũ��
		
  
}


/****
*******��ʾ����
*****/
void Display_function(void)
{
	switch(flag_display)								//���ݲ�ͬ����ʾģʽ��־λ����ʾ��ͬ
	{
		case 0:
      Oled_ShowCHinese(0,0,(uint8_t *)"��ʪ��");
      OLED_Show_Temp(50,0,temp);    
      OLED_Show_Humi(90,0,humi/10.0);    
			Oled_ShowString(0,2,(uint8_t *)"PM2_5");
      OLED_ShowNum(80,2,PM2_5,3,16);    
      Oled_ShowCHinese(0,4,(uint8_t *)"��������");
      OLED_ShowNum(80,4,AIR,3,16);    
      Oled_ShowCHinese(0,6,(uint8_t *)"������̼");
      OLED_ShowNum(80,6,CO2,3,16);
    
		break;

		case 1:															
      Oled_ShowCHinese(8,2,(uint8_t *)"�������¶���ֵ");
      OLED_ShowNum(55,4,temp_max,3,16);
    
		break;

		case 2:	
      Oled_ShowCHinese(0,2,(uint8_t *)"���ö�����̼��ֵ");
      OLED_ShowNum(55,4,CO2_max,3,16);
														
		break;

		case 3:	
	      Oled_ShowCHinese(0,2,(uint8_t *)"���ÿ���������ֵ");
      OLED_ShowNum(55,4,AIR_max,3,16);
													
		break;
	}
}


/****
*******��������
*****/
void Manage_function(void)
{
	if(temp/10>temp_max||CO2>CO2_max||AIR>AIR_max||PM2_5>100)//������ֵ�ᱨ��
  {
    BEEP(1);
    SG90_1=500+1000;//�򿪴��������ת��90��    
  }
  if(flag_1 == 1)                       //ͨ��WiFi�������ϴ���������
  {
    flag_1 = 0;
    u2_printf("�¶�:%.1f�� ʪ��:%d PM2.5:%d ��������:%d CO2:%d\r\n",temp/10.0,humi/10,PM2_5,AIR,CO2);
  }

}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim1);
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);       //��ʱ��3pwm��ʼ��

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  OLED_Init();
  OLED_Clear();
  DHT11_GPIO_Init();

  while (1)
  {
    Key_function();							//��������
		Monitor_function();					//��⺯��
		Display_function();					//��ʾ����
		Manage_function();					//��������

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,SG90_1);    //����ĽǶȿ���
    
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == htim1.Instance)		//��ʱ��1�����ж�
	{
		time_1ms++;
		if(time_1ms>= 500)
		{
			time_1ms= 0;
			time_500ms = 1;
		}
    time_1++;
    if(time_1 >= 1000)
    {
      time_1 = 0;
      flag_1 = 1;
    }
    time_2++;
    if(time_2 >= 1600)
    {
      time_2 = 0;
      flag_2 = 1;
    }

	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
