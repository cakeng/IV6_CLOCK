/*
*	IV6_Board.cpp
*
*	Created: 2018-10-06 오후 11:19:46
*	Author: Cakeng (PARK JONG SEOK)
*
*	NO LICENCE INCLUDED
*	Contact cakeng@naver.com to
*	use, modify, or share the software for any purpose
*	other than personal use.
*
*/

#define  F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>

#define PWM_PIN PB2
#define REF_PIN PB1

#include "Buttons.h"
#include "DisplayOut.h"
#include "ClockWorks.h"

Buttons buttonObj(5000);
DisplayOut displayObj(5000);
ClockWorks clockObj(0, 0, 0, 5000);

ISR(TIMER1_COMPA_vect)
{
	if (ACSR&(1<<ACO))
	{
		TCCR0A = 0b01000011;
	}
	else
	{
		TCCR0A = 0b00000011;
		PORTB &= ~(1<<PORTB2);
	}
	displayObj.refreshDisplay();
	displayObj.animationTickCounter();
	clockObj.autoRoutine();
	buttonObj.buttonTickCounter();
	wdt_reset();
}

inline void inputFunction(uint8_t buttonReturn, uint16_t buttonCounts)
{
	if (buttonReturn == BUTTON_UNDER_TICKS)
	{
	}
	else if (buttonReturn == NULL)
	{
		
	}
	else
	{
		if (buttonCounts < 6)
		{
			if (buttonReturn == BUTTON_UP_UNPRESSED)
			{
				clockObj.addMin();
			}
		}
		else
		{
			if (buttonReturn == BUTTON_UP_PRESSED)
			{
				clockObj.addMin();
			}
		}
	}
}

int main(void)
{
	// Timer Setups
	TCCR1A = 0b00000000;
	TCCR1B = 0b00001011;
	TIMSK = 0b01000000;
	OCR1AL = 49; //5000Hz, 1/64 preScaler
	
	TCCR0A = 0b01000011;
	TCCR0B = 0b00001010;
	OCR0A = 255;
	
	sei();
	wdt_enable(WDTO_30MS);

	// Analog Comparator setup
	ACSR = 0b01010000;
	DIDR = 0b00000000;
	
	DDRB = 0b11111100;
	DDRD = 0b11111111;
	
	bool hour12Mode = false;
	bool bootUp = true;
	while (bootUp)
	{
		if (clockObj.getSec()/3 == 0)
		{
			displayObj.setDisplayBootUp(0);
			if ((PINB & (1<<PINB0)) == 0)
			{
				hour12Mode = true;
			}
		}
		else if (clockObj.getSec() == 3 || clockObj.getSec() == 4)
		{
			displayObj.setDisplayBootUp(1);
			if ((PINB & (1<<PINB0)) == 0)
			{
				hour12Mode = true;
			}
		}
		else if (clockObj.getSec() == 5)
		{
			displayObj.setDisplayBootUp(2);
		}
		else
		{
			bootUp = false;
			clockObj.setTime(0,0,0);
		}
		displayObj.animationRoutin();
	}
	
	
	while (1)
	{
		
		/*if ((clockObj.getHour()&0x01))
		{
		displayObj.setDot(3,true);
		}
		else
		{
		displayObj.setDot(3,false);
		}
		if ((clockObj.getHour()&0x02))
		{
		displayObj.setDot(2,true);
		}
		else
		{
		displayObj.setDot(2,false);
		}
		if ((clockObj.getHour()&0x04))
		{
		displayObj.setDot(1,true);
		}
		else
		{
		displayObj.setDot(1,false);
		}
		if ((clockObj.getHour()&0x08))
		{
		displayObj.setDot(0,true);
		}
		else
		{
		displayObj.setDot(0,false);
		}*/
		if (clockObj.getTimeTicksMs() > 500)
		{
			displayObj.setDot(3,true);
		}
		else
		{
			displayObj.setDot(3,false);
		}
		buttonObj.buttonFunction(displayObj);
		inputFunction(buttonObj.getButtonPressed(), buttonObj.getButtonCounter());
		displayObj.setDisplay(clockObj, hour12Mode);
		
		displayObj.animationRoutin();
	}
}

