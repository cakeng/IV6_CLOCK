/*
*	Buttons.cpp
*
*	Created: 2018-06-26 오전 8:26:47
*	Author: Cakeng (PARK JONG SEOK)
*
*	NO LICENCE INCLUDED
*	Contact cakeng@naver.com to
*	use, modify, or share the software for any purpose.
*
*/
#include "Buttons.h"


void Buttons::checkInputs()
{
	buttonPressedData = BUTTON_NULL;
	PORTC_DIRCLR = 0b00000001;
	PORTB_DIRCLR = 0b00000011;
	if((PORTC_IN&0b00000001))
	{
		buttonPressedData = BUTTON_UP_PRESSED;
	}
	else if(!(PORTB_IN&0b00000001))
	{
		buttonPressedData = BUTTON_DOWN_PRESSED;
	}
	else if(!(PORTB_IN&0b00000010))
	{
		buttonPressedData = BUTTON_ALARM_PRESSED;
	}
}

Buttons::Buttons(uint16_t tickFreq) //Ticks to reach 1 second.
{
	buttonTicks = 0;
	uint16_t temp = tickFreq/1000; // 1ms
	buttonCheckRateConsts[0] = (temp*24); //24ms
	buttonCheckRateConsts[1] = (temp*220); //220ms
	buttonCheckRate = buttonCheckRateConsts[0];
	buttonCounter = 0;
	buttonPressedData = BUTTON_NULL;
	buttonLastData = BUTTON_NULL;
}


void Buttons::buttonFunction(DisplayOut& displayObj)
{
	buttonPressedOut = BUTTON_UNDER_TICKS;
	buttonCountOut = buttonCounter;
	if(buttonTicks < buttonCheckRate)
	{
		return;
	}
	buttonPressedOut = BUTTON_NULL;
	buttonTicks = 0;
	checkInputs();
	if(buttonPressedData == BUTTON_NULL) // If no buttons were pressed
	{
		if(buttonLastData != BUTTON_NULL) // If something was pressed before
		{
			
			buttonPressedOut = buttonLastData;
			buttonLastData = BUTTON_NULL;
			buttonCounter = 0;
			buttonCheckRate = buttonCheckRateConsts[0];
			displayObj.animationNormalSpeed();
		}
		return;
	}
	buttonCounter++;
	if(buttonCounter > 24)
	{
		buttonCheckRate = buttonCheckRateConsts[0];
		displayObj.animationFasterSpeed();
	}
	else if(buttonCounter > 5)
	{
		buttonCheckRate = buttonCheckRateConsts[1];
	}
	else if (buttonCounter > 2)
	{
		displayObj.animationFastSpeed();
	}
	buttonPressedOut = buttonPressedData;
	buttonLastData = buttonPressedData+3;
}

