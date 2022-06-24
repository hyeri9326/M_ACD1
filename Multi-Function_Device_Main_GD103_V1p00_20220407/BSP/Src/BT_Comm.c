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
#include <stdio.h>
#include <string.h>
#include "i2c.h"
#include "gpio.h"
#include "usart.h"
#include "BT_Comm.h"
#include "SystemProcess.h"
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint8_t Cnt_Rx1Uart;
extern uint8_t BtCnt_Rx1Uart;
extern uint8_t BtRx1Packet[16];
extern uint8_t e_time;
extern uint8_t speaker_en;

uint8_t language = 0;


const char *serial_number = "CHD210100001";
System_datatypedef	data0;
System_datatypedef 	data1;
System_datatypedef 	data2;
System_datatypedef 	data3;

uint8_t set_data0;
uint8_t set_data1;
uint8_t set_data2;
uint8_t set_data3;

typedef struct {
    uint8_t cmd;
    void (*handler)();
} CMD_HANDLER;

void send_packet(uint8_t cmd, uint8_t *data, uint8_t data_len) {
    BT_Packet packet;
    packet.HEADER = START;
    packet.LENGTH = data_len+2;
    packet.COMMAND = cmd;
    memcpy(packet.DATA, data, data_len);
    packet.CHECKSUM = CheckSumModVerify(&packet, packet.LENGTH);
    send_transmit(&packet, data_len);
}

void res_serial_number(){
	printf("res_serial_number\r\n"); 
	send_packet(RESP_SERIALNUMBER, serial_number, REQ_SER_NUM);
}
void res_sys_status(){
	printf("res_sys_status\r\n");
	read_systemstatus();
	uint8_t data[2] = {data0.data, data1.data};
	printf("data: %d\r\n", data[0]);
	printf("data: %d\r\n", data[1]);
	send_packet(RESP_SYSSTATUS, data, 2);
}
void res_reset(){
	printf("res_reset\r\n");
	LED_OFF;
	INDICATOR_OFF;

	OUT_SEL1_LOW;
	OUT_SEL2_LOW;

	HeadLEDControl(HEAD_NONE, 0);
	DiffusionLEDControl(POWER_OFF);

	SysMode.KeyLockStatus = KEY_UNLOCKED;
	SysMode.OperationIntensity = LEVEL1;
	SysMode.OperationMode= SOOTHING_MODE;
	uint8_t data = 0xFF;
	send_packet(RESP_RESET, data, 1);
}
void res_factoryreset(){
	printf("res_factoryreset\r\n");
	uint8_t data = 0xFF;
	send_packet(RESP_FACTORYRESET, data, 1);
}
void res_debugmode(){
	printf("res_debugmode\r\n"); 
	uint8_t data = 0xFF;
	send_packet(RESP_DEBUGMODE, data, 1);
}
void res_sysevent(){
	printf("res_sysevent\r\n");
	read_systemstatus();
	SysMode.OperationIntensity = set_data0 & 0x07; //Level 변경
	LCD_Lv_Image();

	printf("SysMode.OperationIntensity  : %d\r\n", SysMode.OperationIntensity);
	SysMode.OperationMode = ((set_data0 & 0x1C)>>3)+2; //Mode 변경
	printf("SysMode.OperationMode : %d\r\n", ((set_data0 & 0x1C)>>3)+2);
	//SysMode.OperationMode = (set_data0 & 0x40); //Mode 변경
	printf("SysMode.Flag_OpStatus : %d\r\n", (set_data0 & 0x40)>>6);
	SysMode.KeyLockStatus = (set_data0 & 0x40)>>6;
	printf("SysMode.KeyLockStatus : %d\r\n", (set_data0 & 0x80)>>7);
	SysMode.KeyLockStatus = (set_data0 & 0x80)>>7;
	Display_BackgroundImg();

	printf("set_data0 : %d\r\n",set_data0);
	printf("set_data1 : %d\r\n",set_data1);
	printf("set_data2 : %d\r\n",set_data2);
	printf("set_data3 : %d\r\n",set_data3);
	uint8_t data = 0xFF;
	send_packet(RESP_SYSEVENT, data, 1);
}
void res_write_set(){
	printf("res_write_set\r\n");
	speaker_en = (set_data0 & 0x80)>>7;
	printf("set_data0 : %x \r\n", set_data0);

	if(speaker_en){
		VoiceOutput(V_DONE);
	}
	language = (set_data0 & 0x60)>>5;
	printf("speaker_en : %d \r\n", speaker_en);
	printf("language : %d \r\n", language);

	uint8_t data = 0xFF;
	send_packet(RESP_WRITE_SET, data, 1);
}

