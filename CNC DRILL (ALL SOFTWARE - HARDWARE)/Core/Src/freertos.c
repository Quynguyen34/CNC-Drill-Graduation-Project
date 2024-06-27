/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  * Created on: Jan 21, 2024
  * Author: Nguyen Ngoc Quy (Wis)
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdbool.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "adc.h"
#include "i2c.h"
#include "AccelStepper.h"
#include "Inverse_cnc.h"
#include "lcd.h"
#include "menulcd.h"
#include "cli.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* Definitions for defaultTask */
osMutexId_t lcdMutexHandle;
const osMutexAttr_t lcdMutex_attributes = {
  .name = "lcdMutex"
};
osSemaphoreId_t uartRxSemaphoreHandle;
const osSemaphoreAttr_t uartRxSemaphore_attributes = {
  .name = "uartRxSemaphore"
};

osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for startADC */
osThreadId_t startADCHandle;
const osThreadAttr_t startADC_attributes = {
  .name = "startADC",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for startLCD */
osThreadId_t startLCDHandle;
const osThreadAttr_t startLCD_attributes = {
  .name = "startLCD",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for startUART_TX */
osThreadId_t startUART_TXHandle;
const osThreadAttr_t startUART_TX_attributes = {
  .name = "startUART_TX",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for startUART_RX */
osThreadId_t startUART_RXHandle;
const osThreadAttr_t startUART_RX_attributes = {
  .name = "startUART_RX",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
CNC_pos_t CNC_pos;
Inv_CNC_t CNC;

osThreadId_t defaultTaskHandle;
osThreadId_t startADCHandle;
osThreadId_t startLCDHandle;
osThreadId_t startUARTHandle;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
#define max(a,b) (a > b) ? a : b;
#define min(a,b) (a > b) ? b : a;
/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/*----------------------------------*/
/* Config speed for stepper */
double_t max3(double_t a, double_t b, double_t c) {
	double_t n1 = max(a, b);
    return max(n1, c);
}
/* Function set home */
void DelayUs_step(uint32_t us)
{
	HAL_TIM_Base_Start_IT(&htim1);
	//(&htim7)->Instance->CNT = (0);
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while(__HAL_TIM_GET_COUNTER(&htim1) < us);
	HAL_TIM_Base_Stop_IT(&htim1);
}

void HOME(void)
{
    // Home Z axis
    HAL_GPIO_WritePin(dir_3_GPIO_Port, dir_3_pin, GPIO_PIN_RESET); // Set direction to move towards home
    while (HAL_GPIO_ReadPin(moveZsub_port, moveZsub_pin) != CNC_pos.Lsw6) {
        HAL_GPIO_TogglePin(step_3_GPIO_Port, step_3_pin); // Toggle step pin to move towards home
        DelayUs_step(30); // Adjust delay as needed
    }
    // Home X axis
    HAL_GPIO_WritePin(dir_1_GPIO_Port, dir_1_pin, GPIO_PIN_RESET); // Set direction to move towards home
    bool isXHome = false;

    // Home Y axis
    HAL_GPIO_WritePin(dir_2_GPIO_Port, dir_2_pin, GPIO_PIN_RESET); // Set direction to move towards home
    bool isYHome = false;

    while (!(isXHome && isYHome)) {
        if (!isXHome && (HAL_GPIO_ReadPin(moveXsub_port, moveXsub_pin) != CNC_pos.Lsw2)) {
            HAL_GPIO_TogglePin(step_1_GPIO_Port, step_1_pin); // Toggle step pin to move towards home
        } else {
            isXHome = true;
        }

        if (!isYHome && (HAL_GPIO_ReadPin(moveYsub_port, moveYsub_pin) != CNC_pos.Lsw4)) {
            HAL_GPIO_TogglePin(step_2_GPIO_Port, step_2_pin); // Toggle step pin to move towards home
        } else {
            isYHome = true;
        }
        DelayUs_step(30); // Adjust delay as needed
    }
}
/* Function control 3 axis */
//Move X-Y
void MoveToPosXY(float x, float y) {
    trans_to_posXY(x,y);
    CNC_pos.MoveX = caculate_pos(CNC.set_posX, 161);
    CNC_pos.MoveY = caculate_pos(CNC.set_posY, 161);
    long long int step_max = max3(llabs(CNC_pos.MoveX), llabs(CNC_pos.MoveY), llabs(CNC_pos.MoveZ));
    double_t coef1 = fabs(CNC_pos.MoveX) / step_max;
    double_t coef2 = fabs(CNC_pos.MoveY) / step_max;
    CNC_pos.pos1dot = CNC_pos.max_speedXY * coef1;
    CNC_pos.pos2dot = CNC_pos.max_speedXY * coef2;
    CNC_pos.accel1 = CNC_pos.a_maxX * coef1;
    CNC_pos.accel2 = CNC_pos.a_maxY * coef2;
    CNC_pos.jerk1 = CNC_pos.j_maxX * coef1;
    CNC_pos.jerk2 = CNC_pos.j_maxY * coef2;
    if (Stepper1.run_state != 1 && Stepper2.run_state != 1) {
        Accel_Stepper_Move(&Stepper1, CNC_pos.MoveX, CNC_pos.accel1, CNC_pos.jerk1, CNC_pos.pos1dot);
        Accel_Stepper_Move(&Stepper2, CNC_pos.MoveY, CNC_pos.accel2, CNC_pos.jerk2, CNC_pos.pos2dot);
        while (Stepper1.run_state != STOP || Stepper2.run_state != STOP) {
        	if(HAL_GPIO_ReadPin(moveXplus_port, moveXplus_pin) == CNC_pos.Lsw1 ||
        		HAL_GPIO_ReadPin(moveYplus_port, moveYplus_pin) == CNC_pos.Lsw3){
        		handle_stop_button_press();
        	}
        	osDelay(1);
        }
        CNC.pos_x = x;
        CNC.pos_y = y;
    }
}
// Move Z
void MoveToPosZ(float z) {
    trans_to_posZ(z);
    CNC_pos.MoveZ = caculate_pos(CNC.set_posZ, 161);
    long long int step_max = max3(llabs(CNC_pos.MoveX), llabs(CNC_pos.MoveY), llabs(CNC_pos.MoveZ));
    double_t coef3 = fabs(CNC_pos.MoveZ) / step_max;
    CNC_pos.pos3dot = CNC_pos.max_speedZ * coef3;
    CNC_pos.accel3 = CNC_pos.a_maxZ * coef3;
    CNC_pos.jerk3 = CNC_pos.j_maxZ * coef3;
    if (Stepper3.run_state != 1) {
        Accel_Stepper_Move(&Stepper3, CNC_pos.MoveZ, CNC_pos.accel3, CNC_pos.jerk3, CNC_pos.pos3dot);
        while(Stepper3.run_state != STOP){
        	if(HAL_GPIO_ReadPin(moveZplus_port, moveZplus_pin) ==  CNC_pos.Lsw5){
        		handle_stop_button_press();
        	}
			osDelay(1);
		}
        CNC.pos_z = z;
    }
}


// void SmoothMoveCircularPath(float centerX, float centerY, float radius) {
//     const int NUM_POINTS = 100; // �?i�?u chỉnh tùy theo độ mịn của đư�?ng tròn
//     const float angleIncrement = 2 * M_PI / NUM_POINTS;
//     float angle = 0;

//     // Tính toán và di chuyển theo đư�?ng tròn
//     for (int i = 0; i < NUM_POINTS; i++) {
//         int x = (int)(centerX + radius * cos(angle));
//         int y = (int)(centerY + radius * sin(angle));
//         MoveToPosXY(x, y);
//         angle += angleIncrement;
//     }
// }


/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  HOME();
  initializeCNC_pos(&CNC_pos);
  Accel_Stepper_SetPin(&Stepper1, step_1_GPIO_Port, step_1_pin, dir_1_GPIO_Port, dir_1_pin);
  Accel_Stepper_SetPin(&Stepper2, step_2_GPIO_Port, step_2_pin, dir_2_GPIO_Port, dir_2_pin);
  Accel_Stepper_SetPin(&Stepper3, step_3_GPIO_Port, step_3_pin, dir_3_GPIO_Port, dir_3_pin);

  Accel_Stepper_SetTimer(&Stepper1, &htim2);
  Accel_Stepper_SetTimer(&Stepper2, &htim3);
  Accel_Stepper_SetTimer(&Stepper3, &htim4);

  CNC.pos_x = 0;
  CNC.pos_y = 0;
  CNC.pos_z = 0;

  //vTaskDelay(pdMS_TO_TICKS(2000));
  /* Infinite loop */
  for(;;)
  {
      if (state.start_press) {
    	  HAL_GPIO_WritePin(drill_port, drill_pin, 1);
          move_to_coordinates();
      }

    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartADC */
/**
* @brief Function implementing the startADC thread.
* @param argument: Not used
* @retval None
*/

/* USER CODE END Header_StartADC */
void StartADC(void *argument)
{
  /* USER CODE BEGIN StartADC */
  /* Infinite loop */
  //HAL_ADC_Start_DMA(&hadc1, (uint32_t*)LCD_adc.readValue, 2);
  for(;;)
  {
	startADC();
    osDelay(1);
  }
  /* USER CODE END StartADC */
}

/* USER CODE BEGIN Header_StartLCD */
/**
* @brief Function implementing the startLCD thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLCD */
void StartLCD(void *argument)
{
  /* USER CODE BEGIN StartLCD */
  /* Infinite loop */
  initialize_LCD(&LCD_adc);
  initialize_Kalman(&kalman_fil_curr);
  initialize_Kalman(&kalman_fil_volt);
  display_menu();
  for(;;)
  {
	ButtonTask();
    osDelay(1);
  }
  /* USER CODE END StartLCD */
}

/* USER CODE BEGIN Header_StartUART_TX */
/**
* @brief Function implementing the startUART_TX thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUART_TX */
void StartUART_TX(void *argument)
{
  /* USER CODE BEGIN StartUART_TX */
  /* Infinite loop */
  UART_transmit_init();
  for(;;)
  {
    osDelay(1000);  // Delay 1000 milliseconds (1 second)
    send_uart_data();  // Send data every second
  }
  /* USER CODE END StartUART_TX */
}

/* USER CODE BEGIN Header_StartUART_RX */
/**
* @brief Function implementing the startUART_RX thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUART_RX */

void StartUART_RX(void *argument) {
    /* USER CODE BEGIN StartUART_RX */
    /* Initialize UART receive in DMA mode */
    UART_RECEIVE_Init();
    /* Infinite loop */
    for(;;)
    {
        if (osSemaphoreAcquire(uartRxSemaphoreHandle, osWaitForever) == osOK) {
            UART_rx_process();
        }
        osDelay(1);
    }
    /* USER CODE END StartUART_RX */
}
/* USER CODE END 0 */

/* USER CODE END FunctionPrototypes */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  lcdMutexHandle = osMutexNew(&lcdMutex_attributes);
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  uartRxSemaphoreHandle = osSemaphoreNew(1, 1, &uartRxSemaphore_attributes);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of startADC */
  startADCHandle = osThreadNew(StartADC, NULL, &startADC_attributes);

  /* creation of startLCD */
  startLCDHandle = osThreadNew(StartLCD, NULL, &startLCD_attributes);

  /* creation of startUART_TX */
  startUART_TXHandle = osThreadNew(StartUART_TX, NULL, &startUART_TX_attributes);

  /* creation of startUART_RX */
  startUART_RXHandle = osThreadNew(StartUART_RX, NULL, &startUART_RX_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE END Application */
