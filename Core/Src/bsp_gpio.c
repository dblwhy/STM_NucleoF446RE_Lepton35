/*
 * bsp_gpio.c
 *
 *  Created on: Sep 5, 2025
 *      Author: yuki-yoshimura
 */

#include "bsp.h"
#include "lepton.h"
#include "stm32f4xx_hal.h"

void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef gpio_init;

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	/* *
	 * Use alternate function for SPI1
	 *
	 * PA5 -> SPI1_SCK
	 * PA6 -> SPI1_MISO
	 * PA7 -> SPI1_MOSI
	 * */
	gpio_init.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	gpio_init.Mode = GPIO_MODE_AF_PP;
	gpio_init.Pull = GPIO_NOPULL;
	gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	gpio_init.Alternate = GPIO_AF5_SPI1;
	HAL_GPIO_Init(GPIOA, &gpio_init);

	/*
	 * PB12 -> SPI NSS
	 * */
	gpio_init.Pin = LEPTON_CS_PIN;
	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init.Pull = GPIO_NOPULL;
	gpio_init.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(LEPTON_PORT, &gpio_init);

	/*
	 * PA5 -> LD2
	 * PA8 -> PW_DWN_L
	 * PA9 -> RESET_L
	 * */
	gpio_init.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init.Pull = GPIO_NOPULL;
	gpio_init.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOA, &gpio_init);

	/*
	 * PB6 -> I2C1_SCL
	 * PB7 -> I2C1_SDA
	 * */
	gpio_init.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	gpio_init.Mode = GPIO_MODE_AF_OD;
	gpio_init.Pull = GPIO_PULLUP;
	gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	gpio_init.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &gpio_init);

	/*
	 * Configure PA0 as external interrupt for VSYNC
	 * */
	__HAL_RCC_SYSCFG_CLK_ENABLE();

	memset(&gpio_init, 0, sizeof(gpio_init));
	gpio_init.Pin = GPIO_PIN_0;
	gpio_init.Mode = GPIO_MODE_IT_RISING;
	gpio_init.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &gpio_init);

	HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}
