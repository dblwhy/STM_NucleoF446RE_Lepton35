/*
 * lepton.h
 *
 *  Created on: Sep 5, 2025
 *      Author: yuki-yoshimura
 */

#ifndef INC_LEPTON_H_
#define INC_LEPTON_H_

#pragma once
#include "stm32f4xx_hal.h"
#include "LEPTON_OEM.h"
#include "LEPTON_SDK.h"
#include "LEPTON_SYS.h"
#include "LEPTON_AGC.h"
#include "LEPTON_VID.h"

#define IMG_ROWS           120u
#define IMG_COLS           160u
#define BYTES_PER_PIXEL    3
#define VOSPI_PKT_SIZE     244u   /* 4-byte header + 240-byte payload for RGB888 mode */
#define VOSPI_PKT_BODY_SIZE   240u   /* 4-byte header + 240-byte payload for RGB888 mode */
#define VOSPI_FRAME_SIZE   (IMG_ROWS * IMG_COLS * BYTES_PER_PIXEL) // 57.6kb

#define LEPTON_PORT       GPIOA
#define LEPTON_CS_PIN     GPIO_PIN_4
#define LEPTON_PWR_PIN    GPIO_PIN_8
#define LEPTON_RESET_PIN  GPIO_PIN_9

#define LEPTON_PW_DWN_HIGH	HAL_GPIO_WritePin(LEPTON_PORT, LEPTON_PWR_PIN, GPIO_PIN_SET)
#define LEPTON_PW_DWN_LOW	HAL_GPIO_WritePin(LEPTON_PORT, LEPTON_PWR_PIN, GPIO_PIN_RESET)
#define LEPTON_RESET_L_HIGH	HAL_GPIO_WritePin(LEPTON_PORT, LEPTON_RESET_PIN, GPIO_PIN_SET)
#define LEPTON_RESET_L_LOW	HAL_GPIO_WritePin(LEPTON_PORT, LEPTON_RESET_PIN, GPIO_PIN_RESET)
#define LEPTON_CS_HIGH      HAL_GPIO_WritePin(LEPTON_PORT, LEPTON_CS_PIN, GPIO_PIN_SET)
#define LEPTON_CS_LOW       HAL_GPIO_WritePin(LEPTON_PORT, LEPTON_CS_PIN, GPIO_PIN_RESET)

LEP_RESULT Lepton_OpenPort(void);
void Lepton_StartupSequence(void);
void Lepton_InitVsync(void);
void Lepton_InitVideoOutputFormat(void);
void Lepton_PrintSdkVersion(void);
void Lepton_ReceivePacketDMA(void);
void Lepton_ProcessPacket(void);


#endif /* INC_LEPTON_H_ */
