//#include "user_uart_comm.h"
#include "a_image2_0429.h"
#include "stm32f1xx_hal.h"
#include "user_func.h"
//#include "ntc_table.h"
#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "LCD_Driver.h"
#include <stdio.h>
#include <stdarg.h>
#include "SystemProcess.h"
#include "factory_image.h"

extern uint8_t battery_level;
/*******************************************************************************
* Function Name  : LED_Enable
* Description    : Enable PWM for LED
* Input          : None
* Output         : None
*******************************************************************************/

void LED_Enable(void)
{
	// TIM1 Ch1 is used for driving source of LED
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
}

void LED_Disable(void)
{
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
}
/*******************************************************************************
* Function Name  : LED_INTENSITY_PROCESS
* Description    : Change PWM for LED INTENSITY
* Input          : None
* Output         : None
*******************************************************************************/

void diffusion_led_process(uint8_t led_r, uint8_t led_g, uint8_t led_b){
	uint16_t ccr;
	if (led_r) {
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
		/*if(led_r > LED_LIMIT)
		 led_r = LED_LIMIT;
		 */
		ccr = led_r; //GET_DIFFU_INTENSITY_CCR(led_r);

		if (ccr > (DIFF_LED_ARR + 1))
			ccr = (DIFF_LED_ARR + 1);

		if (ccr > DIFFU_LED_CCR1)
			DIFFU_LED_CCR1++;
		if (ccr < DIFFU_LED_CCR1)
			DIFFU_LED_CCR1--;
	} else {
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	}
	if (led_g) {
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
		if (led_g > LED_LIMIT)
			led_g = LED_LIMIT;

		ccr = GET_DIFFU_INTENSITY_CCR(led_g);

		if (ccr > (DIFF_LED_ARR + 1))
			ccr = (DIFF_LED_ARR + 1);

		if (ccr > DIFFU_LED_CCR2)
			DIFFU_LED_CCR2++;
		if (ccr < DIFFU_LED_CCR2)
			DIFFU_LED_CCR2--;
	} else {
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
	}
	if (led_b) {
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
		if (led_b > LED_LIMIT)
			led_b = LED_LIMIT;

		ccr = GET_DIFFU_INTENSITY_CCR(led_b);

		if (ccr > (DIFF_LED_ARR + 1))
			ccr = (DIFF_LED_ARR + 1);

		if (ccr > DIFFU_LED_CCR3)
			DIFFU_LED_CCR3++;
		if (ccr < DIFFU_LED_CCR3)
			DIFFU_LED_CCR3--;
	} else {
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
	}
}

/*******************************************************************************
 * Function Name  : LED_INTENSITY_PROCESS
 * Description    : Change PWM for LED INTENSITY
 * Input          : None
 * Output         : None
 *******************************************************************************/

void led_intensity_process(uint8_t led_b, uint8_t led_r, uint8_t led_ir) {
	uint16_t ccr;
	if (led_r) {
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
		if (led_r > LED_LIMIT)
			led_r = LED_LIMIT;

		ccr = GET_INTENSITY_CCR(led_r);

		if (ccr > (LED_ARR + 1))
			ccr = (LED_ARR + 1);

		if (ccr > LED_CCR3)
			LED_CCR3++;
		if (ccr < LED_CCR3)
			LED_CCR3--;
	} else {
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	}
	if (led_b) {
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
		if (led_b > LED_LIMIT)
			led_b = LED_LIMIT;

		ccr = GET_INTENSITY_CCR(led_b);

		if (ccr > (LED_ARR + 1))
			ccr = (LED_ARR + 1);

		if (ccr > LED_CCR1)
			LED_CCR1++;
		if (ccr < LED_CCR1)
			LED_CCR1--;
	} else {
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
	}
	if (led_ir) {
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
		if (led_ir > LED_LIMIT)
			led_ir = LED_LIMIT;

		ccr = GET_INTENSITY_CCR(led_ir);

		if (ccr > (LED_ARR + 1))
			ccr = (LED_ARR + 1);

		if (ccr > LED_CCR4)
			LED_CCR4++;
		if (ccr < LED_CCR4)
			LED_CCR4--;
	} else {
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
	}
}
/*******************************************************************************
 * Function Name  : NTC_GETTEMPADC
 * Description    : Temperature detection.
 * Input          : None
 * Output         : None
 *******************************************************************************/
float ntc_GetTempADC(const float *table, uint16_t adc) {
//	uint16_t index;
//	int32_t dx_adc, width_adc;
//	float ret;
//
//	if(table[0] > table[NTC_TABLE_SIZE-1]){
//		// NTC is Bottom Side
//		if(adc >= table[0]){
//			ret = NTC_TABLE_BOT; // ?�??
//		}
//		else if(adc <= table[ NTC_TABLE_SIZE-1]){
//			ret =  NTC_TABLE_TOP; // 고온
//		}
//		else {
//			// Serch Level
//			for(index=1 ; index <  NTC_TABLE_SIZE ; index++){
//				if(adc >= table[index]) break;
//			}
//			// 보간�?
//			dx_adc = adc - table[index - 1];
//			width_adc = table[index] - table[index - 1];
//			ret = ((float) dx_adc / width_adc + index - 1) * NTC_TABLE_UNIT + NTC_TABLE_BOT;
//		}
//	}
//	else {
//		// NTC is Top Side
//		if(adc <= table[0]){
//			ret = NTC_TABLE_BOT; // ?�??
//		}
//		else if(adc >= table[ NTC_TABLE_SIZE-1]){
//			ret =  NTC_TABLE_TOP; // 고온
//		}
//		else {
//			// Serch Level
//			for(index=1 ; index <  NTC_TABLE_SIZE ; index++){
//				if(adc <= table[index]) break;
//			}
//			// 보간�?
//			dx_adc = adc - table[index - 1];
//			width_adc = table[index] - table[index - 1];
//			ret = ((float) dx_adc / width_adc + index - 1) * NTC_TABLE_UNIT + NTC_TABLE_BOT;
//			//ret = ((float) dx_adc *  NTC_TABLE_UNIT) / width_adc + ((float) index - 1) *  NTC_TABLE_UNIT +  NTC_TABLE_BOT;
//		}
//	}
//	return ret;
}
#ifdef ENABLE_C_IMAGE
/*******************************************************************************
* Function Name  : LCD IMAGE SEQ
* Description    : Change LCD for Indicator
* Input          : None
* Output         : None
*******************************************************************************/
void LCD_SIZE_Image(void){
	LCD_Clear(BLACK);
	Paint_DrawImageMod(gImage_bmp_test, 0, 0, 160, 80);
	//HAL_Delay(500);
}

void LCD_LOGO_Image(void){
	LCD_Clear(BLACK);
	Paint_DrawImageMod(gImage_img_logo, 54, 4, 40, 72);
	//HAL_Delay(500);
}

void LCD_SOOTHING_Image(void){
//	LCD_Clear(BLACK);
	//LCD_ClearWindowMod(30,4,18,72,BLACK);
	LCD_ClearWindowMod(54,4,10,72,BLACK);
	LCD_ClearWindowMod(138, 4, 18, 72, BLACK);
	//LCD_ClearWindowMod(128, 4, 18, 72, BLACK);

	Paint_DrawImageMod(gImage_txt_mode_soothing, 30, 4, 18, 72);
	Paint_DrawImageMod(gImage_ic_page_base, 150, 7, 6, 66);
	Paint_DrawImageMod(gImage_ic_page_active, 150, 66, 6, 6);
	Paint_DrawImageMod(gImage_ic_mode_soothing, 68, 4, 60, 72);
}


