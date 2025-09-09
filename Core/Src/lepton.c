/*
 * lepton.c
 *
 *  Created on: Sep 5, 2025
 *      Author: yuki-yoshimura
 */

#include "lepton.h"
#include "bsp.h"

extern SPI_HandleTypeDef hspi1;
extern I2C_HandleTypeDef hi2c1;

LEP_CAMERA_PORT_DESC_T hport_desc = {
	.deviceAddress = 0x2A,
	.portBaudRate = 100,
	.portID = 0
};

uint8_t lepton_packet[VOSPI_PKT_SIZE];
uint8_t lepton_frame[VOSPI_FRAME_SIZE];
uint8_t lepton_frame_index = 0;
uint8_t last_segment_number = 0;

/* Dummy TX for full-duplex SPI */
static uint8_t dummy_tx_buffer[VOSPI_PKT_SIZE] = {0};

/* Packet state tracking */
static uint16_t current_packet = 0;

LEP_RESULT Lepton_OpenPort(void)
{
	LEP_RESULT result = LEP_OpenPort(0, LEP_CCI_TWI, 100, &hport_desc);
	if (result != LEP_OK)
	{
		uint32_t err = HAL_I2C_GetError(&hi2c1);
		printf("I2C read failed, HAL error: 0x%08lx\r\n", err);
		printf("Could not open Lepton I2C port! %d\r\n", result);
		while (1);
	}
}

void Lepton_StartupSequence(void)
{
	LEPTON_PW_DWN_LOW;
	HAL_Delay(200);
	LEPTON_PW_DWN_HIGH;
	HAL_Delay(200);

	LEPTON_RESET_L_LOW;
	HAL_Delay(200);
	LEPTON_RESET_L_HIGH;

	HAL_Delay(5000); // Let sensor boot (5 seconds for camera with shutter)
}

void Lepton_InitVsync(void)
{
	LEP_RESULT result;
	LEP_OEM_GPIO_MODE_E gpio_mode;

	gpio_mode = LEP_OEM_END_GPIO_MODE;
	result = LEP_GetOemGpioMode(&hport_desc, &gpio_mode);
	printf("LEP_GetOemGpioMode gpio_mode = %d result = %d.\r\n", gpio_mode, result);

	result = LEP_SetOemGpioMode(&hport_desc, LEP_OEM_GPIO_MODE_VSYNC);
	printf("LEP_SetOemGpioMode result = %d.\r\n", result);

	gpio_mode = LEP_OEM_END_GPIO_MODE;
	result = LEP_GetOemGpioMode(&hport_desc, &gpio_mode);
	printf("LEP_GetOemGpioMode gpio_mode = %d result = %d.\r\n", gpio_mode, result);

	//    // If you want vsync delay
	//	result = LEP_SetOemGpioVsyncPhaseDelay(&hport_desc, LEP_OEM_VSYNC_DELAY_PLUS_3);
	//	printf("vsyncResult: %d\r\n", result);
}

void Lepton_InitVideoOutputFormat(void)
{
	LEP_RESULT result = LEP_SetVidVideoOutputFormat(&hport_desc, LEP_VID_VIDEO_OUTPUT_FORMAT_RGB888);
	printf("LEP_SetVidVideoOutputFormat: %d\r\n", result);
}

void Lepton_PrintSdkVersion(void)
{
	LEP_RESULT result;
	LEP_SDK_VERSION_T version;

	result = LEP_GetSDKVersion(&hport_desc, &version);
	if (result != LEP_OK)
	{
		printf("Failed to get Lepton SDK version = %d.\r\n", result);
		return;
	}

	printf("Lepton SDK version: %u.%u.%u\r\n", version.major, version.minor, version.build);
}

void Lepton_ReceivePacketDMA(void)
{
	LEPTON_CS_LOW;

	HAL_StatusTypeDef result = HAL_SPI_TransmitReceive_DMA(&hspi1, dummy_tx_buffer, lepton_packet, VOSPI_PKT_SIZE);
	if (result != HAL_OK)
	{
		printf("=== HAL_SPI_TransmitReceive_DMA ===\r\n");
		printf("Result: %d\r\n", result);
		printf("SPI State after: %d\r\n", hspi1.State);
		printf("SPI ErrorCode after: 0x%08X\r\n", hspi1.ErrorCode);
		printf("DMA State after: %d\r\n", hspi1.hdmarx->State);
		printf("DMA ErrorCode after: 0x%08X\r\n", hspi1.hdmarx->ErrorCode);
		while (1);
	}
}

int Lepton_ProcessPacket(void)
{
	uint32_t header = ((uint32_t)lepton_packet[0] << 24) |
					  ((uint32_t)lepton_packet[1] << 16) |
					  ((uint32_t)lepton_packet[2] << 8) |
					  (uint32_t)lepton_packet[3];
	uint8_t segment_number = (header >> 28) & 0x07;
	uint16_t packet_number = (header >> 16) & 0x0FFF;
	bool discard = (lepton_packet[0] & 0x0F) == 0x0F;

	if (discard)
	{
		// discard Discard Packet
		return LEPTON_ERR;
	}
	if (segment_number == 0 && packet_number == 20)
	{
		// discard Segment 0 (valid segment is 1..4)
		return LEPTON_ERR;
	}

	// valid packet
	if (packet_number == 20)
	{
		last_segment_number = segment_number;
	}

	// store packet into frame buffer
	uint8_t *packet_body = &lepton_packet[4];
	memcpy(&lepton_frame[lepton_frame_index * VOSPI_PKT_BODY_SIZE],
		   packet_body,
		   VOSPI_PKT_BODY_SIZE);

	lepton_frame_index++;

	return LEPTON_OK;
}
