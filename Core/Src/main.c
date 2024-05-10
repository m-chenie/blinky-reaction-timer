/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "tim.h"
#include "gpio.h"
#include <stdlib.h>

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

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

uint8_t start = 0;
uint8_t led_state = 0;
uint8_t reaction_press = 0;

void start_reaction_timer(){

	uint8_t newPeriod = rand() % 1000; // generate random number between 0 and 255
	__HAL_TIM_SET_AUTORELOAD(&htim11, newPeriod);
	__HAL_TIM_SET_COUNTER(&htim11, 0);

	// turn on led and start timer
	HAL_GPIO_TogglePin(greenled_GPIO_Port, greenled_Pin); // turn on led
	led_state = HAL_GPIO_ReadPin(greenled_GPIO_Port, greenled_Pin);
	HAL_TIM_Base_Start(&htim11);

}


int main(void)
{

  HAL_Init();
  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM11_Init();

  /* Infinite loop */
  /* USER CODE BEGIN WHILE *
   */
//  HAL_TIM_Base_Start_IT(&htim11);
  while (1)
  {
	  if (reaction_press && start){
		  HAL_TIM_Base_Stop(&htim11);
			uint8_t reaction_time = __HAL_TIM_GetCounter(&htim11);
			display_reaction_time(reaction_time);
			start = 0;
			reaction_press = 0;
			HAL_GPIO_WritePin(greenled_GPIO_Port, greenled_Pin, 0);
	  }

  }
}

void display_reaction_time(uint8_t reaction_time){
	for (int i = 7; i >= 0; --i){ // start with most significant bit 2^7

		// (1 << i): This is a bitwise left shift operation.
		// Takes binary representation of the number 1 and shifts it to the left by i positions.
		// value & (1 << i) is a bitwise AND operation between the value variable and the result of the left shift operation.
		// It checks if the i-th bit of value is set (1) or not (0). Returns 0 if false, non-zero if true
		if (reaction_time & (1 << i)){
			// if bit is 1
			HAL_GPIO_WritePin(greenled_GPIO_Port, greenled_Pin, 1);
			HAL_Delay(500);
		} else{
			HAL_GPIO_WritePin(greenled_GPIO_Port, greenled_Pin, 0);
			HAL_Delay(500);
		}
		HAL_GPIO_TogglePin(greenled_GPIO_Port, greenled_Pin);
		HAL_Delay(100);
	}
}

// button interrupt
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

	// if button is pressed and start
	if (GPIO_Pin == bluebtn_Pin && start){
		reaction_press = 1;

//		HAL_GPIO_WritePin(greenled_GPIO_Port, greenled_Pin, 0);
	}else{
		// we set state of game to start
		start = 1;
		start_reaction_timer();
	}
}

// timer has rolled over
// TIMER INTERRUPT
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  // Check which version of the timer triggered this callback and toggle LED
  // also check that we are not in middle of game
//  if (htim == &htim11)
//  {
//	  // display reaction time
//  }
//	HAL_GPIO_WritePin(greenled_GPIO_Port, greenled_Pin, 0);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 64;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
