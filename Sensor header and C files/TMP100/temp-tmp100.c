/*
IBEX UK LTD http://www.ibexuk.com
Electronic Product Design Specialists
RELEASED SOFTWARE

The MIT License (MIT)

Copyright (c) 2013, IBEX UK Ltd, http://ibexuk.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
//Project Name:		TEXAS INSTRUMENTS TMP100 TEMPERATURE SENSOR DRIVER



#include "main.h"					//Global data type definitions (see https://github.com/ibexuk/C_Generic_Header_File )
#define	TEMP_C
#include "temp-tmp100.h"




//***************************************
//***************************************
//********** SET CONFIGURATION **********
//***************************************
//***************************************
//Returns 1 if configuration was set sucessfully, 0 if not
//config:
//	OS/ALERT | R1 | R0 | F1 | F0 | POL | TM | 1=Shutdown
//	
//	R1:0		Resolution			Convertsion Time (typ)
//	00			9 Bits (0.5°C)		40ms
//	01			10 Bits (0.25°C)	80ms
//	10			11 Bits (0.125°C)	160ms
//	11			12 Bits (0.0625°C)	320ms
BYTE temperature_set_config (BYTE config)
{

	//Send Start
	TMP100_I2C_IDLE_I2C();					//Wait for I2C bus to be ready
	TMP100_I2C_START_I2C();
	while(TMP100_I2C_START_IN_PROGRESS_BIT)
		;

	//Write Slave address and with RW bit 0 for write
	TMP100_I2C_IDLE_I2C();
	TMP100_I2C_WRITE_BYTE(TMP100_I2C_ADDRESS);
	while(TMP100_I2C_TX_IN_PROGRESS_BIT)
		;
	if(TMP100_I2C_ACK_NOT_RECEIVED_BIT ==1)
		goto temperature_set_config_fail;
	
	//Write pointer to Configuration register
	TMP100_I2C_IDLE_I2C();
	TMP100_I2C_WRITE_BYTE(0x01);
	while(TMP100_I2C_TX_IN_PROGRESS_BIT)
		;
	if(TMP100_I2C_ACK_NOT_RECEIVED_BIT == 1)
		goto temperature_set_config_fail;

	//Write Configuration
	TMP100_I2C_IDLE_I2C();
	TMP100_I2C_WRITE_BYTE(config);
	while(TMP100_I2C_TX_IN_PROGRESS_BIT)
		;
	if(TMP100_I2C_ACK_NOT_RECEIVED_BIT == 1)
		goto temperature_set_config_fail;

	//Send Stop
	TMP100_I2C_IDLE_I2C();
	TMP100_I2C_STOP_I2C();
	while(TMP100_I2C_STOP_IN_PROGRESS_BIT)
		;
	
	return(1);


temperature_set_config_fail:
	//Send Stop
	TMP100_I2C_IDLE_I2C();
	TMP100_I2C_STOP_I2C();
	while(TMP100_I2C_STOP_IN_PROGRESS_BIT)
		;

	return(0);
}



//**************************************
//**************************************
//********** READ TEMPERATURE **********
//**************************************
//**************************************
//Returns 1 if temperature was read sucessfully, 0 if not
//temperature:
//	Returned 12 bit temperature reading is x0.0625ºC (16 bits per 1ºC).  0x0000 = 0ºC
BYTE temperature_read (SIGNED_WORD *temperature)
{

	//Send Start
	TMP100_I2C_IDLE_I2C();					//Wait for I2C bus to be ready
	TMP100_I2C_START_I2C();
	while(TMP100_I2C_START_IN_PROGRESS_BIT)
		;

	//Write Slave address and with RW bit 0 for write
	TMP100_I2C_IDLE_I2C();
	TMP100_I2C_WRITE_BYTE(TMP100_I2C_ADDRESS);
	while(TMP100_I2C_TX_IN_PROGRESS_BIT)
		;
	if(TMP100_I2C_ACK_NOT_RECEIVED_BIT ==1)
		goto read_temperature_fail;
	
	//Write pointer to Temperature register
	TMP100_I2C_IDLE_I2C();
	TMP100_I2C_WRITE_BYTE(0x00);
	while(TMP100_I2C_TX_IN_PROGRESS_BIT)
		;
	if(TMP100_I2C_ACK_NOT_RECEIVED_BIT == 1)
		goto read_temperature_fail;

	//Send Restart
	TMP100_I2C_IDLE_I2C();
	TMP100_I2C_RESTART_I2C();
	while(TMP100_I2C_RESTART_IN_PROGRESS_BIT)
		;

	//Write Slave address and with RW bit 1 for read
	TMP100_I2C_IDLE_I2C();
	TMP100_I2C_WRITE_BYTE(TMP100_I2C_ADDRESS | 0x01);
	while(TMP100_I2C_TX_IN_PROGRESS_BIT)
		;
	if(TMP100_I2C_ACK_NOT_RECEIVED_BIT == 1)
		goto read_temperature_fail;

	//Get byte 0
	TMP100_I2C_IDLE_I2C();
	#ifdef TMP100_I2C_READ_BYTE_START
		TMP100_I2C_READ_BYTE_START()
	#endif
	*temperature = ((SIGNED_WORD)TMP100_I2C_READ_BYTE() << 8);


	//Send Ack
	TMP100_I2C_IDLE_I2C();
	TMP100_I2C_ACK();
	while(TMP100_I2C_ACK_IN_PROGRESS_BIT)
		;

	//Get byte 1 (Least significant bits are in bit7:4, rest are unused)
	TMP100_I2C_IDLE_I2C();
	#ifdef TMP100_I2C_READ_BYTE_START
		TMP100_I2C_READ_BYTE_START()
	#endif
	
	*temperature |= (SIGNED_WORD)TMP100_I2C_READ_BYTE();
	
	
	//Send NAK
	TMP100_I2C_IDLE_I2C();
	TMP100_I2C_NOT_ACK();
	while(TMP100_I2C_ACK_IN_PROGRESS_BIT)
		;

	//Send Stop
	TMP100_I2C_IDLE_I2C();
	TMP100_I2C_STOP_I2C();
	while(TMP100_I2C_STOP_IN_PROGRESS_BIT)
		;

	
	//----- CONVERT TEMPERATURE -----
	*temperature >>= 4;
	if (*temperature & 0x0800)			//Sign extend
		*temperature |= 0xf000;

	
	return(1);



read_temperature_fail:
	//Send Stop
	TMP100_I2C_IDLE_I2C();
	TMP100_I2C_STOP_I2C();
	while(TMP100_I2C_STOP_IN_PROGRESS_BIT)
		;

	*temperature = 0;

	return(0);
}













