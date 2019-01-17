/*
*	main.cpp
*
*	Created: 2018-12-22 오후 6:02:10
*	Author : Cakeng (PARK JONG SEOK)
*
*	NO LICENCE INCLUDED
*	Contact cakeng@naver.com to
*	use, modify, or share the software for any purpose.
*
*/
#define  F_CPU 20000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#include "Music.h"
#include "AlarmWorks.h"
#include "Buttons.h"
#include "DisplayOut.h"
#include "MCP7940N_RTC.h"

MCP7940n clockObj;
//ClockWorks clockObj(0, 0, 0, 8192);
DisplayOut displayObj(0, 8192);
MusicObj musicObj(8192);
AlarmWorks alarmObj;
Buttons buttonObj(8192);


bool alarmTimeSetMode = false;
bool alarmShowMode = false;
bool hour12Mode = false;
uint16_t alarmShowTicks = 0;

bool toggleDots = false;
uint16_t toggleDotsTicks = 0;

ISR(RTC_PIT_vect)
{
	displayObj.refreshDisplay();
	displayObj.animationTickCounter();
	buttonObj.buttonTickCounter();
	musicObj.autoRoutine();
	
	RTC_PITINTFLAGS = (1<<0); // Clear Interrupt bit.
}



void inputFunction(uint8_t buttonReturn, uint16_t buttonCounts)
{
	if (buttonReturn == BUTTON_UNDER_TICKS)
	{
	}
	// If nothing was pressed - load time from RTC
	else if (buttonReturn == BUTTON_NULL)
	{
		clockObj.loadTime();
	}
	else
	{
		ClockWorks* tempObj;
		// Do different things depending on the mode.
		if (alarmTimeSetMode)
		{
			tempObj = &alarmObj;
		}
		else
		{
			tempObj = &clockObj;
		}
		// If alarm button was unpressed...
		if (buttonReturn == BUTTON_ALARM_UNPRESSED)
		{
			// If the has been triggered, reset the alarm.
			if (alarmObj.isTriggered())
			{
				alarmObj.unTriggerAlarm();
				musicObj.musicOff();
				toggleDots = false;
				displayObj.setDot(false);
			}
			// If the system is in alarm time setting mode, exit the mode & set the alarm.
			else if (alarmTimeSetMode)
			{
				displayObj.setDot(false);
				alarmTimeSetMode = false;
				alarmObj.setAlarm();
			}
			// If the button wasn't pressed long enough & if the alarm hasn't been triggered & if the system is in normal mode...
			else if (buttonCounts <12)
			{
				// If the alarm is set, unset alarm.
				if (alarmObj.isSet())
				{
					alarmObj.unSetAlarm();
				}
				// If not set, set alarm.
				else
				{
					alarmObj.setAlarm();
				}
				// Display alarm time for a brief period.
				alarmShowMode = true;
				displayObj.setDot(true); // Dots On
			}
			// If the button was pressed long enough & if the alarm hasn't been triggered & if the system is in normal mode...
			else
			{
				// Enter alarm time setting mode. (No matter whether the alarm is currently set or not.)
				displayObj.setDot(true); // Dots On
				alarmTimeSetMode = true;
				alarmObj.unSetAlarm();
			}
			// End of alarm button routine.
		}
		// If the button has been pressed only for a short period, only change time when the button is depressed.
		else if (buttonCounts < 5)
		{
			if (buttonReturn == BUTTON_UP_UNPRESSED)
			{
				tempObj->addMin();
			}
			else if(buttonReturn == BUTTON_DOWN_UNPRESSED)
			{
				tempObj->subMin();
			}
		}
		// If the button has been pressed for a  while, keep changing time while the button is pressed.
		else
		{
			if (buttonReturn == BUTTON_UP_PRESSED)
			{
				tempObj->addMin();
			}
			else if(buttonReturn == BUTTON_DOWN_PRESSED)
			{
				tempObj->subMin();
			}
		}
		// Save time to RTC when the button is unpressed & if the clock time was the one being changed.
		if (tempObj == &clockObj)
		{
			if (buttonReturn == BUTTON_UP_UNPRESSED || buttonReturn == BUTTON_DOWN_UNPRESSED)
			{
				clockObj.saveTimeSecZero();
			}
		}
	}
}



