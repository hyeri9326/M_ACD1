/**
* Multi-function Device Main Program
* 
* Copyright 2021 by Home beauty Device Team, CLASSYS Inc.
* All rights reserved.
*
* I2C_IOExpander.h : This file contains Defines, Extern Variables and etc.
to control I/O Expander IC.
*
* @author     Park, Sungjun
* @version    V0.00
* @date       09.06.2021
*/
#ifndef __IOEXPANDER_I2C_H
#define __IOEXPANDER_I2C_H

/* includes ------------------------------------------------------------------*/
#include "main.h"

/* defines -------------------------------------------------------------------*/
/* structs -------------------------------------------------------------------*/
/* Extern Variables ----------------------------------------------------------*/
/* Extern struncts -----------------------------------------------------------*/
/* functions -----------------------------------------------------------------*/
void Write_IOExpander(uint8_t io_data);
uint8_t Read_IOExpander(void);

#endif  /* __IOEXPANDER_I2C_H */
