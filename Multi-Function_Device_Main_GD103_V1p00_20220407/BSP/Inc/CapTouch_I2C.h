/**
* Multi-function Device Main Program
*
* Copyright 2021 by Home beauty Device Team, CLASSYS Inc.
* All rights reserved.
*
* CapTouch_I2C.h : This file contains Defines, Extern Variables and etc.
to control Capacitive Touch Driver IC.
*
* @author     Park, Sungjun
* @version    V0.00
* @date       12.14.2021
*/
#ifndef __CAPTOUCH_I2C_H
#define __CAPTOUCH_I2C_H

/* includes ------------------------------------------------------------------*/
#include "main.h"

/* defines -------------------------------------------------------------------*/
#define ADDR_SLAVE_TS06		0xD2

#define R_SENSITIVE1		0x00
#define R_SENSITIVE2		0x01
#define R_SENSITIVE3		0x02
#define R_CTRL1				0x03
#define R_CTRL2				0x04
#define R_REF_RST			0x05
#define R_CH_HOLD			0x06
#define R_CAL_HOLD			0x07
#define R_DOME_EN			0x08
#define R_CAL_CTRL			0x09
#define R_PAT_CTRL			0x0A
#define R_PAT_A0			0x0B
#define R_PAT_A1			0x0C
#define R_PAT_A2			0x0D
#define R_PAT_A3			0x0E
#define R_PAT_B0			0x0F
#define R_PAT_B1			0x10
#define R_PAT_B2			0x11
#define R_PAT_B3			0x12
#define R_PWM0				0x13
#define R_PWM1				0x14
#define R_PWM2				0x15
#define R_OUTPUT0			0x25


/* structs -------------------------------------------------------------------*/
/* Extern Variables ----------------------------------------------------------*/
/* Extern struncts -----------------------------------------------------------*/
/* functions -----------------------------------------------------------------*/
void Configure_CapTouch(void);
uint8_t Read_CapTouch(void);
void CapTouch_SleepModeEnable(void);

#endif  /* __CAPTOUCH_I2C_H */
