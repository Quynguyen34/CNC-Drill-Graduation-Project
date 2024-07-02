/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    cli.c
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
 * cli.c
 *
 *  Created on: Jan 21, 2024
 *  Author: Nguyen Ngoc Quy (Wis)
 */
/* USER CODE END Header */
/* USER CODE BEGIN Includes */
/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
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
typedef struct {
    char voltage_data[20];
    char current_data[20];
    char temperature_data[20];
    char power_data[20];
} packetData;
packetData transmitData;
typedef struct CoordinateNode {
    float x, y, z;
    struct CoordinateNode *next;
} CoordinateNode;
CoordinateNode *head = NULL;
uint8_t buffer[100];
uint8_t rxBuffer[256];
char cmd[256];
char ip_config[20];
uint8_t cmdstate;
float coordinate_X;
float coordinate_Y;
float coordinate_Z;
/* USER CODE END PTD */
extern osMutexId_t lcdMutexHandle;
extern osSemaphoreId_t uartRxSemaphoreHandle;
extern UART_HandleTypeDef huart2;
/* UART TX BEGIN */
void prepare_data(void) {
    snprintf(transmitData.voltage_data, sizeof(transmitData.voltage_data), "%.2f", LCD_adc.voltage);
    snprintf(transmitData.current_data, sizeof(transmitData.current_data), "%.2f", LCD_adc.current);
    snprintf(transmitData.temperature_data, sizeof(transmitData.temperature_data), "%.2f", LCD_adc.Temp);
    snprintf(transmitData.power_data, sizeof(transmitData.power_data), "%.2f", LCD_adc.power);
}


void UART_transmit_init(void) {
    send_uart_data();
}

void send_uart_data(void) {
    prepare_data();
    int len = snprintf((char *)buffer, sizeof(buffer),
                       "V:%s,C:%s,T:%s,P:%s\n",
                       transmitData.voltage_data,
                       transmitData.current_data,
                       transmitData.temperature_data,
                       transmitData.power_data);
    HAL_UART_Transmit_IT(&huart2, (uint8_t *)buffer, len);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        // Do nothing, the timer will call send_uart_data to send data
    }
}

/* UART TX END */

/* UART RX BEGIN */
void UART_RECEIVE_Init(void) {
    HAL_UART_Receive_IT(&huart2, rxBuffer, 1);  // Nhận từng byte một
}

void start_command(void) {
	handle_start_button_press();
}

void stop_command(void) {
	handle_stop_button_press();
}

void reset_command(void) {
	handle_reset_button_press();
}

void drill_on_command(void) {
    HAL_GPIO_WritePin(drill_port, drill_pin, GPIO_PIN_SET);
}

void drill_off_command(void) {
    HAL_GPIO_WritePin(drill_port, drill_pin, GPIO_PIN_RESET);
}

void low_command(void) {
    CNC_pos.max_speedXY = 10000;
    CNC_pos.max_speedZ = 5000;
    CNC_pos.a_maxX = 5000;
    CNC_pos.j_maxX = 5000;
    CNC_pos.a_maxY = 5000;
    CNC_pos.j_maxY = 5000;
    CNC_pos.a_maxZ = 500;
    CNC_pos.j_maxZ = 500;
}

void medium_command(void) {
    CNC_pos.max_speedXY = 30000;
    CNC_pos.max_speedZ = 7000;
    CNC_pos.a_maxX = 10000;
    CNC_pos.j_maxX = 7000;
    CNC_pos.a_maxY = 10000;
    CNC_pos.j_maxY = 7000;
    CNC_pos.a_maxZ = 1000;
    CNC_pos.j_maxZ = 1000;
}

void high_command(void) {
    CNC_pos.max_speedXY = 50000;
    CNC_pos.max_speedZ = 10000;
    CNC_pos.a_maxX = 30000;
    CNC_pos.j_maxX = 30000;
    CNC_pos.a_maxY = 20000;
    CNC_pos.j_maxY = 20000;
    CNC_pos.a_maxZ = 2000;
    CNC_pos.j_maxZ = 2000;
}

void add_coordinate(float x, float y, float z) {
    CoordinateNode *newNode = (CoordinateNode*)malloc(sizeof(CoordinateNode));
    newNode->x = x;
    newNode->y = y;
    newNode->z = z;
    newNode->next = NULL;

    if (head == NULL) {
        head = newNode;
    } else {
        CoordinateNode *current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

void clear_coordinates(void) {
    CoordinateNode *current = head;
    CoordinateNode *next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    head = NULL;
}

void move_to_coordinates(void) {
    CoordinateNode *current = head;

    while (current != NULL) {
    	coordinate_X = current->x;
    	coordinate_Y = current->y;
    	coordinate_Z = current->z;
        MoveToPosXY(coordinate_X, coordinate_Y);
        MoveToPosZ(coordinate_Z);
        current = current->next;
    }
}

void process_goto_command(char *cmd) {
    char *line = strtok(cmd, "GOTO");
    clear_coordinates(); // Xóa danh sách tọa độ hiện tại

    while (line != NULL) {
        float x, y, z;
        if (sscanf(line, "%f,%f,%f", &x, &y, &z) == 3) {
            add_coordinate(x, y, z);
        }
        line = strtok(NULL, "GOTO");
    }

    state.start_press = 1; // Đặt cờ để bắt đầu di chuyển đến tọa độ
    state.stop_press = 0;
    state.reset_press = 0;
}

void UART_rx_process(void) {
    if (cmdstate) {
    	cmdstate = 0;

        if (strcmp(cmd, "START") == 0) {
            start_command();
        } else if (strcmp(cmd, "STOP") == 0) {
            stop_command();
        } else if (strcmp(cmd, "RESET") == 0) {
            reset_command();
        } else if (strcmp(cmd, "ON") == 0) {
            drill_on_command();
        } else if (strcmp(cmd, "OFF") == 0) {
            drill_off_command();
        } else if (strcmp(cmd, "LOW") == 0) {
            low_command();
        } else if (strcmp(cmd, "MEDIUM") == 0) {
            medium_command();
        } else if (strcmp(cmd, "HIGH") == 0) {
            high_command();
        } else if (strncmp(cmd, "GOTO", 4) == 0) {
            process_goto_command(cmd);
        } else {
        	process_ip_address(cmd);
        }
    }
}

void process_ip_address(char *ip_address) {
    // Store the received IP address
    strncpy(ip_config, ip_address, sizeof(ip_config) - 1);
    ip_config[sizeof(ip_config) - 1] = '\0';  // Ensure null termination
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    static uint8_t index = 0;

    if (huart->Instance == USART2) {
        if (rxBuffer[0] != '\r' && rxBuffer[0] != '\n') {
            if (index < sizeof(cmd) - 1) {
            	cmd[index++] = rxBuffer[0];
            }
        } else if (rxBuffer[0] == '\r') {
            if (index > 0) {
            	cmd[index] = '\0';
                index = 0;
                cmdstate = 1;
                osSemaphoreRelease(uartRxSemaphoreHandle);
            }
        }
        HAL_UART_Receive_IT(&huart2, rxBuffer, 1);
    }
}
/* UART RX BEGIN */
