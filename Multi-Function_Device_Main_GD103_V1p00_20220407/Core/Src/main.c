/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f1xx_it.h"
#include "SystemProcess.h"
#include "KeyScanTask.h"
#include "CapTouch_I2C.h"
#include "Voice_I2C.h"
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "user_func.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEBUG_MODE__

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t temp_key;
__IO uint32_t adcVal[1];

extern uint8_t Rx1Packet[16];
extern uint8_t Cnt_Rx1Uart;

uint8_t Tx1Packet[16];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

static void System_Init_Config(void);
void StopMode(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*******************************************************************************
* Function Name  : delay_us
* Description    : Initiate system parameters
* Input          : None
* Output         : None
*******************************************************************************/
void delay_us(uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&htim6, 0);				// set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(&htim6) < us);		// wait for the counter to reach the us input in the parameter
}


/*******************************************************************************
* Function Name  : System_Information
* Description    : Display system information
* Input          : None
* Output         : None
*******************************************************************************/
void System_Information(void)
{
    printf("================= %s =================\r\n", MODELNAME);
    printf("  VERSION   : H/W %s, S/W %s\r\n", HW_VERSION, FW_VERSION);
    printf("  BUILD DATE: %s              \r\n\n", BUILD_DATE);

    printf("  SYSCLK_Frequency = %lu\r\n", HAL_RCC_GetSysClockFreq());
    printf("  HCLK_Frequency = %lu\r\n", HAL_RCC_GetHCLKFreq());
    printf("  PCLK1_Frequency = %lu\r\n", HAL_RCC_GetPCLK1Freq());
    printf("  PCLK2_Frequency = %lu\r\n", HAL_RCC_GetPCLK2Freq());
    printf("=========================================\r\n\n");
}


/*******************************************************************************
* Function Name  : System_Init_Config
* Description    : Initiate system parameters
* Input          : None
* Output         : None
*******************************************************************************/
static void System_Init_Config(void)
{
	/* Display Device Information */
	System_Information();

	/* UART Initiation */
	HAL_UART_Receive_IT(&huart1, (uint8_t*) Rx1Data, 1);
	HAL_UART_Receive_IT(&huart3, (uint8_t*) Rx3Data, 1);

	/* TIMER Initiation */
	/* TIM6 is main timer for Scheduling */
	HAL_TIM_Base_Start_IT(&htim6);

	// TIM1 Ch1, Ch2, Ch3 are used for Head LED Control
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);

	// TIM1 Ch4 is used for driving source of TENS intensity
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4);

	// TIM1 Ch1, Ch2, Ch3 are used for Head LED Control
	DiffusionLEDControl(POWER_OFF);
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);

	// TIM3 Ch1 is used for driving source of TEC(Peltier) Module
	// There is a problem with SPI1 and TIM3_CH1 remapping
	// Conflict between the TIM3_CH1 signal and the I2C1 MISO signal, (even if MISO is not used).
	//HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
	TEC_DIR_HIGH;
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);

	// TIM8 Ch1, Ch2, Ch3 is used for driving source of TENS and Galvanic function
	//htim8.Instance -> PSC = 71;								// Set Prescaler
	//htim8.Instance -> ARR = 999;								// Set Period
	//htim8.Instance -> CCR4 = 499;								// Set Duty(Pulse)
	HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_3);

	// TIM8 Ch4 is used for driving source of RF function
	//htim8.Instance -> PSC = 1;								// Set Prescaler, 63
	//htim8.Instance -> ARR = 35;								// Set Period, 999
	//htim8.Instance -> CCR4 = 18;								// Set Duty(Pulse), 499
	HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_4);

	/* ADC Initiation */
	HAL_ADC_Start(&hadc1);

	/* Key Function Initiation */
	KeyVariableInit();
	// Capacitive Touch Driver Initiation
	Configure_CapTouch();

	/* Bluetooth Initiation */
//	BLE_OFF;
//	BLE_PD_LOW;
//	BLE_WAKEUP_LOW;
	BLE_ON;

	DISABLE_TENS;
	/* GPIO Control */
	//RF_ENABLE;
	//RF_DISABLE;
	//RFHV_DISABLE;

