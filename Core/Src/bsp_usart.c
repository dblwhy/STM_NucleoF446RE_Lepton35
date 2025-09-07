/*
 * bsp_usart.c
 *
 *  Created on: Sep 5, 2025
 *      Author: yuki-yoshimura
 */

#include "bsp.h"

UART_HandleTypeDef huart2;

void MX_USART2_Init(void)
{
	__HAL_RCC_USART2_CLK_ENABLE();

	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;

	if (HAL_UART_Init(&huart2) != HAL_OK) {
		printf("UART2 initialization failed!\r\n");
		while(1);
    }
	printf("\r\n========= START =========\r\n");
	printf("UART2 initialization succeed!\r\n");
}
