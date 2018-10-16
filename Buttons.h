/*
*	Buttons.h
*
*	Created: 2018-06-26 오전 8:26:47
*	Author: Cakeng (PARK JONG SEOK)
*
*	NO LICENCE INCLUDED
*	Contact cakeng@naver.com to
*	use, modify, or share the software for any purpose
*	other than personal use.
*
*/
#ifndef _BUTTONS_CAKENG_H
#define _BUTTONS_CAKENG_H
#include <avr/io.h>
#include "Basic.h"
#include "DisplayOut.h"

#define BUTTON_UP_PRESSED			1
#define BUTTON_DOWN_PRESSED	    2
#define BUTTON_ALARM_PRESSED		3
#define BUTTON_UP_UNPRESSED	    4
#define BUTTON_DOWN_UNPRESSED	5
#define BUTTON_ALARM_UNPRESSED	6
#define BUTTON_UNDER_TICKS		7
#define NULL 0

#ifndef	BUTTON_INPUT_UP   

#define BUTTON_INPUT_UP                PINB0
#define BUTTON_INPUT_UP_PORT_GROUP  PINB
#define BUTTON_INPUT_UP_DATA_GROUP  DDRB
#endif



class Buttons
{
	private:
	volatile uint16_t buttonTicks;
	uint16_t buttonCheckRateConsts[3];
	uint16_t buttonCheckRate;
	uint16_t buttonCounter;
	uint8_t buttonPressed;
	uint8_t buttonLast;
	uint8_t buttonPressedOut;
	uint8_t buttonCountNum;
	
	void checkInputs();
	
	public:
	Buttons(uint16_t tickFreq);
	Buttons(uint16_t tickFreq, bool pinOn);
	
	void buttonTickCounter()
	{
		buttonTicks++;
	}
	
	void buttonFunction(DisplayOut& displayObj);
	
	uint8_t getButtonPressed()
	{
		return buttonPressedOut;
	}
	uint8_t getButtonCounter()
	{
		return buttonCountNum;
	}
	uint8_t getButtonCheckRate()
	{
		if (buttonCheckRate == buttonCheckRateConsts[0])
		{
			return 0;
		}
		else if (buttonCheckRate == buttonCheckRateConsts[1])
		{
			return 1;
		}
		else if (buttonCheckRate == buttonCheckRateConsts[2])
		{
			return 2;
		}
		return 0;
	}
};

#endif
