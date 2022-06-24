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
#include "IOExpander_I2C.h"
#include "main.h"
#include "i2c.h"
#include "gpio.h"

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* functions -----------------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Write_IOExpander
* Description    : Configure IO Expander
* Input          : None
* Output         : None
*******************************************************************************/
void Configure_IOExpander(uint8_t io_data)
{
	uint8_t memVal[1];

	memVal[0] = io_data;

	HAL_I2C_Mem_Write(&hi2c1, 0x70, 0x03, 1, (uint8_t*)memVal, 1, 10);
}


/*******************************************************************************
* Function Name  : Write_IOExpander
* Description    : Write Data to IO Expander
* Input          : None
* Output         : None
*******************************************************************************/
void Write_IOExpander(uint8_t io_data)
{
	uint8_t memVal[1];

	memVal[0] = io_data;

	HAL_I2C_Mem_Write(&hi2c1, 0x70, 0x01, 1, (uint8_t*)memVal, 1, 10);
}

/*******************************************************************************
* Function Name  : Read_IOExpander
* Description    : Read Data from IO Expander
* Input          : None
* Output         : None
*******************************************************************************/
uint8_t Read_IOExpander(void)
{
	uint8_t m_readData;
	uint8_t memVal[1];

	memVal[0] = 0;

	HAL_I2C_Master_Receive(&hi2c1, 0x40, &memVal[0], 1, 10);

	m_readData = memVal[0];

	return m_readData;
}
