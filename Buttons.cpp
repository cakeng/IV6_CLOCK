/*
*	Buttons.cpp
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
#include "Buttons.h"


void Buttons::checkInputs()
{
	buttonPressed = NULL;
	if(!BIT_CHECK(BUTTON_INPUT_UP_PORT_GROUP, BUTTON_INPUT_UP))
	{
		buttonPressed = BUTTON_UP_PRESSED;
	}
}

Buttons::Buttons(uint16_t tickFreq, bool pinOn) //Ticks to reach 1 second.
{
	buttonTicks = 0;
	buttonCheckRateConsts[0] = (tickFreq/500*20); //40ms
	buttonCheckRateConsts[1] = (tickFreq/500*80); //160ms
	buttonCheckRateConsts[2] = (tickFreq/500*30); //60ms
	buttonCheckRate = buttonCheckRateConsts[2];
	buttonCounter = 0;
	buttonPressed = NULL;
	buttonLast = NULL;
}

Buttons::Buttons(uint16_t tickFreq) //Ticks to reach 1 second.
{
	buttonTicks = 0;
	buttonCheckRateConsts[0] = (tickFreq/500*7); //14ms
	buttonCheckRateConsts[1] = (tickFreq/500*80); //160ms
	buttonCheckRateConsts[2] = (tickFreq/500*30); //60ms
	buttonCheckRate = buttonCheckRateConsts[2];
	buttonCounter = 0;
	buttonPressed = NULL;
	buttonLast = NULL;
	BIT_OFF(BUTTON_INPUT_UP_DATA_GROUP, BUTTON_INPUT_UP);
}

void Buttons::buttonFunction(DisplayOut& displayObj)
{
	buttonPressedOut = BUTTON_UNDER_TICKS;
	buttonCountNum = buttonCounter;
	if(buttonTicks < buttonCheckRate)
	{
		return;
	}
	buttonPressedOut = NULL;
	buttonTicks = 0;
	checkInputs();
	if(buttonPressed == NULL) // If no buttons were pressed
	{
		if(buttonLast != NULL) // If something was pressed before
		{
			buttonPressedOut = buttonLast;
			buttonLast = NULL;
			buttonCounter = 0;
			buttonCheckRate = buttonCheckRateConsts[2];
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
	else if(buttonCounter > 4)
	{
		buttonCheckRate = buttonCheckRateConsts[1];
	}
	else if (buttonCounter == 1)
	{
		displayObj.animationFastSpeed();
	}
	buttonPressedOut = buttonPressed;
	buttonLast = buttonPressed+3;
}
