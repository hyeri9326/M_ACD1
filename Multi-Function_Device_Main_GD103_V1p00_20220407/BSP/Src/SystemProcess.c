/**
 * Multi-function Device Main Program
 *
 * Copyright 2021 by Home beauty Device Team, CLASSYS Inc.
 * All rights reserved.
 *
 * SystemProcess.c : This file contains functions for System Processing
 *
 * @author     Park, Sungjun
 * @version    V0.00
 * @date       09.06.2021
 */
/* includes ------------------------------------------------------------------*/
#include <a_image.bak>
#include "main.h"
#include "SystemProcess.h"
#include "tim.h"
#include "adc.h"
#include "IOExpander_I2C.h"
#include "KeyScanTask.h"
#include "CapTouch_I2C.h"
#include "Voice_I2C.h"
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "user_func.h"
#include "CapTouch_I2C.h"

/* Private define ------------------------------------------------------------*/
#define POWER_ON		1
#define POWER_OFF		0

#define LED_PERIOD		10

#define HEATER_DUTY			20
#define COOLING_DUTY_HIGH	400//40
#define COOLING_DUTY_LOW	300//35

/* Private variables ---------------------------------------------------------*/
EMS_ModeTypeDef			emsType;
SystemParameterDef		sysPara;

uint8_t Flag_LevelChg = 0;
uint8_t Flag_BLEConn = 0;
uint32_t BuffADCData[6];
uint8_t RxDebugData[8];
uint32_t cnt_redcolor = 0;
uint8_t Flag_KeyLock = 0;
uint8_t Flag_KeyUnlock = 0;
uint8_t Cnt_Keylock = 0;
uint8_t Flag_ProcessDone = 0;
uint8_t start_time = 0;
uint8_t chgstat = 0;

extern uint8_t language;


static uint8_t diff_cnt = 0;


uint16_t SineTable[32] = {250,299,346,389,427,458,481,495,
		500,495,481,458,427,389,346,299,
		250,201,154,111,73,42,19,5,
		0,5,19,42,73,111,154,201};

extern __IO uint32_t adcVal[3];
extern uint8_t Flag_CSInt;
extern unsigned char Flag_KeyPressed;

extern uint8_t speaker_en;
uint8_t e_time = 0;
uint8_t battery_level = 0;

/* functions -----------------------------------------------------------------*/
/*******************************************************************************
 * Function Name  : CheckMonitoringVoltage
 * Description    : Check Voltage
 * Input          : None
 * Output         : None
 *******************************************************************************/
void MonitoringSystemStatus(uint8_t cnt_adcSeq)
{
	static uint8_t cnt_LowBat = 0;
	static uint8_t cnt_LowBatDisp = 0;
	static uint32_t cnt_Contact = 0;
	static uint32_t cnt_HandContact = 0;
	static uint32_t cnt_HeadContact = 0;
	static uint32_t cnt_Current = 0;
	static uint32_t cnt_BattStable = 0;
	static uint8_t lv_chg_cnt1 = 0;
	static uint8_t lv_chg_cnt2 = 0;

	switch(cnt_adcSeq)
	{
	case ADC_BATTERY:
		MUX2_RESET;		MUX1_RESET;		MUX0_RESET;
		break;
	case ADC_TENSVOLT:
		MUX2_RESET;		MUX1_RESET;		MUX0_SET;
		break;
	case ADC_CURRENT:
		MUX2_RESET;		MUX1_SET;		MUX0_RESET;
		break;
	case ADC_HEADTEMP:
		MUX2_RESET;		MUX1_SET;		MUX0_SET;
		break;
	case ADC_CONTACT_HAND:
		MUX2_SET;		MUX1_RESET;		MUX0_RESET;
		break;
	case ADC_CONTACT_HEAD:
		MUX2_SET;		MUX1_RESET;		MUX0_SET;
		break;
	}

	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	adcVal[0] = HAL_ADC_GetValue(&hadc1);
	BuffADCData[cnt_adcSeq] = (uint32_t)((3300 * (float)adcVal[0] / 4096));

	sysPara.BattVolt = (uint32_t)((float)BuffADCData[ADC_BATTERY] * 3.2);
	sysPara.TensVolt = (uint32_t)((float)BuffADCData[ADC_TENSVOLT] * 12);
	sysPara.AdCurrent = (uint32_t)((float)BuffADCData[ADC_CURRENT] * 1);
	sysPara.HeadTemp = (uint32_t)((float)BuffADCData[ADC_HEADTEMP] * 1);
	sysPara.ContactHand = (uint32_t)((float)BuffADCData[ADC_CONTACT_HAND] * 1);
	sysPara.ContactHead = (uint32_t)((float)BuffADCData[ADC_CONTACT_HEAD] * 1);

	switch(cnt_adcSeq)
	{
	case ADC_BATTERY:
		if(SysMode.Flag_OpStatus == DISABLE_OUTPUT)
		{
			if(sysPara.BattVolt >= 8000)
			{
				if(battery_level != 3)
				{
					cnt_BattStable++;
					if(cnt_BattStable > 10)
					{
						cnt_BattStable = 10;
						battery_level = 3;
					}
				}
				else
				{
					cnt_BattStable = 0;
				}

			}
			else if((sysPara.BattVolt >= 7200) && (sysPara.BattVolt < 8000))
			{
				if(battery_level != 2)
				{
					cnt_BattStable++;
					if(cnt_BattStable > 10)
					{
						cnt_BattStable = 10;
						battery_level = 2;
					}
				}
				else
				{
					cnt_BattStable = 0;
				}
			}
			else
			{
				if(battery_level != 1)
				{
					cnt_BattStable++;
					if(cnt_BattStable > 10)
					{
						cnt_BattStable = 10;
						battery_level = 1;
					}
				}
				else
				{
					cnt_BattStable = 0;

					if(sysPara.BattVolt < 7000)
					{
						/*
						cnt_LowBat++;
						if(cnt_LowBat > 5)
						{
							cnt_LowBat = 0;

							if((SysMode.OperationMode >= SOOTHING_MODE) && (SysMode.OperationMode <= EYECARE_MODE))
							{
								DiffusionLEDControl(POWER_OFF);

								while(1)
								{
									printf("LOW BATTERY!!!!!\r\n");

									LCD_LOW_BATTERY();
									cnt_LowBatDisp++;
									if(cnt_LowBatDisp > 10)
									{
										cnt_LowBatDisp = 0;
										break;
									}
								}
							}

							SysMode.OperationMode = STANDBY_MODE;

							NVIC_SystemReset();
						}*/
					}
				}
			}
		}
		else
		{

		}

//		if(sysPara.AdCurrent < 1000)
//		{
//			if(sysPara.BattVolt >= 8000)
//			{
//				battery_level = 3;
//			}
//			else if((sysPara.BattVolt >= 7500) && (sysPara.BattVolt < 8000))
//			{
//				lv_chg_cnt1++;
//				if(lv_chg_cnt1 > 6)
//				{
//					lv_chg_cnt1 = 0;
//					battery_level = 2;
//				}
//			}
//			else
//			{
//				lv_chg_cnt2++;
//				if(lv_chg_cnt2 > 6)
//				{
//					lv_chg_cnt2 = 0;
//					battery_level = 1;
//
//					DiffusionLEDControl(LOW_BAT);
//				}
//
//				if(sysPara.BattVolt < 7300)
//				{
//					cnt_LowBat++;
//					if(cnt_LowBat > 5)
//					{
//						DiffusionLEDControl(POWER_OFF);
//						cnt_LowBat = 0;
//
//						/* Disable RF */
//						//RFHV_DISABLE;
//						RF_DISABLE;
//						RF_PulseDisable();
//
//						/* Disable Heater */
//						printf(">> Heater Disabled\r\n");
//						Disable_Heating();
//
//						SysMode.OutputFunc &= ~OUTPUT_COOL;
//						htim3.Instance->CCR4 = 0;
//						HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);
//
//						/* Disable Ion Function */
//						EMS_PulseDisable();
//
//						while(1)
//						{
//							printf("LOW BATTERY!!!!!\r\n");
//
//							LCD_LOW_BATTERY();
//							cnt_LowBatDisp++;
//							if(cnt_LowBatDisp > 10)
//							{
//								cnt_LowBatDisp = 0;
//								break;
//							}
//						}
//
//						SysMode.OperationMode = STANDBY_MODE;
//
//						NVIC_SystemReset();
//					}
//				}
//				else
//				{
//					cnt_LowBat = 0;
//				}
//			}
//		}
		break;
	case ADC_TENSVOLT:
		break;
	case ADC_CURRENT:
		//printf("sysPara.AdCurrent : %d \r\n",sysPara.AdCurrent);
		if(sysPara.AdCurrent < 1750)//1750
		{
			cnt_Current++;
			if(cnt_Current > 20)
			{
				cnt_Current = 0;

				if(SysMode.Flag_OpStatus == ENABLE_OUTPUT)
				{
					LED_OFF;
					SysMode.Flag_OpStatus = DISABLE_OUTPUT;
					printf("(Current) Output Disabled\r\n");
				}
			}
		}
		else
		{
			cnt_Current = 0;

			if(SysMode.Flag_OpStatus == DISABLE_OUTPUT)
			{
				LED_ON;
				SysMode.Flag_OpStatus = ENABLE_OUTPUT;
				printf("(Current) Output Enabled\r\n");

				if(SysMode.KeyLockStatus == KEY_UNLOCKED)		Flag_KeyLock = 1;
			}
		}
		break;
	case ADC_HEADTEMP:
		break;
	case ADC_CONTACT_HAND:
		if(sysPara.ContactHand < 100)
		{
			cnt_HeadContact = 0;
			LED_ON;
			SysMode.Flag_OpStatus = ENABLE_OUTPUT;
		}
		else
		{
			cnt_HeadContact++;
			if(cnt_HeadContact > 500)
			{
				cnt_HeadContact = 500;
				if(SysMode.Flag_OpStatus == ENABLE_OUTPUT)
				{
					LED_OFF;
					SysMode.Flag_OpStatus = DISABLE_OUTPUT;
				}
			}
		}
		break;
	case ADC_CONTACT_HEAD:
		printf("%d\r\n", sysPara.ContactHead);

		if(sysPara.ContactHead < 100)
		{
			cnt_HandContact = 0;
			INDICATOR_ON;
		}
		else
		{
			cnt_HandContact++;
			if(cnt_HandContact > 500)
			{
				cnt_HandContact = 500;
				INDICATOR_OFF;
			}
		}
		break;
	}
}