static CMD_HANDLER _cmd_handler_list[]={
	/*Communication for data verification*/
	{REQ_SERIALNUMBER, res_serial_number}, //Request Device serial number
	{REQ_SYSSTATUS, res_sys_status}, // Request Device status
	{REQ_RESET, res_reset}, // Request System reboot
	{REQ_FACTORYRESET, res_factoryreset}, // Request System factory reset
	{REQ_DEBUGMODE, res_debugmode}, // Request into the Debugging mode

	/*Communication for Event*/
	{SEND_SYSEVENT, res_sysevent}, // Request System event
	{REQ_WRITE_SET, res_write_set} // Request System event

	/*Communication for Debug*/
	//{DBG_REQSTATUS, res_reqstatus}, // Request System event
	//{DBG_SETCONRTROL, res_write_set} // Request System event
};

/* functions -----------------------------------------------------------------*/

void BT_Proc(void)
{
   // 패킷 시작 플래그 찾기
   int start_idx = -1;

   for (int i = 0; i < BtCnt_Rx1Uart; i++) {
      if (BtRx1Packet[i] == START) {
         start_idx = i;
         break;
      }
   }
   
   if (start_idx < 0) {
      // 패킷 비우기
      BtCnt_Rx1Uart = 0;
   }
   else if (start_idx > 0) {
      // 패킷 시작위치로 쉬프트
      BtCnt_Rx1Uart -= start_idx;
      memmove(BtRx1Packet, BtRx1Packet + start_idx, BtCnt_Rx1Uart);
   }
   
   // 패킷이 수신 중이면 종료
   if (BtCnt_Rx1Uart < 2)
      return;

   uint8_t length = BtRx1Packet[1];
   uint8_t pack_size = length+2;
   // 패킷이 수신 중이면 종료   
   if (BtCnt_Rx1Uart < pack_size)
      return;

   // 패킷 수신 완료

   // checksum 확인
   uint8_t chksum = BtRx1Packet[length+1];
   if (chksum != CheckSumModVerify(BtRx1Packet, length-2)) {
      printf("checksum error \r\n");
      // 패킷 1바이트 쉬프트
      BtCnt_Rx1Uart--;
      memmove(BtRx1Packet, BtRx1Packet+1, BtCnt_Rx1Uart);
      return;
   }

   // 패킷에 대해서 응답
   uint8_t res_ok = 0;
   uint8_t cmd = BtRx1Packet[2];
   switch (length) {
   case REQ_PACK_SIZE:
		Chk_Cmd(cmd);
		res_ok = 1;
		break;
   case WRI_SYS_SET_SIZE:
		set_data0 = BtRx1Packet[3];
		Chk_Cmd(cmd);
		res_ok = 1;
		break;
   case SYS_EVT_SIZE:
		set_data0 = BtRx1Packet[3];
		set_data1 = BtRx1Packet[4];
		set_data2 = BtRx1Packet[5];
		set_data3 = BtRx1Packet[6];
		Chk_Cmd(cmd);
		res_ok = 1;
		break;
   default:
      printf("Undefined packet size. \r\n");
      break;
   }

   if (res_ok) {
      if (pack_size < BtCnt_Rx1Uart) {
         //패킷 크기만큼 쉬프트
         BtCnt_Rx1Uart -= pack_size;
         memmove(BtRx1Packet, BtRx1Packet+pack_size, BtCnt_Rx1Uart);         
      }
      else {
         // 패킷 버퍼 비우기
         BtCnt_Rx1Uart = 0;
      }
   }
   else {
      // 패킷 1바이트 쉬프트
      BtCnt_Rx1Uart--;
      memmove(BtRx1Packet, BtRx1Packet+1, BtCnt_Rx1Uart);
      //return;
   }
}

void Chk_Cmd(uint8_t cmd){
	//BT_Packet packet;
	uint8_t data_size;
	cmd = BtRx1Packet[2];

	int success = 0;
	for (int i = 0; i<sizeof(_cmd_handler_list); i++){
		CMD_HANDLER handler = _cmd_handler_list[i];
		if(handler.cmd == cmd){
			handler.handler();
			success = 1;
			break;
		}
	}
	if(!success){
		printf("Handler Error \r\n");
	}
}

