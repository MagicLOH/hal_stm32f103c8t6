/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "rtc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//sys
#include "stdio.h"
#include "string.h"
#include "delay.h"
#include "i2c.h"

//hardwares
#include "led.h"
#include "esp8266.h"

//middlewares
#include "u8g2.h"
#include "oled_ui.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define WIFI_NAME 		"HUAWEI_S10"
#define WIFI_PASSWORD 	"sb123456"

#define TencentCloud_ServerIp    "106.55.124.154"   //#云服务器ip地址[固定值]  
#define TencentCloud_ServerPort  1883               //#云服务器端口号[固定值]

#define Seniverse_ServerIP		"api.seniverse.com"
#define Seniverse_ServerPort	80
#define SeniverseAPI "GET https://api.seniverse.com/v3/weather/now.json?key=S0E1V0SzhjSNMOB2S&location=beijing&language=en&unit=c\r\n"
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//RTC_DateTypeDef GetDate;	// 获取日期结构体
//RTC_DateTypeDef SetDate;	// 设置日期结构体
//RTC_TimeTypeDef GetTime;	// 获取时间结构体
//RTC_TimeTypeDef SetTime;	// 设置时间结构体

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
  MX_DMA_Init();
  MX_RTC_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
    //ui_draw_image();
	ESP8266_TCPClient_Init(WIFI_NAME, WIFI_PASSWORD, Seniverse_ServerIP, Seniverse_ServerPort);
  /* USER CODE END 2 */


  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    uint8_t count = 0;
    while (1)
    {
//		delay_ms(1000);
//		count++;
//		if (count == 3)
//		{
//			HAL_UART_Transmit(&huart3, (uint8_t *)SeniverseAPI, strlen(SeniverseAPI), 0xFFFF);
//			count = 0;
//		}
		
        /*------------------------------串口1接收------------------------------*/
        if (usart1.rx_flag)
        {
			//ESP8266_ExtiSeriaNet();
			//ESP8266_SendATCmd("AT\r\n", "OK", 100);
			HAL_UART_Transmit(&huart3, usart1.rx_buffer, strlen((char *)(usart1.rx_buffer)), 0xFFFF);
			LED1 = !LED1;
			usart1.rx_flag = 0;
            usart1.rx_size = 0;
            //printf("usart[1]_rx:%s\n", usart1.rx_buffer);
            memset(usart1.rx_buffer, 0, RXBUFFERSIZE);
		}
		if (usart3.rx_flag)
        {
			usart3.rx_flag = 0;
            usart3.rx_size = 0;
            printf("usart[3]_rx:\n%s\n", usart3.rx_buffer);
            memset(usart3.rx_buffer, 0, RXBUFFERSIZE);
		}
		
        /*---------------------------------end---------------------------------*/
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
        //////////运行指示灯///////////
//        delay_ms(1);
//        count++;
//        if (count % 233 == 0)
//        {
//            LED1 = !LED1;
//            count = 0;
//        }
        ///////////////////////////////
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
