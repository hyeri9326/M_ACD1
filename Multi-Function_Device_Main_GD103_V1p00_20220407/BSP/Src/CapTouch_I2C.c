/**
* Multi-function Device Main Program
*
* Copyright 2021 by Home beauty Device Team, CLASSYS Inc.
* All rights reserved.
*
* IOExpander_I2C.c : This file contains functions for I/O Expander via I2C interface
*
* @author     Park, Sungjun
* @version    V0.00
* @date       09.06.2021
*/
/* includes ------------------------------------------------------------------*/
#include "CapTouch_I2C.h"
#include "main.h"
#include "i2c.h"
#include "gpio.h"
#include "SystemProcess.h"

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* functions -----------------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Write_IOExpander
* Description    : Configure IO Expander
* Input          : None
* Output         : None
*******************************************************************************/
void Configure_CapTouch(void)
{
	uint8_t memVal[1];

	HAL_Delay(100);

	// Calibration Speed Control Register
	memVal[0] = 0x00;//0x59;
	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_CAL_CTRL, 1, (uint8_t*)memVal, 1, 10);
	HAL_Delay(10);

	// Sensitive Register
	memVal[0] = 0x95;
	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_SENSITIVE1, 1, (uint8_t*)memVal, 1, 10);	// Sensitive 1
	HAL_Delay(10);
	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_SENSITIVE2, 1, (uint8_t*)memVal, 1, 10);	// Sensitive 2
	HAL_Delay(10);

	// General Control Register
	memVal[0] = 0x02;
	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_CTRL1, 1, (uint8_t*)memVal, 1, 10);
	HAL_Delay(10);
	memVal[0] = 0x13;
	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_CTRL2, 1, (uint8_t*)memVal, 1, 10);
	HAL_Delay(10);

	// Channel 1~6 Sensing Control Register
	memVal[0] = 0x38;
	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_CH_HOLD, 1, (uint8_t*)memVal, 1, 10);
	HAL_Delay(10);

	// Channel 2~6 Dome Key Input Control Register
	memVal[0] = 0x00;
	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_DOME_EN, 1, (uint8_t*)memVal, 1, 10);
	HAL_Delay(10);

	// PWM Control Register
	memVal[0] = 0x1F;
	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_PWM0, 1, (uint8_t*)memVal, 1, 10);
	HAL_Delay(10);
	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_PWM1, 1, (uint8_t*)memVal, 1, 10);
	HAL_Delay(10);

//	// Pattern Sleep Sequence to Enter
//	memVal[0] = 0x9F;
//	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_PAT_CTRL, 1, (uint8_t*)memVal, 1, 10);
//	HAL_Delay(10);
//	memVal[0] = 0x01;
//	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_PAT_A0, 1, (uint8_t*)memVal, 1, 10);
//	HAL_Delay(10);
//	memVal[0] = 0x02;
//	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_PAT_A1, 1, (uint8_t*)memVal, 1, 10);
//	HAL_Delay(10);
//	memVal[0] = 0x01;
//	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_PAT_B0, 1, (uint8_t*)memVal, 1, 10);
//	HAL_Delay(10);
//	memVal[0] = 0x02;
//	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_PAT_B1, 1, (uint8_t*)memVal, 1, 10);
//	HAL_Delay(10);

	memVal[0] = 0x13;
	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_CTRL2, 1, (uint8_t*)memVal, 1, 10);
	HAL_Delay(10);
}


/*******************************************************************************
* Function Name  : Write_IOExpander
* Description    : Write Data to IO Expander
* Input          : None
* Output         : None
*******************************************************************************/
void Write_CapTouch(uint8_t io_data)
{
	uint8_t memVal[1];

	memVal[0] = io_data;

	HAL_I2C_Mem_Write(&hi2c1, 0xD2, 0x01, 1, (uint8_t*)memVal, 1, 10);
}

/*******************************************************************************
* Function Name  : Read_IOExpander
* Description    : Read Data from IO Expander
* Input          : None
* Output         : None
*******************************************************************************/
uint8_t Read_CapTouch(void)
{
	uint8_t m_readData;
	uint8_t memVal[1];

	memVal[0] = 0;

	HAL_I2C_Mem_Read(&hi2c1, ADDR_SLAVE_TS06, R_OUTPUT0, 1, &memVal[0], 1, 10);
	m_readData = memVal[0];

	return m_readData;
}


/*******************************************************************************
* Function Name  : Read_IOExpander
* Description    : Read Data from IO Expander
* Input          : None
* Output         : None
*******************************************************************************/
void LedToggle_CapTouch(void)
{
	static uint8_t m_ledstatus = 0;
	uint8_t memVal[1];

	switch(m_ledstatus)
	{
	case 0:
		memVal[0] = 0x1F;
		m_ledstatus = 1;
		break;
	case 1:
		memVal[0] = 0x00;
		m_ledstatus = 0;
		break;
	}


	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_PWM1, 1, (uint8_t*)memVal, 1, 10);
}


/*******************************************************************************
 * Function Name  : Write_IOExpander
 * Description    : Configure IO Expander
 * Input          : None
 * Output         : None
 *******************************************************************************/
void CapTouch_SleepModeEnable(void)
{
	uint8_t memVal[1];

	HAL_Delay(100);

	// Pattern Sleep Sequence to Enter
	memVal[0] = 0x92;
	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_PAT_CTRL, 1, (uint8_t*)memVal, 1, 10);
	HAL_Delay(10);
	memVal[0] = 0x02;
	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_PAT_A0, 1, (uint8_t*)memVal, 1, 10);
	HAL_Delay(10);
	memVal[0] = 0x01;
	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_PAT_A1, 1, (uint8_t*)memVal, 1, 10);
	HAL_Delay(10);
	memVal[0] = 0x02;
	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_PAT_B0, 1, (uint8_t*)memVal, 1, 10);
	HAL_Delay(10);
	memVal[0] = 0x01;
	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_PAT_B1, 1, (uint8_t*)memVal, 1, 10);
	HAL_Delay(10);

	memVal[0] = 0x37;
	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_CTRL2, 1, (uint8_t*)memVal, 1, 10);
	HAL_Delay(10);

	printf("Touch Sleep Mode Enabled\r\n");
}


/*******************************************************************************
 * Function Name  : Write_IOExpander
 * Description    : Configure IO Expander
 * Input          : None
 * Output         : None
 *******************************************************************************/
void CapTouch_SleepModeDisable(void)
{
	uint8_t memVal[1];

	HAL_Delay(100);

	// Pattern Sleep Sequence to Enter
	memVal[0] = 0x00;
	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_PAT_CTRL, 1, (uint8_t*)memVal, 1, 10);
	HAL_Delay(10);

	memVal[0] = 0x13;
	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_TS06, R_CTRL2, 1, (uint8_t*)memVal, 1, 10);
	HAL_Delay(10);

	printf("Touch Sleep Mode Disabled\r\n");
}
