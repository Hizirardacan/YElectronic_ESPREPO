/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"
#include <string.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

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
/*----------- AT-command helper --------------*/
#define AT_RX_TIMEOUT 1000
char cmdBuf[64];

HAL_StatusTypeDef ESP_SendCmd(const char *cmd, uint32_t timeout)
{
	   // 1) Flush any old data out of the ESP32 Rx FIFO
	    __HAL_UART_FLUSH_DRREGISTER(&huart6);

	    // 2) Echo the exact command we’re about to send to the PC (so you can see it)
	    HAL_UART_Transmit(&huart3, (uint8_t*)cmd, strlen(cmd), HAL_MAX_DELAY);

	    // 3) Send the AT command over UART6 to the ESP32
	    HAL_UART_Transmit(&huart6, (uint8_t*)cmd, strlen(cmd), HAL_MAX_DELAY);

	    // 4) Now collect *all* bytes the ESP32 replies with (up to `timeout`) into a buffer
	    uint32_t start = HAL_GetTick();
	    uint8_t  respBuf[256];
	    size_t   rlen = 0;

	    while ((HAL_GetTick() - start) < timeout && rlen < sizeof(respBuf)-1) {
	        uint8_t c;
	        // give each byte up to 20 ms to arrive
	        if (HAL_UART_Receive(&huart6, &c, 1, 20) == HAL_OK) {
	            respBuf[rlen++] = c;
	            start = HAL_GetTick();  // reset our timeout on each new byte
	        }
	    }
	    respBuf[rlen] = '\0';

	    // 5) Dump the *entire* response back to the PC in one go
	    if (rlen) {
	        HAL_UART_Transmit(&huart3, respBuf, rlen, HAL_MAX_DELAY);
	    }
	    return HAL_OK;
}
/*--------------------------------------------*/
uint8_t b;
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
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
  /* Give ESP32 time to boot */

  /* Give ESP32 time to boot ------------------------------------------------*/
     HAL_Delay(1500);

     /* 1) Basic AT test --------------------------------------------------------*/
     ESP_SendCmd("AT\r\n", AT_RX_TIMEOUT);

     /* 2) Reset module to clear previous state --------------------------------*/
     ESP_SendCmd("AT+RST\r\n", 2000);
     HAL_Delay(2000);

     /* 3) Set Wi-Fi to AP+STA mode (3) -----------------------------------------*/
     ESP_SendCmd("AT+CWMODE=3\r\n", AT_RX_TIMEOUT);
     ESP_SendCmd("AT+CWSAP=\"name\",\"123456789\",6,3,4,0\r\n", AT_RX_TIMEOUT);

     /* 5) Enable multiple connections -----------------------------------------*/
     ESP_SendCmd("AT+CIPMUX=1\r\n", AT_RX_TIMEOUT);

     /* 6) Start TCP server on port 5000 ---------------------------------------*/
     ESP_SendCmd("AT+CIPSERVER=1,5000\r\n", AT_RX_TIMEOUT);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  // b) Receive inbound TCP data from ESP32 and parse
	          char recvBuf[128] = {0};
	          size_t rcvLen = 0;
	          uint32_t t0 = HAL_GetTick();
	          while ((HAL_GetTick() - t0) < 1000 && rcvLen < sizeof(recvBuf)-1) {
	              uint8_t ch;
	              if (HAL_UART_Receive(&huart6, &ch, 1, 50) == HAL_OK) {
	                  HAL_UART_Transmit(&huart3, &ch, 1, HAL_MAX_DELAY);
	                  recvBuf[rcvLen++] = ch;
	                  t0 = HAL_GetTick();
	              }
	          }
	          recvBuf[rcvLen] = '\0';

	          // c) If specific letter or keyword found, send a response
	          if (strstr(recvBuf, "PING") != NULL) {
	              const char *resp = "PONG from STM32!\r\n";
	              sprintf(cmdBuf, "AT+CIPSEND=0,%u\r\n", (unsigned)strlen(resp));
	              ESP_SendCmd(cmdBuf, AT_RX_TIMEOUT);
	              HAL_UART_Transmit(&huart6, (uint8_t*)resp, strlen(resp), HAL_MAX_DELAY);
	          }

	          HAL_Delay(500);
    /* USER CODE BEGIN 3 */
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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
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
