/*
*	MCP7940N_RTC.h
*
*	Created: 2018-07-13 오후 8:40:18
*	Author: Cakeng (PARK JONG SEOK)
*
*	NO LICENCE INCLUDED
*	Contact cakeng@naver.com to
*	use, modify, or share the software for any purpose.
*
*/

#ifndef MCP7940N_RTC_CAKENG_H
#define MCP7940N_RTC_CAKENG_H

#include <util/delay_basic.h>
#include <avr/interrupt.h>
#include "Basic.h"
#include "ClockWorks.h"


#define ADR_WRITE (0b11011110)
#define ADR_READ  (0b11011111)

#define RTCSEC    (0x00)
#define RTCMIN    (0x01)
#define RTCHOUR   (0x02)
#define RTCWKDAY  (0x03)
#define RTCDATE   (0x04)
#define RTCMTH    (0x05)
#define RTCYEAR   (0x06)


class MCP7940n : public ClockWorks
{
	private:
	volatile bool i2cFinished;	
	
	void i2cWrite(uint8_t address, uint8_t* data, uint8_t num);
	void i2cRead(uint8_t address, uint8_t* data, uint8_t num);

	uint8_t dec2bcd(int8_t dec)
	{
		return (uint8_t)((dec%10)&0x0f)|((dec/10)<<4);
	}
	uint8_t bcd2dec(uint8_t bcd)
	{
		return (uint8_t)((bcd&(0x70))>>4)*10+(bcd&(0x0f));
	}
	
	public:
	void i2cInterruptHandler();
	
	void loadTime();
	void saveTime();
	void saveTimeSecZero();
	
	void enableOscillator()
	{
		uint8_t temp[] = {RTCSEC,(uint8_t)(secs|0x80)};
		i2cWrite(ADR_WRITE, temp, 2);
	}

	void enableBattery()
	{
		uint8_t temp[] = {RTCWKDAY, 0x08};
		i2cWrite(ADR_WRITE, temp, 2);
	}
	MCP7940n();

};

#endif