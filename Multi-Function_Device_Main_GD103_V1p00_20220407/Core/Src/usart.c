/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "tim.h"
#include "SystemProcess.h"

uint8_t Rx3Data[1];
uint8_t Rx3Packet[16];
uint8_t Cnt_Rx3Uart = 0;

uint8_t Rx1Data[1];
uint8_t Rx1Packet[16];
uint8_t Cnt_Rx1Uart = 0;

uint8_t BtRx1Packet[16];
uint8_t BtCnt_Rx1Uart = 0;

extern uint8_t Flag_ModeOp;
extern uint8_t Flag_KeyLock;

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PB6     ------> USART1_TX
    PB7     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_AFIO_REMAP_USART1_ENABLE();

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PB6     ------> USART1_TX
    PB7     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

    /* USART3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* uartHandle)
{
	static uint32_t temp_tecduty = 1;
	static uint8_t temp_voiceCnt = 0;

	if(uartHandle->Instance == USART1)
	{
		BtRx1Packet[BtCnt_Rx1Uart++] = Rx1Data[0];
		HAL_UART_Receive_IT(&huart1, (uint8_t*) Rx1Data, 1);

	}
	else if(uartHandle->Instance == USART3)
	{
		Cnt_Rx3Uart++;
		Rx3Packet[Cnt_Rx3Uart-1] = Rx3Data[0];

		switch(Rx3Data[0])
		{
		case 'a':
		case 'A':
			SysMode.OperationMode++;
			if(SysMode.OperationMode > EYECARE_MODE)
			{
				SysMode.OperationMode = SOOTHING_MODE;
			}
			break;
		case 'b':
		case 'B':
//			SysMode.Flag_OpStatus = ENABLE_OUTPUT;
			break;
		case 'c':
		case 'C':
			Flag_KeyLock = 1;
			break;
		case 'd':
		case 'D':
			SysMode.OperationIntensity++;
			if(SysMode.OperationIntensity > LEVEL5){
				SysMode.OperationIntensity = LEVEL1;
			}
			break;
		case '+':
			temp_tecduty++;
			if(temp_tecduty > 1799)	temp_tecduty = 1799;
			htim3.Instance -> CCR3 = temp_tecduty;
			htim3.Instance -> CCR4 = temp_tecduty;

			printf("TEC DUTYRATE : %d\r\n", temp_tecduty);
			break;
		case '-':
			temp_tecduty--;
			if(temp_tecduty < 1)	temp_tecduty = 1;
			htim3.Instance -> CCR3 = temp_tecduty;
			htim3.Instance -> CCR4 = temp_tecduty;

			printf("TEC DUTYRATE : %d\r\n", temp_tecduty);
			break;
		case '0':
			printf(">> EMS Enabled\r\n");
			SysMode.OutputFunc |= OUTPUT_TENS_A;

			EMS_PulseEnable(EMS_A_MODE);
			break;
		case '1':
			temp_tecduty = 0;
			htim3.Instance -> CCR3 = temp_tecduty;
			htim3.Instance -> CCR4 = temp_tecduty;
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);
			TEC_DIR_LOW;
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

			printf("TEC DUTYRATE : %d\r\n", temp_tecduty);
			break;
		case '2':
			temp_tecduty = 0;
			htim3.Instance -> CCR3 = temp_tecduty;
			htim3.Instance -> CCR4 = temp_tecduty;
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
			TEC_DIR_HIGH;
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);

			printf("TEC DUTYRATE : %d\r\n", temp_tecduty);
			break;
		case '3':
			temp_tecduty = 0;
			htim3.Instance -> CCR3 = temp_tecduty;
			htim3.Instance -> CCR4 = temp_tecduty;
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);

			printf("TEC DUTYRATE : %d\r\n", temp_tecduty);
			break;
////		case '4':
////			RFHV_ENABLE;
////			printf("Enable High Voltage\r\n");
////			break;
		case '5':
			/* Enable Positive Ion */
			printf(">> Ion(+) Enabled\r\n");
			SysMode.OutputFunc |= OUTPUT_ION_P;

			EMS_PulseEnable(ION_P_MODE);
			break;
		case '6':
			/* Enable Negative Ion */
			printf(">> Ion(-) Enabled\r\n");
			SysMode.OutputFunc |= OUTPUT_ION_N;

			EMS_PulseEnable(ION_N_MODE);
			break;
		case '7':
			/* Disable Ion Function */
			printf(">> Ion(-) Disabled\r\n");
			SysMode.OutputFunc &= ~OUTPUT_ION_P;
			SysMode.OutputFunc &= ~OUTPUT_ION_N;

			EMS_PulseDisable();
			break;
		case '8':
			printf(">> 1MHz RF Enabled\r\n");
			SysMode.OutputFunc |= OUTPUT_RF;

			RF_PulseEnable();
			RF_ENABLE;
			break;
		case '9':
			printf(">> 1MHz RF Disabled\r\n");
			SysMode.OutputFunc &= ~OUTPUT_RF;

			RF_DISABLE;
			RF_PulseDisable();
			break;
		}


		Cnt_Rx3Uart = 0;

		HAL_UART_Receive_IT(&huart3, (uint8_t*) Rx3Data, 1);
	}
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