void LCD_BLEMISHCARE_Image(void)
{
	//LCD_ClearWindowMod(30,4,18,72,BLACK);
	//LCD_ClearWindowMod(54,4,20,72,BLACK);
	//LCD_ClearWindowMod(138, 4, 18, 72, BLACK);
	LCD_ClearWindowMod(138, 4, 18, 72, BLACK);

	Paint_DrawImageMod(gImage_txt_mode_blemishcare, 30, 4, 30, 72);
	Paint_DrawImageMod(gImage_ic_page_base, 150, 7, 6, 66);
	Paint_DrawImageMod(gImage_ic_page_active, 150, 56, 6, 6);
	Paint_DrawImageMod(gImage_ic_mode_blemishcare, 68, 4, 60, 72);
}


void LCD_MOISTURE_Image(void){
	LCD_ClearWindowMod(40,4,30,72,BLACK);
	LCD_ClearWindowMod(138, 4, 18, 72, BLACK);
	//LCD_ClearWindowMod(128, 4, 18, 72, BLACK);

	Paint_DrawImageMod(gImage_txt_mode_moisture, 30, 4, 18, 72);
	Paint_DrawImageMod(gImage_ic_page_base, 150, 7, 6, 66);
	Paint_DrawImageMod(gImage_ic_page_active, 150, 46, 6, 6);
	//Level Change
	Paint_DrawImageMod(gImage_ic_mode_moisture, 68, 4, 60, 72);

}


void LCD_MOISTURE_Lv_Image(void)
{
	if(SysMode.OperationIntensity == LEVEL1){
		Paint_DrawImageMod(gImage_txt_level_1, 86, 30, 26, 20);
	}
	else if(SysMode.OperationIntensity == LEVEL2){
		Paint_DrawImageMod(gImage_txt_level_2, 86, 30, 26, 20);
	}
	else if(SysMode.OperationIntensity == LEVEL3){
		Paint_DrawImageMod(gImage_txt_level_3, 86, 30, 26, 20);
	}
	else if(SysMode.OperationIntensity == LEVEL4){
		Paint_DrawImageMod(gImage_txt_level_4, 86, 30, 26, 20);
	}
	else if(SysMode.OperationIntensity == LEVEL5){
		Paint_DrawImageMod(gImage_txt_level_5, 86, 30, 26, 20);
	}
}
void LCD_COLLAGEN_Image(void){

	//LCD_ClearWindowMod(30,4,18,72,BLACK);
	//LCD_ClearWindowMod(54,4,20,72,BLACK);
	LCD_ClearWindowMod(138, 4, 18, 72, BLACK);

	Paint_DrawImageMod(gImage_txt_mode_collagen, 30, 4, 18, 72);
	Paint_DrawImageMod(gImage_ic_page_base, 150, 7, 6, 66);
	Paint_DrawImageMod(gImage_ic_page_active, 150, 36, 6, 6);
	Paint_DrawImageMod(gImage_ic_mode_collagen, 68, 4, 60, 72);
}
void LCD_TIGHTENING_Image(void)
{
	//LCD_ClearWindowMod(30,4,18,72,BLACK);
	//LCD_ClearWindowMod(54,4,20,72,BLACK);
	LCD_ClearWindowMod(138, 4, 18, 72, BLACK);
	//LCD_ClearWindowMod(128, 4, 18, 72, BLACK);

	Paint_DrawImageMod(gImage_txt_mode_tightening, 30, 4, 18, 72);
	Paint_DrawImageMod(gImage_ic_page_base, 150, 7, 6, 66);
	Paint_DrawImageMod(gImage_ic_page_active, 150, 26, 6, 6);
	Paint_DrawImageMod(gImage_ic_mode_tightening, 68, 4, 60, 72);
}
void LCD_LIFTING_Image(void){
	//LCD_ClearWindowMod(30,4,18,72,BLACK);
	//LCD_ClearWindowMod(54,4,20,72,BLACK);
	LCD_ClearWindowMod(138, 4, 18, 72, BLACK);
	//LCD_ClearWindowMod(128, 4, 18, 72, BLACK);

	Paint_DrawImageMod(gImage_txt_mode_lifting, 30, 4, 18, 72);
	Paint_DrawImageMod(gImage_ic_page_base, 150, 7, 6, 66);
	Paint_DrawImageMod(gImage_ic_page_active, 150, 16, 6, 6);
	Paint_DrawImageMod(gImage_ic_mode_lifting, 68, 4, 60, 72);
}


void LCD_LIFTING_Lv_Image(void)
{
	if(SysMode.OperationIntensity == LEVEL1){
		Paint_DrawImageMod(gImage_txt_level_1, 86, 30, 26, 20);
	}
	else if(SysMode.OperationIntensity == LEVEL2){
		Paint_DrawImageMod(gImage_txt_level_2, 86, 30, 26, 20);
	}
	else if(SysMode.OperationIntensity == LEVEL3){
		Paint_DrawImageMod(gImage_txt_level_3, 86, 30, 26, 20);
	}
	else if(SysMode.OperationIntensity == LEVEL4){
		Paint_DrawImageMod(gImage_txt_level_4, 86, 30, 26, 20);
	}
	else if(SysMode.OperationIntensity == LEVEL5){
		Paint_DrawImageMod(gImage_txt_level_5, 86, 30, 26, 20);
	}
}


void LCD_EYECARE_Image(void)
{
	//LCD_ClearWindowMod(30,4,18,72,BLACK);
	//LCD_ClearWindowMod(54,4,20,72,BLACK);
	//LCD_ClearWindowMod(138, 4, 18, 72, BLACK);
	//LCD_ClearWindowMod(128, 4, 18, 72, BLACK);

	Paint_DrawImageMod(gImage_txt_mode_eyecare, 30, 4, 18, 72);
	Paint_DrawImageMod(gImage_ic_page_base, 150, 7, 6, 66);
	Paint_DrawImageMod(gImage_ic_page_active, 150, 6, 6, 6);
	Paint_DrawImageMod(gImage_ic_mode_eyecare, 68, 4, 60, 72);
}
//void LCD_BLUETOOTH_PAIRING(void){
//	LCD_Clear(BLACK);
////	for(int i=0; i<4; i++){
//		Paint_DrawImageMod(gImage_txt_mode_bluetooth, 32, 8, 13, 64);
//		Paint_DrawImageMod(gImage_ic_mode_bluetooth_2, 55, 8, 50, 64);
//		Paint_DrawImageMod(gImage_txt_pairing, 115, 8, 26, 80);
//		HAL_Delay(700);
//		Paint_DrawImageMod(gImage_ic_mode_bluetooth_1, 55, 8, 50, 64);
//		HAL_Delay(300);
////	}
//}


