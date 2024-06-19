/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : menulcd.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  * Created on: Jan 21, 2024
  * Author: Nguyen Ngoc Quy (Wis)
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
 * menulcd.c
 *
 *  Created on: Jan 21, 2024
 *
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

/* USER CODE BEGIN PM */
LCD_adc_t LCD_adc;
Kalman_filter kalman_fil_curr;
Kalman_filter kalman_fil_volt;
Button button;
State_button state;
/* USER CODE END PM */

/* USER CODE BEGIN 1 */

Button upButton = {upKey, up_port, GPIO_PIN_SET, 0, handle_up_button_press};
Button downButton = {downKey, down_port, GPIO_PIN_SET, 0, handle_down_button_press};
Button backButton = {backKey, back_port, GPIO_PIN_SET, 0, handle_back_button_press};
Button selectButton = {selectKey, select_port, GPIO_PIN_SET, 0, handle_select_button_press};
Button resetButton = {resetKey, reset_port, GPIO_PIN_SET, 0, handle_reset_button_press};
Button startButton = {start_pin, start_port, GPIO_PIN_SET, 0, handle_start_button_press};
Button stopButton = {stop_pin, stop_port, GPIO_PIN_SET, 0, handle_stop_button_press};

Button* buttons[] = {&upButton, &downButton, &backButton, &selectButton, &resetButton, &startButton, &stopButton};
const int numButtons = sizeof(buttons) / sizeof(Button*);

float l,m,k;

void initialize_LCD(LCD_adc_t *lcd)
{
	lcd->sensitivity = 0.066;
	lcd->ACSoffset=2.5;
	lcd->m = 0.4;
	lcd->C = 2;
	lcd->V25 = 0.0025;
	lcd->Avg_Slope = 0.76;
}

void initialize_Kalman(Kalman_filter *kf)
{
    kf->N = 10;
    kf->ema_filtered_value = 0.0f;
    kf->Q = KALMAN_Q;
    kf->R = KALMAN_R;
    kf->Kg = 0.0f;
    kf->P_k_k1 = 1.0f;
    kf->kalman_adc_old = 0.0f;
    kf->index = 0;
    kf->sum = 0;
    for (int i = 0; i < kf->N; ++i) {
        kf->buffer[i] = 0;
    }
}


void delay_lcd(uint16_t delay)
{
	HAL_TIM_Base_Start_IT(&htim6);
	__HAL_TIM_SET_COUNTER(&htim6, 0);
	while(__HAL_TIM_GET_COUNTER(&htim6) < delay);
	HAL_TIM_Base_Stop_IT(&htim6);
}

