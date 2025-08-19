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
#include "stm32g4xx_hal.h"
#include "stm32g4xx_nucleo.h"
#include <stdio.h>

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
#define RCC_OSC32_IN_Pin GPIO_PIN_14
#define RCC_OSC32_IN_GPIO_Port GPIOC
#define RCC_OSC32_OUT_Pin GPIO_PIN_15
#define RCC_OSC32_OUT_GPIO_Port GPIOC


#define RCC_OSC_IN_Pin GPIO_PIN_0
#define RCC_OSC_IN_GPIO_Port GPIOF
#define RCC_OSC_OUT_Pin GPIO_PIN_1
#define RCC_OSC_OUT_GPIO_Port GPIOF


#define BMS_SPI1_CS_Pin GPIO_PIN_12
#define BMS_SPI1_CS_GPIO_Port GPIOB
#define BMS_SPI1_SCK_Pin GPIO_PIN_13
#define BMS_SPI1_SCK_GPIO_Port GPIOB
#define BMS_SPI1_MISO_Pin GPIO_PIN_14
#define BMS_SPI1_MISO_GPIO_Port GPIOB
#define BMS_SPI1_MOSI_Pin GPIO_PIN_15
#define BMS_SPI1_MOSI_GPIO_Port GPIOB


#define BMS_INTR1_Pin GPIO_PIN_7
#define BMS_INTR1_GPIO_Port GPIOC
#define BMS_WAKE1_Pin GPIO_PIN_8
#define BMS_WAKE1_GPIO_Port GPIOC


#define CAN_RX_Pin GPIO_PIN_11
#define CAN_RX_GPIO_Port GPIOA
#define CAN_TX_Pin GPIO_PIN_12
#define CAN_TX_GPIO_Port GPIOA


#define T_SWDIO_Pin GPIO_PIN_13
#define T_SWDIO_GPIO_Port GPIOA
#define T_SWCLK_Pin GPIO_PIN_14
#define T_SWCLK_GPIO_Port GPIOA


#define BMS_SPI2_NSS_Pin GPIO_PIN_15
#define BMS_SPI2_NSS_GPIO_Port GPIOA
#define BMS_SPI2_SCK_Pin GPIO_PIN_10
#define BMS_SPI2_SCK_GPIO_Port GPIOC
#define BMS_SPI2_MISO_Pin GPIO_PIN_11
#define BMS_SPI2_MISO_GPIO_Port GPIOC
#define BMS_SPI2_MOSI_Pin GPIO_PIN_12
#define BMS_SPI2_MOSI_GPIO_Port GPIOC


#define T_SWO_Pin GPIO_PIN_3
#define T_SWO_GPIO_Port GPIOB


#define BMS_WAKE2_Pin GPIO_PIN_5
#define BMS_WAKE2_GPIO_Port GPIOB
#define BMS_INTR2_Pin GPIO_PIN_6
#define BMS_INTR2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
