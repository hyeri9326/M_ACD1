/**
* Multi-function Device Main Program
*
* Copyright 2021 by Home beauty Device Team, CLASSYS Inc.
* All rights reserved.
*
* I2C_IOExpander.h : This file contains Defines, Extern Variables and etc.
to control I/O Expander IC.
*
* @author     Park, Sungjun
* @version    V0.00
* @date       09.06.2021
*/

#ifndef INC_SYSTEMPROCESS_H_
#define INC_SYSTEMPROCESS_H_

/* includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>

/* defines -------------------------------------------------------------------*/
#define TRUE 				1
#define FALSE				0

#define POWERON_MODE		1
#define POWEROFF_MODE		0

#define ENABLE_OUTPUT		1
#define DISABLE_OUTPUT		0

#define OUTPUT_NONE			0x00
#define OUTPUT_RF			0x01
#define OUTPUT_TENS_A		0x02
#define OUTPUT_TENS_B		0x04
#define OUTPUT_ION_P		0x08
#define OUTPUT_ION_N		0x10
#define OUTPUT_COOL			0x20
#define OUTPUT_HEAT			0x40

#define TYPE_LED1			0x01
#define TYPE_LED2			0x02
#define TYPE_LED3			0x04
#define TYPE_LED4			0x08

#define HEAD_NONE			0
#define HEAD_IR				1
#define HEAD_BLUE			2
#define HEAD_RED			3
#define HEAD_IRRED			4

#define	POWER_OFF		0
#define	POWER_ON		1
#define	MODE_CHG		2
#define	TREATMENT		3
#define	LOW_BAT			4
#define	CHARGING		5
#define	BT_PAIR			6
#define	BT_CONNECTED	7
#define	BT_STANDBY		8

#define EMS_NONE			0xA0
#define EMS_A_MODE			0xA1	// Head In & Head Out
#define EMS_B_MODE			0xA2	// Head Out & Hand
#define ION_P_MODE			0xA3
#define ION_N_MODE			0xA4

#define ADC_BATTERY			0
#define ADC_TENSVOLT		1
#define ADC_CURRENT			2
#define ADC_HEADTEMP		3
#define ADC_CONTACT_HAND	4
#define ADC_CONTACT_HEAD	5

#define KEY_UNLOCKED		0
#define KEY_LOCKED			1

/* structs -------------------------------------------------------------------*/
// Operation Mode Details
// CLEANSING MODE : RF and Ion Cleansing
// EYECARE MODE : RF + Micro-Current
// MOISTURE MODE :RF + DYHP(Dynamic Hyer Pulse) + LED(Red/Orange)
// LIFTING MODE : RF + Micro-Current
// COOLING MODE : TEC(Peltier) + LED(Blue)
// TIGHTENING MODE : EMS + LED(Red)
enum OperationMode
{
	STANDBY_MODE			= 1,
	SOOTHING_MODE,
	BLEMISH_CARE,
	MOISTURE_MODE,
	COLLAGEN_MODE,
	TIGHTENING_MODE,
	LIFTING_MODE,
	EYECARE_MODE,
	BLECONN_MODE,
	SLEEP_MODE,
	TEST_MODE
};

enum OperationIntensity
{
	LEVEL1					= 1,
	LEVEL2,
	LEVEL3,
	LEVEL4,
	LEVEL5
};

typedef struct
{
	uint8_t PowerStatus;
	uint8_t ChargingStatus;
	uint8_t OperationMode;
	uint8_t Flag_OpStatus;
	uint8_t OperationIntensity;
	uint8_t SideLEDColor;
	uint8_t OutputFunc;
	uint32_t Cnt_AutoStart;
	uint32_t Cnt_AutoSleep;
	uint8_t KeyLockStatus;
} SystemModeTypeDef;

typedef struct
{
	uint16_t BattVolt;
	uint16_t TensVolt;
	uint16_t AdCurrent;
	uint16_t HeadTemp;
	uint16_t ContactHand;
	uint16_t ContactHead;
} SystemParameterDef;

typedef struct {
	uint8_t stx;
	uint8_t cmd;
	uint8_t mode;
	uint8_t power_en;
	uint8_t hv_en;
	uint8_t rf_r;
	uint8_t rf;
	uint8_t adc_read;
	uint8_t tec;
	uint8_t tec_dir;
	uint8_t led_red;
	uint8_t led_blue;
	uint8_t led_ir;
	uint8_t ioe_int;
	uint8_t tens;
	uint8_t lcd_cmd;
	uint8_t reset;
	uint8_t etx;
} UART_RxPacket;

typedef struct
{
  uint8_t	pulseEnable;
} EMS_ModeTypeDef;

/* Extern Variables ----------------------------------------------------------*/


/* Extern struncts -----------------------------------------------------------*/
SystemModeTypeDef	SysMode;
SystemModeTypeDef	Pre_SysMode;

UART_RxPacket		rxPacket;
//UART_Send_Packet	txPacket;

/* functions -----------------------------------------------------------------*/
void BlinkingStatusLED(uint8_t lednum);
void HeadLEDControl(uint8_t ledcolor, uint8_t ledbright);
void DiffusionLEDControl(uint8_t mode);
void StandbyMode_Process(void);
void SoothingMode_Process(void);
void BlemishCare_Process(void);
void MoistureMode_Process(void);
void CollagenMode_Process(void);
void TighteningMode_Process(void);
void LiftingMode_Process(void);
void EyeCare_Process(void);
void Enable_Cooling(uint32_t dutyHeater);
void Disable_Cooling(void);
void RF_PulseEnable(void);
void RF_PulseDisable(void);
void Enable_Heating(uint32_t dutyHeater);
void Disable_Heating(void);
void Cooling_PulseEnable(void);
void Cooling_PulseDisable(void);
void EMS_PulseEnable(uint8_t emsMode);
void EMS_PulseDisable(void);
void EMSFunctionControl(uint8_t emsMode, uint32_t emsIntensity);
void Display_BackgroundImg(void);
void BleConnection_Process(void);

#endif /* INC_SYSTEMPROCESS_H_ */