void stepX(int steps, uint8_t direction, uint16_t delay)
{
    int a;
    HAL_GPIO_WritePin(dir_1_GPIO_Port, dir_1_pin, direction == 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    for (a = 0; a < steps; ++a)
    {
        HAL_GPIO_TogglePin(step_1_GPIO_Port, step_1_pin);
        delay_lcd(delay);
    }
}

void stepY(int steps, uint8_t direction, uint16_t delay)
{
    int b;
    HAL_GPIO_WritePin(dir_2_GPIO_Port, dir_2_pin, direction == 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    for (b = 0; b < steps; ++b)
    {
        HAL_GPIO_TogglePin(step_2_GPIO_Port, step_2_pin);
        delay_lcd(delay);
    }
}

void stepZ(int steps, uint8_t direction, uint16_t delay)
{
    int c;
    HAL_GPIO_WritePin(dir_3_GPIO_Port, dir_3_pin, direction == 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    for (c = 0; c < steps; ++c)
    {
        HAL_GPIO_TogglePin(step_3_GPIO_Port, step_3_pin);
        delay_lcd(delay);
    }
}

uint16_t moving_average_filter(Kalman_filter *kf, uint16_t ADC_Value)
{
    kf->sum -= kf->buffer[kf->index];
    kf->buffer[kf->index] = ADC_Value;
    kf->sum += kf->buffer[kf->index];

    kf->index = (kf->index + 1) % kf->N;

    return (uint16_t)(kf->sum / kf->N);
}

uint16_t exponential_moving_average_filter(Kalman_filter *kf, uint16_t ADC_Value, float alpha)
{
    kf->ema_filtered_value = (alpha * ADC_Value) + ((1 - alpha) * kf->ema_filtered_value);
    return (uint16_t)kf->ema_filtered_value;
}

uint16_t kalman_filter(Kalman_filter *kf, uint16_t ADC_Value)
{
    kf->Z_k = (float)ADC_Value;
    kf->x_k1_k1 = kf->kalman_adc_old;

    kf->x_k_k1 = kf->x_k1_k1;
    kf->P_k_k1 = kf->P_k1_k1 + kf->Q;

    kf->Kg = kf->P_k_k1 / (kf->P_k_k1 + kf->R);

    kf->kalman_adc = kf->x_k_k1 + kf->Kg * (kf->Z_k - kf->kalman_adc_old);
    kf->P_k1_k1 = (1 - kf->Kg) * kf->P_k_k1;

    kf->kalman_adc_old = kf->kalman_adc;

    return (uint16_t)kf->kalman_adc;
}

void vol_messure(void)
{
    ADC_Select_CH10();  
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 1);
    LCD_adc.readValue[0] = HAL_ADC_GetValue(&hadc1);
    uint16_t moving_avg_filtered = moving_average_filter(&kalman_fil_volt, LCD_adc.readValue[0]); // Apply moving average filter
    uint16_t ema_filtered = exponential_moving_average_filter(&kalman_fil_volt, moving_avg_filtered, EMA_ALPHA_VOLT); // Apply EMA filter
    kalman_fil_volt.filter_kal = kalman_filter(&kalman_fil_volt, ema_filtered); // Apply Kalman filter

    LCD_adc.volt = ((float)kalman_fil_volt.filter_kal / 4095) * 3.6f;
    LCD_adc.sum = (LCD_adc.volt * 6.5f) + 0.2f;
    if (LCD_adc.sum > 16.3 && LCD_adc.sum < 21)
        LCD_adc.voltage = LCD_adc.sum; // Default value when no voltage exceeds the threshold
    if (LCD_adc.sum < 14)
        LCD_adc.voltage = 0;
    HAL_ADC_Stop(&hadc1);
}

void cur_messure(void)
{
    ADC_Select_CH11();
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 1);
    LCD_adc.readValue[1] = HAL_ADC_GetValue(&hadc1);
    uint16_t moving_avg_filtered = moving_average_filter(&kalman_fil_curr, LCD_adc.readValue[1]); // Apply moving average filter
    uint16_t ema_filtered = exponential_moving_average_filter(&kalman_fil_curr, moving_avg_filtered, EMA_ALPHA_CURR); // Apply EMA filter
    kalman_fil_curr.filter_kal = kalman_filter(&kalman_fil_curr, ema_filtered); // Apply Kalman filter
    //    // Calculate CURRENT using the cubic polynomial equation
    //    LCD_adc.sum1 = 0.0000002f * kalman_fil_curr.filter_kal_cur * kalman_fil_curr.filter_kal_cur - 0.0114f * kalman_fil_curr.filter_kal_cur + 35.5522898f -0.43 -  0.277999997; //- 0.897746623 + 0.105 + 0.085 - 0.02
    //    if (LCD_adc.sum1 > 0.43 && LCD_adc.sum1 < 15) LCD_adc.current = LCD_adc.sum1;
    //    if (LCD_adc.sum1 < 0.43) LCD_adc.current = 0;
    //    LCD_adc.Temp = ((3.3 * kalman_fil_curr.filter_kal_cur / 4095 - LCD_adc.V25) / LCD_adc.Avg_Slope) + 25;
    	// Calculate CURRENT using the cubic polynomial equation
    LCD_adc.sum1 = 0.00000009 * kalman_fil_curr.filter_kal * kalman_fil_curr.filter_kal + 0.0102 * kalman_fil_curr.filter_kal - 34.52249168 + l ;
    if (LCD_adc.sum1 > 0.5 && LCD_adc.sum1 < 15)
        LCD_adc.current = LCD_adc.sum1;
    if (LCD_adc.sum1 < 0.5)
        LCD_adc.current = 0;

    HAL_ADC_Stop(&hadc1);
}

void power_messure(void)
{
    LCD_adc.power = LCD_adc.current * LCD_adc.voltage;
}

void temperature_messure(void)
{
    if (LCD_adc.power == 0)
    {
        LCD_adc.temp = 0;
    }
    else
    {
        LCD_adc.T = (LCD_adc.voltage * LCD_adc.C) / LCD_adc.power;
        LCD_adc.joule = LCD_adc.power * LCD_adc.T;
        LCD_adc.temp = LCD_adc.joule / (LCD_adc.m * 20);
        LCD_adc.Temp = ((3.3 * kalman_fil_curr.filter_kal / 4095 - LCD_adc.V25) / LCD_adc.Avg_Slope) + 25;
    }
}

void startADC(void)
{
    vol_messure();
    cur_messure();
    power_messure();
    temperature_messure();
}

void float_to_string(float num, char *str, int decimalPlaces) {
    int intPart = (int)num;
    int decPart = (int)((num - intPart) * pow(10, decimalPlaces));

    // Convert integer part to string
    itoa(intPart, str, 10);

    // Find length of integer part
    int len = strlen(str);

    // Append decimal point
    str[len] = '.';
    str[len + 1] = '\0';

    // Convert decimal part to string
    char decStr[decimalPlaces + 1];
    itoa(decPart, decStr, 10);

    // Pad with zeros if needed
    int decLen = strlen(decStr);
    for (int i = 0; i < decimalPlaces - decLen; ++i) {
        strcat(str, "0");
    }

    // Append decimal part to the string
    strcat(str, decStr);

    // Null-terminate the string
    str[len + 1 + decimalPlaces] = '\0';
}


void display_menu(void) {
    lcd_init();
    lcd_clear();
    lcd_put_cur(0, 2);
    lcd_send_string("CNC DRILL 3 AXIS");
    lcd_put_cur(1, 2);
    lcd_send_string("HCMUTE CDT K20");
    lcd_put_cur(2, 2);
    lcd_send_string("KHOA CO KHI CTM");
    lcd_put_cur(3, 0);
    lcd_send_string("GVHD: ThS N.M. TRIET");
}

void display_main(void)
{
	if (LCD_adc.menu_main == 0)
	{
		lcd_clear();
		lcd_put_cur(0, 0);
		lcd_send_string(">VOLT/CURRENT");
		lcd_put_cur(1, 0);
		lcd_send_string("POWER/TEMPER");
		lcd_put_cur(2, 0);
		lcd_send_string("CONTROLL CNC");
		lcd_put_cur(3, 0);
		lcd_send_string("SPEED-XY/SPEED-Z");
	}
	else if (LCD_adc.menu_main == 1)
	{
		lcd_clear();
		lcd_put_cur(0, 0);
		lcd_send_string("VOLTAGE/CURRENT");
		lcd_put_cur(1, 0);
		lcd_send_string(">POWER/TEMPER");
        lcd_put_cur(2, 0);
		lcd_send_string("CONTROLL CNC");
		lcd_put_cur(3, 0);
		lcd_send_string("SPEED-XY/SPEED-Z");
	}
	else if (LCD_adc.menu_main == 2)
	{
		lcd_clear();
        lcd_put_cur(0, 0);
		lcd_send_string("VOLTAGE/CURRENT");
		lcd_put_cur(1, 0);
		lcd_send_string("POWER/TEMPER");
		lcd_put_cur(2, 0);
		lcd_send_string(">CONTROLL CNC");
		lcd_put_cur(3, 0);
		lcd_send_string("SPEED-XY/SPEED-Z");
	}
	else if (LCD_adc.menu_main == 3)
	{
		lcd_clear();
        lcd_put_cur(0, 0);
		lcd_send_string("VOLTAGE/CURRENT");
		lcd_put_cur(1, 0);
		lcd_send_string("POWER/TEMPER");
		lcd_put_cur(2, 0);
		lcd_send_string("CONTROLL CNC");
		lcd_put_cur(3, 0);
		lcd_send_string(">SPEED-XY/SPEED-Z");
	}
}

void chonmenu_tong(void) // CHỌN MENU TỔNG
{
  switch (LCD_adc.menu_main)
  {
    case 0:
      lcd_clear();
      lcd_put_cur(5, 0);
      lcd_send_string("MENU 1");
      lcd_put_cur(0,1);
      lcd_send_string("VOLTAGE/CURRENT");
      break;
    case 1:
      lcd_clear();
      lcd_put_cur(5,0);
      lcd_send_string("MENU 2");
	  lcd_put_cur(0,1);
	  lcd_send_string("POWER/TEMPER");
      break;
    case 2:
      lcd_clear();
      lcd_put_cur(5,0);
      lcd_send_string("MENU 3");
      lcd_put_cur(0,1);
      lcd_send_string("CONTROLL CNC");
      break;
    case 3:
      lcd_clear();
      lcd_put_cur(5,0);
      lcd_send_string("MENU 4");
      lcd_put_cur(0,1);
      lcd_send_string("SPEED-XY/SPEED-Z");
      break;
  }
}

void menu_1(void)
{
    lcd_clear();
    lcd_put_cur(0,0);
    lcd_send_string("VOLTAGE= ");
    lcd_put_cur(1,0);
    lcd_send_string("CURRENT= ");
    lcd_put_cur(2,0);
    lcd_send_string("ADC VOL= ");
    lcd_put_cur(3,0);
    lcd_send_string("ADC CUR= ");

    float last_voltage = -1;
    float last_current = -1;
    int last_adc_vol = -1;
    int last_adc_cur = -1;
    while (HAL_GPIO_ReadPin(back_port, backKey) != GPIO_PIN_RESET && HAL_GPIO_ReadPin(reset_port, resetKey) != GPIO_PIN_RESET
        		&& HAL_GPIO_ReadPin(start_port, start_pin) != GPIO_PIN_RESET && HAL_GPIO_ReadPin(stop_port, stop_pin) != GPIO_PIN_RESET) // Kiểm tra nút "Back" chưa được nhấn
        {
        // Đo điện áp và dòng điện
        vol_messure();
        cur_messure();

        // Cập nhật điện áp nếu có thay đổi
        if (LCD_adc.voltage != last_voltage)
        {
            last_voltage = LCD_adc.voltage;
            float_to_string(LCD_adc.voltage, LCD_adc.volVal, 2);
            lcd_put_cur(0, 9);
            lcd_send_string(LCD_adc.volVal);
            lcd_put_cur(0, 15);
            lcd_send_string("V");
        }

        // Cập nhật dòng điện nếu có thay đổi
        if (LCD_adc.current != last_current)
        {
            last_current = LCD_adc.current;
            float_to_string(LCD_adc.current, LCD_adc.curVal, 3);
            lcd_put_cur(1, 9);
            lcd_send_string(LCD_adc.curVal);
            lcd_put_cur(1, 15);
            lcd_send_string("A");
        }

        // Cập nhật adc volt nếu có thay đổi
        if (kalman_fil_volt.filter_kal != last_adc_vol)
        {
            last_adc_vol = kalman_fil_curr.filter_kal; // Fixed: updating last_adc instead of last_current
            snprintf(LCD_adc.adc_volVal, 6, "%d", kalman_fil_volt.filter_kal);
            lcd_put_cur(2, 9);
            lcd_send_string(LCD_adc.adc_volVal);
        }

        // Cập nhật adc curr nếu có thay đổi
        if (kalman_fil_curr.filter_kal != last_adc_cur)
        {
            last_adc_cur = kalman_fil_curr.filter_kal; // Fixed: updating last_adc instead of last_current
            snprintf(LCD_adc.adc_curVal, 6, "%d", kalman_fil_curr.filter_kal);
            lcd_put_cur(3, 9);
            lcd_send_string(LCD_adc.adc_curVal);
        }

        // Thêm một khoảng trễ nhỏ để ngăn việc sử dụng CPU quá mức
        osDelay(150);
    }
}



void menu_2(void)
{
    lcd_clear();
    lcd_put_cur(0,0);
    lcd_send_string("POWER= ");
    lcd_put_cur(1,0);
    lcd_send_string("TEMPER= ");

    float last_power = -1;
    float last_temp = -1;

    while (HAL_GPIO_ReadPin(back_port, backKey) != GPIO_PIN_RESET && HAL_GPIO_ReadPin(reset_port, resetKey) != GPIO_PIN_RESET
        		&& HAL_GPIO_ReadPin(start_port, start_pin) != GPIO_PIN_RESET && HAL_GPIO_ReadPin(stop_port, stop_pin) != GPIO_PIN_RESET) // Kiểm tra nút "Back" chưa được nhấn
        {
        // Đo điện áp và dòng điện (để tính công suất và nhiệt độ)
        vol_messure();
        cur_messure();

        // Cập nhật công suất nếu có thay đổi
        if (LCD_adc.power != last_power)
        {
            last_power = LCD_adc.power;
            float_to_string(LCD_adc.power, LCD_adc.powVal, 2);
            lcd_put_cur(0, 9);
            lcd_send_string(LCD_adc.powVal);
            lcd_put_cur(0, 15);
            lcd_send_string("W");
        }

        // Cập nhật nhiệt độ nếu có thay đổi
        if (LCD_adc.temp != last_temp)
        {
            last_temp = LCD_adc.Temp;
            float_to_string(LCD_adc.Temp, LCD_adc.tempVal, 3);
            char celsiusSymbol[] = {0xDF, 'C', '\0'};
            strcat(LCD_adc.tempVal, celsiusSymbol);
            lcd_put_cur(1, 9);
            lcd_send_string(LCD_adc.tempVal);
        }

        // Thêm một khoảng trễ nhỏ để ngăn việc sử dụng CPU quá mức
        osDelay(100);
    }
}


void menu_3(void)
{
	if (LCD_adc.selected_menu3_item == 0)
	{
		lcd_clear();
		lcd_put_cur(0, 0);
		lcd_send_string(">TYPE X= ");
		lcd_put_cur(1, 0);
		lcd_send_string("TYPE Y= ");
		lcd_put_cur(2, 0);
		lcd_send_string("TYPE Z= ");
	}
	else if (LCD_adc.selected_menu3_item == 1)
	{
		lcd_clear();
		lcd_put_cur(0, 0);
		lcd_send_string("TYPE X= ");
		lcd_put_cur(1, 0);
		lcd_send_string(">TYPE Y= ");
		lcd_put_cur(2, 0);
		lcd_send_string("TYPE Z= ");
	}
	else if (LCD_adc.selected_menu3_item == 2)
	{
		lcd_clear();
		lcd_put_cur(0, 0);
		lcd_send_string("TYPE X= ");
		lcd_put_cur(1, 0);
		lcd_send_string("TYPE Y= ");
		lcd_put_cur(2, 0);
		lcd_send_string(">TYPE Z= ");
	}
}

void select_menu3(void)
{
    switch (LCD_adc.selected_menu3_item)
    {
        case 0:
            lcd_clear();
            break;
        case 1:
            lcd_clear();
            break;
        case 2:
            lcd_clear();
            break;
    }
}

void X_count(void)
{
    while (HAL_GPIO_ReadPin(up_port, upKey) == GPIO_PIN_RESET)
    {
        LCD_adc.typeX_value++;
        stepX(abs(LCD_adc.typeX_value), 0, 15);
    }
    while (HAL_GPIO_ReadPin(down_port, downKey) == GPIO_PIN_RESET)
    {
        LCD_adc.typeX_value--;
        stepX(abs(LCD_adc.typeX_value), 1, 15);
    }
}

void Y_count(void)
{
    while (HAL_GPIO_ReadPin(up_port, upKey) == GPIO_PIN_RESET)
    {
        LCD_adc.typeY_value++;
        stepY(abs(LCD_adc.typeY_value), 0, 15);
    }
    while (HAL_GPIO_ReadPin(down_port, downKey) == GPIO_PIN_RESET)
    {
        LCD_adc.typeY_value--;
        stepY(abs(LCD_adc.typeY_value), 1, 15);
    }
}

void Z_count(void)
{
    while (HAL_GPIO_ReadPin(up_port, upKey) == GPIO_PIN_RESET)
    {
        LCD_adc.typeZ_value++;
        stepZ(abs(LCD_adc.typeZ_value), 1, 15);
    }
    while (HAL_GPIO_ReadPin(down_port, downKey) == GPIO_PIN_RESET)
    {
        LCD_adc.typeZ_value--;
        stepZ(abs(LCD_adc.typeZ_value), 0, 15);
    }
}

void menu_4(void)
{
    if (LCD_adc.selected_menu4_item == 0)
    {
        lcd_clear();
        lcd_put_cur(0, 0);
        lcd_send_string(">SPEED X-Y= ");
        lcd_put_cur(1, 0);
        lcd_send_string("SPEED Z= ");
        speedXY_count();
    }
    else if (LCD_adc.selected_menu4_item == 1)
    {
        lcd_clear();
        lcd_put_cur(0, 0);
        lcd_send_string("SPEED X-Y= ");
        lcd_put_cur(1, 0);
        lcd_send_string(">SPEED Z= ");
        speedZ_count();
    }
}

void select_menu4(void)
{
    switch (LCD_adc.selected_menu4_item)
    {
        case 0:
            lcd_clear();
            break;
        case 1:
            lcd_clear();
            break;
    }
}

void speedXY_count(void)
{
    if (HAL_GPIO_ReadPin(up_port, upKey) == GPIO_PIN_RESET)
    {
        LCD_adc.speed_valueXY += 100;
        CNC_pos.max_speedXY = LCD_adc.speed_valueXY;
        osDelay(10);
    }
    if (HAL_GPIO_ReadPin(down_port, downKey) == GPIO_PIN_RESET)
    {
        LCD_adc.speed_valueXY -= 100;
        CNC_pos.max_speedXY = LCD_adc.speed_valueXY;
        osDelay(10);
    }
}

void speedZ_count(void)
{
    if (HAL_GPIO_ReadPin(up_port, upKey) == GPIO_PIN_RESET)
    {
        LCD_adc.speed_valueZ += 100;
        CNC_pos.max_speedZ = LCD_adc.speed_valueZ;
        osDelay(10);
    }
    if (HAL_GPIO_ReadPin(down_port, downKey) == GPIO_PIN_RESET)
    {
        LCD_adc.speed_valueZ -= 100;
        CNC_pos.max_speedZ = LCD_adc.speed_valueZ;
        osDelay(10);
    }
}

// Function to update button state and debounce time
void updateButtonState(Button* button, uint32_t currentTime) {
    button->last_stable_state = HAL_GPIO_ReadPin(button->port, button->pin);
    button->last_debounce_time = currentTime;
}

// HAL GPIO EXTI Callback function
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    uint32_t currentTime = HAL_GetTick();

    for (int i = 0; i < numButtons; ++i) {
        if (buttons[i]->pin == GPIO_Pin) {
            updateButtonState(buttons[i], currentTime);
            break;
        }
    }
}

