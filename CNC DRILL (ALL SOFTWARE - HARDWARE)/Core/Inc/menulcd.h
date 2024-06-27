/*
 * menulcd.h
 *
  * Created on: Jan 21, 2024
  * Author: Nguyen Ngoc Quy (Wis)
 */

#ifndef INC_MENULCD_H_
#define INC_MENULCD_H_
#include "main.h"
#include "stdint.h"
#define KALMAN_Q 0.0001f
#define KALMAN_R 0.1f
#define EMA_ALPHA_VOLT 0.005f
#define EMA_ALPHA_CURR 0.001f
#define DEBOUNCE_DELAY 80 // Example debounce delay, adjust as needed

typedef struct{
	//state of LCD
	int menu_main;
	int man_auto;
	int macdinh;
	int demtong;
	int demback;
	int selected_menu3_item;
	int selected_menu4_item ;
	int demmenu_3;
	int demmenu_4;
	//value of 3 AXIS x-y-z
	int typeX_value;
	int typeY_value;
	int typeZ_value;
	//speed value of step
	int speed_valueXY;
	int speed_valueZ;
	//cnt/cnt1/sum/sum1
	float cnt;
	float sum;
	float cnt1;
	float sum1;
	//adc
	uint32_t adc1;
	//read value from ADC
	uint16_t readValue[2];
	//read value Current/Voltage/Temperature/Power
	float current;
	float voltage;
	float temp;
	float power;
	float joule;
	float cur;
	float volt;
	float average;
	float average1;
	float V25;
	float Avg_Slope;
	float Temp;
	//set value for adc
	float sensitivity;
	float ACSoffset;
	float m;
	float C;
	float T;
	//Array
	char volVal[5];
	char curVal[5];
	char adc_volVal[5];
	char adc_curVal[5];
	char tempVal[5];
	char powVal[5];
	char X_Val[5];
	char Y_Val[5];
	char Z_Val[5];
	char speed_ValXY[5];
	char speed_ValZ[5];
}LCD_adc_t;

typedef struct {
    // Kalman filter
    int N;
    uint16_t filter_kal;
    float ema_filtered_value;
    float x_k1_k1, x_k_k1;
    float Z_k;
    float P_k1_k1;
    float Q;
    float R;
    float Kg;
    float P_k_k1;
    float kalman_adc;
    float kalman_adc_old;
    uint16_t buffer[10];
    int index;
    uint32_t sum;
} Kalman_filter;

typedef struct {
    uint16_t pin;
    GPIO_TypeDef* port;
    uint32_t last_stable_state;
    uint32_t last_debounce_time;
    void (*handler)();
} Button;

typedef struct {
	volatile uint8_t start_press;
	volatile uint8_t stop_press;
	volatile uint8_t reset_press;
}State_button;

extern LCD_adc_t LCD_adc;
extern Kalman_filter kalman_fil_curr;
extern Kalman_filter kalman_fil_volt;
extern Button button;
extern State_button state;

void initialize_LCD(LCD_adc_t *lcd);
void initialize_Kalman(Kalman_filter *kf);
void delay_lcd(uint16_t delay);
void stepX (int steps, uint8_t direction, uint16_t delay);
void stepY (int steps, uint8_t direction, uint16_t delay);
void stepZ (int steps, uint8_t direction, uint16_t delay);
void display_menu(void);
void display_main(void);
void chonmenu_tong(void);
void menu_1(void);
void menu_2(void);
void menu_3(void);
void menu_4(void);
//void handle_button_press(void);
void updateButtonState(Button* button, uint32_t currentTime);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void ButtonTask(void);
void handle_up_button_press(void);
void handle_down_button_press(void);
void handle_back_button_press(void);
void handle_reset_button_press(void);
void handle_select_button_press(void);
void handle_start_button_press(void);
void handle_stop_button_press(void);
uint16_t kalman_filter_cur(uint16_t ADC_Value) ;
uint16_t moving_average_filter_cur(uint16_t ADC_Value) ;
uint16_t exponential_moving_average_filter_cur(uint16_t ADC_Value) ;
uint16_t kalman_filter_vol(uint16_t ADC_Value) ;
uint16_t moving_average_filter_vol(uint16_t ADC_Value) ;
uint16_t exponential_moving_average_filter_vol(uint16_t ADC_Value) ;
void vol_messure(void);
void cur_messure(void);
void power_messure(void);
void temperature_messure(void);
void startADC(void);
void select_menu3(void);
void X_count(void);
void Y_count(void);
void Z_count(void);
void select_menu4(void);
void speedXY_count(void);
void speedZ_count(void);
void resetProgram(void);
void StopProgram(void);
void StartProgram(void);
#endif /* INC_MENULCD_H_ */
