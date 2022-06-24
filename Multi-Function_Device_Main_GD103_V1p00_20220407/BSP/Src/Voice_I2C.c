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
#include "Voice_I2C.h"
#include "main.h"
#include "i2c.h"
#include "gpio.h"

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t speaker_en = 0;
/* functions -----------------------------------------------------------------*/
/*******************************************************************************
 * Function Name  : Read_IOExpander
 * Description    : Read Data from IO Expander
 * Input          : None
 * Output         : None
 *******************************************************************************/
void Init_Voice(uint8_t DDATA) {

	//static uint8_t m_ledstatus = 0;
	uint8_t memVal[1];

	memVal[0] = 0xF3;
	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_VOICE, P_DATA, 1, (uint8_t*) memVal, 1,10);
	memVal[0] = 0x02;
	HAL_I2C_Mem_Write(&hi2c1, ADDR_SLAVE_VOICE, P_DATA, 1, (uint8_t*) memVal, 1,10);
}


/*******************************************************************************
* Function Name  : Line_1A_WT588F
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void Line_1A_WT588F(uint8_t ddata)
{
	uint8_t sdata, j;
	uint8_t bdata;

	SPK_CLK_HIGH;
	HAL_Delay(8);

	sdata = ddata;
	SPK_CLK_LOW;
	HAL_Delay(5);
	bdata = sdata & 0x01;

	for(j = 0; j < 8; j++)
	{
		if(bdata == 0)
		{
			SPK_CLK_HIGH;
			delay_us(170);
			SPK_CLK_LOW;
			delay_us(430);
		}
		else
		{
			SPK_CLK_HIGH;
			delay_us(430);
			SPK_CLK_LOW;
			delay_us(170);
		}

		sdata = sdata >> 1;
		bdata = sdata & 0x01;
	}

	SPK_CLK_HIGH;
	//HAL_Delay(2);
	//SPK_CLK_LOW;
}


/*******************************************************************************
* Function Name  : delay_us
* Description    : Initiate system parameters
* Input          : None
* Output         : None
*******************************************************************************/
void Line_2A_WT588F(uint8_t ddata)
{
	uint8_t sdata, j;
	uint8_t bdata;

	SPK_CLK_HIGH;
	SPK_DAT_HIGH;

	sdata = ddata;
	SPK_CLK_LOW;
	HAL_Delay(5);
	bdata = sdata & 0x01;

	for(j = 0; j < 8; j++)
	{
		SPK_CLK_LOW;
		if(bdata == 0)	SPK_DAT_LOW;
		else			SPK_DAT_HIGH;

		delay_us(100);
		SPK_CLK_HIGH;
		delay_us(100);

		sdata = sdata >> 1;
		bdata = sdata & 0x01;
	}

	SPK_CLK_HIGH;
	SPK_DAT_HIGH;
}


/*******************************************************************************
* Function Name  : delay_us
* Description    : Initiate system parameters
* Input          : None
* Output         : None
*******************************************************************************/
void VoiceOutput(uint8_t voiceNum)
{
	//USER_I2C1toGPIO();
	if(speaker_en){
		Line_1A_WT588F(voiceNum);
	}
	//USER_I2C1_ReInit();
}