// Function to handle button tasks
void ButtonTask(void) {
    uint32_t current_time = HAL_GetTick();

    for (int i = 0; i < numButtons; ++i) {
        Button* button = buttons[i];

        if ((current_time - button->last_debounce_time > DEBOUNCE_DELAY) && (button->last_stable_state == GPIO_PIN_RESET)) {
            button->handler();
            button->last_debounce_time = current_time;
        }
    }

    // Check button states for menu 3 and menu 4
    if (LCD_adc.demtong == 3 && LCD_adc.demmenu_3 == 1) {
        switch (LCD_adc.selected_menu3_item) {
            case 0:
                lcd_put_cur(0, 0);
                lcd_send_string(">TYPE X= ");
                float_to_string(LCD_adc.typeX_value, LCD_adc.X_Val, 2);
                lcd_put_cur(0, 9);
                lcd_send_string(LCD_adc.X_Val);
                X_count();
                break;
            case 1:
                lcd_put_cur(1, 0);
                lcd_send_string(">TYPE Y= ");
                float_to_string(LCD_adc.typeY_value, LCD_adc.Y_Val, 2);
                lcd_put_cur(1, 9);
                lcd_send_string(LCD_adc.Y_Val);
                Y_count();
                break;
            case 2:
                lcd_put_cur(2, 0);
                lcd_send_string(">TYPE Z= ");
                float_to_string(LCD_adc.typeZ_value, LCD_adc.Z_Val, 2);
                lcd_put_cur(2, 9);
                lcd_send_string(LCD_adc.Z_Val);
                Z_count();
                break;
        }
    }

    if (LCD_adc.demtong == 3 && LCD_adc.demmenu_4 == 1) {
        switch (LCD_adc.selected_menu4_item) {
            case 0:
                lcd_put_cur(0, 0);
                lcd_send_string(">SPEED X-Y= ");
                float_to_string(LCD_adc.speed_valueXY, LCD_adc.speed_ValXY, 2);
                lcd_put_cur(0, 12);
                lcd_send_string(LCD_adc.speed_ValXY);
                speedXY_count();
                break;
            case 1:
                lcd_put_cur(1, 0);
                lcd_send_string(">SPEED Z= ");
                float_to_string(LCD_adc.speed_valueZ, LCD_adc.speed_ValZ, 2);
                lcd_put_cur(1, 12);
                lcd_send_string(LCD_adc.speed_ValZ);
                speedZ_count();
                break;
        }
    }
}