/*******************************************************************************
* Function Name  : LCD_BLUETOOTH_PAIRING
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void LCD_BLUETOOTH_PAIRING(void)
{
	LCD_Clear(BLACK);
	//Paint_DrawImageMod(gImage_ic_battery_3, 10, 31, 18, 18);
	Paint_DrawImageMod(gImage_txt_mode_bluetooth, 30, 4, 18, 72);

	//Paint_DrawImageMod(gImage_ic_mode_bluetooth_2, 55, 8, 50, 64);
	Paint_DrawImageMod(gImage_txt_pairing, 138, 4, 18, 72);
	//HAL_Delay(700);
	Paint_DrawImageMod(gImage_ic_mode_bluetooth_1, 68, 4, 60, 72);
	//HAL_Delay(300);

	//Paint_DrawImageMod(gImage_ic_page_base, 131, 35, 4, 11);
	//Paint_DrawImageMod(gImage_ic_page_active, 131, 42, 4, 4);
}


/*******************************************************************************
* Function Name  : LCD_BLUETOOTH_CONNECTED
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void LCD_BLUETOOTH_CONNECTED(void)
{
	Paint_DrawImageMod(gImage_txt_connected, 138, 4, 18, 72);
}


/*******************************************************************************
* Function Name  : LCD_LOCK
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void LCD_LOCK(void)
{
	//LCD_Clear(BLACK);
//	LCD_ClearWindowMod(150,4,60,72,BLACK);
//	LCD_ClearWindowMod(150,4,60,72,BLACK);
	Paint_DrawImageMod(gImage_ic_status_lock, 4, 31, 18, 18);
}


/*******************************************************************************
* Function Name  : LCD_LOCK_POP
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void LCD_LOCK_POP(void)
{
//	LCD_Clear(BLACK);
	LCD_ClearWindowMod(30,4,30,72,BLACK);
	LCD_ClearWindowMod(150, 7, 6, 66, BLACK);

	Paint_DrawImageMod(gImage_ic_mode_lock, 68, 4, 60, 72);
	Paint_DrawImageMod(gImage_txt_lock, 138, 4, 18, 72);
}


/*******************************************************************************
* Function Name  : LCD_UNLOCK_POP
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void LCD_UNLOCK_POP(void)
{
	LCD_ClearWindowMod(4, 31, 18, 18, BLACK);
	LCD_ClearWindowMod(30,4,30,72,BLACK);
	LCD_ClearWindowMod(150, 7, 6, 66, BLACK);

	Paint_DrawImageMod(gImage_ic_mode_unlock, 68, 4, 60, 72);
	Paint_DrawImageMod(gImage_txt_unlock, 138, 4, 18, 72);
}


/*******************************************************************************
* Function Name  : LCD_LOW_BATTERY
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void LCD_LOW_BATTERY(void)
{
	LCD_Clear(BLACK);
	Paint_DrawImageMod(gImage_ic_battery_low_1, 68, 4, 60, 72);
	HAL_Delay(700);
	Paint_DrawImageMod(gImage_ic_battery_low_2, 68, 4, 60, 72);
	HAL_Delay(300);
}

/*******************************************************************************
* Function Name  : LCD_CHARGING
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void LCD_CHARGING(void)
{
	//LCD_Clear(BLACK);
	Paint_DrawImageMod(gImage_ic_battery_charging, 68, 4, 60, 72);
}
#endif

#ifdef ENABLE_B_IMAGE
/*******************************************************************************
* Function Name  : LCD IMAGE SEQ
* Description    : Change LCD for Indicator
* Input          : None
* Output         : None
*******************************************************************************/
void LCD_SIZE_Image(void){
	LCD_Clear(BLACK);
	Paint_DrawImageMod(gImage_bmp_test, 0, 0, 160, 80);
	//HAL_Delay(500);
}

void LCD_LOGO_Image(void){
	LCD_Clear(BLACK);
	Paint_DrawImageMod(gImage_img_logo, 54, 4, 40, 72);
	//HAL_Delay(500);
}

void LCD_TIME_Image(uint8_t time) {
	printf(">> origin = %d ,remain = %d : %d : %d \r\n", time, time / 60,
			(time % 60) / 10, (time % 60) % 10);
	Paint_DrawImageMod(gImage_ic_time_dot, 113, 45, 26, 9); //47
	if (time > 0 && time!= 180) {
		switch (time / 60) {
		case 0:
			Paint_DrawImageMod(gImage_txt_time_2, 113, 54, 26, 19); // �?
			break;
		case 1:
			Paint_DrawImageMod(gImage_txt_time_1, 113, 54, 26, 19); // �?
			break;
		case 2:
			Paint_DrawImageMod(gImage_txt_time_0, 113, 54, 26, 19); // �?
			break;
		case 3:
			Paint_DrawImageMod(gImage_txt_time_3, 113, 54, 26, 19); // �?
			break;
		}
		switch ((time % 60) / 10) {
		case 0:
			Paint_DrawImageMod(gImage_txt_time_5, 113, 26, 26, 19); //?�위_�?
			break;
		case 1:
			Paint_DrawImageMod(gImage_txt_time_4, 113, 26, 26, 19); //?�위_�?
			break;
		case 2:
			Paint_DrawImageMod(gImage_txt_time_3, 113, 26, 26, 19); //?�위_�?
			break;
		case 3:
			Paint_DrawImageMod(gImage_txt_time_2, 113, 26, 26, 19); //?�위_�?
			break;
		case 4:
			Paint_DrawImageMod(gImage_txt_time_1, 113, 26, 26, 19); //?�위_�?
			break;
		case 5:
			Paint_DrawImageMod(gImage_txt_time_0, 113, 26, 26, 19); //?�위_�?
			break;
		}
		switch ((time % 60) % 10) {
		case 0:
			Paint_DrawImageMod(gImage_txt_time_9, 113, 7, 26, 19); //?�위_�?
			break;
		case 1:
			Paint_DrawImageMod(gImage_txt_time_8, 113, 7, 26, 19); //?�위_�?
			break;
		case 2:
			Paint_DrawImageMod(gImage_txt_time_7, 113, 7, 26, 19); //?�위_�?
			break;
		case 3:
			Paint_DrawImageMod(gImage_txt_time_6, 113, 7, 26, 19); //?�위_�?
			break;
		case 4:
			Paint_DrawImageMod(gImage_txt_time_5, 113, 7, 26, 19); //?�위_�?
			break;
		case 5:
			Paint_DrawImageMod(gImage_txt_time_4, 113, 7, 26, 19); //?�위_�?
			break;
		case 6:
			Paint_DrawImageMod(gImage_txt_time_3, 113, 7, 26, 19); //?�위_�?
			break;
		case 7:
			Paint_DrawImageMod(gImage_txt_time_2, 113, 7, 26, 19); //?�위_�?
			break;
		case 8:
			Paint_DrawImageMod(gImage_txt_time_1, 113, 7, 26, 19); //?�위_�?
			break;
		case 9:
			Paint_DrawImageMod(gImage_txt_time_0, 113, 7, 26, 19); //?�위_�?
			break;
		}
	}
	else{
		Paint_DrawImageMod(gImage_txt_time_3, 113, 54, 26, 19); // �?
		Paint_DrawImageMod(gImage_txt_time_0, 113, 26, 26, 19); //?�위_�?
		Paint_DrawImageMod(gImage_txt_time_0, 113, 7, 26, 19); //?�위_�?
	}
	//Paint_DrawImageMod(gImage_txt_time_0, 113, 7, 26, 19); //?�위_�?

//	Paint_DrawImageMod(gImage_txt_time_0, 113, 26, 26, 19); //?�위_�?

//	Paint_DrawImageMod(gImage_txt_time_0, 113, 54, 26, 19); // �?
}

void LCD_SOOTHING_Image(void){
	//LCD_Clear(BLACK);
	//LCD_ClearWindowMod(22,4,100,80,BLACK);
	//LCD_ClearWindowMod(115, 15, 12, 50,BLACK);
	Paint_DrawImageMod(gImage_txt_mode_soothing, 22, 4, 15, 72);
	Paint_DrawImageMod(gImage_ic_mode_soothing, 51, 4, 58, 72);
}

void LCD_BLEMISHCARE_Image(void)
{
	//LCD_Clear(BLACK);
	//LCD_ClearWindowMod(22,4,100,80,BLACK);
	//LCD_ClearWindowMod(115, 15, 12, 50,BLACK);
	Paint_DrawImageMod(gImage_txt_mode_blemishcare, 22, 4, 28, 72);
	Paint_DrawImageMod(gImage_ic_mode_blemishcare, 51, 4, 58, 72);
}


void LCD_MOISTURE_Image(void){
	LCD_ClearWindowMod(37,4,46,72,BLACK);

	Paint_DrawImageMod(gImage_txt_mode_moisture, 22, 4, 15, 72);
	Paint_DrawImageMod(gImage_ic_mode_moisture, 51, 4, 58, 72);

	//Level Change
}

