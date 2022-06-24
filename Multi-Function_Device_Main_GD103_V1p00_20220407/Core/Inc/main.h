/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern uint8_t flag_1kHz;
extern uint8_t flag_100Hz;
extern uint8_t flag_10Hz;
extern uint8_t flag_2Hz;

extern uint8_t g_ioexpval;

extern uint8_t Rx1Data[1];
extern uint8_t Rx1Packet[16];
extern uint8_t Rx3Data[1];
extern uint8_t Rx3Packet[16];

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void delay_us(uint16_t us);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BLE_WAKEUP_Pin GPIO_PIN_13
#define BLE_WAKEUP_GPIO_Port GPIOC
#define CHG_STAT_Pin GPIO_PIN_15
#define CHG_STAT_GPIO_Port GPIOC
#define CHG_STAT_EXTI_IRQn EXTI15_10_IRQn
#define ADC_MUX0_Pin GPIO_PIN_0
#define ADC_MUX0_GPIO_Port GPIOC
#define ADC_MUX1_Pin GPIO_PIN_1
#define ADC_MUX1_GPIO_Port GPIOC
#define ADC_MUX2_Pin GPIO_PIN_2
#define ADC_MUX2_GPIO_Port GPIOC
#define TEC_DIR_Pin GPIO_PIN_3
#define TEC_DIR_GPIO_Port GPIOC
#define SPK_SCL_Pin GPIO_PIN_5
#define SPK_SCL_GPIO_Port GPIOA
#define SPK_SDA_Pin GPIO_PIN_6
#define SPK_SDA_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_7
#define LED_GPIO_Port GPIOA
#define MOTOR_CTL_Pin GPIO_PIN_4
#define MOTOR_CTL_GPIO_Port GPIOC
#define OUT_SEL2_M_Pin GPIO_PIN_5
#define OUT_SEL2_M_GPIO_Port GPIOC
#define LED_RING_Pin GPIO_PIN_2
#define LED_RING_GPIO_Port GPIOB
#define CS_INT_Pin GPIO_PIN_12
#define CS_INT_GPIO_Port GPIOB
#define CS_INT_EXTI_IRQn EXTI15_10_IRQn
#define VOICE_BUSY_Pin GPIO_PIN_13
#define VOICE_BUSY_GPIO_Port GPIOB
#define BLE_PD_Pin GPIO_PIN_14
#define BLE_PD_GPIO_Port GPIOB
#define BLE_RST_Pin GPIO_PIN_15
#define BLE_RST_GPIO_Port GPIOB
#define RF_PWM_Pin GPIO_PIN_11
#define RF_PWM_GPIO_Port GPIOA
#define nEN_TENS_Pin GPIO_PIN_12
#define nEN_TENS_GPIO_Port GPIOA
#define RF_ENABLE_Pin GPIO_PIN_15
#define RF_ENABLE_GPIO_Port GPIOA
#define LCD_RS_Pin GPIO_PIN_11
#define LCD_RS_GPIO_Port GPIOC
#define LCD_RES_Pin GPIO_PIN_12
#define LCD_RES_GPIO_Port GPIOC
#define LCD_CS_Pin GPIO_PIN_2
#define LCD_CS_GPIO_Port GPIOD
#define OUT_SEL1_M_Pin GPIO_PIN_4
#define OUT_SEL1_M_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
/* Model Information */
#define	MODELNAME			"KUIU"
#define	HW_VERSION			"1.00"
#define	FW_VERSION			"1.00"
#define	BUILD_DATE			"2022.01.07."
#define	SERIALNUM			"CHD210100001"

/* Definition of GPIO */
#define LED_ON				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET)
#define LED_OFF				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET)
#define LED_TOG				HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin)

/* Definition of Bluetooth Control */
#define BLE_ON				HAL_GPIO_WritePin(BLE_RST_GPIO_Port, BLE_RST_Pin, GPIO_PIN_SET)
#define BLE_OFF				HAL_GPIO_WritePin(BLE_RST_GPIO_Port, BLE_RST_Pin, GPIO_PIN_RESET)

#define BLE_PD_HIGH			HAL_GPIO_WritePin(BLE_PD_GPIO_Port, BLE_PD_Pin, GPIO_PIN_SET)
#define BLE_PD_LOW			HAL_GPIO_WritePin(BLE_PD_GPIO_Port, BLE_PD_Pin, GPIO_PIN_RESET)

#define BLE_WAKEUP_HIGH		HAL_GPIO_WritePin(BLE_WAKEUP_GPIO_Port, BLE_WAKEUP_Pin, GPIO_PIN_SET)
#define BLE_WAKEUP_LOW		HAL_GPIO_WritePin(BLE_WAKEUP_GPIO_Port, BLE_WAKEUP_Pin, GPIO_PIN_RESET)

/* Definition of TENS Control */
#define TENS_A_LOW			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET)
#define TENS_A_HIGH			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET)

#define TENS_B_LOW			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET)
#define TENS_B_HIGH			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET)

#define TENS_C_LOW			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET)
#define TENS_C_HIGH			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET)

#define ENABLE_TENS			HAL_GPIO_WritePin(nEN_TENS_GPIO_Port, nEN_TENS_Pin, GPIO_PIN_RESET)
#define DISABLE_TENS		HAL_GPIO_WritePin(nEN_TENS_GPIO_Port, nEN_TENS_Pin, GPIO_PIN_SET)