/*******************************************************************************
 * Function Name  : BlinkingStatusLED
 * Description    : Blink Status LED
 * Input          : lednum = Indicator LED number
 * Output         : None
 *******************************************************************************/
void BlinkingStatusLED(uint8_t lednum)
{
	//	uint8_t onDuty;
	//	uint8_t offDuty;
	//	static uint8_t Cnt_Led;
	//
	//	switch(lednum)
	//	{
	//	case TYPE_LED1:		onDuty = 1;		offDuty = 1;	break;
	//	case TYPE_LED2:		onDuty = 2;		offDuty = 1;	break;
	//	case TYPE_LED3:		onDuty = 3;		offDuty = 1;	break;
	//	case TYPE_LED4:		onDuty = 1;		offDuty = 3;	break;
	//	}
	//
	//	Cnt_Led++;
	//	if(Cnt_Led == (onDuty*LED_PERIOD))
	//	{
	//		LED_OFF;
	//	}
	//	else if(Cnt_Led == ((onDuty + offDuty)*LED_PERIOD))
	//	{
	//		Cnt_Led = 0;
	//		LED_ON;
	//	}
}


/*******************************************************************************
 * Function Name  : BlinkingStatusLED
 * Description    : Blink Status LED
 * Input          : lednum = Indicator LED number
 * Output         : None
 *******************************************************************************/
void HeadLEDControl(uint8_t ledcolor, uint8_t ledbright)
{
	uint16_t temp_ledbright;

	htim1.Instance->PSC = 1;
	htim1.Instance->ARR = 2999;

	temp_ledbright = (uint32_t)((((float)ledbright / 100) * 3000) - 1);

	switch(ledcolor)
	{
	case HEAD_NONE:
		htim1.Instance->CCR1 = temp_ledbright;
		htim1.Instance->CCR2 = temp_ledbright;
		htim1.Instance->CCR3 = temp_ledbright;

		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
		break;
	case HEAD_IR:
		htim1.Instance->CCR1 = temp_ledbright;

		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
		break;
	case HEAD_BLUE:
		htim1.Instance->CCR2 = temp_ledbright;

		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
		break;
	case HEAD_RED:
		htim1.Instance->CCR3 = temp_ledbright;

		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
		break;
	case HEAD_IRRED:
		htim1.Instance->CCR1 = temp_ledbright;
		htim1.Instance->CCR3 = temp_ledbright;

		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
		break;
	}
}


/*******************************************************************************
 * Function Name  : DiffusionLEDControl
 * Description    : Diffusion LED Control function
 * Input          : mode = Currnet mode
 * Output         : None
 *******************************************************************************/
void DiffusionLEDControl(uint8_t mode)
{
	static uint8_t bt_cnt = 0;

	if(mode == MODE_CHG)		diff_cnt++;
	else						diff_cnt = 0;

	switch(mode)
	{
	case POWER_OFF:
		htim2.Instance->CCR1 = 0;
		htim2.Instance->CCR2 = 0;
		htim2.Instance->CCR3 = 0;
		//HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
		//HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
		//HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
		break;
	case POWER_ON:
		htim2.Instance->CCR1 = 200;
		htim2.Instance->CCR2 = 200;
		htim2.Instance->CCR3 = 200;
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
		break;
	case MODE_CHG:
		htim2.Instance->CCR1 = 0;
		htim2.Instance->CCR2 = 0;
		htim2.Instance->CCR3 = 0;
		break;
	case LOW_BAT:
		printf("BAT LOW LED\r\n");

		htim2.Instance->CCR1 = 500;
		htim2.Instance->CCR2 = 196;
		htim2.Instance->CCR3 = 0;
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
		break;
	case CHARGING:
		htim2.Instance->CCR1 = 0;
		htim2.Instance->CCR2 = 392;
		htim2.Instance->CCR3 = 274;
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
		break;
	case BT_PAIR:
		switch(bt_cnt)
		{
		case 0:
			htim2.Instance->CCR1 = 110;
			htim2.Instance->CCR2 = 223;
			htim2.Instance->CCR3 = 255;
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
			bt_cnt = 1;
			break;
		case 1:
			htim2.Instance->CCR1 = 170;
			htim2.Instance->CCR2 = 396;
			htim2.Instance->CCR3 = 460;
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
			bt_cnt = 2;
			break;
		case 2:
			htim2.Instance->CCR1 = 340;
			htim2.Instance->CCR2 = 792;
			htim2.Instance->CCR3 = 920;
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
			bt_cnt = 3;
			break;
		case 3:
			htim2.Instance->CCR1 = 170;
			htim2.Instance->CCR2 = 396;
			htim2.Instance->CCR3 = 460;
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
			bt_cnt = 0;
			break;
		}
		break;
	case BT_CONNECTED:
		htim2.Instance->CCR1 = 0;
		htim2.Instance->CCR2 = 2;
		htim2.Instance->CCR3 = 300;
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
		break;
	case BT_STANDBY:
		htim2.Instance->CCR1 = 255;
		htim2.Instance->CCR2 = 255;
		htim2.Instance->CCR3 = 255;
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
		break;
	}
}


/*******************************************************************************
 * Function Name  : ChargingStatus
 * Description    : Diffusion LED Control function
 * Input          : mode = Currnet mode
 * Output         : None
 *******************************************************************************/
void ChargingStatus(void)
{
	static cnt_chgstatus = 0;

	chgstat = HAL_GPIO_ReadPin(CHG_STAT_GPIO_Port, CHG_STAT_Pin);
	//chgstat = !HAL_GPIO_ReadPin(CHG_STAT_GPIO_Port, CHG_STAT_Pin);

	if((SysMode.ChargingStatus == 0) && (chgstat == 1))
	{
		/* Disable RF */
		RF_DISABLE;
		RF_PulseDisable();

		/* Disable Cooling */
		printf(">> Cooling Disabled\r\n");
		SysMode.OutputFunc &= ~OUTPUT_COOL;
		Disable_Cooling();

		/* Disable Heater */
		printf(">> Heater Disabled\r\n");
		SysMode.OutputFunc &= ~OUTPUT_HEAT;
		Disable_Heating();

		/* Disable Ion Function */
		EMS_PulseDisable();

		cnt_chgstatus++;
		if(cnt_chgstatus > 10)
		{
			cnt_chgstatus = 0;
			SysMode.ChargingStatus = 1;

			htim2.Instance->CCR4 = 0;			// Backlight off
			HeadLEDControl(HEAD_NONE, 0);
			DiffusionLEDControl(POWER_OFF);

//			if(sysPara.BattVolt > 8100)			INDICATOR_ON;
//			else if(sysPara.BattVolt < 8000)	INDICATOR_OFF;

			if(sysPara.BattVolt < 8000)		INDICATOR_ON;
			else							INDICATOR_OFF;
		}
	}
	else if((SysMode.ChargingStatus == 1) && (chgstat == 0))
	{
		SysMode.ChargingStatus = 0;
		cnt_chgstatus = 0;

		INDICATOR_OFF;

		SysMode.OperationMode = STANDBY_MODE;

		NVIC_SystemReset();
	}
//	//printf("%d\r\n", chgstat);
}


/*******************************************************************************
 * Function Name  : Common_Process
 * Description    : Common Processing routine
 * Input          : None
 * Output         : None
 *******************************************************************************/
