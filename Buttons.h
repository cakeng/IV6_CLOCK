/*
*	Buttons.h
*
*	Created: 2018-06-26 오전 8:26:47
*	Author: Cakeng (PARK JONG SEOK)
*
*	NO LICENCE INCLUDED
*	Contact cakeng@naver.com to
*	use, modify, or share the software for any purpose.
*
*/

#include <avr/io.h>
#include "DisplayOut.h"
#ifndef _BUTTONS_CAKENG_H
#define _BUTTONS_CAKENG_H


#define BUTTON_UP_PRESSED		1
#define BUTTON_DOWN_PRESSED	    2
#define BUTTON_ALARM_PRESSED	3
#define BUTTON_UP_UNPRESSED	    4
#define BUTTON_DOWN_UNPRESSED	5
#define BUTTON_ALARM_UNPRESSED	6
#define BUTTON_UNDER_TICKS		7
#define BUTTON_NULL 0

class Buttons
{
	private:
	volatile uint16_t buttonTicks;
	uint16_t buttonCheckRateConsts[2];
	uint16_t buttonCheckRate;
	uint16_t buttonCounter;
	uint8_t buttonPressedData;
	uint8_t buttonLastData;
	uint8_t buttonPressedOut;
	uint8_t buttonCountOut;
	
	void checkInputs();
	
	public:
	Buttons(uint16_t tickFreq);
	
	void buttonFunction(DisplayOut& displayObj);
	
	void buttonTickCounter()
	{
		buttonTicks++;
	}
	uint8_t getButtonPressed()
	{
		return buttonPressedOut;
	}
	uint8_t getButtonCounter()
	{
		return buttonCountOut;
	}
	
};

#endif