/* Definition of TEC Control */
#define TEC_DIR_HIGH		HAL_GPIO_WritePin(TEC_DIR_GPIO_Port, TEC_DIR_Pin, GPIO_PIN_SET)
#define TEC_DIR_LOW			HAL_GPIO_WritePin(TEC_DIR_GPIO_Port, TEC_DIR_Pin, GPIO_PIN_RESET)

/* Definition of Output path Control */
#define OUT_SEL1_HIGH		HAL_GPIO_WritePin(OUT_SEL1_M_GPIO_Port, OUT_SEL1_M_Pin, GPIO_PIN_SET)
#define OUT_SEL1_LOW		HAL_GPIO_WritePin(OUT_SEL1_M_GPIO_Port, OUT_SEL1_M_Pin, GPIO_PIN_RESET)
#define OUT_SEL1_TOG		HAL_GPIO_TogglePin(OUT_SEL1_M_GPIO_Port, OUT_SEL1_M_Pin)

#define OUT_SEL2_HIGH		HAL_GPIO_WritePin(OUT_SEL2_M_GPIO_Port, OUT_SEL2_M_Pin, GPIO_PIN_SET)
#define OUT_SEL2_LOW		HAL_GPIO_WritePin(OUT_SEL2_M_GPIO_Port, OUT_SEL2_M_Pin, GPIO_PIN_RESET)
#define OUT_SEL2_TOG		HAL_GPIO_TogglePin(OUT_SEL2_M_GPIO_Port, OUT_SEL2_M_Pin)

/* Definition of LED Control */
#define INDICATOR_ON		HAL_GPIO_WritePin(LED_RING_GPIO_Port, LED_RING_Pin, GPIO_PIN_SET)
#define INDICATOR_OFF		HAL_GPIO_WritePin(LED_RING_GPIO_Port, LED_RING_Pin, GPIO_PIN_RESET)
#define INDICATOR_TOG		HAL_GPIO_TogglePin(LED_RING_GPIO_Port, LED_RING_Pin)

/* Definition of RF Control */
#define RF_ENABLE			HAL_GPIO_WritePin(RF_ENABLE_GPIO_Port, RF_ENABLE_Pin, GPIO_PIN_RESET)
#define RF_DISABLE			HAL_GPIO_WritePin(RF_ENABLE_GPIO_Port, RF_ENABLE_Pin, GPIO_PIN_SET)

//#define RFHV_ENABLE			HAL_GPIO_WritePin(HV_EN_GPIO_Port, HV_EN_Pin, GPIO_PIN_SET)
//#define RFHV_DISABLE		HAL_GPIO_WritePin(HV_EN_GPIO_Port, HV_EN_Pin, GPIO_PIN_RESET)

/* Definition of Voice IC Control */
#define SPK_CLK_HIGH		HAL_GPIO_WritePin(SPK_SCL_GPIO_Port, SPK_SCL_Pin, GPIO_PIN_SET)
#define SPK_CLK_LOW			HAL_GPIO_WritePin(SPK_SCL_GPIO_Port, SPK_SCL_Pin, GPIO_PIN_RESET)

#define SPK_DAT_HIGH		HAL_GPIO_WritePin(SPK_SDA_GPIO_Port, SPK_SDA_Pin, GPIO_PIN_SET)
#define SPK_DAT_LOW			HAL_GPIO_WritePin(SPK_SDA_GPIO_Port, SPK_SDA_Pin, GPIO_PIN_RESET)

/* Definition of Voice IC Control */
#define MUX0_SET			HAL_GPIO_WritePin(ADC_MUX0_GPIO_Port, ADC_MUX0_Pin, GPIO_PIN_SET)
#define MUX0_RESET			HAL_GPIO_WritePin(ADC_MUX0_GPIO_Port, ADC_MUX0_Pin, GPIO_PIN_RESET)

#define MUX1_SET			HAL_GPIO_WritePin(ADC_MUX1_GPIO_Port, ADC_MUX1_Pin, GPIO_PIN_SET)
#define MUX1_RESET			HAL_GPIO_WritePin(ADC_MUX1_GPIO_Port, ADC_MUX1_Pin, GPIO_PIN_RESET)

#define MUX2_SET			HAL_GPIO_WritePin(ADC_MUX2_GPIO_Port, ADC_MUX2_Pin, GPIO_PIN_SET)
#define MUX2_RESET			HAL_GPIO_WritePin(ADC_MUX2_GPIO_Port, ADC_MUX2_Pin, GPIO_PIN_RESET)

/* Definition of LCD Control */
#define LCD_RESET_ENABLE	HAL_GPIO_WritePin(LCD_RES_GPIO_Port, LCD_RES_Pin, GPIO_PIN_SET)
#define LCD_RESET_DISABLE	HAL_GPIO_WritePin(LCD_RES_GPIO_Port, LCD_RES_Pin, GPIO_PIN_RESET)

/* Definition of Motor Control */
#define MOTOR_ON			HAL_GPIO_WritePin(MOTOR_CTL_GPIO_Port, MOTOR_CTL_Pin, GPIO_PIN_SET)
#define MOTOR_OFF			HAL_GPIO_WritePin(MOTOR_CTL_GPIO_Port, MOTOR_CTL_Pin, GPIO_PIN_RESET)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
