/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : lcd.c
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
/*
 * lcd.c
 *
 *  Created on: Jan 21, 2024
 *  Author: Nguyen Ngoc Quy (Wis)
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "lcd.h"
#include <stdbool.h>
/* USER CODE BEGIN 0 */
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "adc.h"
#include "i2c.h"
#include "AccelStepper.h"
#include "lcd.h"
#include "menulcd.h"
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure LCD                                                            */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

extern I2C_HandleTypeDef hi2c1;  // Change your handler here accordingly
#define SLAVE_ADDRESS_LCD 0x4E    // Change this according to your setup
extern osMutexId_t lcdMutexHandle;
extern osSemaphoreId_t uartRxSemaphoreHandle;

void lcd_send_cmd(char cmd) {
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (cmd & 0xF0);
    data_l = ((cmd << 4) & 0xF0);
    data_t[0] = data_u | 0x0C;  // EN=1, RS=0
    data_t[1] = data_u | 0x08;  // EN=0, RS=0
    data_t[2] = data_l | 0x0C;  // EN=1, RS=0
    data_t[3] = data_l | 0x08;  // EN=0, RS=0
    HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, (uint8_t *)data_t, 4, 100);
}

void lcd_send_data(char data) {
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (data & 0xF0);
    data_l = ((data << 4) & 0xF0);
    data_t[0] = data_u | 0x0D;  // EN=1, RS=1
    data_t[1] = data_u | 0x09;  // EN=0, RS=1
    data_t[2] = data_l | 0x0D;  // EN=1, RS=1
    data_t[3] = data_l | 0x09;  // EN=0, RS=1
    HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, (uint8_t *)data_t, 4, 100);
}

void lcd_clear(void) {
    osMutexAcquire(lcdMutexHandle, osWaitForever);
    lcd_send_cmd(0x01);  // Clear display
    HAL_Delay(2);        // Delay for clearing
    osMutexRelease(lcdMutexHandle);
}

void lcd_put_cur(int row, int col) {
    int row_offsets[] = {0x00, 0x40, 0x14, 0x54}; // Line offsets for 20x04 LCD
    lcd_send_cmd(0x80 | (col + row_offsets[row])); // Set DDRAM address
}

void lcd_init(void) {
    HAL_Delay(50);        // Wait for >40ms
    lcd_send_cmd(0x33);   // Initialization sequence for 4-bit mode
    HAL_Delay(5);         // Wait for >4.1ms
    lcd_send_cmd(0x32);   // Initialization sequence for 4-bit mode
    HAL_Delay(1);         // Wait for >100us

    // Display initialization
    lcd_send_cmd(0x28);   // Function set: DL=0 (4-bit mode), N=2 (2 lines), F=0 (5x8 dots)
    HAL_Delay(1);
    lcd_send_cmd(0x08);   // Display on/off control: D=0, C=0, B=0 (Display off)
    HAL_Delay(1);
    lcd_send_cmd(0x01);   // Clear display
    HAL_Delay(2);
    lcd_send_cmd(0x06);   // Entry mode set: I/D=1 (Increment), S=0 (No shift)
    HAL_Delay(1);
    lcd_send_cmd(0x0C);   // Display on/off control: D=1, C=0, B=0 (Display on, cursor off, blink off)
}

void lcd_send_string(char *str) {
    osMutexAcquire(lcdMutexHandle, osWaitForever);
    while (*str) {
        lcd_send_data(*str++);
    }
    osMutexRelease(lcdMutexHandle);
}
