/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* USER CODE BEGIN Private defines */
//Pin and port of driver controll Motor
#define step_1_pin GPIO_PIN_0
#define step_1_GPIO_Port GPIOA
#define dir_1_pin GPIO_PIN_1
#define dir_1_GPIO_Port GPIOA
#define step_2_pin GPIO_PIN_2
#define step_2_GPIO_Port GPIOA
#define dir_2_pin GPIO_PIN_3
#define dir_2_GPIO_Port GPIOA
#define step_3_pin GPIO_PIN_4
#define step_3_GPIO_Port GPIOA
#define dir_3_pin GPIO_PIN_5
#define dir_3_GPIO_Port GPIOA
//Button for Begin
#define start_pin GPIO_PIN_7
#define start_port GPIOA
#define stop_pin GPIO_PIN_9
#define stop_port GPIOE
#define backKey GPIO_PIN_6
#define back_port GPIOA
#define downKey GPIO_PIN_4
#define down_port GPIOC
#define upKey GPIO_PIN_10
#define up_port GPIOE
#define resetKey GPIO_PIN_11
#define reset_port GPIOE
#define selectKey GPIO_PIN_12
#define select_port GPIOE
//switch for Move x - y - z
#define moveXplus_pin GPIO_PIN_5
#define moveXplus_port GPIOC
#define moveXsub_pin GPIO_PIN_0
#define moveXsub_port GPIOB
#define moveYplus_pin GPIO_PIN_2
#define moveYplus_port GPIOB
#define moveYsub_pin GPIO_PIN_1
#define moveYsub_port GPIOB
#define moveZplus_pin GPIO_PIN_7
#define moveZplus_port GPIOE
#define moveZsub_pin GPIO_PIN_8
#define moveZsub_port GPIOE
//Output: Led, Drill
 #define led1_pin GPIO_PIN_13
 #define led1_port GPIOE
 #define led2_pin GPIO_PIN_15
 #define led2_port GPIOE
 #define led3_pin GPIO_PIN_11
 #define led3_port GPIOB
#define drill_pin GPIO_PIN_13
#define drill_port GPIOB

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
