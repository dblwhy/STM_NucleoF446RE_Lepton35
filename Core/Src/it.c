/*
 * it.c
 *
 *  Created on: Sep 5, 2025
 *      Author: yuki-yoshimura
 */

#include "stm32f4xx_hal.h"
#include "bsp.h"
#include "lepton.h"

extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma2_stream2_rx;
extern DMA_HandleTypeDef hdma2_stream2_tx;

extern volatile uint8_t vsync_flag;
extern volatile uint8_t segment_in_progress;
extern volatile uint16_t current_packet;

extern uint8_t lepton_packet[VOSPI_PKT_SIZE];
extern uint8_t lepton_frame[VOSPI_FRAME_SIZE];
extern uint8_t lepton_frame_index;
extern uint8_t last_segment_number;

void SysTick_Handler(void)
{
	HAL_IncTick();
}

void EXTI0_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void DMA2_Stream2_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&hdma2_stream2_rx);
}
void DMA2_Stream3_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&hdma2_stream2_tx);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	// Received VSYNC
	if (GPIO_Pin == GPIO_PIN_0 && !segment_in_progress)
	{
		current_packet = 0;
		vsync_flag = 1;
	}
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi == &hspi1)
	{
		LEPTON_CS_HIGH;
		current_packet++;

		Lepton_ProcessPacket();

		if (current_packet < 61)
		{
			// This condition seems accurate to check `< 60` but the first packet starts from 0x38 with it somehow.
			// Added 1 extra call to make it work.
			Lepton_ReceivePacketDMA();
		}
		else
		{
			segment_in_progress = 0;

			if (last_segment_number == 0) {
				lepton_frame_index = 0;
			}
			if (last_segment_number <= 3)
			{
				return;
			}

			// Frame completed!!

			// Printing last byte for debugging. Note that it may get out of sync if you debug more than 1 byte
			// (next vsync may trigger before finishing the printing).
			printf("%02X ", lepton_frame[VOSPI_FRAME_SIZE-1]);

			lepton_frame_index = 0;
			last_segment_number = 0;
		}
	}
}
