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
CoordinateNode *head = NULL;
packetData transmitData;
Coordinate Coor;
Data_Buffer SaveData;
/* USER CODE END PTD */

/* UART TX BEGIN */
void prepare_data(void) {
    snprintf(transmitData.voltage_data, sizeof(transmitData.voltage_data), "%.2f", LCD_adc.voltage);
    snprintf(transmitData.current_data, sizeof(transmitData.current_data), "%.2f", LCD_adc.current);
    snprintf(transmitData.temperature_data, sizeof(transmitData.temperature_data), "%.2f", LCD_adc.temp);
    snprintf(transmitData.power_data, sizeof(transmitData.power_data), "%.2f", LCD_adc.power);
}

void send_uart_data(void) {
    prepare_data();
    int len = snprintf((char *)SaveData.buffer, sizeof(SaveData.buffer),
                       "{\"voltage\":%s,\"current\":%s,\"temperature\":%s,\"power\":%s}\n",
                       transmitData.voltage_data,
                       transmitData.current_data,
                       transmitData.temperature_data,
                       transmitData.power_data);
    HAL_UART_Transmit_IT(&huart2, (uint8_t *)SaveData.buffer, len);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        // Do nothing, the timer will call send_uart_data to send data
    }
}

void UART_transmit_init(void) {
    send_uart_data();
}

/* UART TX END */

/* UART RX BEGIN */
void UART_RECEIVE_Init(void) {
    HAL_UART_Receive_DMA(&huart2, SaveData.rxBuffer, 1);  // Nhận từng byte một
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
    CNC_pos.max_speedXY = 50000;
    CNC_pos.max_speedZ = 50000;
    CNC_pos.a_maxX = 5000;
    CNC_pos.j_maxX = 5000;
    CNC_pos.a_maxY = 5000;
    CNC_pos.j_maxY = 5000;
    CNC_pos.a_maxZ = 2000;
    CNC_pos.j_maxZ = 1000;
}

void medium_command(void) {
    CNC_pos.max_speedXY = 100000;
    CNC_pos.max_speedZ = 100000;
    CNC_pos.a_maxX = 10000;
    CNC_pos.j_maxX = 7000;
    CNC_pos.a_maxY = 10000;
    CNC_pos.j_maxY = 7000;
    CNC_pos.a_maxZ = 5000;
    CNC_pos.j_maxZ = 2000;
}

void high_command(void) {
    CNC_pos.max_speedXY = 150000;
    CNC_pos.max_speedZ = 150000;
    CNC_pos.a_maxX = 100000;
    CNC_pos.j_maxX = 10000;
    CNC_pos.a_maxY = 100000;
    CNC_pos.j_maxY = 10000;
    CNC_pos.a_maxZ = 10000;
    CNC_pos.j_maxZ = 6000;
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
    	Coor.coordinate_X = current->x;
    	Coor.coordinate_Y = current->y;
    	Coor.coordinate_Z = current->z;
        MoveToPosXY(Coor.coordinate_X, Coor.coordinate_Y);
        MoveToPosZ(Coor.coordinate_Z);
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
    if (SaveData.cmdstate) {
    	SaveData.cmdstate = 0;

        if (strcmp(SaveData.cmd, "START") == 0) {
            start_command();
        } else if (strcmp(SaveData.cmd, "STOP") == 0) {
            stop_command();
        } else if (strcmp(SaveData.cmd, "RESET") == 0) {
            reset_command();
        } else if (strcmp(SaveData.cmd, "ON") == 0) {
            drill_on_command();
        } else if (strcmp(SaveData.cmd, "OFF") == 0) {
            drill_off_command();
        } else if (strcmp(SaveData.cmd, "LOW") == 0) {
            low_command();
        } else if (strcmp(SaveData.cmd, "MEDIUM") == 0) {
            medium_command();
        } else if (strcmp(SaveData.cmd, "HIGH") == 0) {
            high_command();
        } else if (strncmp(SaveData.cmd, "GOTO", 4) == 0) {
            process_goto_command(SaveData.cmd);
        } else {
        	process_ip_address(SaveData.cmd);
        }
    }
}

void process_ip_address(char *ip_address) {
    // Store the received IP address
    strncpy(SaveData.ip_config, ip_address, sizeof(SaveData.ip_config) - 1);
    SaveData.ip_config[sizeof(SaveData.ip_config) - 1] = '\0';  // Ensure null termination
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    static uint8_t index = 0;

    if (huart->Instance == USART2) {
        if (SaveData.rxBuffer[0] != '\r' && SaveData.rxBuffer[0] != '\n') {
            if (index < sizeof(SaveData.cmd) - 1) {
            	SaveData.cmd[index++] = SaveData.rxBuffer[0];
            }
        } else if (SaveData.rxBuffer[0] == '\r') {
            if (index > 0) {
            	SaveData.cmd[index] = '\0';
                index = 0;
                SaveData.cmdstate = 1;
                osSemaphoreRelease(uartRxSemaphoreHandle);
            }
        }
        HAL_UART_Receive_DMA(&huart2, SaveData.rxBuffer, 1);
    }
}
/* UART RX BEGIN */
