/**
* Multi-function Device Main Program
*
* Copyright 2021 by Home beauty Device Team, CLASSYS Inc.
* All rights reserved.
*
* KeyScanTask.c : This file contains functions to control Key switch
*
* @author     Park, Sungjun
* @version    V0.00
* @date       09.06.2021
*/
/* includes ------------------------------------------------------------------*/
#include "KeyScanTask.h"
#include "SystemProcess.h"
#include "tim.h"
#include "Voice_I2C.h"
#include "CapTouch_I2C.h"

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
int KeyScanState;
int KeyDebounceDlyCtr;
int KeyRptStartDlyCtr;
int KeyNRead;
int KeyBufInIx;
int KeyBufOutIx;
unsigned char KeyCodeBuf;
unsigned char KeyBuf[KEY_BUF_SIZE];

unsigned char Flag_KeyPressed = 0;


extern uint8_t Flag_LevelChg;
extern uint8_t Flag_BLEConn;
extern uint8_t Flag_CSInt;
extern uint8_t Flag_KeyLock;
extern uint8_t Flag_KeyUnlock;

/* functions -----------------------------------------------------------------*/
/*******************************************************************************
* Function Name  : KeyVariableInit
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void KeyVariableInit(void)
{
    KeyScanState = KEY_STATE_UP;
    KeyNRead     = 0;
    KeyBufInIx   = KeyBufOutIx  = 0;
}


/*******************************************************************************
* Function Name  : KeyDecode
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
unsigned char KeyDecode(void)
{
    volatile unsigned short key_temp;
    unsigned char keycode = 0x00;

    // Read Key Status(400usec)
    key_temp = Read_CapTouch();
    //printf("Key = %d\r\n", key_temp);

    if(key_temp == KEY1_PRESSED)		keycode =  KEY1;
    else if(key_temp == KEY2_PRESSED)	keycode =  KEY2;
    else if(key_temp == KEY3_PRESSED)	keycode =  KEY3;

    return keycode;
}


/*******************************************************************************
* Function Name  : KeyBufIn
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void KeyBufIn(unsigned char keycode)
{
	if ( KeyNRead < KEY_BUF_SIZE )
	{
		KeyNRead++;
		KeyBuf[KeyBufInIx++] = keycode;
		if ( KeyBufInIx >= KEY_BUF_SIZE ) 	KeyBufInIx = 0;
	}
	else
	{
		KeyNRead = 0;
	}
}


/*******************************************************************************
* Function Name  : KeyIsKeyDown
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
unsigned char KeyIsKeyDown(void)
{
	uint8_t temp_KeyStatus;

	temp_KeyStatus = Read_CapTouch();
	//printf("Key = %d\r\n", temp_KeyStatus);

    if(temp_KeyStatus == 0x00)
    {
        return FALSE;
    }
    else
    {
        //printf("Key is Down\r\n");
        return TRUE;
    }
}


/*******************************************************************************
* Function Name  : KeyHit
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
unsigned char KeyHit(void)
{
	return((KeyNRead > 0) ? TRUE : FALSE);
}


/*******************************************************************************
* Function Name  : KeyGetKey
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
unsigned char KeyGetKey(void)
{
    unsigned char code;

    if (KeyNRead > 0)
    {
        KeyNRead--;
        code = KeyBuf[KeyBufOutIx];
        KeyBufOutIx++;
        if ( KeyBufOutIx >= KEY_BUF_SIZE ) { KeyBufOutIx = 0; }
        return code;
    }
    else
    {
        return 0xff;
    }
}


/*******************************************************************************
* Function Name  : Key_Function
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void Key_Function(void)
{
    if(KeyHit())
    {
    	switch(KeyGetKey())
    	{
    	case KEY1:
    		printf(">> KEY1 Pressed\r\n");

    		if(SysMode.KeyLockStatus == KEY_UNLOCKED)
    		{
    			if((SysMode.PowerStatus == POWERON_MODE) && (SysMode.Flag_OpStatus == DISABLE_OUTPUT))
    			{
    				Flag_KeyPressed = 1;

    				if((SysMode.OperationMode == MOISTURE_MODE) || (SysMode.OperationMode == LIFTING_MODE))
    				{
    					Flag_LevelChg = 1;

    					SysMode.OperationIntensity++;
    					if(SysMode.OperationIntensity > LEVEL5)
    					{
    						SysMode.OperationIntensity = LEVEL1;
    					}

    					SysMode.Cnt_AutoStart = 0;
    				}
    				else if((SysMode.OperationMode == SOOTHING_MODE) || (SysMode.OperationMode == BLEMISH_CARE))
    				{
    					Flag_LevelChg = 1;

    					SysMode.OperationIntensity++;
    					if(SysMode.OperationIntensity > LEVEL3)
    					{
    						SysMode.OperationIntensity = LEVEL1;
    					}
    				}
    			}
    		}
    		break;
    	case KEY2:
    		printf(">> KEY2 Pressed\r\n");

    		if(SysMode.KeyLockStatus == KEY_UNLOCKED)
    		{
    			if(SysMode.PowerStatus == POWERON_MODE)
    			{
    				Flag_KeyPressed = 1;

    				if(SysMode.OperationMode != STANDBY_MODE)
    				{
    					SysMode.OperationMode++;
    					if(SysMode.OperationMode > EYECARE_MODE)
    					{
    						SysMode.OperationMode = SOOTHING_MODE;
    					}

    					SysMode.Cnt_AutoStart = 0;
    				}
    			}
    		}
    		break;
    	case KEY3:
    		printf(">> KEY3 Pressed\r\n");

    		if(SysMode.KeyLockStatus == KEY_UNLOCKED)
    		{
    			if((SysMode.PowerStatus == POWERON_MODE) && (SysMode.Flag_OpStatus == DISABLE_OUTPUT))
    			{
    				Flag_KeyPressed = 1;
    			}
    		}
    		break;
    	case RPTKEY1:
    		if(SysMode.PowerStatus == POWERON_MODE)
    		{
    			printf(">> RPTKEY1 Pressed\r\n");
    			Flag_KeyPressed = 1;

    			if(SysMode.KeyLockStatus == KEY_UNLOCKED)		Flag_KeyLock = 1;
    			else if(SysMode.KeyLockStatus == KEY_LOCKED)	Flag_KeyUnlock = 1;
    		}
    		break;
    	case RPTKEY2:
    		printf(">> RPTKEY2 Pressed\r\n");
    		if(SysMode.KeyLockStatus == KEY_UNLOCKED)
    		{
    			if((SysMode.OperationMode == STANDBY_MODE) || (SysMode.OperationMode == SLEEP_MODE))
    			{
    				printf(">> Power On......\r\n");
    				Flag_KeyPressed = 1;

    				SysMode.PowerStatus = POWERON_MODE;
    				SysMode.OperationMode = SOOTHING_MODE;
    				//SysMode.OperationMode = TEST_MODE;
    			}
    			else if((SysMode.OperationMode >= SOOTHING_MODE) && (SysMode.OperationMode <= EYECARE_MODE))
    			{
    				printf(">> Power Off......\r\n");
    				Flag_KeyPressed = 1;

    				SysMode.OperationMode = SLEEP_MODE;

    				HeadLEDControl(HEAD_NONE, 0);
    				DiffusionLEDControl(POWER_OFF);
    				VoiceOutput(VOICE_ALARM2);

    				LCD_RESET_DISABLE;
    				htim2.Instance -> CCR4 = 0;

    				while(1)
    				{
    					if(Flag_CSInt)
    						break;
    				}
    			}
    		}
    		break;
    	case RPTKEY3:
    		if(SysMode.KeyLockStatus == KEY_UNLOCKED)
    		{
    			if((SysMode.PowerStatus == POWERON_MODE) && (SysMode.Flag_OpStatus == DISABLE_OUTPUT))
    			{
    				printf(">> RPTKEY3 Pressed\r\n");

    				Flag_BLEConn = 1;
    			}
    		}
    		break;
    	}
    	SysMode.Cnt_AutoSleep = 0;
    }
}


/*******************************************************************************
* Function Name  : KeyScanTask
* Description    :
* Input          : None
* Output         : None
*******************************************************************************/
void KeyScanTask(void)
{
    switch(KeyScanState)
    {
    case KEY_STATE_UP:
        if(KeyIsKeyDown())
        {
            KeyScanState = KEY_STATE_DEBOUNCE;
            KeyDebounceDlyCtr = KEY_DEBOUNCE_DLY;
        }
        break;
    case KEY_STATE_DEBOUNCE:
        if(KeyDebounceDlyCtr > 0)
        {
            KeyDebounceDlyCtr--;
            if(KeyDebounceDlyCtr == 0)	KeyScanState = (KeyIsKeyDown())? KEY_STATE_GET_KEY : KEY_STATE_UP;
        }
        break;
    case KEY_STATE_GET_KEY:
        if(KeyIsKeyDown())
        {
            KeyCodeBuf = KeyDecode();                           // KeyCodeBuf Store, Not press action
            KeyScanState = KEY_STATE_WAIT;
            KeyRptStartDlyCtr = KEY_RPT_DLY;
        }
        else	KeyScanState = KEY_STATE_WAIT_DEBOUNCE;			// Not Press
        break;
    case KEY_STATE_WAIT:
        if(KeyIsKeyDown())										// During Press, NO Action
        {
            if(KeyRptStartDlyCtr > 0)							// RPT Key Delay Counter
            {
                KeyRptStartDlyCtr--;
                if(KeyRptStartDlyCtr == 0)
                {
                    KeyCodeBuf = KeyDecode() + RPT_OFFSET;
                    KeyBufIn(KeyCodeBuf);
                    KeyScanState = KEY_STATE_WAIT_DEBOUNCE;
                    KeyDebounceDlyCtr = KEY_DEBOUNCE_DLY;
                }
            }
        }
        else
        {
            KeyBufIn(KeyCodeBuf);
            KeyScanState = KEY_STATE_WAIT_DEBOUNCE;
            KeyDebounceDlyCtr = KEY_DEBOUNCE_DLY;
        }
        break;
    case KEY_STATE_WAIT_DEBOUNCE:
        if(KeyDebounceDlyCtr > 0)
        {
            KeyDebounceDlyCtr--;
            if(KeyDebounceDlyCtr == 0)
            {
                if(KeyIsKeyDown())
                {
                    //KeyScanState = KEY_STATE_WAIT;
                    KeyScanState = KEY_STATE_WAIT_DEBOUNCE;
                    KeyDebounceDlyCtr = KEY_DEBOUNCE_DLY;
                }
                else
                {
                    KeyScanState = KEY_STATE_UP;
                    if(KeyCodeBuf > RPT_OFFSET)
                    {
                        KeyBufIn(KeyCodeBuf+OFF_OFFSET);
                        //KeyBufIn(OFFKEY);
                    }
                }
            }
        }
        else
        {
            if(!KeyIsKeyDown())
            {
                KeyScanState = KEY_STATE_UP;
            }
        }
        break;
    }
}
