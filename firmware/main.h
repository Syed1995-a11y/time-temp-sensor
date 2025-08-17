/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DB7_Pin GPIO_PIN_0
#define DB7_GPIO_Port GPIOC
#define DB6_Pin GPIO_PIN_1
#define DB6_GPIO_Port GPIOC
#define RS_Pin GPIO_PIN_0
#define RS_GPIO_Port GPIOA
#define E_Pin GPIO_PIN_1
#define E_GPIO_Port GPIOA
#define DB4_Pin GPIO_PIN_4
#define DB4_GPIO_Port GPIOA
#define ROW1_Pin GPIO_PIN_6
#define ROW1_GPIO_Port GPIOA
#define ROW2_Pin GPIO_PIN_7
#define ROW2_GPIO_Port GPIOA
#define DB5_Pin GPIO_PIN_0
#define DB5_GPIO_Port GPIOB
#define COL3_Pin GPIO_PIN_10
#define COL3_GPIO_Port GPIOB
#define ROW4_Pin GPIO_PIN_7
#define ROW4_GPIO_Port GPIOC
#define COL2_Pin GPIO_PIN_8
#define COL2_GPIO_Port GPIOA
#define COL1_Pin GPIO_PIN_9
#define COL1_GPIO_Port GPIOA
#define COL4_Pin GPIO_PIN_4
#define COL4_GPIO_Port GPIOB
#define ROW3_Pin GPIO_PIN_6
#define ROW3_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