void Common_Process(SystemModeTypeDef *sysmodetype)
{
	//static int cnt_leddisplay = 0;
	//static int cnt_ledseq = 0;
	static uint8_t Flag_lockdisplay = 0;
	static uint8_t cnt_lockdisplay = 0;
//	int temp_ledRed;
//	int temp_ledGreen;
//	int temp_ledBlue;

	if(flag_1kHz)
	{
		flag_1kHz = 0;

		if (!SysMode.PowerStatus) {
			//DiffusionLEDControl(POWER_OFF);
		}
//		else if((SysMode.OperationMode >= SOOTHING_MODE) && (SysMode.OperationMode <= EYECARE_MODE))
//		{
//			if(SysMode.Flag_OpStatus)		DiffusionLEDControl(TREATMENT);
//			else							DiffusionLEDControl(POWER_ON);
//		}

		ChargingStatus();
	}

	if(flag_100Hz)
	{
		flag_100Hz = 0;

//		if((SysMode.OperationMode == TIGHTENING_MODE) || (SysMode.OperationMode == LIFTING_MODE))
//		{
//			if((SysMode.OutputFunc & 0x1E) == 0)
//				MonitoringSystemStatus(ADC_CONTACT_HAND);
//		}
//
//		MonitoringSystemStatus(ADC_CONTACT_HEAD);

		KeyScanTask();
		Key_Function();

		if(Flag_BLEConn)
		{
			Flag_BLEConn = 0;

			printf(">> Bluetooth Connection!!\r\n");
			SysMode.OperationMode = BLECONN_MODE;
		}
	}

	if(flag_10Hz)
	{
		flag_10Hz = 0;

		//read_systemstatus();
		  BT_Proc();

		if((SysMode.OutputFunc & OUTPUT_RF) == 1)
			MonitoringSystemStatus(ADC_CURRENT);

		MonitoringSystemStatus(ADC_BATTERY);
		//printf("%d\r\n", sysPara.BattVolt);

		if(SysMode.OperationMode == STANDBY_MODE)
		{
			LED_TOG;
		}
		//else if(((SysMode.OperationMode >= SOOTHING_MODE) && (SysMode.OperationMode <= EYECARE_MODE)) || (SysMode.OperationMode <= TEST_MODE))
		else if((SysMode.OperationMode == TIGHTENING_MODE) || (SysMode.OperationMode == LIFTING_MODE))
		{
			if((SysMode.Flag_OpStatus == DISABLE_OUTPUT) && (Flag_ProcessDone == 0))
			{
				SysMode.Cnt_AutoStart++;
				if(SysMode.Cnt_AutoStart > 50)
				{
					SysMode.Cnt_AutoStart = 0;
					DiffusionLEDControl(POWER_OFF);
					printf("Auto Start\r\n");
					SysMode.Flag_OpStatus = 1;

					if(SysMode.KeyLockStatus == KEY_UNLOCKED)		Flag_KeyLock = 1;

				}
				//printf("Cnt_AutoStart = %d\r\n", SysMode.Cnt_AutoStart);
			}
		}
/*
		if((SysMode.Flag_OpStatus != ENABLE_OUTPUT) && (SysMode.ChargingStatus == 0))
		{

			SysMode.Cnt_AutoSleep++;
			if(SysMode.Cnt_AutoSleep > 100)
			{
				SysMode.Cnt_AutoSleep = 0;
				printf("Auto Sleep Mode\r\n");

				HeadLEDControl(HEAD_NONE, 0);
//				DiffusionLEDControl(0, 0, 0);

				if(SysMode.OperationMode != STANDBY_MODE)
					VoiceOutput(VOICE_ALARM2);

				LCD_RESET_DISABLE;
				htim2.Instance -> CCR4 = 0;

				SysMode.OperationMode = SLEEP_MODE;
				SysMode.PowerStatus = POWEROFF_MODE;
			}
			//printf("Cnt_AutoSleep = %d\r\n", SysMode.Cnt_AutoSleep);
		}
		else
		{
			SysMode.Cnt_AutoSleep = 0;
		}
*/
		/* IMAGE TYPE */
#ifdef ENABLE_A_IMAGE
		if (Flag_KeyUnlock != 1 && Flag_lockdisplay != 1) {
			LCD_BATTARY_STATE();
			LCD_TIME_Image(e_time);
		}
#endif

#ifdef ENABLE_B_IMAGE
		if(Flag_KeyUnlock !=1 && Flag_lockdisplay !=1){
			LCD_TIME_Image(e_time);
		}
#endif

		if(Flag_KeyLock)
		{
			Flag_KeyLock = 0;

			SysMode.KeyLockStatus = KEY_LOCKED;
			LCD_LOCK_POP();

			Flag_lockdisplay = 1;
		}

		if(Flag_KeyUnlock)
		{
			Flag_KeyUnlock = 0;

			SysMode.KeyLockStatus = KEY_UNLOCKED;
			LCD_UNLOCK_POP();

			Flag_lockdisplay = 1;
		}

		if(Flag_lockdisplay)
		{
			cnt_lockdisplay++;
			if(cnt_lockdisplay > 20)
			{
				cnt_lockdisplay = 0;
				Flag_lockdisplay = 0;

				Display_BackgroundImg();
			}
		}

//		if((SysMode.OperationMode >= SOOTHING_MODE) && (SysMode.OperationMode <= EYECARE_MODE))
//		{
//			if(SysMode.Flag_OpStatus == DISABLE_OUTPUT)
//			{
//				temp_ledRed = SineTable[cnt_leddisplay];
//				temp_ledGreen = SineTable[cnt_leddisplay];
//				temp_ledBlue = SineTable[cnt_leddisplay];
//
//				DiffusionLEDControl(temp_ledRed, temp_ledGreen, temp_ledBlue);
//				cnt_leddisplay++;
//				if(cnt_leddisplay >= 32)	cnt_leddisplay = 0;
//			}
//			else if(SysMode.Flag_OpStatus == ENABLE_OUTPUT)
//			{
//				switch(cnt_ledseq)
//				{
//				case 0:
//					temp_ledRed = SineTable[cnt_leddisplay];
//					temp_ledGreen = (uint32_t)(((float)SineTable[cnt_leddisplay] / 500) * 320);
//					temp_ledBlue = (uint32_t)(((float)SineTable[cnt_leddisplay] / 500) * 320);
//					break;
//				case 1:
//					temp_ledRed = SineTable[cnt_leddisplay];
//					temp_ledGreen = (uint32_t)(((float)SineTable[cnt_leddisplay] / 500) * 380);
//					temp_ledBlue = (uint32_t)(((float)SineTable[cnt_leddisplay] / 500) * 450);
//					break;
//				case 2:
//					temp_ledRed = (uint32_t)(((float)SineTable[cnt_leddisplay] / 500) * 380);
//					temp_ledGreen = (uint32_t)(((float)SineTable[cnt_leddisplay] / 500) * 450);
//					temp_ledBlue = SineTable[cnt_leddisplay];
//					break;
//				case 3:
//					temp_ledRed = (uint32_t)(((float)SineTable[cnt_leddisplay] / 500) * 380);
//					temp_ledGreen = (uint32_t)(((float)SineTable[cnt_leddisplay] / 500) * 400);
//					temp_ledBlue = SineTable[cnt_leddisplay];
//					break;
//				}
//
//
//				DiffusionLEDControl(temp_ledRed, temp_ledGreen, temp_ledBlue);
//
//				cnt_leddisplay++;
//				if(cnt_leddisplay >= 32)
//				{
//					cnt_leddisplay = 0;
//					cnt_ledseq++;
//					if(cnt_ledseq > 3)	cnt_ledseq = 0;
//				}
//			}
//		}
	}
	/*
	if(flag_2Hz){
		 // set_systemstatus();
		  flag_2Hz = 0;
		  BT_Proc();
	}*/
}


/*******************************************************************************
 * Function Name  : StandbyMode_Process
 * Description    : Processing routine in Standby mode
 * Input          : None
 * Output         : None
 *******************************************************************************/
void StandbyMode_Process(void)
{
	uint8_t temp_keytouch;

	printf("\n>> STANDBY MODE\r\n");

	LED_OFF;
	INDICATOR_OFF;

	OUT_SEL1_LOW;
	OUT_SEL2_LOW;

	HeadLEDControl(HEAD_NONE, 0);
	DiffusionLEDControl(POWER_OFF);

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);


	while(1)
	{
		temp_keytouch = Read_CapTouch();
		printf("key = %02x\r\n", temp_keytouch);

		if(temp_keytouch <= 3)		break;
		HAL_Delay(1000);
	}

	INDICATOR_ON;
	HAL_Delay(10);
	INDICATOR_OFF;
	HAL_Delay(50);
	INDICATOR_ON;
	HAL_Delay(10);
	INDICATOR_OFF;

	SysMode.KeyLockStatus = KEY_UNLOCKED;

	while(1)
	{
		if(flag_2Hz)
		{
			flag_2Hz = 0;

//			CheckMonitoringVoltage();
//			CheckHeadTemperature();

			//BT_Proc();
		}

		Common_Process(&SysMode);

		if((SysMode.OperationMode >= SOOTHING_MODE) && (SysMode.OperationMode <= EYECARE_MODE))
		{
			// Display splash image
			/* LCD Initiation */
			LCD_Init();
			LCD_Clear(BLACK);
			Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 0, WHITE);

			LCD_RESET_ENABLE;
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
			htim2.Instance->CCR4 = 0;

			VoiceOutput(V_INTRO);

			LCD_LOGO_Image();
			htim2.Instance->CCR4 = 500;		// Backlight on
			HAL_Delay(1000);
			htim2.Instance->CCR4 = 0;			// Backlight off

			//VoiceOutput(VOICE_ALARM3);

			SysMode.Cnt_AutoStart = 0;

			break;
		}
		else if(SysMode.OperationMode == SLEEP_MODE)
		{
			break;
		}
	}
}


/*******************************************************************************
 * Function Name  : CleansingMode_Process
 * Description    : Processing routine in Cleansing mode
 * Input          : None
 * Output         : None
 *******************************************************************************/
