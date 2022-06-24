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
#ifndef __Voice_I2C_H
#define __Voice_I2C_H

/* includes ------------------------------------------------------------------*/
#include "main.h"

/* defines -------------------------------------------------------------------*/
#define ADDR_SLAVE_VOICE	0xF3
#define P_DATA 				0x01
#define VOICE_1				0x00H

#define VOICE_LEVEL1		1//10
#define VOICE_LEVEL2		2//11
#define VOICE_LEVEL3		3//12
#define VOICE_LEVEL4		4//13
#define VOICE_LEVEL5		5//14
#define VOICE_ALARM1		6//25
#define VOICE_ALARM2		7//26	// Power Off
#define VOICE_ALARM3		8//27	// Power On
#define VOICE_ALARM4		9//28	// Treatment Done


#define CHV__20MS	0
#define CHV_FORE_HEAD	1
#define CHV_TIGHTENING_MODE	2
#define CHV_TREATMEND_START	3
#define CHV_TREATMEND_FINISHED	4
#define CHV_OPPOSITE_FACE	5
#define CHV_COLLAGEN_MODE	6
#define V_INTRO	7
#define V_BUTTON 8
#define V_PARING 9
#define V_CONNECTED 10
#define V_ERROR 11
#define V_LOCK 12
#define V_UNLOCK 13
#define V_CHARGING 14
#define V_OUTRO	15
#define ENV_BLEMISHCARE_MODE	16
#define ENV_COLLAGEN_MODE	17
#define V_DONE	18
#define ENV_EYECARE_MODE	19
#define ENV_FOREHEAD	20
#define ENV_LIFTING_MODE	21
#define ENV_MOISTURE_MODE	22
#define V_NEXT	23
#define ENV_OPPOSITE_FACE	24
#define V_PAGE	25
#define ENV_SOOTHING_MODE	26
#define ENV_TIGHTENING_MODE	27
#define ENV_TREATMENT_FINIISHED	28
#define ENV_TREATMENT_START	29
#define CHV_LIFTING_MODE	30
#define CHV_MOISTURE_MODE	31
#define CHV_BLEMISHCARE_MODE	32
#define CHV_SOOTHING_MODE	33
#define CHV_EYECARE_MODE	34

/* structs -------------------------------------------------------------------*/
/* Extern Variables ----------------------------------------------------------*/
/* Extern struncts -----------------------------------------------------------*/
/* functions -----------------------------------------------------------------*/
void Init_Voice(uint8_t DDATA);
void VoiceOutput(uint8_t voiceNum);

#endif  /* __Voice_I2C_H__ */
