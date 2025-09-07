/*
 * msp.c
 *
 *  Created on: Sep 5, 2025
 *      Author: yuki-yoshimura
 */

#include "stm32f4xx_hal.h"
#include "main.h"

DMA_HandleTypeDef hdma2_stream2_rx;
DMA_HandleTypeDef hdma2_stream2_tx;

void HAL_MspInit(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
    	__HAL_RCC_USART2_CLK_ENABLE();
    	__HAL_RCC_GPIOA_CLK_ENABLE();

    	GPIO_InitTypeDef gpio_uart;
    	gpio_uart.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    	gpio_uart.Mode = GPIO_MODE_AF_PP;
    	gpio_uart.Pull = GPIO_PULLUP;
    	gpio_uart.Speed = GPIO_SPEED_FREQ_MEDIUM;
    	gpio_uart.Alternate = GPIO_AF7_USART2;
    	HAL_GPIO_Init(GPIOA, &gpio_uart);
    }
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
    if(hspi->Instance == SPI1)
    {
        /* Enable ALL clocks */
        __HAL_RCC_SPI1_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_DMA2_CLK_ENABLE();

        DMA_InitTypeDef dma_init_rx;

        dma_init_rx.Channel = DMA_CHANNEL_3;
        dma_init_rx.Direction = DMA_PERIPH_TO_MEMORY;
        dma_init_rx.PeriphInc = DMA_PINC_DISABLE;
        dma_init_rx.MemInc = DMA_MINC_ENABLE;
        dma_init_rx.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        dma_init_rx.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        dma_init_rx.Mode = DMA_NORMAL;
        dma_init_rx.Priority = DMA_PRIORITY_VERY_HIGH;
        dma_init_rx.FIFOMode = DMA_FIFOMODE_DISABLE;
        dma_init_rx.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
        dma_init_rx.MemBurst = DMA_MBURST_SINGLE;
        dma_init_rx.PeriphBurst = DMA_PBURST_SINGLE;

        hdma2_stream2_rx.Instance = DMA2_Stream2;
        hdma2_stream2_rx.Init = dma_init_rx;
        if (HAL_DMA_Init(&hdma2_stream2_rx) != HAL_OK)
        {
      		printf("DMA_RX initialization failed!\r\n");
      		while(1);
        }

        __HAL_LINKDMA(hspi, hdmarx, hdma2_stream2_rx);
        HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 2, 0);
        HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

        DMA_InitTypeDef dma_init_tx;

        dma_init_tx.Channel = DMA_CHANNEL_3;
        dma_init_tx.Direction = DMA_MEMORY_TO_PERIPH;
        dma_init_tx.PeriphInc = DMA_PINC_DISABLE;
        dma_init_tx.MemInc = DMA_MINC_ENABLE;
        dma_init_tx.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        dma_init_tx.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        dma_init_tx.Mode = DMA_NORMAL;
        dma_init_tx.Priority = DMA_PRIORITY_VERY_HIGH;
        dma_init_tx.FIFOMode = DMA_FIFOMODE_DISABLE;
        dma_init_tx.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
        dma_init_tx.MemBurst = DMA_MBURST_SINGLE;
        dma_init_tx.PeriphBurst = DMA_PBURST_SINGLE;

        hdma2_stream2_tx.Instance = DMA2_Stream3;
        hdma2_stream2_tx.Init = dma_init_tx;
        if (HAL_DMA_Init(&hdma2_stream2_tx) != HAL_OK)
        {
      		printf("DMA_TX initialization failed!\r\n");
      		while(1);
        }

        __HAL_LINKDMA(hspi, hdmatx, hdma2_stream2_tx);
        HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 2, 0);
        HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
    }
}