int main(void)
{
	//System Clock Setup
	CPU_CCP = 0xD8; // Unprotect IO Reg.
	CLKCTRL_MCLKCTRLA = 0b00000000; // 20Mhz Selected.
	CPU_CCP = 0xD8; // Unprotect IO Reg.
	CLKCTRL_MCLKCTRLB = 0b00000000; // No Clock Prescaler.
	
	//PORT Direction Setup
	PORTA_DIR = 0b11111110; // Anode7 ~ Anode1
	PORTB_DIR = 0b00111100; // Anode8, Grid 4, LED 1, Buzzer, SW3/SDA, SW2/SCL
	PORTC_DIR = 0b00001110; // Grid 3, Grid 2, Grid 1, SW1/CDS
	
	PORTA_OUT = 0;
	PORTB_OUT = 0;
	PORTC_OUT = 0;
	
	//Internal RTC Setup
	RTC_CLKSEL = 0b00000000; // Use internal low power 32.768kHz oscillator.
	RTC_PITINTCTRL = 0b00000001; // PIT Interrupt enabled
	RTC_PITCTRLA = 0b00001001; // PIT Period 4 cycles. PIT Enabled. 8192Hz Interrupts.
	
	
	alarmTimeSetMode = false;
	sei();
	
	bool bootUp = true;
	bool secMode = false;
	hour12Mode = true;
	
	// BOOTUP SEQUENCE
	toggleDotsTicks = 0;
	while (bootUp)
	{
		DisplayOut temp(1, 8192);
		toggleDotsTicks++;
		buttonObj.buttonFunction(temp);
		if (toggleDotsTicks < 120)
		{
			// Display bootup screen #1 (P.J.S.)
			displayObj.setDisplayBootUp(0);
			// Enter 24 hour display mode when any of the buttons were pressed.
			if (buttonObj.getButtonPressed() != BUTTON_NULL && buttonObj.getButtonPressed() != BUTTON_UNDER_TICKS)
			{
				hour12Mode = false;
			}
		}
		else if (toggleDotsTicks < 300)
		{
			// Display bootup screen #2 (IV-6)
			displayObj.setDisplayBootUp(1);
			// Enter 24 hour display mode when any of the buttons were pressed.
			if (buttonObj.getButtonPressed() != BUTTON_NULL && buttonObj.getButtonPressed() != BUTTON_UNDER_TICKS)
			{
				hour12Mode = false;
			}
			// Enter Seconds display mode when Alarm button is pressed until the end of bootup screen #2
			if (buttonObj.getButtonPressed() == BUTTON_ALARM_PRESSED)
			{
				secMode = true;
			}
			else if (buttonObj.getButtonPressed() != BUTTON_UNDER_TICKS)
			{
				secMode = false;
			}
		}
		else if (toggleDotsTicks < 400)
		{
			// Display bootup screen #3 (Blank)
			displayObj.setDisplayBootUp(2);
		}
		else
		{
			bootUp = false;
			clockObj.loadTime();
		}
		
		// SecMode indication LED
		if (secMode)
		{
			PORTB_OUTSET = 0b00001000; // LED1 On
		}
		else
		{
			PORTB_OUTCLR = 0b00001000; // LED1 Off
		}
		
		displayObj.animationRoutin();
		_delay_ms(10);
	}
	
	while (1)
	{
		// Button Controls
		uint8_t oldSec = clockObj.getSec();
		buttonObj.buttonFunction(displayObj);
		inputFunction(buttonObj.getButtonPressed(), buttonObj.getButtonCounter());
		if (toggleDots)
		{
			toggleDotsTicks++;
			if (toggleDotsTicks > 1000)
			{
				displayObj.setDot(false);
				toggleDotsTicks = 0;
			}
			else if (toggleDotsTicks > 500)
			{
				displayObj.setDot(true);
			}
		}
		
		// Alarm & Music Controls
		if(alarmObj.checkAlarm(clockObj))
		{
			musicObj.musicOn();
			toggleDots = true;
		}
		
		musicObj.musicFunction();
		
		// Alarm indication LED toggler
		if (alarmObj.isSet())
		{
			PORTB_OUTSET = 0b00001000; // LED1 On
		}
		else
		{
			PORTB_OUTCLR = 0b00001000; // LED1 Off
		}

		// Seconds Dot toggler
		static uint16_t mills = 0;
		mills++;
		if (oldSec < clockObj.getSec())
		{
			mills = 0;
		}
		if (mills > 500)
		{
			displayObj.setDot(3, true);
		}
		else
		{
			displayObj.setDot(3, false);
		}
		
		// Display Output Selection
		if (alarmTimeSetMode)
		{
			displayObj.setDisplay(alarmObj, hour12Mode);
		}
		// when displaying only for a short period of time.
		else if (alarmShowMode)
		{
			displayObj.setDisplay(alarmObj, hour12Mode);
			alarmShowTicks++;
			if (alarmShowTicks > 750)
			{
				alarmShowTicks = 0;
				alarmShowMode = false;
				displayObj.setDot(false);
			}
		}
		else
		{
			displayObj.setDisplay(clockObj, hour12Mode);
			if (secMode)
			{
				displayObj.setDisplayNoAnimationSecs(clockObj);
			}
		}
		displayObj.animationRoutin();
		_delay_ms(1);
	}
	
}
