/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <string.h>
#include "task.h"
#include "ds1307.h"
#include "SH1106.h"
#include "tm_stm32f4_mfrc522.h"

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
char rx_buf[100];
uint8_t rx_idx = 0;
uint8_t rx_data;

// Flag flags
volatile uint8_t cmd_received = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint8_t hex2int(char c);
void ProcessCommand(char* cmd);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_I2C3_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  MX_SPI4_Init();
  /* USER CODE BEGIN 2 */
  Task_Init();

  // Start UART Receive Interrupt
  HAL_UART_Receive_IT(&huart1, &rx_data, 1);
  /* USER CODE END 2 */
  DS1307_Init();
  SH1106_Init();
  TM_MFRC522_Init();

  char buffer[30];
  // DS1307_TimeTypeDef setTime = { .sec = 10, .min = 15, .hour = 14, .day = 7, .month = 5, .year = 26, .date = 9};
  // DS1307_TimeTypeDef getTime;
  // SetTime(&setTime);

  uint8_t cardId[5];
  sprintf(buffer, "Hello");
  SH1106_GotoXY(12, 10);
  SH1106_Puts(buffer, &Font_11x18, 1);
  SH1106_UpdateScreen();

  // SH1106_Init();
  // DS1307_Init();
  // TM_MFRC522_Init();

  // char buffer[30];
  // uint8_t cardId[5];

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // HAL_Delay(200);
    Task_Run();
    // GetTime(&getTime);
    // sprintf(buffer, "20%02d/%02d/%02d %02d:%02d:%02d\r\n", getTime.year, getTime.month, getTime.date, getTime.hour, getTime.min, getTime.sec);
    // sprintf(buffer, "20%02d/%02d/%02d ", getTime.year, getTime.month, getTime.date);
    // HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);

    // SH1106_GotoXY(5, 0);
    // SH1106_Puts(buffer, &Font_11x18, 1);

    // sprintf(buffer, "%02d:%02d:%02d", getTime.hour, getTime.min, getTime.sec);
    // SH1106_GotoXY(5, 20);
    // SH1106_Puts(buffer, &Font_11x18, 1);

    // SH1106_UpdateScreen();
    // HAL_Delay(1000);

    HAL_Delay(200);
    // HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_RESET);
    // if (TM_MFRC522_Check(cardId) == MI_OK) {
    //   sprintf(buffer, "%x%x%x%x%x", cardId[0], cardId[1], cardId[2], cardId[3], cardId[4]);
    //   HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
    // }
    // else {
    //   sprintf(buffer, "----------");
    //   HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);
    // }

    // HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
    // SH1106_GotoXY(12, 10);
    // SH1106_Puts(buffer, &Font_11x18, 1);
    // SH1106_UpdateScreen();
    /* USER CODE END WHILE */

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
    if (!cmd_received)
    {
      if (rx_data == '\r' || rx_data == '\n')
      {
        rx_buf[rx_idx] = '\0';
        cmd_received = 1;
      }
      else if (rx_idx < sizeof(rx_buf) - 1)
      {
        rx_buf[rx_idx++] = rx_data;
      }
    }

      HAL_UART_Receive_IT(&huart1, &rx_data, 1);
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
#ifdef USE_FULL_ASSERT
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
