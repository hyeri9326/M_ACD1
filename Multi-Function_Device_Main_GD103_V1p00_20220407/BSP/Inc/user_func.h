#ifndef __USER_FUNC_H
#define __USER_FUNC_H

#ifdef __cplusplus

extern "C"{
#endif

#include "usart.h"
#include "tim.h"

// LED CLK, PWM
#define LED_ARR	TIM1->ARR //1.5khz
#define LED_CCR1	TIM1->CCR1
#define LED_CCR3	TIM1->CCR3
#define LED_CCR4	TIM1->CCR4

#define DIFF_LED_ARR	TIM2->ARR //1.5khz
#define DIFFU_LED_CCR1	TIM2->CCR1
#define DIFFU_LED_CCR2	TIM2->CCR2
#define DIFFU_LED_CCR3	TIM2->CCR3

#define LED_MAX 100.0f	//unit=%
#define LED_LIMIT 100.0f	//unit=%
#define GET_INTENSITY_CCR(intensity)	 ((uint32_t)(LED_ARR+1) * intensity / LED_MAX)
#define GET_DIFFU_INTENSITY_CCR(intensity)	 ((uint32_t)(DIFF_LED_ARR+1) * intensity / LED_MAX)

#define NTC_TABLE_SIZE   	41
extern const float ERTJ1VG103F[NTC_TABLE_SIZE];
/* ERTJ1VG103F */
#define	C3a	(14.50478745F/10000000.0F)
#define	C2a	(-0.761659699F/10000.0F)
#define	C1a	(2.88965412F/1000.0F)

float ntc_GetTempADC(const float *table, uint16_t adc);  // ADC direct 보간

void RF_PulseEnable(void);
void led_intensity_process(uint8_t led_b, uint8_t led_r, uint8_t led_ir);
void LED_Enable(void);
void LED_Disable(void);

void LCD_LOGO_Image(void);

void LCD_SOOTHING_Image(void);
void LCD_BLEMISHCARE_Image(void);
void LCD_MOISTURE_Image(void);
void LCD_MOISTURE_Lv_Image(void);
void LCD_COLLAGEN_Image(void);
void LCD_TIGHTENING_Image(void);
void LCD_LIFTING_Image(void);
void LCD_LIFTING_Lv_Image(void);
void LCD_EYECARE_Image(void);

void diffusion_led_process(uint8_t led_r, uint8_t led_g, uint8_t led_b);

void LCD_BLUETOOTH_PAIRING(void);
void LCD_BLUETOOTH_CONNECTED(void);
void LCD_BLUETOOTH_REMOVE(void);
void LCD_LOCK(void);
void LCD_LOCK_POP(void);
void LCD_UNLOCK_POP(void);
void LCD_LOW_BATTERY(void);
void LCD_INIT_TIME_Image(void);
void LCD_TIME_Image(uint8_t time);
#ifdef __cplusplus
}
#endif

#endif /*__USER_FUNC_H*/