//	OUT_SEL1_HIGH;
//	OUT_SEL2_HIGH;
//	DISABLE_TENS;

	SysMode.PowerStatus = POWEROFF_MODE;
	SysMode.OperationMode = STANDBY_MODE;
	SysMode.OperationIntensity = LEVEL1;
	SysMode.Flag_OpStatus = DISABLE_OUTPUT;
	SysMode.Cnt_AutoStart = 0;

	//USER_I2C1toGPIO();
	//
	//  Line_1A_WT588F(0xEF);
	//  HAL_Delay(500);

	/* Check STANDBY Wake-up */
	if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB))
	{
		printf("Standby Wake-up\r\n");
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU | PWR_FLAG_SB);
	}

	//printf("\n>> Enter Stop mode\r\n");
	//StopMode();						// Stop mode is done by EXTI

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
//	static uint32_t temp_test = 0;
//	char buf_atcom[16];
	//static uint8_t cnt_sound = 0;
	char Tx1Buf[1];
	uint8_t j;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();
  MX_TIM8_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM3_Init();
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */
  System_Init_Config();

//  htim3.Instance -> CCR4 = 450;
//  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);


//  // TIM1 Ch4 is used for driving source of TENS intensity
//  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
//  htim1.Instance -> CCR4 = 10;
//
//  while(1)
//  {
//	  if(flag_10Hz)
//	  {
//		  flag_10Hz = 0;
//		  // TENS Level Control
//		  temp_test += 5;
//		  if(temp_test >= 250)
//		  {
//			  temp_test = 0;
//		  }
//		  htim1.Instance -> CCR4 = temp_test;						// Set Duty(Pulse)
//	  }
//  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
/*
	  if(flag_2Hz){
		  flag_2Hz = 0;
		  set_systemstatus();
		  BT_Proc();
	  }
*/
	  HeadLEDControl(HEAD_BLUE, 10);
	  HAL_Delay(500);
	  HeadLEDControl(HEAD_RED, 10);
	  HAL_Delay(500);
	  HeadLEDControl(HEAD_IR, 10);
	  HAL_Delay(500);

/*
	  switch(SysMode.OperationMode)
	  {
	  case STANDBY_MODE:		StandbyMode_Process();		break;
	  case SOOTHING_MODE:		SoothingMode_Process();		break;
	  case BLEMISH_CARE:		BlemishCare_Process();		break;
	  case MOISTURE_MODE:		MoistureMode_Process();		break;
	  case COLLAGEN_MODE:		CollagenMode_Process();		break;
	  case TIGHTENING_MODE:		TighteningMode_Process();	break;
	  case LIFTING_MODE:		LiftingMode_Process();		break;
	  case EYECARE_MODE:		EyeCare_Process();			break;
	  case BLECONN_MODE:		BleConnection_Process();	break;
	  default:
		  Tx1Packet[0] = 'A';
		  Tx1Packet[1] = 'T';
		  Tx1Packet[2] = '+';
		  Tx1Packet[3] = 'S';
		  Tx1Packet[4] = 'L';
		  Tx1Packet[5] = 'E';
		  Tx1Packet[6] = 'E';
		  Tx1Packet[7] = 'P';
		  Tx1Packet[8] = '\n';

		  for(j = 0; j < 9; j++)
		  {
			  Tx1Buf[0] = Tx1Packet[j];
			  HAL_UART_Transmit(&huart1, (uint8_t *)&Tx1Buf, 1, 0xFFFF);
		  }

		  //StandbyMode();
		  StopMode();

		  KeyVariableInit();
		  SysMode.OperationMode = STANDBY_MODE;

		  NVIC_SystemReset();

		  break;
	  }
*/
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //Line_1A_WT588F(0xF3);
	  //Line_2A_WT588F(0xF3);
	  //SPK_CLK_HIGH;
	  //HAL_Delay(2);
	  //delay_us(300);

	  //cnt_sound++;
	  //Line_1A_WT588F(cnt_sound);
	  //Line_2A_WT588F(0x01);
	  //SPK_CLK_LOW;
	  //HAL_Delay(2000);
	  //delay_us(300);