void read_systemstatus(void) {
	switch(SysMode.OperationIntensity){
		case LEVEL1:
			data0.b0 = 1;
			data0.b1 = 0;
			data0.b2 = 0;
			break;
		case LEVEL2:
			data0.b0 = 0;
			data0.b1 = 1;
			data0.b2 = 0;
			break;
		case LEVEL3:
			data0.b0 = 1;
			data0.b1 = 1;
			data0.b2 = 0;
			break;
		case LEVEL4:
			data0.b0 = 0;
			data0.b1 = 0;
			data0.b2 = 1;
			break;
		case LEVEL5:
			data0.b0 = 1;
			data0.b1 = 0;
			data0.b2 = 1;
			break;
		default:
			data0.b0 = 0;
			data0.b1 = 0;
			data0.b2 = 0;
			break;
	}
	switch(SysMode.OperationMode){
		case STANDBY_MODE:
			data0.b6 = 0;
			break;
		case SOOTHING_MODE:
			data0.b3 = 0;
			data0.b4 = 0;
			data0.b5 = 0;
			data0.b6 = 1;
			break;
		case BLEMISH_CARE:
			data0.b3 = 1;
			data0.b4 = 0;
			data0.b5 = 0;
			data0.b6 = 1;
			break;
		case MOISTURE_MODE:
			data0.b3 = 0;
			data0.b4 = 1;
			data0.b5 = 0;
			data0.b6 = 1;
			break;
		case COLLAGEN_MODE:
			data0.b3 = 1;
			data0.b4 = 1;
			data0.b5 = 0;
			data0.b6 = 1;
			break;
		case TIGHTENING_MODE:
			data0.b3 = 0;
			data0.b4 = 0;
			data0.b5 = 1;
			data0.b6 = 1;
			break;
		case LIFTING_MODE:
			data0.b3 = 1;
			data0.b4 = 0;
			data0.b5 = 1;
			data0.b6 = 1;
			break;
		case EYECARE_MODE:
			data0.b3 = 0;
			data0.b4 = 1;
			data0.b5 = 1;
			data0.b6 = 1;
			break;
	}
	switch(SysMode.KeyLockStatus){
		case KEY_UNLOCKED:
			data0.b7 = 0;
			break;
		case KEY_LOCKED:
			data0.b7 = 1;
			break;
	}

	//Dec2Bin(180); //remain time
	Dec2Bin(180-e_time); //remain time
}

/*******************************************************************************
* Function Name  : CheckSumVerify
* Description    : Verify a checksum field
* Input          : pacBuffer : pointer to the desired host protocol
*                  wPayloadSize : Payload Size (not included checksum)
* Output         : None
* Return         : TRUE if Checksum verified successfully, FALSE for others
* Note           : Check Sum of all fields excluding Check Sum itself will be
*                  stored as Check Sum. In case of response message (Ack/Nack),
*                  Check Sum field will be ommitted.
*                  Calculation method of Check Sum is :
*                  (1) All Bytes of all data will be added.
*                  (2) Take least significant one Byte alone out
*                  (3) Calculate twos complement of the value taken out
*                  (4) Carry-over will be neglected.
* Attension      : None
*******************************************************************************/

unsigned char CheckSumVerify (unsigned char *pacBuffer, unsigned short wPayloadSize)
{
 unsigned char cChksum = 0;
    unsigned short wI;

    for(wI = 0; wI < wPayloadSize-1; wI++)
    {
        cChksum += pacBuffer[wI];
    }

    cChksum = (~cChksum)+0x01;

    return cChksum;
}

unsigned char CheckSumModVerify (BT_Packet *pacBuffer, uint8_t wPayloadSize)
{

	unsigned char cChksum = 0;
    unsigned short wI;

    cChksum = pacBuffer->HEADER + pacBuffer->LENGTH + pacBuffer->COMMAND;
    for(wI = 0; wI < wPayloadSize; wI++)
    {
        cChksum += pacBuffer->DATA[wI];
    }

    cChksum = (~cChksum)+0x01;
	printf("cChksum :%x\r\n", cChksum);

    return cChksum;
}

void Dec2Bin(int num) {
	uint8_t result[7];
    for (int i = 7; i >= 0; --i) { //8자리 숫자까지 나타냄
        result[i] = num >> i & 1;
        //printf("result[%d] : %d\r\n",i, result[i]);
        //printf("data %d : %d", i, data1[i]);
    }
    data1.b0 = result[0];
    data1.b1 = result[1];
    data1.b2 = result[2];
    data1.b3 = result[3];
    data1.b4 = result[4];
    data1.b5 = result[5];
    data1.b6 = result[6];
    data1.b7 = result[7];
}

void send_transmit(BT_Packet *send, uint8_t data_size){
	HAL_UART_Transmit(&huart1, &send->HEADER, 1, 0xFFFF);
	HAL_UART_Transmit(&huart1, &send->LENGTH, 1, 0xFFFF);
	HAL_UART_Transmit(&huart1, &send->COMMAND, 1, 0xFFFF);
	HAL_UART_Transmit(&huart1, &send->DATA, data_size, 0xFFFF);
	HAL_UART_Transmit(&huart1, &send->CHECKSUM, 1, 0xFFFF);

}
