/*
*	DisplayOut.h
*
*	Created: 2018-07-10 오전 6:43:41
*	Author: Cakeng (PARK JONG SEOK)
*
*	NO LICENCE INCLUDED
*	Contact cakeng@naver.com to
*	use, modify, or share the software for any purpose
*	other than personal use.
*/

#ifndef DISPLAYOUT_CAKENG_H
#define DISPLAYOUT_CAKENG_H
#include <avr/io.h>
#include "Basic.h"
#include "ClockWorks.h"

class DisplayOut
{
	private:
	volatile uint8_t cathodeNum;
	uint8_t segDataBuffer[4]; //MSB is dot flag
	
	uint8_t screenData[4];
	
	uint16_t animationTicks[4];
	uint16_t animationConst[4];
	uint16_t animationRate[4];
	uint8_t animationCount[4];

	public:
	DisplayOut(uint16_t cons);

	void refreshDisplay();

	void setDisplay(ClockWorks& clockObj);
	void setDisplay(uint8_t h, uint8_t m, uint8_t s);
	void setDisplay(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
	void setDisplayNoAnimation(ClockWorks& clockObj);
	void setDisplayNoAnimation(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
	void setDot(uint8_t digit, bool set);
	
	void setDisplayBootUp(uint8_t num);
	
	void animationTickCounter();
	void animationRoutin();
	void animationFasterSpeed()
	{
		animationRate[2] = animationConst[3];
		animationRate[3] = animationConst[2];
		animationTicks[2] = 0;
		animationTicks[3] = 0;
	}
	void animationFastSpeed()
	{
		animationRate[3] = animationConst[1];
		animationTicks[3] = 0;
	}
	void animationNormalSpeed()
	{
		animationRate[2] = animationConst[0];
		animationRate[3] = animationConst[0];
		animationTicks[2] = 0;
		animationTicks[3] = 0;
	}
};

#endif