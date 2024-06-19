/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    cli.h
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  * Created on: Jan 21, 2024
  * Author: Nguyen Ngoc Quy (Wis)
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/*
 * cli.h
 *
 *  Created on: Jan 21, 2024
 *  Author: Nguyen Ngoc Quy (Wis)
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/

#ifndef INC_CLI_H_
#define INC_CLI_H_

#include "usart.h"

/* Config fucntion */
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern osMutexId_t lcdMutexHandle;
extern osSemaphoreId_t uartRxSemaphoreHandle;
extern UART_HandleTypeDef huart2;

typedef struct {
    char voltage_data[20];
    char current_data[20];
    char temperature_data[20];
    char power_data[20];
} packetData;
extern packetData transmitData;

typedef struct CoordinateNode {
    float x, y, z;
    struct CoordinateNode *next;
} CoordinateNode;
extern CoordinateNode *head;

typedef struct {
	float coordinate_X;
	float coordinate_Y;
	float coordinate_Z;
}Coordinate;
extern Coordinate Coor;
typedef struct {
	uint8_t buffer[100];
	uint8_t rxBuffer[256];
	char cmd[256];
	char ip_config[20];
	uint8_t cmdstate;
}Data_Buffer;
extern Data_Buffer SaveData;
/* USER CODE END PTD */

void prepare_data(void);
void send_uart_data(void);
void UART_transmit_init(void);

/* Config fucntion */
void UART_RECEIVE_Init(void);
void start_command(void);
void stop_command(void);
void reset_command(void);
void drill_on_command(void);
void drill_off_command(void);
void low_command(void);
void medium_command(void);
void high_command(void);
void add_coordinate(float x, float y, float z);
void clear_coordinates(void);
void move_to_coordinates(void);
void process_goto_command(char *cmd);
void process_ip_address(char *ip_address);
void UART_rx_process(void);

/* Callback UART */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif /* INC_CLI_H_ */
