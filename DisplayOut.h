/*
*	DisplayOut.h
*
*	Created: 2018-07-10 오전 6:43:41
*	Author: Cakeng (PARK JONG SEOK)
*
*	NO LICENCE INCLUDED
*	Contact cakeng@naver.com to
*	use, modify, or share the software for any purpose.
*
*/
#define F_CPU 20000000

#ifndef DISPLAYOUT_CAKENG_H
#define DISPLAYOUT_CAKENG_H
#include <avr/io.h>
#include <util/delay.h>
#include "ClockWorks.h"
#include "MCP7940N_RTC.h"

class DisplayOut
{
	private:
	uint8_t brighnessConst[3];
	uint8_t displayConst;
	volatile uint8_t outNum;
	
	uint8_t segDataBuffer[6];
	uint8_t screenData[6];

	uint16_t animationTicks[6];
	uint16_t animationRateConst[4];
	uint16_t animationRate[6];
	uint8_t animationCount[6];
	
	public:
	DisplayOut(uint8_t cons, uint16_t freq);
	void shiftOut(uint8_t data);
	void refreshDisplay();
	void setBrighness(uint8_t cons);
	
	void setDisplay(ClockWorks& clockObj, bool hour12Mode);
	void setDisplay(ClockWorks& clockObj);
	void setDisplaySecs(ClockWorks& clockObj);
	void setDisplay(uint8_t h, uint8_t m, uint8_t s);
	void setDisplay(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f);
	void setDisplayBootUp(uint8_t num);
	void setDisplayNoAnimation(ClockWorks& clockObj);
	void setDisplayNoAnimationMills(ClockWorks& clockObj);
	void setDisplayNoAnimationSecs(ClockWorks& clockObj);
	void setDisplayNoAnimation(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f);
	void setDot(uint8_t digit, bool set);
	void setDot(bool set);
	
	void animationTickCounter();
	void animationRoutin();
	void animationFasterSpeed()
	{
		animationRate[2] = animationRateConst[3];
		animationRate[3] = animationRateConst[2];
		animationTicks[2] = 0;
		animationTicks[3] = 0;
	}
	void animationFastSpeed()
	{
		animationRate[3] = animationRateConst[1];
		animationTicks[3] = 0;
	}
	void animationNormalSpeed()
	{
		animationRate[2] = animationRateConst[0];
		animationRate[3] = animationRateConst[0];
		animationTicks[2] = 0;
		animationTicks[3] = 0;
	}
};

#endif