void handle_up_button_press(void)
{
    if (LCD_adc.demtong == 1) // move down in menu_main
    {
        if (LCD_adc.menu_main <= 0)
        {
            LCD_adc.menu_main = 3;
        }
        else
        {
            LCD_adc.menu_main -= 1;
        }
        display_main();
    }
    else if(LCD_adc.demtong == 2 && LCD_adc.menu_main == 2)
    {
        if(LCD_adc.selected_menu3_item <= 0)
        {
            LCD_adc.selected_menu3_item = 2;
        }
        else
        {
            LCD_adc.selected_menu3_item -= 1;
        }
        menu_3();
    }
    else if(LCD_adc.demtong == 2 && LCD_adc.menu_main == 3)
    {
        if(LCD_adc.selected_menu4_item <= 0)
        {
            LCD_adc.selected_menu4_item = 1;
        }
        else
        {
            LCD_adc.selected_menu4_item -= 1;
        }
        menu_4();
    }
}

void handle_down_button_press(void)
{
	if (LCD_adc.demtong == 1) // move up in menu_main
	{
		if (LCD_adc.menu_main >= 3)
		{
		   LCD_adc.menu_main = 0;
		}
		else
		{
		   LCD_adc.menu_main += 1;
		}
		display_main();
	}
	else if(LCD_adc.demtong == 2 && LCD_adc.menu_main == 2)
	{
		if(LCD_adc.selected_menu3_item >= 2)
		{
			LCD_adc.selected_menu3_item = 0;
		}
		else
		{
			LCD_adc.selected_menu3_item += 1;
		}
		menu_3();
	}
	else if(LCD_adc.demtong == 2 && LCD_adc.menu_main == 3)
	{
		if(LCD_adc.selected_menu4_item >= 1)
		{
			LCD_adc.selected_menu4_item = 0;
		}
		else
		{
			LCD_adc.selected_menu4_item += 1;
		}
		menu_4();
	}
}