void LCD_MOISTURE_Lv_Image(void)
{
	if(SysMode.OperationIntensity == LEVEL1){
		Paint_DrawImageMod(gImage_ic_level_1, 54, 12, 52, 8);
	}
	else if(SysMode.OperationIntensity == LEVEL2){
		Paint_DrawImageMod(gImage_ic_level_2, 54, 12, 52, 8);
	}
	else if(SysMode.OperationIntensity == LEVEL3){
		Paint_DrawImageMod(gImage_ic_level_3, 54, 12, 52, 8);
	}
	else if(SysMode.OperationIntensity == LEVEL4){
		Paint_DrawImageMod(gImage_ic_level_4, 54, 12, 52, 8);
	}
	else if(SysMode.OperationIntensity == LEVEL5){
		Paint_DrawImageMod(gImage_ic_level_5, 54, 12, 52, 8);
	}
}
void LCD_COLLAGEN_Image(void){
	//LCD_ClearWindowMod(22,4,100,80,BLACK);
	//LCD_ClearWindowMod(115, 15, 12, 50,BLACK);
	Paint_DrawImageMod(gImage_txt_mode_collagen, 22, 4, 15, 72);
	Paint_DrawImageMod(gImage_ic_mode_collagen, 51, 4, 58, 72);

}
void LCD_TIGHTENING_Image(void)
{
	//LCD_ClearWindowMod(22,4, 100,80,BLACK);
	//LCD_ClearWindowMod(115, 15, 12, 50,BLACK);

	Paint_DrawImageMod(gImage_txt_mode_tightening, 22, 4, 15, 72);
	Paint_DrawImageMod(gImage_ic_mode_tightening, 51, 4, 58, 72);
}
void LCD_LIFTING_Image(void){
	//LCD_ClearWindowMod(22,4,100,80,BLACK);
	//LCD_ClearWindowMod(115, 15, 12, 50,BLACK);
	Paint_DrawImageMod(gImage_txt_mode_lifting, 22, 4, 15, 72);
	Paint_DrawImageMod(gImage_ic_mode_lifting, 51, 4, 58, 72);
}


void LCD_LIFTING_Lv_Image(void)
{
	if(SysMode.OperationIntensity == LEVEL1){
		Paint_DrawImageMod(gImage_ic_level_1, 54, 12, 52, 8);
	}
	else if(SysMode.OperationIntensity == LEVEL2){
		Paint_DrawImageMod(gImage_ic_level_2, 54, 12, 52, 8);
	}
	else if(SysMode.OperationIntensity == LEVEL3){
		Paint_DrawImageMod(gImage_ic_level_3, 54, 12, 52, 8);
	}
	else if(SysMode.OperationIntensity == LEVEL4){
		Paint_DrawImageMod(gImage_ic_level_4, 54, 12, 52, 8);
	}
	else if(SysMode.OperationIntensity == LEVEL5){
		Paint_DrawImageMod(gImage_ic_level_5, 54, 12, 52, 8);
	}
}


void LCD_EYECARE_Image(void)
{
	//LCD_ClearWindowMod(22,4,100,80,BLACK);
	//LCD_ClearWindowMod(115, 15, 12, 50,BLACK);
	Paint_DrawImageMod(gImage_txt_mode_eyecare, 22, 4, 15, 72);
	Paint_DrawImageMod(gImage_ic_mode_eyecare, 51, 4, 58, 72);
}
//void LCD_BLUETOOTH_PAIRING(void){
//	LCD_Clear(BLACK);
////	for(int i=0; i<4; i++){
//		Paint_DrawImageMod(gImage_txt_mode_bluetooth, 32, 8, 13, 64);
//		Paint_DrawImageMod(gImage_ic_mode_bluetooth_2, 55, 8, 50, 64);
//		Paint_DrawImageMod(gImage_txt_pairing, 115, 8, 26, 80);
//		HAL_Delay(700);
//		Paint_DrawImageMod(gImage_ic_mode_bluetooth_1, 55, 8, 50, 64);
//		HAL_Delay(300);
////	}
//}


