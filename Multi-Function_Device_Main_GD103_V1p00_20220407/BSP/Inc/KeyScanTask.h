/**
* Multi-function Device Main Program
*
* Copyright 2021 by Home beauty Device Team, CLASSYS Inc.
* All rights reserved.
*
* KeyScanTask.h : This file contains Defines, Extern Variables and etc.
to control Key Switch.
*
* @author     Park, Sungjun
* @version    V0.00
* @date       09.06.2021
*/
#ifndef __KEYSCANTASK_H
#define __KEYSCANTASK_H

/* includes ------------------------------------------------------------------*/
#include "main.h"

/* defines -------------------------------------------------------------------*/
#define	KEY_BASE					0x18000000
#define ID_BASE    					0x18400000

#define KEY_STATE_UP				1
#define KEY_STATE_DEBOUNCE   		2
#define KEY_STATE_GET_KEY			3
#define KEY_STATE_WAIT	            4
#define KEY_STATE_WAIT_DEBOUNCE		5

#define KEY_DEBOUNCE_DLY  			2
#define KEY_RPT_DLY         		100
#define KEY_BUF_SIZE				10
#define RPT_OFFSET					0x10
#define OFF_OFFSET					0x10

#define OFFKEY 						0xAA

#define KEY1						0x01
#define KEY2						0x02
#define KEY3						0x03
#define KEY4						0x04
#define KEY5						0x05
#define KEY6						0x06
#define KEY7						0x07
#define KEY8						0x08

#define RPTKEY1						0x11
#define RPTKEY2						0x12
#define RPTKEY3						0x13
#define RPTKEY4						0x14
#define RPTKEY5						0x15
#define RPTKEY6						0x16
#define RPTKEY7						0x17
#define RPTKEY8						0x18

#define KEY1_PRESSED				0x01
#define KEY2_PRESSED				0x02
#define KEY3_PRESSED				0x03
#define KEY4_PRESSED				0x08
#define KEY5_PRESSED				0x10
#define KEY6_PRESSED				0x20
#define KEY7_PRESSED				0x40
#define KEY8_PRESSED				0x80

/* structs -------------------------------------------------------------------*/
/* Extern Variables ----------------------------------------------------------*/
/* Extern struncts -----------------------------------------------------------*/
/* functions -----------------------------------------------------------------*/
void KeyVariableInit(void);
void Key_Function(void);
void KeyScanTask(void);

#endif  /* __KEYSCANTASK_H */
