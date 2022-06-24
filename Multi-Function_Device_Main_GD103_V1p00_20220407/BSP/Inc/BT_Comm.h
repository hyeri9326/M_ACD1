/**
* Multi-function Device Main Program
*
* Copyright 2021 by Home beauty Device Team, CLASSYS Inc.
* All rights reserved.
*
* CapTouch_I2C.h : This file contains Defines, Extern Variables and etc.
to control Capacitive Touch Driver IC.
*
* @author     Park, Sungjun
* @version    V0.00
* @date       12.14.2021
*/
#ifndef __BT_COMM_H
#define __BT_COMM_H

/* includes ------------------------------------------------------------------*/
#include "main.h"

/* defines -------------------------------------------------------------------*/
#define REQ_SERIALNUMBER	0x00
#define RESP_SERIALNUMBER	0x01
#define REQ_SYSSTATUS		0x02
#define RESP_SYSSTATUS		0x03
#define REQ_RESET			0x04
#define RESP_RESET			0x05
#define REQ_FACTORYRESET	0x06
#define RESP_FACTORYRESET	0x07
#define REQ_DEBUGMODE		0x08
#define RESP_DEBUGMODE		0x09

#define SEND_SYSEVENT		0x10
#define RESP_SYSEVENT		0x11

#define REQ_WRITE_SET		0x12
#define RESP_WRITE_SET		0x13

#define DBG_REQSTATUS		0x20
#define RESP_REQSTATUS		0x21
#define DBG_SETCONRTROL		0x22
#define RESP_SETCONRTROL	0x23

#define REQ_PACK_SIZE			2	//4
#define WRI_SYS_SET_SIZE		3	//5
#define SYS_EVT_SIZE			6	//8

#define START				0xFA
#define REQ_SER_NUM			12

/* structs -------------------------------------------------------------------*/
typedef struct {
	union {
		struct {
			uint8_t b0		:1;
			uint8_t b1		:1;
			uint8_t b2		:1;
			uint8_t b3		:1;
			uint8_t b4		:1;
			uint8_t	b5		:1;
			uint8_t b6		:1;
			uint8_t b7		:1;
		};
		uint8_t data;
	};
}System_datatypedef;

typedef struct {
	uint8_t HEADER;
	uint8_t LENGTH;
	uint8_t COMMAND;
	uint8_t DATA[12];
	uint8_t CHECKSUM;
} __attribute__((packed)) BT_Packet; // Response P/N

typedef struct {
	uint8_t HEADER;
	uint8_t LENGTH;
	uint8_t COMMAND;
	uint8_t CHECKSUM;
} __attribute__((packed)) BT_ResPacket; //Etc Response

typedef struct {
	uint8_t HEADER;
	uint8_t LENGTH;
	uint8_t DATA[4];
	uint8_t COMMAND;
	uint8_t CHECKSUM;
} __attribute__((packed)) BT_Evt_Packet;

typedef struct {
	uint8_t STX;
	uint8_t LEN;
	uint8_t CMD;
	uint8_t D0;
	uint8_t D1;
	uint8_t D2;
	uint8_t D3;
	uint8_t D4;
	uint8_t D5;
	uint8_t D6;
	uint8_t D7;
	uint8_t CS;
}__attribute__((packet))BT_REV_Packet;


/* Extern Variables ----------------------------------------------------------*/
/* Extern struncts -----------------------------------------------------------*/
/* functions -----------------------------------------------------------------*/
void BT_Proc(void);
unsigned char CheckSumVerify (unsigned char *pacBuffer, unsigned short wPayloadSize);
void Chk_Cmd(uint8_t cmd);
void send_transmit(BT_Packet *send, uint8_t data_size);
unsigned char CheckSumModVerify (BT_Packet *pacBuffer, uint8_t wPayloadSize);
void read_systemstatus(void);
void Dec2Bin(int num);

#endif  /* __BT_COMM_H */