/*******************************************************************************
* Function Name  : LCD_BLUETOOTH_PAIRING
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void LCD_BLUETOOTH_PAIRING(void)
{
	LCD_Clear(BLACK);
	//Paint_DrawImageMod(gImage_ic_battery_3, 10, 31, 18, 18);
	Paint_DrawImageMod(gImage_txt_mode_bluetooth, 22, 4, 15, 72);

	//Paint_DrawImageMod(gImage_ic_mode_bluetooth_2, 55, 8, 50, 64);
	Paint_DrawImageMod(gImage_txt_pairing, 114, 4, 15, 72);
	//HAL_Delay(700);
	Paint_DrawImageMod(gImage_ic_mode_bluetooth_1, 51, 4, 58, 72);
	//HAL_Delay(300);
}


/*******************************************************************************
* Function Name  : LCD_BLUETOOTH_CONNECTED
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void LCD_BLUETOOTH_CONNECTED(void)
{
	LCD_ClearWindowMod(114, 4, 15, 72, BLACK);
	Paint_DrawImageMod(gImage_txt_connected, 115, 4, 15, 72);
}

/*******************************************************************************
 * Function Name  : LCD_BLUETOOTH_CONNECTED
 * Description    :
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LCD_BLUETOOTH_REMOVE(void) {
	LCD_ClearWindowMod(114,4,15,72,BLACK);
}
/*******************************************************************************
* Function Name  : LCD_LOCK
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void LCD_LOCK(void)
{
	//LCD_Clear(BLACK);
//	LCD_ClearWindowMod(150,4,60,72,BLACK);
//	LCD_ClearWindowMod(150,4,60,72,BLACK);
	Paint_DrawImageMod(gImage_ic_status_lock, 4, 35, 14, 10);

}


/*******************************************************************************
* Function Name  : LCD_LOCK_POP
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void LCD_LOCK_POP(void)
{
	LCD_Clear(BLACK);
//	LCD_ClearWindowMod(150,4,60,72,BLACK);
//	LCD_ClearWindowMod(150,4,60,72,BLACK);
//	LCD_ClearWindowMod(150,4,60,72,BLACK);

	Paint_DrawImageMod(gImage_ic_mode_lock, 51, 4, 58, 72);
	Paint_DrawImageMod(gImage_txt_lock, 115, 4, 15, 72);

}


/*******************************************************************************
* Function Name  : LCD_UNLOCK_POP
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void LCD_UNLOCK_POP(void)
{
	LCD_Clear(BLACK);
	//	LCD_ClearWindowMod(150,4,60,72,BLACK);
	//	LCD_ClearWindowMod(150,4,60,72,BLACK);
	//	LCD_ClearWindowMod(150,4,60,72,BLACK);
	Paint_DrawImageMod(gImage_ic_mode_unlock, 51, 4, 58, 72);
	Paint_DrawImageMod(gImage_txt_unlock, 115, 4, 15, 72);
}


/*******************************************************************************
* Function Name  : LCD_LOW_BATTERY
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void LCD_LOW_BATTERY(void)
{
	LCD_Clear(BLACK);
	Paint_DrawImageMod(gImage_ic_battery_low_1, 64, 6, 33, 69);
	HAL_Delay(700);
	Paint_DrawImageMod(gImage_ic_battery_low_2, 64, 6, 33, 69);
	HAL_Delay(300);
}

/*******************************************************************************
* Function Name  : LCD_CHARGING
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void LCD_CHARGING(void)
{
	//LCD_Clear(BLACK);
	Paint_DrawImageMod(gImage_ic_battery_charging, 64, 6, 33, 69);
}
#endif

#ifdef ENABLE_A_IMAGE
/*******************************************************************************
 * Function Name  : LCD IMAGE SEQ
 * Description    : Change LCD for Indicator
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LCD_SIZE_Image(void) {
	LCD_Clear(BLACK);
	Paint_DrawImageMod(gImage_bmp_test, 0, 0, 160, 80);
	//HAL_Delay(500);
}

void LCD_LOGO_Image(void) {
	LCD_Clear(BLACK);
	Paint_DrawImageMod(gImage_img_logo, 54, 4, 40, 72);
	//HAL_Delay(500);
}
void LCD_BATTARY_STATE(void) {
	if (battery_level == 1) {
		Paint_DrawImageMod(gImage_ic_battery_1, 4, 32, 8, 16);
	} else if (battery_level == 2) {
		Paint_DrawImageMod(gImage_ic_battery_2, 4, 32, 8, 16);
	} else if (battery_level == 3) {
		Paint_DrawImageMod(gImage_ic_battery_3, 4, 32, 8, 16);
	}
}

void LCD_TIME_Image(uint8_t time) {
	//printf(">> origin = %d ,remain = %d : %d : %d \r\n", time, time / 60,
	//		(time % 60) / 10, (time % 60) % 10);
	Paint_DrawImageMod(gImage_ic_time_dot, 113, 45, 26, 9); //47
	if (time > 0 && time!= 180) {
		switch (time / 60) {
		case 0:
			Paint_DrawImageMod(gImage_txt_time_2, 113, 54, 26, 19); // �?
			break;
		case 1:
			Paint_DrawImageMod(gImage_txt_time_1, 113, 54, 26, 19); // �?
			break;
		case 2:
			Paint_DrawImageMod(gImage_txt_time_0, 113, 54, 26, 19); // �?
			break;
		case 3:
			Paint_DrawImageMod(gImage_txt_time_3, 113, 54, 26, 19); // �?
			break;
		}
		switch ((time % 60) / 10) {
		case 0:
			Paint_DrawImageMod(gImage_txt_time_5, 113, 26, 26, 19); //?�위_�?
			break;
		case 1:
			Paint_DrawImageMod(gImage_txt_time_4, 113, 26, 26, 19); //?�위_�?
			break;
		case 2:
			Paint_DrawImageMod(gImage_txt_time_3, 113, 26, 26, 19); //?�위_�?
			break;
		case 3:
			Paint_DrawImageMod(gImage_txt_time_2, 113, 26, 26, 19); //?�위_�?
			break;
		case 4:
			Paint_DrawImageMod(gImage_txt_time_1, 113, 26, 26, 19); //?�위_�?
			break;
		case 5:
			Paint_DrawImageMod(gImage_txt_time_0, 113, 26, 26, 19); //?�위_�?
			break;
		}
		switch ((time % 60) % 10) {
		case 0:
			Paint_DrawImageMod(gImage_txt_time_9, 113, 7, 26, 19); //?�위_�?
			break;
		case 1:
			Paint_DrawImageMod(gImage_txt_time_8, 113, 7, 26, 19); //?�위_�?
			break;
		case 2:
			Paint_DrawImageMod(gImage_txt_time_7, 113, 7, 26, 19); //?�위_�?
			break;
		case 3:
			Paint_DrawImageMod(gImage_txt_time_6, 113, 7, 26, 19); //?�위_�?
			break;
		case 4:
			Paint_DrawImageMod(gImage_txt_time_5, 113, 7, 26, 19); //?�위_�?
			break;
		case 5:
			Paint_DrawImageMod(gImage_txt_time_4, 113, 7, 26, 19); //?�위_�?
			break;
		case 6:
			Paint_DrawImageMod(gImage_txt_time_3, 113, 7, 26, 19); //?�위_�?
			break;
		case 7:
			Paint_DrawImageMod(gImage_txt_time_2, 113, 7, 26, 19); //?�위_�?
			break;
		case 8:
			Paint_DrawImageMod(gImage_txt_time_1, 113, 7, 26, 19); //?�위_�?
			break;
		case 9:
			Paint_DrawImageMod(gImage_txt_time_0, 113, 7, 26, 19); //?�위_�?
			break;
		}
	}
	else{
		Paint_DrawImageMod(gImage_txt_time_3, 113, 54, 26, 19); // �?
		Paint_DrawImageMod(gImage_txt_time_0, 113, 26, 26, 19); //?�위_�?
		Paint_DrawImageMod(gImage_txt_time_0, 113, 7, 26, 19); //?�위_�?
	}
	//Paint_DrawImageMod(gImage_txt_time_0, 113, 7, 26, 19); //?�위_�?

//	Paint_DrawImageMod(gImage_txt_time_0, 113, 26, 26, 19); //?�위_�?

//	Paint_DrawImageMod(gImage_txt_time_0, 113, 54, 26, 19); // �?
}

void LCD_SOOTHING_Image(void) {
	//LCD_ClearWindowMod(113,45,139,47,BLACK);

	Paint_DrawImageMod(gImage_txt_mode_soothing, 22, 4, 15, 72);
	Paint_DrawImageMod(gImage_ic_mode_soothing, 46, 4, 58, 72);
	Paint_DrawImageMod(gImage_ic_page_base, 152, 14, 4, 52);
	Paint_DrawImageMod(gImage_ic_page_active, 152, 62, 4, 4);
}

void LCD_BLEMISHCARE_Image(void) {
	//LCD_ClearWindowMod(113,45,139,47,BLACK);

	Paint_DrawImageMod(gImage_txt_mode_blemishcare, 22, 4, 28, 72);
	Paint_DrawImageMod(gImage_ic_mode_blemishcare, 50, 4, 54, 72);

	Paint_DrawImageMod(gImage_ic_page_base, 152, 14, 4, 52);
	Paint_DrawImageMod(gImage_ic_page_active, 152, 54, 4, 4);
}

void LCD_MOISTURE_Image(void) {
	//LCD_ClearWindowMod(113,45,139,47,BLACK);
	LCD_ClearWindowMod(37,4,46,72,BLACK);
	Paint_DrawImageMod(gImage_txt_mode_moisture, 22, 4, 15, 72);
	Paint_DrawImageMod(gImage_ic_mode_moisture, 46, 4, 58, 72);

	Paint_DrawImageMod(gImage_ic_page_base, 152, 14, 4, 52);
	Paint_DrawImageMod(gImage_ic_page_active, 152, 46, 4, 4);
	//Level Change
}

void LCD_MOISTURE_Lv_Image(void) {
	if (SysMode.OperationIntensity == LEVEL1) {
		Paint_DrawImageMod(gImage_txt_level_1, 64, 31, 23, 18);
	} else if (SysMode.OperationIntensity == LEVEL2) {
		Paint_DrawImageMod(gImage_txt_level_2, 64, 31, 23, 18);
	} else if (SysMode.OperationIntensity == LEVEL3) {
		Paint_DrawImageMod(gImage_txt_level_3, 64, 31, 23, 18);
	} else if (SysMode.OperationIntensity == LEVEL4) {
		Paint_DrawImageMod(gImage_txt_level_4, 64, 31, 23, 18);
	} else if (SysMode.OperationIntensity == LEVEL5) {
		Paint_DrawImageMod(gImage_txt_level_5, 64, 31, 23, 18);
	}
}
void LCD_COLLAGEN_Image(void) {
	//LCD_ClearWindowMod(113,45,139,47,BLACK);

	Paint_DrawImageMod(gImage_txt_mode_collagen, 22, 4, 15, 72);
	Paint_DrawImageMod(gImage_ic_mode_collagen, 46, 4, 58, 72);

	Paint_DrawImageMod(gImage_ic_page_base, 152, 14, 4, 52);
	Paint_DrawImageMod(gImage_ic_page_active, 152, 38, 4, 4);
}
void LCD_TIGHTENING_Image(void) {
	//LCD_ClearWindowMod(113,45,139,47,BLACK);

	Paint_DrawImageMod(gImage_txt_mode_tightening, 22, 4, 15, 72);

	Paint_DrawImageMod(gImage_ic_mode_tightening, 46, 4, 58, 72);

	Paint_DrawImageMod(gImage_ic_page_base, 152, 14, 4, 52);
	Paint_DrawImageMod(gImage_ic_page_active, 152, 30, 4, 4);
}
void LCD_LIFTING_Image(void) {
	//LCD_ClearWindowMod(113,45,139,47,BLACK);

	Paint_DrawImageMod(gImage_txt_mode_lifting, 22, 4, 15, 72);

	Paint_DrawImageMod(gImage_ic_mode_lifting, 46, 4, 58, 72);

	Paint_DrawImageMod(gImage_ic_page_base, 152, 14, 4, 52);
	Paint_DrawImageMod(gImage_ic_page_active, 152, 22, 4, 4);
}

void LCD_LIFTING_Lv_Image(void) {
	if (SysMode.OperationIntensity == LEVEL1) {
		Paint_DrawImageMod(gImage_txt_level_1, 64, 31, 23, 18);
	} else if (SysMode.OperationIntensity == LEVEL2) {
		Paint_DrawImageMod(gImage_txt_level_2, 64, 31, 23, 18);
	} else if (SysMode.OperationIntensity == LEVEL3) {
		Paint_DrawImageMod(gImage_txt_level_3, 64, 31, 23, 18);
	} else if (SysMode.OperationIntensity == LEVEL4) {
		Paint_DrawImageMod(gImage_txt_level_4, 64, 31, 23, 18);
	} else if (SysMode.OperationIntensity == LEVEL5) {
		Paint_DrawImageMod(gImage_txt_level_5, 64, 31, 23, 18);
	}
}

void LCD_EYECARE_Image(void) {
	//LCD_ClearWindowMod(113,45,139,47,BLACK);
	Paint_DrawImageMod(gImage_txt_mode_eyecare, 22, 4, 15, 72);

	Paint_DrawImageMod(gImage_ic_mode_eyecare, 46, 4, 58, 72);
	Paint_DrawImageMod(gImage_ic_page_base, 152, 14, 4, 52);
	Paint_DrawImageMod(gImage_ic_page_active, 152, 14, 4, 4);
}
//void LCD_BLUETOOTH_PAIRING(void){
//	LCD_Clear(BLACK);
////	for(int i=0; i<4; i++){
//		Paint_DrawImageMod(gImage_txt_mode_bluetooth, 32, 8, 13, 64);
//		Paint_DrawImageMod(gImage_ic_mode_bluetooth_2, 55, 8, 50, 64);
//		Paint_DrawImageMod(gImage_txt_pairing, 115, 8, 26, 80);
//		HAL_Delay(700);
//		Paint_DrawImageMod(gImage_ic_mode_bluetooth_1, 55, 8, 50, 64);
//		HAL_Delay(300);
////	}
//}

/*******************************************************************************
 * Function Name  : LCD_BLUETOOTH_PAIRING
 * Description    :
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LCD_BLUETOOTH_PAIRING(void) {
	LCD_Clear(BLACK);
	//Paint_DrawImageMod(gImage_ic_battery_3, 10, 31, 18, 18);
	Paint_DrawImageMod(gImage_txt_mode_bluetooth, 22, 6, 15, 72);

	//Paint_DrawImageMod(gImage_ic_mode_bluetooth_2, 55, 8, 50, 64);
	Paint_DrawImageMod(gImage_txt_pairing, 115, 4, 15, 72);
	//HAL_Delay(700);
	Paint_DrawImageMod(gImage_ic_mode_bluetooth_1, 46, 4, 58, 72);
	//HAL_Delay(300);

	Paint_DrawImageMod(gImage_ic_set_page_base, 152, 34, 4, 12);
	Paint_DrawImageMod(gImage_ic_page_active, 152, 34, 4, 4);
}

/*******************************************************************************
 * Function Name  : LCD_BLUETOOTH_CONNECTED
 * Description    :
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LCD_BLUETOOTH_CONNECTED(void) {
	Paint_DrawImageMod(gImage_txt_connected, 115, 4, 15, 72);
}
/*******************************************************************************
 * Function Name  : LCD_BLUETOOTH_CONNECTED
 * Description    :
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LCD_BLUETOOTH_REMOVE(void) {
	LCD_ClearWindowMod(115,4,15,72,BLACK);
}
/*******************************************************************************
 * Function Name  : LCD_LOCK
 * Description    :
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LCD_LOCK(void) {
	//LCD_Clear(BLACK);
//	LCD_ClearWindowMod(150,4,60,72,BLACK);
//	LCD_ClearWindowMod(150,4,60,72,BLACK);
	Paint_DrawImageMod(gImage_ic_status_lock, 87, 2, 14, 10);
}

/*******************************************************************************
 * Function Name  : LCD_LOCK_POP
 * Description    :
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LCD_LOCK_POP(void) {
	LCD_Clear(BLACK);
//	LCD_ClearWindowMod(150,4,60,72,BLACK);
//	LCD_ClearWindowMod(150,4,60,72,BLACK);
//	LCD_ClearWindowMod(150,4,60,72,BLACK);

	Paint_DrawImageMod(gImage_ic_mode_lock, 46, 4, 58, 72);
	Paint_DrawImageMod(gImage_txt_lock, 115, 4, 15, 72);
}

/*******************************************************************************
 * Function Name  : LCD_UNLOCK_POP
 * Description    :
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LCD_UNLOCK_POP(void) {
	LCD_Clear(BLACK);
	//	LCD_ClearWindowMod(150,4,60,72,BLACK);
	//	LCD_ClearWindowMod(150,4,60,72,BLACK);
	//	LCD_ClearWindowMod(150,4,60,72,BLACK);
	Paint_DrawImageMod(gImage_ic_mode_unlock, 46, 4, 58, 72);
	Paint_DrawImageMod(gImage_txt_unlock, 115, 4, 15, 72);
}

/*******************************************************************************
 * Function Name  : LCD_LOW_BATTERY
 * Description    :
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LCD_LOW_BATTERY(void) {
	LCD_Clear(BLACK);
	Paint_DrawImageMod(gImage_ic_battery_low_1, 64, 6, 33, 69);
	HAL_Delay(700);
	Paint_DrawImageMod(gImage_ic_battery_low_2, 64, 6, 33, 69);
	HAL_Delay(300);
}

/*******************************************************************************
 * Function Name  : LCD_CHARGING
 * Description    :
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LCD_CHARGING(void) {
	//LCD_Clear(BLACK);
	Paint_DrawImageMod(gImage_ic_battery_charging, 64, 6, 33, 69);
}
#endif


#ifdef FACTORY_IMAGE

void LCD_FACTORY(void) {
	//LCD_Clear(BLACK);
	/*Paint_DrawImageMod(gImage_cool_80x24, 0, 0, 24, 80);
	HAL_Delay(500);
	Paint_DrawImageMod(gImage_ems_80x24, 0, 0, 24, 80);
	HAL_Delay(500);
	Paint_DrawImageMod(gImage_ion_n_80x24, 0, 0, 24, 80);
	HAL_Delay(500);
	Paint_DrawImageMod(gImage_ion_p_80x24, 0, 0, 24, 80);
	HAL_Delay(500);
	Paint_DrawImageMod(gImage_rf_80x24, 0, 0, 24, 80);
	HAL_Delay(500);
	Paint_DrawImageMod(gImage_warm_80x24, 0, 0, 24, 80);
	HAL_Delay(500);*/
	Paint_DrawImageMod(gImage_level1_18x22, 0, 25, 22, 18);
	/*Paint_DrawImageMod(gImage_level2_18x22, 22, 18, 22, 18);
	HAL_Delay(500);
	Paint_DrawImageMod(gImage_level3_18x22, 22, 18, 22, 18);
	HAL_Delay(500);
	Paint_DrawImageMod(gImage_level4_18x22, 22, 18, 22, 18);
	HAL_Delay(500);
	Paint_DrawImageMod(gImage_level5_18x22, 22, 18, 22, 18);
	HAL_Delay(500);*/
}