void SoothingMode_Process(void)
{
	static uint32_t Cnt_ModeOpTime = 0;
	static uint8_t temp_Headled = 0;
	static uint8_t cnt_RFOut = 0;
	static uint8_t Flag_Contact = 0;


	printf("\n>> SOOTHING MODE\r\n");
	printf(">> RF + BLUE LED\r\n");
	printf("language : %d\r\n",language);

	if(language){
		VoiceOutput(CHV_SOOTHING_MODE);
		printf("CHV_SOOTHING_MODE\r\n");

	}else{
		VoiceOutput(ENV_SOOTHING_MODE);
		printf("ENV_SOOTHING_MODE\r\n");
	}

	Display_BackgroundImg();
	DiffusionLEDControl(POWER_ON);
	htim2.Instance->CCR4 = 500;		// Backlight on

	INDICATOR_OFF;

	HeadLEDControl(HEAD_NONE, 0);

	SysMode.Flag_OpStatus = DISABLE_OUTPUT;
	SysMode.OutputFunc = OUTPUT_NONE;
	//SysMode.OperationIntensity = LEVEL1;
	Cnt_ModeOpTime = 0;

	Flag_LevelChg = 1;

	OUT_SEL1_LOW;
	OUT_SEL2_LOW;

	RF_PulseEnable();
	temp_Headled = 0;
	Flag_ProcessDone = 0;
	Flag_Contact = 1;
	e_time = 0;

	while(1)
	{
		if(flag_2Hz)
		{
			flag_2Hz = 0;
			if((Flag_ProcessDone == 0) && (SysMode.ChargingStatus == 0))
			{

				if(SysMode.Flag_OpStatus)
				{
					DiffusionLEDControl(POWER_OFF);

					//if(SysMode.KeyLockStatus == KEY_UNLOCKED)		Flag_KeyLock = 1;

					/* Head LED On */
					if(temp_Headled == 0)
					{
						temp_Headled = 1;
						HeadLEDControl(HEAD_BLUE, 10);
					}

					// RF Pulse Time
					if((SysMode.OutputFunc & OUTPUT_RF) == 1)
					{
						if(cnt_RFOut == 0)			// Pulse On
						{
							//LED_ON;
							RF_ENABLE;
						}
						else if(cnt_RFOut == 8)		// Pulse Off
						{
							//LED_OFF;
							RF_DISABLE;
						}
						else if(cnt_RFOut >= 10)
						{
							cnt_RFOut = -1;
						}
						cnt_RFOut++;
					}
					else
					{
						RF_DISABLE;
					}

					if((Cnt_ModeOpTime % 2) == 0)
					{
						//printf(">> Elapsed Time = %d sec\r\n",(uint8_t) (Cnt_ModeOpTime / 2));
						e_time = (uint8_t)(Cnt_ModeOpTime/2);
					}

					Cnt_ModeOpTime++;
				}
				else
				{
					/* Head LED Off */
					if(temp_Headled == 1)
					{
						temp_Headled = 0;
						HeadLEDControl(HEAD_NONE, 0);
					}
					printf(">> Flag_Contact = %d \r\n",Flag_Contact);
					// RF Pulse Time
					if(((SysMode.OutputFunc & OUTPUT_RF) == 1) && (Flag_Contact == 1))
					{
						if(cnt_RFOut == 0)			// Pulse On
						{
							RF_ENABLE;
						}
						else if(cnt_RFOut == 2)		// Pulse Off
						{
							RF_DISABLE;
						}
						else if(cnt_RFOut >= 6)
						{
							cnt_RFOut = -1;
						}
						cnt_RFOut++;
					}
					else
					{
						RF_DISABLE;
					}
				}

				if(Cnt_ModeOpTime == 0)
				{
					//printf(">> 1MHz RF Enabled\r\n");
					SysMode.OutputFunc |= OUTPUT_RF;
				}
				else if(Cnt_ModeOpTime > 360)					// Finish treatment mode
				{
					/* Disable RF */
					//printf(">> 1MHz RF Disabled\r\n");
					SysMode.OutputFunc &= ~OUTPUT_RF;

					/* Head LED Off */
					if(temp_Headled == 1)
					{
						temp_Headled = 0;
						HeadLEDControl(HEAD_NONE, 0);
					}

					SysMode.Flag_OpStatus = DISABLE_OUTPUT;

					printf(">> Soothing Mode Done!!\r\n");
					Flag_ProcessDone = 1;

					VoiceOutput(VOICE_ALARM4);
				}
			}
		}

		if(Flag_LevelChg)
		{
			Flag_LevelChg = 0;

			RF_LevelChange();
			LCD_Lv_Image();
			switch(SysMode.OperationIntensity)
			{
			case LEVEL1:
				//VoiceOutput(V_NEXT);
				break;
			case LEVEL2:
				VoiceOutput(V_NEXT);
				break;
			case LEVEL3:
				VoiceOutput(V_NEXT);
				break;
			}
		}

		Common_Process(&SysMode);

		if(SysMode.OperationMode != SOOTHING_MODE)
		{
			SysMode.Flag_OpStatus = DISABLE_OUTPUT;

			/* Disable RF */
			RF_DISABLE;
			RF_PulseDisable();

			DiffusionLEDControl(MODE_CHG);

			//SysMode.OperationIntensity = LEVEL1;
			break;
		}
	}
}


/*******************************************************************************
 * Function Name  : EyecareMode_Process
 * Description    : Processing routine in Eyecare mode
 * Input          : None
 * Output         : None
 *******************************************************************************/
void BlemishCare_Process(void)
{
	static uint32_t Cnt_ModeOpTime = 0;
	static uint8_t temp_Headled = 0;
	static uint8_t cnt_RFOut = 0;
	static uint8_t Flag_Contact = 0;


	printf("\n>> BLEMISH CARE\r\n");
	printf(">> RF + RED LED\r\n");
	if(language){
		VoiceOutput(CHV_BLEMISHCARE_MODE);

	}else{
		VoiceOutput(ENV_BLEMISHCARE_MODE);
	}

	Display_BackgroundImg();
	DiffusionLEDControl(POWER_ON);

	htim2.Instance->CCR4 = 500;		// Backlight on

	INDICATOR_OFF;

	HeadLEDControl(HEAD_NONE, 0);

	SysMode.Flag_OpStatus = DISABLE_OUTPUT;
	SysMode.OutputFunc = OUTPUT_NONE;
	//SysMode.OperationIntensity = LEVEL1;
	Cnt_ModeOpTime = 0;

	Flag_LevelChg = 1;

	OUT_SEL1_LOW;
	OUT_SEL2_LOW;

	RF_PulseEnable();
	temp_Headled = 0;
	Flag_ProcessDone = 0;
	Flag_Contact = 1;
	e_time = 0;

	while(1)
	{
		if(flag_2Hz)
		{
			flag_2Hz = 0;

			if((Flag_ProcessDone == 0) && (SysMode.ChargingStatus == 0))
			{
				if(SysMode.Flag_OpStatus)
				{
					DiffusionLEDControl(POWER_OFF);

					//if(SysMode.KeyLockStatus == KEY_UNLOCKED)		Flag_KeyLock = 1;

					/* Head LED On */
					if(temp_Headled == 0)
					{
						temp_Headled = 1;
						HeadLEDControl(HEAD_RED, 10);
					}

					// RF Pulse Time
					if((SysMode.OutputFunc & OUTPUT_RF) == OUTPUT_RF)
					{
						if(cnt_RFOut == 0)			// Pulse On
						{
							//LED_ON;
							RF_ENABLE;
						}
						else if(cnt_RFOut == 8)		// Pulse Off
						{
							//LED_OFF;
							RF_DISABLE;
						}
						else if(cnt_RFOut >= 10)
						{
							cnt_RFOut = -1;
						}
						cnt_RFOut++;
					}
					else
					{
						RF_DISABLE;
					}

					if((Cnt_ModeOpTime % 2) == 0)
					{
						//printf(">> Elapsed Time = %d sec\r\n", (uint8_t)(Cnt_ModeOpTime/2));
						e_time = (uint8_t) (uint8_t)(Cnt_ModeOpTime/2);
					}

					Cnt_ModeOpTime++;
				}
				else
				{
					/* Head LED Off */
					if(temp_Headled == 1)
					{
						temp_Headled = 0;
						HeadLEDControl(HEAD_NONE, 0);
					}

					// RF Pulse Time
					if(Flag_Contact)
					{
						if(cnt_RFOut == 0)			// Pulse On
						{
							RF_ENABLE;
						}
						else if(cnt_RFOut == 2)		// Pulse Off
						{
							RF_DISABLE;
						}
						else if(cnt_RFOut >= 6)
						{
							cnt_RFOut = -1;
						}
						cnt_RFOut++;
					}
					else
					{
						RF_DISABLE;
					}
				}

				if(Cnt_ModeOpTime == 0)
				{
					//printf(">> 1MHz RF Enabled\r\n");
					SysMode.OutputFunc |= OUTPUT_RF;
				}
				else if(Cnt_ModeOpTime > 360)					// Finish treatment mode
				{
					/* Disable RF */
					printf(">> 1MHz RF Disabled\r\n");
					SysMode.OutputFunc &= ~OUTPUT_RF;

					/* Head LED Off */
					if(temp_Headled == 1)
					{
						temp_Headled = 0;
						HeadLEDControl(HEAD_NONE, 0);
					}

					SysMode.Flag_OpStatus = DISABLE_OUTPUT;

					printf(">> Blemish Care Done!!\r\n");
					Flag_ProcessDone = 1;

					VoiceOutput(VOICE_ALARM4);
				}
			}
		}

		if(Flag_LevelChg)
		{
			Flag_LevelChg = 0;

			LCD_Lv_Image();
		}

		Common_Process(&SysMode);

		if(SysMode.OperationMode != BLEMISH_CARE)
		{
			SysMode.Flag_OpStatus = DISABLE_OUTPUT;

			/* Disable RF */
			RF_DISABLE;
			RF_PulseDisable();

			DiffusionLEDControl(MODE_CHG);
			//SysMode.OperationIntensity = LEVEL1;

			break;
		}
	}
}


/*******************************************************************************
 * Function Name  : MoistureMode_Process
 * Description    : Processing routine in Moisture mode
 * Input          : None
 * Output         : None
 *******************************************************************************/
