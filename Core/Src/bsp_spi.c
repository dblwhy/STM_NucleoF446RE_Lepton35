/*
 * bsp_spi.c
 *
 *  Created on: Sep 5, 2025
 *      Author: yuki-yoshimura
 */

#include "bsp.h"

SPI_HandleTypeDef hspi1;

void MX_SPI1_Init(void)
{
    __HAL_RCC_SPI1_CLK_ENABLE();

    SPI_InitTypeDef spi_init;

    spi_init.Mode = SPI_MODE_MASTER;
    spi_init.Direction = SPI_DIRECTION_2LINES;
    spi_init.DataSize = SPI_DATASIZE_8BIT;
    spi_init.CLKPolarity = SPI_POLARITY_HIGH;
    spi_init.CLKPhase = SPI_PHASE_2EDGE;
    spi_init.NSS = SPI_NSS_SOFT;
    spi_init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    spi_init.FirstBit = SPI_FIRSTBIT_MSB;
    spi_init.TIMode = SPI_TIMODE_DISABLE;
    spi_init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    spi_init.CRCPolynomial = 10;

    hspi1.Instance = SPI1;
    hspi1.Init = spi_init;
	if (HAL_SPI_Init(&hspi1) != HAL_OK) {
		printf("SPI initialization failed!\r\n");
		while(1);
    }

    printf("SPI initialization succeed!\r\n");
}
