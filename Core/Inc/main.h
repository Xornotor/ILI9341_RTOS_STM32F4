/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define TFT_DC_Pin GPIO_PIN_2
#define TFT_DC_GPIO_Port GPIOA
#define TFT_CS_Pin GPIO_PIN_3
#define TFT_CS_GPIO_Port GPIOA
#define TFT_RST_Pin GPIO_PIN_4
#define TFT_RST_GPIO_Port GPIOA
#define TFT_CLK_Pin GPIO_PIN_5
#define TFT_CLK_GPIO_Port GPIOA
#define TFT_MISO_Pin GPIO_PIN_6
#define TFT_MISO_GPIO_Port GPIOA
#define TFT_MOSI_Pin GPIO_PIN_7
#define TFT_MOSI_GPIO_Port GPIOA
#define BTN_3_Pin GPIO_PIN_11
#define BTN_3_GPIO_Port GPIOA
#define BTN_3_EXTI_IRQn EXTI15_10_IRQn
#define BTN_2_Pin GPIO_PIN_12
#define BTN_2_GPIO_Port GPIOA
#define BTN_2_EXTI_IRQn EXTI15_10_IRQn
#define BTN_1_Pin GPIO_PIN_15
#define BTN_1_GPIO_Port GPIOA
#define BTN_1_EXTI_IRQn EXTI15_10_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