void MoistureMode_Process(void)
{
	static int8_t Cnt_ModeOpTime = 0;
	static uint32_t Cnt_ModeOp = 0;
	static uint8_t temp_Headled = 0;
	static uint8_t cnt_RFOut = 0;
	static uint8_t Flag_Contact = 0;

	printf("\n>> MOISTURE MODE\r\n");
	printf(">>RF + ION(-) + RED LED\r\n");

	if(language){
		VoiceOutput(CHV_MOISTURE_MODE);

	}else{
		VoiceOutput(ENV_MOISTURE_MODE);
	}


	DiffusionLEDControl(POWER_ON);
	Display_BackgroundImg();

	htim2.Instance->CCR4 = 500;		// Backlight on

	INDICATOR_OFF;

	HeadLEDControl(HEAD_NONE, 0);

	SysMode.Flag_OpStatus = DISABLE_OUTPUT;
	SysMode.OutputFunc = OUTPUT_NONE;
	//SysMode.OperationIntensity = LEVEL1;
	Cnt_ModeOpTime = 0;

	Flag_LevelChg = 1;

	OUT_SEL1_LOW;
	OUT_SEL2_LOW;

	RF_PulseEnable();
	temp_Headled = 0;
	Flag_ProcessDone = 0;
	Flag_Contact = 1;
	e_time = 0;

	while(1)
	{
		if(flag_2Hz)
		{
			flag_2Hz = 0;

			if((Flag_ProcessDone == 0) && (SysMode.ChargingStatus == 0))
			{
				if(SysMode.Flag_OpStatus)
				{
					DiffusionLEDControl(POWER_OFF);

					//if(SysMode.KeyLockStatus == KEY_UNLOCKED)		Flag_KeyLock = 1;

					/* Head LED On */
					if(temp_Headled == 0)
					{
						temp_Headled = 1;
						HeadLEDControl(HEAD_RED, 10);
					}

					// RF Pulse Time
					if((SysMode.OutputFunc & OUTPUT_RF) == 1)
					{
						if(cnt_RFOut == 0)			// Pulse On
						{
							//LED_ON;
							RF_ENABLE;
						}
						else if(cnt_RFOut == 8)		// Pulse Off
						{
							//LED_OFF;
							RF_DISABLE;
						}
						else if(cnt_RFOut >= 10)
						{
							cnt_RFOut = -1;
						}
						cnt_RFOut++;
					}
					else
					{
						RF_DISABLE;
					}

					if((Cnt_ModeOpTime % 2) == 0)
					{
						//printf(">> Elapsed Time = %lu sec\r\n", (uint8_t)(Cnt_ModeOpTime/2)+(60*Cnt_ModeOp));
						e_time = (uint8_t)(Cnt_ModeOpTime/2)+(60*Cnt_ModeOp);
					}

					Cnt_ModeOpTime++;
				}
				else
				{
					/* Head LED Off */
					if(temp_Headled == 1)
					{
						temp_Headled = 0;
						HeadLEDControl(HEAD_NONE, 0);
					}

					// RF Pulse Time
					if(((SysMode.OutputFunc & OUTPUT_RF) == 1) && (Flag_Contact == 1))
					{
						if(cnt_RFOut == 0)			// Pulse On
						{
							RF_ENABLE;
						}
						else if(cnt_RFOut == 2)		// Pulse Off
						{
							RF_DISABLE;
						}
						else if(cnt_RFOut >= 6)
						{
							cnt_RFOut = -1;
						}
						cnt_RFOut++;
					}
					else
					{
						RF_DISABLE;
					}
				}

				if(Cnt_ModeOpTime == 0)
				{
					//printf(">> 1MHz RF Enabled\r\n");
					SysMode.OutputFunc |= OUTPUT_RF;
				}
				else if(Cnt_ModeOpTime == 30)
				{
					/* Disable RF */
					printf(">> 1MHz RF Disabled\r\n");
					SysMode.OutputFunc &= ~OUTPUT_RF;

					/* Enable Heater */
					printf(">> Heater Enabled\r\n");
					SysMode.OutputFunc |= OUTPUT_HEAT;
					Enable_Heating(HEATER_DUTY);
				}
				else if(Cnt_ModeOpTime == 60)
				{
					/* Enable Negative Ion */
					printf(">> Ion(-) Enabled\r\n");
					SysMode.OutputFunc |= OUTPUT_ION_N;

					/* Enable Ion Function */
					//EMS_PulseEnable(ION_P_MODE);
				}
				else if(Cnt_ModeOpTime > 120)					// Finish treatment mode
				{
					/* Disable Heater */
					printf(">> Heater Disabled\r\n");
					SysMode.OutputFunc &= ~OUTPUT_HEAT;
					Disable_Heating();

					/* Disable Ion Function */
					printf(">> Ion(-) Disabled\r\n");
					SysMode.OutputFunc &= ~OUTPUT_ION_N;
					//EMS_PulseDisable();

					Cnt_ModeOp++;
					if(Cnt_ModeOp >= 3)
					{
						/* Head LED Off */
						if(temp_Headled == 1)
						{
							temp_Headled = 0;
							HeadLEDControl(HEAD_NONE, 0);
						}

						SysMode.Flag_OpStatus = DISABLE_OUTPUT;

						printf(">> Moisture Mode Done!!\r\n");
						Flag_ProcessDone = 1;

						VoiceOutput(VOICE_ALARM4);
					}
					else
					{
						Cnt_ModeOpTime = -1;
					}
				}
			}
		}

		if(Flag_LevelChg)
		{
			Flag_LevelChg = 0;

			LCD_Lv_Image();
		}

		Common_Process(&SysMode);

		if(SysMode.OperationMode != MOISTURE_MODE)
		{
			SysMode.Flag_OpStatus = DISABLE_OUTPUT;

			/* Disable RF */
			RF_DISABLE;
			RF_PulseDisable();

			/* Disable Heater */
			printf(">> Heater Disabled\r\n");
			SysMode.OutputFunc &= ~OUTPUT_HEAT;
			Disable_Heating();

			/* Disable Ion Function */
			EMS_PulseDisable();

			//SysMode.OperationIntensity = LEVEL1;

			DiffusionLEDControl(MODE_CHG);
			break;
		}
	}
}


/*******************************************************************************
 * Function Name  : LiftingMode_Process
 * Description    : Processing routine in Lifting mode
 * Input          : None
 * Output         : None
 *******************************************************************************/
void CollagenMode_Process(void)
{
	static uint32_t Cnt_ModeOpTime = 0;
	static uint32_t Cnt_ModeOp = 0;
	static uint8_t temp_Headled = 0;
	static uint8_t cnt_RFOut = 0;
	static uint8_t Flag_Contact = 0;

	printf("\n>> COLLAGEN MODE\r\n");
	printf(">> RF + HEATING + IR LED\r\n");

	if(language){
		VoiceOutput(CHV_COLLAGEN_MODE);

	}else{
		VoiceOutput(ENV_COLLAGEN_MODE);
	}


	DiffusionLEDControl(POWER_ON);
	Display_BackgroundImg();

	htim2.Instance->CCR4 = 500;		// Backlight on

	INDICATOR_OFF;

	HeadLEDControl(HEAD_NONE, 0);

	SysMode.Flag_OpStatus = DISABLE_OUTPUT;
	SysMode.OutputFunc = OUTPUT_NONE;
	//SysMode.OperationIntensity = LEVEL1;
	Cnt_ModeOpTime = 0;

	Flag_LevelChg = 1;

	OUT_SEL1_LOW;
	OUT_SEL2_LOW;

	RF_PulseEnable();
	temp_Headled = 0;
	Flag_ProcessDone = 0;
	Flag_Contact = 1;
	e_time = 0;

	while(1)
	{
		if(flag_2Hz)
		{
			flag_2Hz = 0;

			if((Flag_ProcessDone == 0) && (SysMode.ChargingStatus == 0))
			{
				if(SysMode.Flag_OpStatus)
				{
					DiffusionLEDControl(POWER_OFF);

					//if(SysMode.KeyLockStatus == KEY_UNLOCKED)		Flag_KeyLock = 1;

					/* Head LED On */
					if(temp_Headled == 0)
					{
						temp_Headled = 1;
						HeadLEDControl(HEAD_IR, 10);
					}

					// RF Pulse Time
					if((SysMode.OutputFunc & OUTPUT_RF) == 1)
					{
						if(cnt_RFOut == 0)			// Pulse On
						{
							//LED_ON;
							RF_ENABLE;
						}
						else if(cnt_RFOut == 8)		// Pulse Off
						{
							//LED_OFF;
							RF_DISABLE;
						}
						else if(cnt_RFOut >= 10)
						{
							cnt_RFOut = -1;
						}
						cnt_RFOut++;
					}
					else
					{
						RF_DISABLE;
					}

					if((Cnt_ModeOpTime % 2) == 0)
					{
						//printf(">> Elapsed Time = %lu sec\r\n", (uint8_t)(Cnt_ModeOpTime/2)+(60*Cnt_ModeOp));
						e_time = (uint8_t)(Cnt_ModeOpTime/2)+(60*Cnt_ModeOp);
					}

					Cnt_ModeOpTime++;
				}
				else
				{
					/* Head LED Off */
					if(temp_Headled == 1)
					{
						temp_Headled = 0;
						HeadLEDControl(HEAD_NONE, 0);
					}

					// RF Pulse Time
					if(((SysMode.OutputFunc & OUTPUT_RF) == 1) && (Flag_Contact == 1))
					{
						if(cnt_RFOut == 0)			// Pulse On
						{
							RF_ENABLE;
						}
						else if(cnt_RFOut == 2)		// Pulse Off
						{
							RF_DISABLE;
						}
						else if(cnt_RFOut >= 6)
						{
							cnt_RFOut = -1;
						}
						cnt_RFOut++;
					}
					else
					{
						RF_DISABLE;
					}
				}

				if(Cnt_ModeOpTime == 0)
				{
					//printf(">> 1MHz RF Enabled\r\n");
					SysMode.OutputFunc |= OUTPUT_RF;
				}
				else if(Cnt_ModeOpTime == 30)
				{
					/* Disable RF */
					printf(">> 1MHz RF Disabled\r\n");
					SysMode.OutputFunc &= ~OUTPUT_RF;

					/* Enable Heater */
					printf(">> Heater Enabled\r\n");
					SysMode.OutputFunc |= OUTPUT_HEAT;
					Enable_Heating(HEATER_DUTY);
				}
				else if(Cnt_ModeOpTime == 60)
				{
					/* Disable Heater */
					printf(">> Heater Disabled\r\n");
					SysMode.OutputFunc &= ~OUTPUT_HEAT;
					Disable_Heating();

					/* Enable RF */
					//printf(">> 1MHz RF Enabled\r\n");
					SysMode.OutputFunc |= OUTPUT_RF;
				}
				else if(Cnt_ModeOpTime == 90)
				{
					/* Disable RF */
					printf(">> 1MHz RF Disabled\r\n");
					SysMode.OutputFunc &= ~OUTPUT_RF;

					/* Enable Heater */
					printf(">> Heater Enabled\r\n");
					SysMode.OutputFunc |= OUTPUT_HEAT;
					Enable_Heating(HEATER_DUTY);
				}
				else if(Cnt_ModeOpTime > 120)					// Finish treatment mode
				{
					/* Disable Heater */
					printf(">> Heater Disabled\r\n");
					SysMode.OutputFunc &= ~OUTPUT_HEAT;
					Disable_Heating();

					Cnt_ModeOp++;
					if(Cnt_ModeOp >= 3)
					{
						/* Head LED Off */
						if(temp_Headled == 1)
						{
							temp_Headled = 0;
							HeadLEDControl(HEAD_NONE, 0);
						}

						SysMode.Flag_OpStatus = DISABLE_OUTPUT;

						printf(">> Collagen Mode Done!!\r\n");
						Flag_ProcessDone = 1;

						VoiceOutput(VOICE_ALARM4);
					}
					else
					{
						Cnt_ModeOpTime = -1;
					}
				}
			}
		}

		if(Flag_LevelChg)
		{
			Flag_LevelChg = 0;

			LCD_Lv_Image();
		}

		Common_Process(&SysMode);

		if(SysMode.OperationMode != COLLAGEN_MODE)
		{
			SysMode.Flag_OpStatus = DISABLE_OUTPUT;

			/* Disable RF */
			RF_DISABLE;
			RF_PulseDisable();

			/* Disable Heater */
			printf(">> Heater Disabled\r\n");
			SysMode.OutputFunc &= ~OUTPUT_HEAT;
			Disable_Heating();

			DiffusionLEDControl(MODE_CHG);
			//SysMode.OperationIntensity = LEVEL1;

			break;
		}
	}
}


