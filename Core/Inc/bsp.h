/*
 * bsp_gpio.h
 *
 *  Created on: Sep 5, 2025
 *      Author: yuki-yoshimura
 */

#ifndef INC_BSP_H_
#define INC_BSP_H_

#pragma once
#include "stm32f4xx_hal.h"

void MX_SPI1_Init(void);
void MX_GPIO_Init(void);
void MX_USART2_Init(void);
void MX_I2C1_Init(void);
void SystemClock_Config(void);

#endif /* INC_BSP_H_ */