#endif
#ifdef ENABLE_A_0429_IMAGE
/*******************************************************************************
 * Function Name  : LCD IMAGE SEQ
 * Description    : Change LCD for Indicator
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LCD_LOGO_Image(void) {
	LCD_Clear(BLACK);
	Paint_DrawImageMod(gImage_img_logo, 54, 4, 40, 72);
	//HAL_Delay(500);
}
void LCD_BATTARY_STATE(void) {
	if (battery_level == 1) {
		Paint_DrawImageMod(gImage_ic_battery_1, 4, 32, 8, 16);
	} else if (battery_level == 2) {
		Paint_DrawImageMod(gImage_ic_battery_2, 4, 32, 8, 16);
	} else if (battery_level == 3) {
		Paint_DrawImageMod(gImage_ic_battery_3, 4, 32, 8, 16);
	}
}

void LCD_TIME_Image(uint8_t time) {
	//printf(">> origin = %d ,remain = %d : %d : %d \r\n", time, time / 60,
	//		(time % 60) / 10, (time % 60) % 10);
	Paint_DrawImageMod(gImage_ic_time_dot, 113, 45, 26, 9); //47
	if (time > 0 && time!= 180) {
		switch (time / 60) {
		case 0:
			Paint_DrawImageMod(gImage_txt_time_2, 113, 54, 26, 19); // �?
			break;
		case 1:
			Paint_DrawImageMod(gImage_txt_time_1, 113, 54, 26, 19); // �?
			break;
		case 2:
			Paint_DrawImageMod(gImage_txt_time_0, 113, 54, 26, 19); // �?
			break;
		case 3:
			Paint_DrawImageMod(gImage_txt_time_3, 113, 54, 26, 19); // �?
			break;
		}
		switch ((time % 60) / 10) {
		case 0:
			Paint_DrawImageMod(gImage_txt_time_5, 113, 26, 26, 19); //?�위_�?
			break;
		case 1:
			Paint_DrawImageMod(gImage_txt_time_4, 113, 26, 26, 19); //?�위_�?
			break;
		case 2:
			Paint_DrawImageMod(gImage_txt_time_3, 113, 26, 26, 19); //?�위_�?
			break;
		case 3:
			Paint_DrawImageMod(gImage_txt_time_2, 113, 26, 26, 19); //?�위_�?
			break;
		case 4:
			Paint_DrawImageMod(gImage_txt_time_1, 113, 26, 26, 19); //?�위_�?
			break;
		case 5:
			Paint_DrawImageMod(gImage_txt_time_0, 113, 26, 26, 19); //?�위_�?
			break;
		}
		switch ((time % 60) % 10) {
		case 0:
			Paint_DrawImageMod(gImage_txt_time_9, 113, 7, 26, 19); //?�위_�?
			break;
		case 1:
			Paint_DrawImageMod(gImage_txt_time_8, 113, 7, 26, 19); //?�위_�?
			break;
		case 2:
			Paint_DrawImageMod(gImage_txt_time_7, 113, 7, 26, 19); //?�위_�?
			break;
		case 3:
			Paint_DrawImageMod(gImage_txt_time_6, 113, 7, 26, 19); //?�위_�?
			break;
		case 4:
			Paint_DrawImageMod(gImage_txt_time_5, 113, 7, 26, 19); //?�위_�?
			break;
		case 5:
			Paint_DrawImageMod(gImage_txt_time_4, 113, 7, 26, 19); //?�위_�?
			break;
		case 6:
			Paint_DrawImageMod(gImage_txt_time_3, 113, 7, 26, 19); //?�위_�?
			break;
		case 7:
			Paint_DrawImageMod(gImage_txt_time_2, 113, 7, 26, 19); //?�위_�?
			break;
		case 8:
			Paint_DrawImageMod(gImage_txt_time_1, 113, 7, 26, 19); //?�위_�?
			break;
		case 9:
			Paint_DrawImageMod(gImage_txt_time_0, 113, 7, 26, 19); //?�위_�?
			break;
		}
	}
	else{
		Paint_DrawImageMod(gImage_txt_time_3, 113, 54, 26, 19); // �?
		Paint_DrawImageMod(gImage_txt_time_0, 113, 26, 26, 19); //?�위_�?
		Paint_DrawImageMod(gImage_txt_time_0, 113, 7, 26, 19); //?�위_�?
	}
	//Paint_DrawImageMod(gImage_txt_time_0, 113, 7, 26, 19); //?�위_�?

//	Paint_DrawImageMod(gImage_txt_time_0, 113, 26, 26, 19); //?�위_�?

//	Paint_DrawImageMod(gImage_txt_time_0, 113, 54, 26, 19); // �?
}

void LCD_SOOTHING_Image(void) {
	LCD_ClearWindowMod(54, 4, 40, 72,BLACK);

	Paint_DrawImageMod(gImage_txt_mode_soothing, 22, 4, 15, 72);
	Paint_DrawImageMod(gImage_ic_mode_soothing, 46, 22, 58, 54);
	Paint_DrawImageMod(gImage_ic_page_base, 152, 14, 4, 52);
	Paint_DrawImageMod(gImage_ic_page_active, 152, 62, 4, 4);
}

void LCD_BLEMISHCARE_Image(void) {
	//LCD_ClearWindowMod(113,45,139,47,BLACK);

	Paint_DrawImageMod(gImage_txt_mode_blemishcare, 22, 4, 28, 72);
	Paint_DrawImageMod(gImage_ic_mode_blemishcare, 50, 22, 54, 54);

	Paint_DrawImageMod(gImage_ic_page_base, 152, 14, 4, 52);
	Paint_DrawImageMod(gImage_ic_page_active, 152, 54, 4, 4);
}

void LCD_MOISTURE_Image(void) {
	//LCD_ClearWindowMod(113,45,139,47,BLACK);
	LCD_ClearWindowMod(37,4,46,72,BLACK);
	Paint_DrawImageMod(gImage_txt_mode_moisture, 22, 4, 15, 72);
	Paint_DrawImageMod(gImage_ic_mode_moisture, 46, 22, 58, 54);

	Paint_DrawImageMod(gImage_ic_page_base, 152, 14, 4, 52);
	Paint_DrawImageMod(gImage_ic_page_active, 152, 46, 4, 4);
	//Level Change
}

void LCD_Lv_Image(void) {
	if (SysMode.OperationIntensity == LEVEL1) {
		Paint_DrawImageMod(gImage_txt_level_1, 64, 4, 23, 18);
	} else if (SysMode.OperationIntensity == LEVEL2) {
		Paint_DrawImageMod(gImage_txt_level_2, 64, 4, 23, 18);
	} else if (SysMode.OperationIntensity == LEVEL3) {
		Paint_DrawImageMod(gImage_txt_level_3, 64, 4, 23, 18);
	} else if (SysMode.OperationIntensity == LEVEL4) {
		Paint_DrawImageMod(gImage_txt_level_4, 64, 4, 23, 18);
	} else if (SysMode.OperationIntensity == LEVEL5) {
		Paint_DrawImageMod(gImage_txt_level_5, 64, 4, 23, 18);
	}
}
void LCD_COLLAGEN_Image(void) {
	//LCD_ClearWindowMod(113,45,139,47,BLACK);

	Paint_DrawImageMod(gImage_txt_mode_collagen, 22, 4, 15, 72);
	Paint_DrawImageMod(gImage_ic_mode_collagen, 46, 22, 58, 54);

	Paint_DrawImageMod(gImage_ic_page_base, 152, 14, 4, 52);
	Paint_DrawImageMod(gImage_ic_page_active, 152, 38, 4, 4);
}
void LCD_TIGHTENING_Image(void) {
	//LCD_ClearWindowMod(113,45,139,47,BLACK);

	Paint_DrawImageMod(gImage_txt_mode_tightening, 22, 4, 15, 72);

	Paint_DrawImageMod(gImage_ic_mode_tightening, 46, 4, 58, 72);

	Paint_DrawImageMod(gImage_ic_page_base, 152, 14, 4, 52);
	Paint_DrawImageMod(gImage_ic_page_active, 152, 30, 4, 4);
}
void LCD_LIFTING_Image(void) {
	//LCD_ClearWindowMod(113,45,139,47,BLACK);

	Paint_DrawImageMod(gImage_txt_mode_lifting, 22, 4, 15, 72);

	Paint_DrawImageMod(gImage_ic_mode_lifting, 46, 22, 58, 54);

	Paint_DrawImageMod(gImage_ic_page_base, 152, 14, 4, 52);
	Paint_DrawImageMod(gImage_ic_page_active, 152, 22, 4, 4);
}

void LCD_LIFTING_Lv_Image(void) {
	if (SysMode.OperationIntensity == LEVEL1) {
		Paint_DrawImageMod(gImage_txt_level_1, 64, 31, 23, 18);
	} else if (SysMode.OperationIntensity == LEVEL2) {
		Paint_DrawImageMod(gImage_txt_level_2, 64, 31, 23, 18);
	} else if (SysMode.OperationIntensity == LEVEL3) {
		Paint_DrawImageMod(gImage_txt_level_3, 64, 31, 23, 18);
	} else if (SysMode.OperationIntensity == LEVEL4) {
		Paint_DrawImageMod(gImage_txt_level_4, 64, 31, 23, 18);
	} else if (SysMode.OperationIntensity == LEVEL5) {
		Paint_DrawImageMod(gImage_txt_level_5, 64, 31, 23, 18);
	}
}

void LCD_EYECARE_Image(void) {
	//LCD_ClearWindowMod(113,45,139,47,BLACK);
	Paint_DrawImageMod(gImage_txt_mode_eyecare, 22, 4, 15, 72);

	Paint_DrawImageMod(gImage_ic_mode_eyecare, 46, 22, 58, 54);
	Paint_DrawImageMod(gImage_ic_page_base, 152, 14, 4, 52);
	Paint_DrawImageMod(gImage_ic_page_active, 152, 14, 4, 4);
}
//void LCD_BLUETOOTH_PAIRING(void){
//	LCD_Clear(BLACK);
////	for(int i=0; i<4; i++){
//		Paint_DrawImageMod(gImage_txt_mode_bluetooth, 32, 8, 13, 64);
//		Paint_DrawImageMod(gImage_ic_mode_bluetooth_2, 55, 8, 50, 64);
//		Paint_DrawImageMod(gImage_txt_pairing, 115, 8, 26, 80);
//		HAL_Delay(700);
//		Paint_DrawImageMod(gImage_ic_mode_bluetooth_1, 55, 8, 50, 64);
//		HAL_Delay(300);
////	}
//}

/*******************************************************************************
 * Function Name  : LCD_BLUETOOTH_PAIRING
 * Description    :
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LCD_BLUETOOTH_PAIRING(void) {
	LCD_Clear(BLACK);
	//Paint_DrawImageMod(gImage_ic_battery_3, 10, 31, 18, 18);
	Paint_DrawImageMod(gImage_txt_mode_bluetooth, 22, 6, 15, 72);

	//Paint_DrawImageMod(gImage_ic_mode_bluetooth_2, 55, 8, 50, 64);
	Paint_DrawImageMod(gImage_txt_pairing, 115, 4, 15, 72);
	//HAL_Delay(700);
	Paint_DrawImageMod(gImage_ic_mode_bluetooth_1, 46, 4, 58, 72);
	//HAL_Delay(300);

	Paint_DrawImageMod(gImage_ic_set_page_base, 152, 34, 4, 12);
	Paint_DrawImageMod(gImage_ic_page_active, 152, 34, 4, 4);
}

/*******************************************************************************
 * Function Name  : LCD_BLUETOOTH_CONNECTED
 * Description    :
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LCD_BLUETOOTH_CONNECTED(void) {
	Paint_DrawImageMod(gImage_txt_connected, 115, 4, 15, 72);
}
/*******************************************************************************
 * Function Name  : LCD_BLUETOOTH_REMOVE
 * Description    :
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LCD_BLUETOOTH_REMOVE(void) {
	LCD_ClearWindowMod(115,4,15,72,BLACK);
}
/*******************************************************************************
 * Function Name  : LCD_VOLUME
 * Description    :
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LCD_VOLUME(void) {
	LCD_Clear(BLACK);

	Paint_DrawImageMod(gImage_txt_mode_vol, 22, 4, 15, 72);

	Paint_DrawImageMod(gImage_ic_mode_vol, 44, 4, 58, 72);
	//HAL_Delay(300);

	Paint_DrawImageMod(gImage_ic_set_page_base, 152, 34, 4, 12);
	Paint_DrawImageMod(gImage_ic_page_active, 152, 34, 4, 4);
}
/*******************************************************************************
 * Function Name  : LCD_VOLUME_MUTE
 * Description    :
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LCD_VOLUME_MUTE(void) {

	Paint_DrawImageMod(gImage_txt_mode_vol, 22, 4, 15, 72);

	Paint_DrawImageMod(gImage_ic_mode_mute, 44, 4, 58, 72);
	//HAL_Delay(300);

	Paint_DrawImageMod(gImage_ic_set_page_base, 152, 34, 4, 12);
	Paint_DrawImageMod(gImage_ic_page_active, 152, 34, 4, 4);
}
/*******************************************************************************
 * Function Name  : LCD_LOCK
 * Description    :
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LCD_LOCK(void) {
	//LCD_Clear(BLACK);
//	LCD_ClearWindowMod(150,4,60,72,BLACK);
//	LCD_ClearWindowMod(150,4,60,72,BLACK);
	Paint_DrawImageMod(gImage_ic_status_lock, 87, 2, 14, 10);
}

/*******************************************************************************
 * Function Name  : LCD_LOCK_POP
 * Description    :
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LCD_LOCK_POP(void) {
	LCD_Clear(BLACK);
//	LCD_ClearWindowMod(150,4,60,72,BLACK);
//	LCD_ClearWindowMod(150,4,60,72,BLACK);
//	LCD_ClearWindowMod(150,4,60,72,BLACK);

	Paint_DrawImageMod(gImage_ic_mode_lock, 46, 4, 58, 72);
	Paint_DrawImageMod(gImage_txt_lock, 115, 4, 15, 72);
}

/*******************************************************************************
 * Function Name  : LCD_UNLOCK_POP
 * Description    :
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LCD_UNLOCK_POP(void) {
	LCD_Clear(BLACK);
	//	LCD_ClearWindowMod(150,4,60,72,BLACK);
	//	LCD_ClearWindowMod(150,4,60,72,BLACK);
	//	LCD_ClearWindowMod(150,4,60,72,BLACK);
	Paint_DrawImageMod(gImage_ic_mode_unlock, 46, 4, 58, 72);
	Paint_DrawImageMod(gImage_txt_unlock, 115, 4, 15, 72);
}

/*******************************************************************************
 * Function Name  : LCD_LOW_BATTERY
 * Description    :
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LCD_LOW_BATTERY(void) {
	LCD_Clear(BLACK);
	Paint_DrawImageMod(gImage_ic_battery_low_1, 64, 6, 33, 69);
	HAL_Delay(700);
	Paint_DrawImageMod(gImage_ic_battery_low_2, 64, 6, 33, 69);
	HAL_Delay(300);
}

/*******************************************************************************
 * Function Name  : LCD_CHARGING
 * Description    :
 * Input          : None
 * Output         : None
 *******************************************************************************/
void LCD_CHARGING(void) {
	//LCD_Clear(BLACK);
	Paint_DrawImageMod(gImage_ic_battery_charging, 64, 6, 33, 69);
}
#endif
