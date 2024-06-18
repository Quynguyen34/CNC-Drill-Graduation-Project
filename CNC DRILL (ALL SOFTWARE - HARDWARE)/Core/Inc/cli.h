/*
 * cli.h
 *
 *  Created on: May 8, 2024
 *      Author: Wis
 */

#ifndef INC_CLI_H_
#define INC_CLI_H_

#include "usart.h"

/* Khai báo các hàm chuẩn bị và truyền dữ liệu */
void prepare_data(void);
void UART_transmit_init(void);

/* Khai báo các hàm nhận dữ liệu UART */
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

/* Khai báo các hàm callback của UART */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif /* INC_CLI_H_ */