//
//	  if(flag_10Hz)
//	  {
//		  flag_10Hz = 0;
//
//		  LedToggle_CapTouch();
//		  LED_TOG;
//		  OUT_SEL1_TOG;
//
//		  temp_key = Read_CapTouch();
//		  printf("Key = %x\r\n", temp_key);
//
//		  /*// TENS Level Control
//		  temp_test += 10;
//		  if(temp_test >= 2000)
//		  {
//			  temp_test = 0;
//		  }
//		  //htim1.Instance -> CCR4 = temp_test;						// Set Duty(Pulse)
//		  */
//	  }
//
//	  if(flag_2Hz)
//	  {
//		  flag_2Hz = 0;
//
//		  //VoiceOutput(VOICE_ALARM2);
//		  //printf("%c\r\n", Rx1Packet[0]);
//	  }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

void USART1_Printf(char *pch)
{
	while(*pch != '\0')
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)&pch, 1, 0xFFFF);
		pch++;
	}
}


/*******************************************************************************
* Function Name  : SleepMode
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void StandbyMode(void)
{
	/* Wait 5 Seconds */
	HAL_Delay(5000);

	/* Enable Power Peripheral */
	//__HAL_RCC_PWR_CLK_ENABLE();

	/* STOP Mode */
	HAL_PWR_EnterSTANDBYMode();
}


/*******************************************************************************
* Function Name  : SleepMode
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void SleepMode(void)
{
	/* Suspend SysTick */
	HAL_SuspendTick();

	/* Enable Power Peripheral */
	__HAL_RCC_PWR_CLK_ENABLE();

	/* Stop Timer Interrup */
	HAL_TIM_Base_Stop_IT(&htim6);

	/* Sleep Mode */
	HAL_PWR_EnterSLEEPMode(0, PWR_SLEEPENTRY_WFI);

	/* Resum SysTick When System Wake-up */
	HAL_ResumeTick();
}


/*******************************************************************************
* Function Name  : SleepMode
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void StopMode(void)
{
	static uint32_t Cnt_StopMode = 0;
	uint8_t temp_keytouch;

	while(1)
	{
		if(flag_2Hz)
		{
			flag_2Hz = 0;

			LED_TOG;

			if(SysMode.OperationMode == SLEEP_MODE)
			{
				Cnt_StopMode++;
				if(Cnt_StopMode > 6)
				{
					temp_keytouch = Read_CapTouch();
					printf("key = %02x\r\n", temp_keytouch);
					if(temp_keytouch == 0)
					{
						Cnt_StopMode = 0;

						break;
					}
				}
			}
			else
			{
				break;
			}
		}
	}



	if(SysMode.PowerStatus == POWEROFF_MODE)
	{
		printf("Cnt_StopMode = %d\r\n", (uint16_t)Cnt_StopMode);
		LED_OFF;

		/* Suspend SysTick */
		HAL_SuspendTick();

		/* Enable Power Peripheral */
		//__HAL_RCC_PWR_CLK_ENABLE();

		/* STOP Mode */
		HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

		/* Resume SysTick When System Wake-up */
		HAL_ResumeTick();

		/* PLL Clock Recovery */
		SystemClock_Config();

		SysMode.PowerStatus = POWERON_MODE;
	}

//	static uint32_t Cnt_StopMode = 0;
//
//	//HAL_Delay(3000);
//	while(1)
//	{
//		if(flag_2Hz)
//		{
//			flag_2Hz = 0;
//
//			LED_TOG;
//
//			Cnt_StopMode++;
//			if(Cnt_StopMode > 6)
//			{
//				Cnt_StopMode = 0;
//
//				/* Suspend SysTick */
//				HAL_SuspendTick();
//
//				/* Enable Power Peripheral */
//				__HAL_RCC_PWR_CLK_ENABLE();
//
//				/* STOP Mode */
//				HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
//
//				/* Resum SysTick When System Wake-up */
//				HAL_ResumeTick();
//
//				/* PLL Clock Recovery */
//				SystemClock_Config();
//			}
//		}
//
//		if(flag_100Hz)
//		{
//			flag_100Hz = 0;
//
//			KeyScanTask();							// Check Key Switch
//			Key_Function();
//		}
//
//		if(SysMode.OperationMode != SLEEP_MODE)
//		{
//			SysMode.OperationMode = STANDBY_MODE;
//		}
//	}
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