/*******************************************************************************
 * Function Name  : CooingMode_Process
 * Description    : Processing routine in Cooling mode
 * Input          : None
 * Output         : None
 *******************************************************************************/
void TighteningMode_Process(void)
{
	static uint32_t Cnt_ModeOpTime = 0;
	static uint32_t Cnt_ModeOp = 0;
	static uint8_t temp_Headled = 0;

	printf("\n>> TIGHTENING MODE\r\n");
	printf(">> COOL + BLUE LED\r\n");

	if(language){
		VoiceOutput(CHV_TIGHTENING_MODE);

	}else{
		VoiceOutput(ENV_TIGHTENING_MODE);
	}

	DiffusionLEDControl(POWER_ON);
	Display_BackgroundImg();

	htim2.Instance->CCR4 = 500;		// Backlight on

	INDICATOR_OFF;

	HeadLEDControl(HEAD_NONE, 0);

	SysMode.Flag_OpStatus = DISABLE_OUTPUT;
	SysMode.OutputFunc = OUTPUT_NONE;
	//SysMode.OperationIntensity = LEVEL1;
	Cnt_ModeOpTime = 0;

	Flag_LevelChg = 1;

	OUT_SEL1_LOW;
	OUT_SEL2_LOW;
	DISABLE_TENS;

	temp_Headled = 0;
	Flag_ProcessDone = 0;
	e_time = 0;

	while(1)
	{
		if(flag_2Hz)
		{
			flag_2Hz = 0;

			if((Flag_ProcessDone == 0) && (SysMode.ChargingStatus == 0))
			{
				if(SysMode.Flag_OpStatus)
				{
					DiffusionLEDControl(POWER_OFF);

					/* Head LED On */
					if(temp_Headled == 0)
					{
						temp_Headled = 1;
						HeadLEDControl(HEAD_BLUE, 10);
					}

					if((Cnt_ModeOpTime % 2) == 0)
					{
						printf(">> Elapsed Time = %lu sec\r\n", (uint8_t)(Cnt_ModeOpTime/2)+(30*Cnt_ModeOp));
						e_time = (uint8_t)(Cnt_ModeOpTime/2)+(30*Cnt_ModeOp);
					}

					Cnt_ModeOpTime++;
				}
				else
				{
					/* Head LED Off */
					if(temp_Headled == 1)
					{
						temp_Headled = 0;
						HeadLEDControl(HEAD_NONE, 0);
					}

					/* Disable Cooling */
					printf(">> Cooling Disabled\r\n");
					SysMode.OutputFunc &= ~OUTPUT_COOL;
					Disable_Cooling();
				}

				if(Cnt_ModeOpTime == 1)
				{
					/* Enable Cooling */
					printf(">> Cooling Enabled Low\r\n");
					SysMode.OutputFunc |= OUTPUT_COOL;
					Enable_Cooling(COOLING_DUTY_LOW);
				}
				else if(Cnt_ModeOpTime == 51)
				{
					/* Enable Cooling */
					printf(">> Cooling Enabled High\r\n");
					Enable_Cooling(COOLING_DUTY_HIGH);
//					/* Disable Cooling */
//					printf(">> Cooling Disabled\r\n");
//					SysMode.OutputFunc &= ~OUTPUT_COOL;
//					Disable_Cooling();
				}
				else if(Cnt_ModeOpTime >= 61)
				{
					Cnt_ModeOp++;
					if(Cnt_ModeOp >= 6)
					{
						/* Head LED Off */
						if(temp_Headled == 1)
						{
							temp_Headled = 0;
							HeadLEDControl(HEAD_NONE, 0);
						}

						/* Disable Cooling */
						printf(">> Cooling Disabled\r\n");
						SysMode.OutputFunc &= ~OUTPUT_COOL;
						Disable_Cooling();

						SysMode.Flag_OpStatus = DISABLE_OUTPUT;

						printf(">> Tightening Mode Done!!\r\n");
						Flag_ProcessDone = 1;

						VoiceOutput(VOICE_ALARM4);
					}
					else
					{
						Cnt_ModeOpTime = 0;
					}
				}
			}
		}

		Common_Process(&SysMode);

		if(SysMode.OperationMode != TIGHTENING_MODE)
		{
			SysMode.Flag_OpStatus = DISABLE_OUTPUT;

			/* Disable Cooling */
			printf(">> Cooling Disabled\r\n");
			SysMode.OutputFunc &= ~OUTPUT_COOL;
			Disable_Cooling();

			DiffusionLEDControl(MODE_CHG);
			//SysMode.OperationIntensity = LEVEL1;

			break;
		}
	}
}


/*******************************************************************************
 * Function Name  : TighteningMode_Process
 * Description    : Processing routine in Tightening mode
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LiftingMode_Process(void)
{
	static uint32_t Cnt_ModeOpTime = 0;
	static uint32_t Cnt_ModeOp = 0;
	static uint8_t temp_Headled = 0;

	printf("\n>> LIFTING MODE\r\n");
	printf(">> EMS + RED LED\r\n");

	if(language){
		VoiceOutput(CHV_LIFTING_MODE);

	}else{
		VoiceOutput(ENV_LIFTING_MODE);
	}

	DiffusionLEDControl(POWER_ON);
	Display_BackgroundImg();

	htim2.Instance->CCR4 = 500;		// Backlight on

	INDICATOR_OFF;

	HeadLEDControl(HEAD_NONE, 0);

	SysMode.Flag_OpStatus = DISABLE_OUTPUT;
	SysMode.OutputFunc = OUTPUT_NONE;
	//SysMode.OperationIntensity = LEVEL1;
	Cnt_ModeOpTime = 0;

	Flag_LevelChg = 1;

	OUT_SEL1_LOW;
	OUT_SEL2_LOW;
	//ENABLE_TENS;

	temp_Headled = 0;
	Flag_ProcessDone = 0;
	e_time = 0;

	while(1)
	{
		if(flag_2Hz)
		{
			flag_2Hz = 0;

			if((Flag_ProcessDone == 0) && (SysMode.ChargingStatus == 0))
			{
				if(SysMode.Flag_OpStatus)
				{
					DiffusionLEDControl(POWER_OFF);

					/* Head LED On */
					if(temp_Headled == 0)
					{
						temp_Headled = 1;
						HeadLEDControl(HEAD_RED, 10);
					}

					if((Cnt_ModeOpTime % 2) == 0)
					{
						//printf(">> Elapsed Time = %lu sec\r\n", (uint8_t)(Cnt_ModeOpTime/2)+(15*Cnt_ModeOp));
						e_time = (uint8_t)(Cnt_ModeOpTime/2)+(15*Cnt_ModeOp);
					}

					Cnt_ModeOpTime++;
				}
				else
				{
					/* Head LED Off */
					if(temp_Headled == 1)
					{
						temp_Headled = 0;
						HeadLEDControl(HEAD_NONE, 0);
					}
				}

				if(Cnt_ModeOpTime == 1)
				{
					/* Enable EMS */
					printf(">> EMS Enabled\r\n");
					SysMode.OutputFunc |= OUTPUT_TENS_A;

					EMS_PulseEnable(EMS_A_MODE);
				}
				else if(Cnt_ModeOpTime == 21)
				{
					/* Disable EMS */
					printf(">> EMS Disabled\r\n");
					SysMode.OutputFunc &= ~OUTPUT_TENS_A;

					EMS_PulseDisable();
				}
				else if(Cnt_ModeOpTime > 31)					// Finish treatment mode
				{
					Cnt_ModeOp++;
					if(Cnt_ModeOp >= 12)
					{
						/* Head LED Off */
						if(temp_Headled == 1)
						{
							temp_Headled = 0;
							HeadLEDControl(HEAD_NONE, 0);
						}

						SysMode.Flag_OpStatus = DISABLE_OUTPUT;

						printf(">> Lifting Mode Done!!\r\n");
						Flag_ProcessDone = 1;

						VoiceOutput(VOICE_ALARM4);
					}
					else
					{
						Cnt_ModeOpTime = 0;
					}
				}
			}
		}


		if(Flag_LevelChg)
		{
			Flag_LevelChg = 0;

			LCD_Lv_Image();
		}

		Common_Process(&SysMode);

		if(SysMode.OperationMode != LIFTING_MODE)
		{
			/* Disable EMS */
			EMS_PulseDisable();
			DISABLE_TENS;

			//SysMode.OperationIntensity = LEVEL1;

			DiffusionLEDControl(MODE_CHG);
			break;
		}
	}
}