void handle_back_button_press(void)
{
    LCD_adc.demback += 1;
    if (LCD_adc.demback == 1)
    {
       if (LCD_adc.demtong == 1 && (LCD_adc.menu_main == 0 || LCD_adc.menu_main == 1 || LCD_adc.menu_main == 2 || LCD_adc.menu_main == 3))
       {
           LCD_adc.demtong -= 1;
           LCD_adc.demback = 0;
           display_menu();
       }
       else if (LCD_adc.demtong == 2 && LCD_adc.menu_main == 0) // From menu_1 back to display main_menu
       {
           LCD_adc.demtong -= 1;
           LCD_adc.demback = 0;
           display_main();
       }
       else if (LCD_adc.demtong == 2 && LCD_adc.menu_main == 1) // From menu_2 back to display main_menu
       {
           LCD_adc.demtong -= 1;
           LCD_adc.demback = 0;
           display_main();
       }
       else if (LCD_adc.demtong == 2 && LCD_adc.menu_main == 2) // From menu_3 back to display main_menu
       {
           LCD_adc.demtong -= 1;
           LCD_adc.demback = 0;
           LCD_adc.demmenu_3 = 0;
           display_main();
       }
       else if (LCD_adc.demtong == 3 && LCD_adc.demmenu_3 >= 1) // From select_menu3 back to menu_3
       {
           LCD_adc.demback = 0;
           LCD_adc.demtong = 2;
           LCD_adc.selected_menu3_item = 0;
           menu_3();
       }
       else if (LCD_adc.demtong == 2 && LCD_adc.menu_main == 3) // From menu_4 back to display main_menu
       {
           LCD_adc.demtong -= 1;
           LCD_adc.demback = 0;
           LCD_adc.demmenu_4 = 0;
           display_main();
       }
       else if (LCD_adc.demtong == 3 && LCD_adc.demmenu_4 >= 1) // From select_menu4 back to menu_4
       {
           LCD_adc.demback = 0;
           LCD_adc.demtong = 2;
           LCD_adc.selected_menu4_item = 0;
           menu_4();
       }
    }
    else
    {
       LCD_adc.demback = 0;
    }
}

