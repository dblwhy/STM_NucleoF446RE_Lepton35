/*
 * main.c
 *
 *  Created on: Sep 5, 2025
 *      Author: yuki-yoshimura
 */

#include "main.h"
#include "bsp.h"
#include "lepton.h"

volatile uint8_t vsync_flag = 0;
volatile uint8_t frame_in_progress = 0;
volatile uint16_t current_packet = 0;

extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart2;

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_USART2_Init();
    MX_SPI1_Init();
    MX_I2C1_Init();

    Print_ClockInfo();

    Lepton_StartupSequence();
    Lepton_InitVsync();
    Lepton_InitVideoOutputFormat();
    Lepton_OpenPort();
    Lepton_PrintSdkVersion();
    printf("System ready. Waiting for VSYNC...\r\n");

    while (1)
    {
        if (vsync_flag)
        {
            vsync_flag = 0;
            frame_in_progress = 1;

            Lepton_ReceivePacketDMA();
        }
    }
}

void Print_ClockInfo(void)
{
	uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
	uint32_t pclk2 = HAL_RCC_GetPCLK2Freq();
    printf("PCLK1: %lu Hz (%lu MHz)\r\n", pclk1, pclk1 / 1000000);
    printf("PCLK2: %lu Hz (%lu MHz)\r\n", pclk2, pclk2 / 1000000);

	uint32_t spi_source_clock = pclk2; // SPI1 is on APB2, so use PCLK2
    uint32_t prescaler_enum = hspi1.Init.BaudRatePrescaler;
    uint32_t prescaler_value;
    switch(prescaler_enum) {
        case SPI_BAUDRATEPRESCALER_2:   prescaler_value = 2; break;
        case SPI_BAUDRATEPRESCALER_4:   prescaler_value = 4; break;
        case SPI_BAUDRATEPRESCALER_8:   prescaler_value = 8; break;
        case SPI_BAUDRATEPRESCALER_16:  prescaler_value = 16; break;
        case SPI_BAUDRATEPRESCALER_32:  prescaler_value = 32; break;
        case SPI_BAUDRATEPRESCALER_64:  prescaler_value = 64; break;
        case SPI_BAUDRATEPRESCALER_128: prescaler_value = 128; break;
        case SPI_BAUDRATEPRESCALER_256: prescaler_value = 256; break;
        default: prescaler_value = 16; break;
    }
    uint32_t actual_spi_clock = spi_source_clock / prescaler_value;
    printf("SPI1 Clock: %lu MHz (prescaler: %lu)\r\n", actual_spi_clock / 1000000,prescaler_value);
}

int _write(int file, char *ptr, int len)
{
	HAL_UART_Transmit(&huart2,(uint8_t *)ptr,len,10);
	return len;
}