/*******************************************************************************
 * Function Name  : TighteningMode_Process
 * Description    : Processing routine in Tightening mode
 * Input          : None
 * Output         : None
 *******************************************************************************/
void EyeCare_Process(void)
{
	static uint32_t Cnt_ModeOpTime = 0;
	static uint32_t Cnt_ModeOp = 0;
	//static uint8_t temp_Headled = 0;
	static uint8_t cnt_RFOut = 0;
	static uint8_t Flag_Contact = 0;

	printf("\n>> EYECARE MODE\r\n");
	printf(">> EMS + RF\r\n");

	if(language){
		VoiceOutput(CHV_EYECARE_MODE);

	}else{
		VoiceOutput(ENV_EYECARE_MODE);
	}

	DiffusionLEDControl(POWER_ON);
	Display_BackgroundImg();

	htim2.Instance->CCR4 = 500;		// Backlight on

	INDICATOR_OFF;

	HeadLEDControl(HEAD_NONE, 0);

	SysMode.Flag_OpStatus = DISABLE_OUTPUT;
	SysMode.OutputFunc = OUTPUT_NONE;
	//SysMode.OperationIntensity = LEVEL1;
	Cnt_ModeOpTime = 0;

	Flag_LevelChg = 1;

	OUT_SEL1_LOW;
	OUT_SEL2_LOW;

	RF_PulseEnable();
	Flag_ProcessDone = 0;
	Flag_Contact = 1;
	e_time = 0;

	while(1)
	{
		if(flag_2Hz)
		{
			flag_2Hz = 0;

			if((Flag_ProcessDone == 0) && (SysMode.ChargingStatus == 0))
			{
				if(SysMode.Flag_OpStatus)
				{
					DiffusionLEDControl(POWER_OFF);

					if(SysMode.KeyLockStatus == KEY_UNLOCKED)		Flag_KeyLock = 1;

					// RF Pulse Time
					if((SysMode.OutputFunc & OUTPUT_RF) == 1)
					{
						if(cnt_RFOut == 0)			// Pulse On
						{
							//LED_ON;
							RF_ENABLE;
						}
						else if(cnt_RFOut == 8)		// Pulse Off
						{
							//LED_OFF;
							RF_DISABLE;
						}
						else if(cnt_RFOut >= 10)
						{
							cnt_RFOut = -1;
						}
						cnt_RFOut++;
					}
					else
					{
						RF_DISABLE;
					}

					if((Cnt_ModeOpTime % 2) == 0)
					{
						printf(">> Elapsed Time = %lu sec\r\n", (uint8_t)(Cnt_ModeOpTime/2)+(30*Cnt_ModeOp));
						e_time = (uint8_t)(Cnt_ModeOpTime/2)+(30*Cnt_ModeOp);
					}

					Cnt_ModeOpTime++;
				}
				else
				{
					// RF Pulse Time
					if(((SysMode.OutputFunc & OUTPUT_RF) == 1) && (Flag_Contact == 1))
					{
						if(cnt_RFOut == 0)			// Pulse On
						{
							RF_ENABLE;
						}
						else if(cnt_RFOut == 2)		// Pulse Off
						{
							RF_DISABLE;
						}
						else if(cnt_RFOut >= 6)
						{
							cnt_RFOut = -1;
						}
						cnt_RFOut++;
					}
					else
					{
						RF_DISABLE;
					}
				}

				if(Cnt_ModeOpTime == 0)
				{
					//printf(">> 1MHz RF Enabled\r\n");
					SysMode.OutputFunc |= OUTPUT_RF;
					RF_PulseEnable();
				}
				else if(Cnt_ModeOpTime == 30)
				{
					/* Disable RF */
					printf(">> 1MHz RF Disabled\r\n");
					SysMode.OutputFunc &= ~OUTPUT_RF;

					/* Enable EMS */
					printf(">> EMS Enabled\r\n");
					SysMode.OutputFunc |= OUTPUT_TENS_A;
					EMS_PulseEnable(EMS_A_MODE);
				}
				else if(Cnt_ModeOpTime > 60)					// Finish treatment mode
				{
					/* Disable EMS */
					printf(">> EMS Disabled\r\n");
					SysMode.OutputFunc &= ~OUTPUT_TENS_A;
					EMS_PulseDisable();

					Cnt_ModeOp++;
					if(Cnt_ModeOp >= 6)
					{
						SysMode.Flag_OpStatus = DISABLE_OUTPUT;

						printf(">> Moisture Mode Done!!\r\n");
						Flag_ProcessDone = 1;

						VoiceOutput(VOICE_ALARM4);
					}
					else
					{
						Cnt_ModeOpTime = -1;
					}
				}
			}
		}

		if(Flag_LevelChg)
		{
			Flag_LevelChg = 0;

			LCD_Lv_Image();
		}

		Common_Process(&SysMode);

		if(SysMode.OperationMode != EYECARE_MODE)
		{
			/* Disable RF */
			RF_DISABLE;
			RF_PulseDisable();

			/* Disable EMS */
			EMS_PulseDisable();

			DiffusionLEDControl(MODE_CHG);
			//SysMode.OperationIntensity = LEVEL1;

			break;
		}
	}
}


/*******************************************************************************
 * Function Name  : Enable_Heating
 * Description    : Enable PWM for RF Driver
 * Input          : None
 * Output         : None
 *******************************************************************************/
void Enable_Cooling(uint32_t dutyHeater)
{
	/* Enable Heater */
	htim3.Instance->CCR4 = dutyHeater;
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
	TEC_DIR_HIGH;
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
}


/*******************************************************************************
 * Function Name  : Disable_Heating
 * Description    : Enable PWM for RF Driver
 * Input          : None
 * Output         : None
 *******************************************************************************/
void Disable_Cooling(void)
{
	/* Disable Heater */
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);
	htim3.Instance->CCR4 = 0;
}


/*******************************************************************************
 * Function Name  : Enable_Cooling
 * Description    : Enable PWM for RF Driver
 * Input          : None
 * Output         : None
 *******************************************************************************/
void Enable_Heating(uint32_t dutyHeater)
{
	/* Enable Heater */
	htim3.Instance->CCR3 = dutyHeater;
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);
	TEC_DIR_LOW;
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
}


/*******************************************************************************
 * Function Name  : Disable_Cooling
 * Description    : Enable PWM for RF Driver
 * Input          : None
 * Output         : None
 *******************************************************************************/
void Disable_Heating(void)
{
	/* Disable Heater */
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
	htim3.Instance->CCR3 = 0;
}


/*******************************************************************************
 * Function Name  : RF_PulseEnable
 * Description    : Enable PWM for RF Driver
 * Input          : None
 * Output         : None
 *******************************************************************************/
void RF_PulseEnable(void)
{
	// TIM8 Ch4 is used for driving source of RF function
	htim8.Instance->PSC = 1;							// Set Prescaler, 63

	switch(SysMode.OperationIntensity)
	{
	case LEVEL1:
		// 1.2MHz
		htim8.Instance -> ARR = 29;								// Set Period, 999
		htim8.Instance -> CCR4 = 15;							// Set Duty(Pulse), 499
		break;
	case LEVEL2:
		// 1.125MHz
		htim8.Instance -> ARR = 31;								// Set Period, 999
		htim8.Instance -> CCR4 = 15;							// Set Duty(Pulse), 499
		break;
	case LEVEL3:
		// 1MHz
		htim8.Instance->ARR = 35;							// Set Period, 999
		htim8.Instance->CCR4 = 18;						// Set Duty(Pulse), 499
		break;
	}

	// 1.4MHz
//	htim8.Instance -> ARR = 25;								// Set Period, 999
//	htim8.Instance -> CCR4 = 13;							// Set Duty(Pulse), 499

	// 1.28MHz
//	htim8.Instance -> ARR = 27;								// Set Period, 999
//	htim8.Instance -> CCR4 = 15;							// Set Duty(Pulse), 499

	// 948kHz
//	htim8.Instance -> ARR = 37;								// Set Period, 999
//	htim8.Instance -> CCR4 = 19;							// Set Duty(Pulse), 499

	// 800kHz
//	htim8.Instance -> ARR = 44;								// Set Period, 999
//	htim8.Instance -> CCR4 = 22;							// Set Duty(Pulse), 499

	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);
}