void handle_select_button_press(void)
{
    LCD_adc.demtong += 1;
    if (LCD_adc.demtong == 1) // in menu_main
    {
       LCD_adc.demback = 0;
       display_main();
    }
    else if (LCD_adc.demtong == 2 && LCD_adc.menu_main == 0) // choose menu 1
    {
       LCD_adc.demback = 0;
       menu_1();
    }
    else if (LCD_adc.demtong == 2 && LCD_adc.menu_main == 1) // choose menu 2
    {
       LCD_adc.demback = 0;
       menu_2();
    }
    else if (LCD_adc.demtong == 2 && LCD_adc.menu_main == 2) // In menu 3
    {
       LCD_adc.demback = 0;
       menu_3();
       LCD_adc.demmenu_3 += 1;
    }
    else if(LCD_adc.demtong == 3 && LCD_adc.demmenu_3 == 1)//choose menu 3
    {
        LCD_adc.demback = 0;
       select_menu3();
    }
    else if (LCD_adc.demtong == 2 && LCD_adc.menu_main == 3) // In menu 4
    {
       LCD_adc.demback = 0;
       menu_4();
       LCD_adc.demmenu_4 += 1;
    }
    else if(LCD_adc.demtong == 3 && LCD_adc.demmenu_4 == 1)//choose menu 4
    {
       LCD_adc.demback = 0;
       select_menu4();
    }
    else if (LCD_adc.demtong > 3)
    {
       LCD_adc.demtong = 3;
       LCD_adc.demback = 0;
    }
}

void handle_reset_button_press(void)
{
    if (state.reset_press == 1)
    {
        resetProgram();
        // when reseted, restarted again
        state.reset_press = 0;
    }
}

void handle_start_button_press(void)
{
    if (state.reset_press == 0)
    {
    	state.start_press = 1;
    	state.stop_press = 0;
        HAL_GPIO_WritePin(drill_port, drill_pin, 1);
    	StartProgram();
    }
}

void handle_stop_button_press(void)
{
    if (state.reset_press == 0)
    {
    	state.stop_press = 1;
        HAL_GPIO_WritePin(drill_port, drill_pin, 0);
        StopProgram();
        // Sau khi dừng, chỉ cho phép nhấn nút reset
        state.reset_press = 1;
    }
}

void resetProgram(void)
{
	state.start_press = 0;
	state.stop_press = 1;
    // reset var
    lcd_clear();
    lcd_put_cur(1, 2);
    lcd_send_string("PROGRAM RESETING");
    LCD_adc.demtong = 0;
    LCD_adc.demback = 0;
    LCD_adc.menu_main = 0;
    LCD_adc.selected_menu3_item = 0;
    LCD_adc.selected_menu4_item = 0;
    LCD_adc.demmenu_3 = 0;
    LCD_adc.demmenu_4 = 0;
    LCD_adc.typeX_value = 0;
    LCD_adc.typeY_value = 0;
    LCD_adc.typeZ_value = 0;
    LCD_adc.speed_valueXY = 0;
    LCD_adc.speed_valueZ = 0;
    LCD_adc.voltage = 0;
    LCD_adc.current = 0;
    LCD_adc.power = 0;
    LCD_adc.temp = 0;
    Stepper1.accel_count = 0;
    Stepper2.accel_count = 0;
    Stepper3.accel_count = 0;
    CNC_pos.x = 0;
    CNC_pos.y = 0;
    CNC_pos.z = 0;
    CNC_pos.MoveX = 0;
    CNC_pos.MoveY = 0;
    CNC_pos.MoveZ = 0;
    CNC.pos_x = 0;
    CNC.pos_y = 0;
    CNC.pos_z = 0;
    CNC.set_posX = 0;
    CNC.set_posY = 0;
    CNC.set_posZ = 0;

    __HAL_TIM_SET_AUTORELOAD(Stepper1.htim, 1000);
    __HAL_TIM_SET_AUTORELOAD(Stepper2.htim, 1000);
    __HAL_TIM_SET_AUTORELOAD(Stepper3.htim, 1000);
    // Reset hardware
    initialize_LCD(&LCD_adc);
    initialize_Kalman(&kalman_fil_curr);
    initialize_Kalman(&kalman_fil_volt);
    initializeCNC_pos(&CNC_pos);
    // add code here
    /*
     * code here
     *
     * */
    HOME();
    // Start display lcd
    lcd_clear();
    lcd_put_cur(1, 2);
    lcd_send_string("PROGRAM  RESETED");
}

void StopProgram(void)
{
	state.start_press = 0;
    lcd_clear();
    lcd_put_cur(1, 2);
    lcd_send_string("PROGRAM  STOPPED");
    lcd_put_cur(3, 0);
}

void StartProgram(void){
    lcd_clear();
    lcd_put_cur(1, 3);
    lcd_send_string("PROGRAM  START");
    lcd_put_cur(2, 0);
    lcd_send_string("IP ADD:");
    lcd_put_cur(2, 8);
    lcd_send_string(SaveData.ip_config);
}