/*******************************************************************************
 * Function Name  : RF_PulseDisable
 * Description    : Enable PWM for RF Driver
 * Input          : None
 * Output         : None
 *******************************************************************************/
void RF_PulseDisable(void)
{
	htim8.Instance->CCR4 = 0;
//	//HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_4);
}


/*******************************************************************************
 * Function Name  : RF_LevelChange
 * Description    : Enable PWM for RF Driver
 * Input          : None
 * Output         : None
 *******************************************************************************/
void RF_LevelChange(void)
{
	switch(SysMode.OperationIntensity)
	{
	case LEVEL1:
		// 1.2MHz
		htim8.Instance -> ARR = 29;								// Set Period, 999
		htim8.Instance -> CCR4 = 15;							// Set Duty(Pulse), 499
		break;
	case LEVEL2:
		// 1.125MHz
		htim8.Instance -> ARR = 31;								// Set Period, 999
		htim8.Instance -> CCR4 = 15;							// Set Duty(Pulse), 499
		break;
	case LEVEL3:
		// 1MHz
		htim8.Instance->ARR = 35;							// Set Period, 999
		htim8.Instance->CCR4 = 18;						// Set Duty(Pulse), 499
		break;
	}
}
//static unsigned int prescale = 7;
//static unsigned int timperiod = 799;
//static unsigned int dutyvalue = 25;
/*******************************************************************************
 * Function Name  : EMS_PulseEnable
 * Description    : Enable EMS Function
 * Input          : None
 * Output         : None
 *******************************************************************************/
void EMS_PulseEnable(uint8_t emsMode)
{
	RF_DISABLE;

	// TIM1 Ch4 is used for driving source of TENS intensity
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

//	// TENS Level Control
//	temp_test += 10;
//	if(temp_test >= 2000)
//	{
//		temp_test = 0;
//	}
//	htim1.Instance -> CCR4 = temp_test;						// Set Duty(Pulse)
	//htim1.Instance -> CCR4 = 1000;					// 23V @3.7V
	htim1.Instance->CCR4 = 250;						// 21V @7.4V

	ENABLE_TENS;

	// TIM8 Ch1, Ch2, Ch3 is used for driving source of TENS and Galvanic function
	htim8.Instance->PSC = 71;								// Set Prescaler
	htim8.Instance->ARR = 999;							// Set Period
	//htim8.Instance -> CCR4 = 499;							// Set Duty(Pulse)

	htim8.Instance->CCR1 = 499;
	htim8.Instance->CCR2 = 499;
	htim8.Instance->CCR3 = 499;

	switch(emsMode)
	{
	case EMS_A_MODE:
		OUT_SEL1_HIGH;
		OUT_SEL2_HIGH;

		HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
		break;
	case EMS_B_MODE:
		OUT_SEL1_HIGH;
		OUT_SEL2_LOW;

		HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
		break;
	case ION_P_MODE:
		OUT_SEL1_HIGH;
		OUT_SEL2_LOW;

		HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_2);
		USER_TIM8toGPIO(GPIO_PIN_7);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);

		HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);

		HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_3);
		USER_TIM8toGPIO(GPIO_PIN_8);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
		break;
	case ION_N_MODE:
		OUT_SEL1_HIGH;
		OUT_SEL2_LOW;

		HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_2);
		USER_TIM8toGPIO(GPIO_PIN_7);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);

		HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_1);

		USER_TIM8toGPIO(GPIO_PIN_6);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
		HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
		break;
	}

	//	// TIM3 Ch2 is used for driving source of TENS intensity
	//	// There is a problem with I2C1 and TIM3_CH2 remapping
	//	// Conflict between the TIM3_CH2 signal and the I2C1 SMBA signal, (even if SMBA is not used).
	//	// To avoid this conflict, TIM3_CH2 can only be used in input mode.
	//	__HAL_RCC_I2C1_CLK_DISABLE();
	//
	//	//dutyvalue = dutyvalue + 10;
	//	//if(dutyvalue > htim3.Instance -> ARR)	dutyvalue = 10;
	//
	//	htim3.Instance -> PSC = prescale;						// Set Prescaler
	//	htim3.Instance -> ARR = timperiod;						// Set Period
	//	htim3.Instance -> CCR2 = dutyvalue;						// Set Duty(Pulse)
	//	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	//
	//	// TIM1 Ch2 is used for driving source of TENS and Galvanic function
	//	htim1.Instance -> PSC = 63;								// Set Prescaler, 63
	//	htim1.Instance -> ARR = 999;							// Set Period, 999
	//	htim1.Instance -> CCR2 = 499;							// Set Duty(Pulse), 499
	//
	//	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	//	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
}


/*******************************************************************************
 * Function Name  : EMS_PulseDisable
 * Description    : Disable EMS Function
 * Input          : None
 * Output         : None
 *******************************************************************************/
void EMS_PulseDisable(void)
{
	// TIM1 Ch4 is used for driving source of TENS intensity
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4);

	DISABLE_TENS;

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);

	USER_TIM8_ReInit();

	// TIM8 Ch1, Ch2, Ch3 is used for driving source of TENS and Galvanic function
	htim8.Instance->CCR1 = 0;
	htim8.Instance->CCR2 = 0;
	htim8.Instance->CCR3 = 0;

	//HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_1);
	//HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_2);
	//HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_3);

	OUT_SEL1_LOW;
	OUT_SEL2_LOW;
	//	// TIM3 Ch2 is used for driving source of TENS intensity
	//	// There is a problem with I2C1 and TIM3_CH2 remapping
	//	// Conflict between the TIM3_CH2 signal and the I2C1 SMBA signal, (even if SMBA is not used).
	//	// To avoid this conflict, TIM3_CH2 can only be used in input mode.
	//	__HAL_RCC_I2C1_CLK_ENABLE();
	//	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
	//
	//	// TIM1 Ch2 is used for driving source of TENS and Galvanic function
	//	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
	//	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
}


/*******************************************************************************
 * Function Name  : EMSFunctionControl
 * Description    : To control Frequency and Intensity of EMS
 * Input          : None
 * Output         : None
 *******************************************************************************/
void EMSFunctionControl(uint8_t emsMode, uint32_t emsIntensity)
{
	//	uint8_t emsCh;
	//
	//	emsCh = 0x00;
	//
	//	switch(emsMode)
	//	{
	//	case EMS_NONE:		emsCh = 0x00;		break;
	//	case EMS_A_MODE:	emsCh = 0x30;		break;
	//	case EMS_B_MODE:	emsCh = 0x28;		break;
	//	case EMS_C_MODE:	emsCh = 0x00;		break;
	//	}
	//
	//	g_ioexpval = ((g_ioexpval & 0xC7) | emsCh);
	//	Write_IOExpander(g_ioexpval);
	//
	//	// Change PWM frequency to adjust output voltage of boost converter
	//	htim3.Instance -> ARR = emsIntensity;						// Set Period
	//	htim3.Instance -> CCR2 = (htim3.Instance -> ARR) >> 1;		// Set Duty(Pulse)
}


/*******************************************************************************
 * Function Name  : BleConnection_Process
 * Description    : Processing routine in BLE Connection Mode
 * Input          : None
 * Output         : None
 *******************************************************************************/
void BleConnection_Process(void)
{
	static uint8_t led_cnt = 0;

	LCD_BLUETOOTH_PAIRING();

	while(1)
	{
		if(flag_1kHz)
		{
			flag_1kHz = 0;
			led_cnt++;
			if(led_cnt >= 5000)
			{
			}
		}

		DiffusionLEDControl(BT_PAIR);
		HAL_Delay(5000);
		LCD_BLUETOOTH_CONNECTED();
		DiffusionLEDControl(BT_CONNECTED);
		HAL_Delay(1000);

#ifdef ENABLE_A_IMAGE
		LCD_BLUETOOTH_REMOVE();
#endif
#ifdef ENABLE_B_IMAGE
		LCD_BLUETOOTH_REMOVE();
#endif

		SysMode.OperationMode = SOOTHING_MODE;
		break;
	}
}


/*******************************************************************************
 * Function Name  : BleConnection_Process
 * Description    : Processing routine in BLE Connection Mode
 * Input          : None
 * Output         : None
 *******************************************************************************/
void Display_BackgroundImg(void)
{
	switch(SysMode.OperationMode)
	{
	case SOOTHING_MODE:
		LCD_SOOTHING_Image();
		LCD_Lv_Image();
		break;
	case BLEMISH_CARE:
		LCD_BLEMISHCARE_Image();
		LCD_Lv_Image();
		break;
	case MOISTURE_MODE:
		LCD_MOISTURE_Image();
		LCD_Lv_Image();
		break;
	case COLLAGEN_MODE:
		LCD_COLLAGEN_Image();
		LCD_Lv_Image();
		break;
	case TIGHTENING_MODE:
		LCD_TIGHTENING_Image();
		break;
	case LIFTING_MODE:
		LCD_LIFTING_Image();
		LCD_Lv_Image();
		break;
	case EYECARE_MODE:
		LCD_EYECARE_Image();
		LCD_Lv_Image();
		break;
	case BLECONN_MODE:
		LCD_BLUETOOTH_PAIRING();
		break;
	case TEST_MODE:
		LCD_SOOTHING_Image();
		break;
	}

	if(SysMode.KeyLockStatus == KEY_LOCKED)		LCD_LOCK();
}
