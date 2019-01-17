/*
*	DisplayOut.cpp
*
*	Created: 2018-07-10 오전 6:43:41
*	Author: Cakeng (PARK JONG SEOK)
*
*	NO LICENCE INCLUDED
*	Contact cakeng@naver.com to
*	use, modify, or share the software for any purpose.
*
*/

#include "DisplayOut.h"


const uint8_t bootUpDisplayData[4][4] =
{
	{
		0b00000000,
		0b11111100,//P.
		0b11010111,//J.
		0b11101011//S.
	},
	{
		0b01010011,//I
		0b00000111,//v
		0b00001000,//-
		0b01101111//6
	},
	{
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000
	}
};


const uint8_t numDataBox[] =
{
	0b01110111,//0
	0b00010010,//1
	0b01011101,//2
	0b01011011,//3
	0b00111010,//4
	0b01101011,//5
	0b01101111,//6
	0b01110010,//7
	0b01111111,//8
	0b01111011//9
};

void DisplayOut::setBrighness(uint8_t cons)
{
	displayConst = brighnessConst[cons]; // Off Time - Higher displayConst results in dimmer display.
}

DisplayOut::DisplayOut(uint8_t cons, uint16_t freq)
{
	brighnessConst[0] = 1;
	brighnessConst[1] = 15;
	brighnessConst[2] = 50;
	setBrighness(cons);
	outNum = 0;
	
	segDataBuffer[0] = 0;
	segDataBuffer[1] = 0;
	segDataBuffer[2] = 0;
	segDataBuffer[3] = 0;
	segDataBuffer[4] = 0;
	segDataBuffer[5] = 0;
	
	screenData[0] = 0;
	screenData[1] = 0;
	screenData[2] = 0;
	screenData[3] = 0;
	screenData[4] = 0;
	screenData[5] = 0;
	
	animationTicks[0] = 0;
	animationTicks[1] = 0;
	animationTicks[2] = 0;
	animationTicks[3] = 0;
	animationTicks[4] = 0;
	animationTicks[5] = 0;
	
	uint16_t buf = freq/100;
	animationRateConst[0] = 40*buf/10; // 40ms - Normal animation speed
	animationRateConst[1] = 16*buf/10; // 16ms - Fast animation speed.
	animationRateConst[2] = 2*buf/10; // 2ms - Fastest animation speed for minutes. (First digit)
	animationRateConst[3] = 12*buf/10; // 12ms - Fastest animation speed for minutes. (Second digit)
	
	animationRate[0] = animationRateConst[0];
	animationRate[1] = animationRateConst[0];
	animationRate[2] = animationRateConst[0];
	animationRate[3] = animationRateConst[0];
	animationRate[4] = animationRateConst[0];
	animationRate[5] = animationRateConst[0];
	
	animationCount[0] = 0;
	animationCount[1] = 0;
	animationCount[2] = 0;
	animationCount[3] = 0;
	animationCount[4] = 0;
	animationCount[5] = 0;
}

// Used to shift out data to 74HC595 Shift register.
void DisplayOut::shiftOut(uint8_t data)
{
	PORTC_OUTCLR = 0b00000100; // Latch pin Low
	for(uint8_t i = 0; i < 8; i++)
	{
		PORTC_OUTCLR = 0b00001010; // Clock/Data pin Low
		if (data&(1<<i))
		{
			PORTC_OUTSET = 0b00001000;
		}
		PORTC_OUTSET = 0b00000010; // Clock pin High
	}
	PORTC_OUTSET = 0b00000100; // Latch pin High
}

void DisplayOut::refreshDisplay()
{
	// If outNum < displayConst[0], the screen is off.
	// If displayConst[0] < outNum < displayConst[1] (displayConst = displayConst[0] + Ontime)
	// Outputs (outNum - displayConsts)th digit.
	outNum++;
	if (outNum < displayConst)
	{
	}
	/*
	//For 6 seg display with 74HC595 shift register.
	else if (outNum < displayConst + 6)
	{
	uint8_t temp = outNum - displayConst;
	shiftOut(0);
	PORTA_OUT = ((~segDataBuffer[temp])<<1);
	PORTB_OUTCLR =  0b00100000;
	PORTB_OUTSET = (segDataBuffer[temp]>>7)<<5;
	shiftOut(1<<(temp+2));
	}*/
	// For 4 seg display.
	else if (outNum < displayConst + 4)
	{
		PORTA_OUTCLR = 0b11111110;
		PORTB_OUTCLR = 0b00110000;
		PORTC_OUTCLR = 0b00001110;
		_delay_us(10);
		uint8_t temp = outNum - displayConst;
		PORTA_OUTSET = (segDataBuffer[temp]<<1);
		PORTB_OUTSET = (segDataBuffer[temp]>>7)<<5;
		PORTC_OUTSET = (1<<(5-(temp+1)))&0b00001110;
		PORTB_OUTSET = (1<<(5-(temp+1)))&0b00010000;
	}
	// Extra display time for the third segment as the said segment is dimmer.
	else if (outNum == displayConst + 4)
	{
		PORTA_OUTCLR = 0b11111110;
		PORTB_OUTCLR = 0b00110000;
		PORTC_OUTCLR = 0b00001110;
		_delay_us(10);
		uint8_t temp = 2;
		PORTA_OUTSET = (segDataBuffer[temp]<<1);
		PORTB_OUTSET = (segDataBuffer[temp]>>7)<<5;
		PORTC_OUTSET = (1<<(5-(temp+1)))&0b00001110;
		PORTB_OUTSET = (1<<(5-(temp+1)))&0b00010000;
	}
	else
	{
		PORTA_OUTCLR = 0b11111110;
		PORTB_OUTCLR = 0b00110000;
		PORTC_OUTCLR = 0b00001110;
		outNum = 0;
	}
}

void DisplayOut::setDisplay(ClockWorks& clockObj, bool hour12Mode)
{
	
	screenData[0] &= 0x80;
	screenData[1] &= 0x80;
	screenData[2] &= 0x80;
	screenData[3] &= 0x80;
	screenData[4] &= 0x80;
	screenData[5] &= 0x80;
	
	if (hour12Mode)
	{
		uint8_t temp = clockObj.getHour();
		if (temp > 11)
		{
			screenData[1] |= 0x80; //PM
		}
		else
		{
			screenData[1] &= 0x7f; //AM
		}
		if (temp > 12)
		{
			temp -= 12;
		}
		else if (temp == 0)
		{
			temp = 12;
		}
		screenData[0] |= numDataBox[temp/10];
		screenData[1] |= numDataBox[temp%10];
	}
	else
	{
		screenData[0] |= numDataBox[clockObj.getHour()/10];
		screenData[1] |= numDataBox[clockObj.getHour()%10];
	}
	screenData[2] |= numDataBox[clockObj.getMin()/10];
	screenData[3] |= numDataBox[clockObj.getMin()%10];
	screenData[4] |= numDataBox[clockObj.getSec()/10];
	screenData[5] |= numDataBox[clockObj.getSec()%10];
	
}

void DisplayOut::setDisplay(ClockWorks& clockObj)
{
	
	screenData[0] &= 0x80;
	screenData[1] &= 0x80;
	screenData[2] &= 0x80;
	screenData[3] &= 0x80;
	screenData[4] &= 0x80;
	screenData[5] &= 0x80;
	
	screenData[0] |= numDataBox[clockObj.getHour()/10];
	screenData[1] |= numDataBox[clockObj.getHour()%10];
	screenData[2] |= numDataBox[clockObj.getMin()/10];
	screenData[3] |= numDataBox[clockObj.getMin()%10];
	screenData[4] |= numDataBox[clockObj.getSec()/10];
	screenData[5] |= numDataBox[clockObj.getSec()%10];
	
}

void DisplayOut::setDisplaySecs(ClockWorks& clockObj)
{
	
	screenData[0] &= 0x80;
	screenData[1] &= 0x80;
	screenData[2] &= 0x80;
	screenData[3] &= 0x80;
	screenData[4] &= 0x80;
	screenData[5] &= 0x80;
	
	screenData[0] |= numDataBox[clockObj.getMin()/10];
	screenData[1] |= numDataBox[clockObj.getMin()%10];
	screenData[2] |= numDataBox[clockObj.getSec()/10];
	screenData[3] |= numDataBox[clockObj.getSec()%10];
	screenData[4] |= 0;
	screenData[5] |= 0;
	
}
void DisplayOut::setDisplay(uint8_t h, uint8_t m, uint8_t s)
{
	screenData[0] &= 0x80;
	screenData[1] &= 0x80;
	screenData[2] &= 0x80;
	screenData[3] &= 0x80;
	screenData[4] &= 0x80;
	screenData[5] &= 0x80;
	
	screenData[0] |= numDataBox[h/10];
	screenData[1] |= numDataBox[h%10];
	screenData[2] |= numDataBox[m/10];
	screenData[3] |= numDataBox[m%10];
	screenData[4] |= numDataBox[s/10];
	screenData[5] |= numDataBox[s%10];
}
void DisplayOut::setDisplay(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f)
{
	screenData[0] = numDataBox[a];
	screenData[1] = numDataBox[b];
	screenData[2] = numDataBox[c];
	screenData[3] = numDataBox[d];
	screenData[4] = numDataBox[e];
	screenData[5] = numDataBox[f];
}
void DisplayOut::setDisplayNoAnimation(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f)
{
	segDataBuffer[0] = numDataBox[a];
	segDataBuffer[1] = numDataBox[b];
	segDataBuffer[2] = numDataBox[c];
	segDataBuffer[3] = numDataBox[d];
	segDataBuffer[4] = numDataBox[e];
	segDataBuffer[5] = numDataBox[f];
}
void DisplayOut::setDisplayNoAnimation(ClockWorks& clockObj)
{
	segDataBuffer[0] = numDataBox[clockObj.getHour()/10];
	segDataBuffer[1] = numDataBox[clockObj.getHour()%10];
	segDataBuffer[2] = numDataBox[clockObj.getMin()/10];
	segDataBuffer[3] = numDataBox[clockObj.getMin()%10];
	segDataBuffer[4] = numDataBox[clockObj.getSec()/10];
	segDataBuffer[5] = numDataBox[clockObj.getSec()%10];
}
void DisplayOut::setDisplayNoAnimationMills(ClockWorks& clockObj)
{
	segDataBuffer[0] = numDataBox[clockObj.getMills()/1000];
	segDataBuffer[1] = numDataBox[(clockObj.getMills()/100)%10];
	segDataBuffer[2] = numDataBox[(clockObj.getMills()/10)%10];
	segDataBuffer[3] = numDataBox[clockObj.getMills()%10];
	segDataBuffer[4] = numDataBox[clockObj.getSec()/10];
	segDataBuffer[5] = numDataBox[clockObj.getSec()%10];
}
void DisplayOut::setDisplayNoAnimationSecs(ClockWorks& clockObj)
{
	segDataBuffer[4] = numDataBox[clockObj.getHour()/10];
	segDataBuffer[5] = numDataBox[clockObj.getHour()%10];
	segDataBuffer[0] = numDataBox[clockObj.getMin()/10];
	segDataBuffer[1] = numDataBox[clockObj.getMin()%10];
	segDataBuffer[2] = numDataBox[clockObj.getSec()/10];
	segDataBuffer[3] = numDataBox[clockObj.getSec()%10];
}
void DisplayOut::setDot(uint8_t digit, bool set)
{
	if (!set)
	{
		screenData[digit] &= 0x7f;
	}
	else
	{
		screenData[digit] |= 0x80;
	}
}
void DisplayOut::setDot( bool set)
{
	if (!set)
	{
		screenData[0] &= 0x7f;
		screenData[1] &= 0x7f;
		screenData[2] &= 0x7f;
		screenData[3] &= 0x7f;
		screenData[4] &= 0x7f;
		screenData[5] &= 0x7f;
	}
	else
	{
		screenData[0] |= 0x80;
		screenData[1] |= 0x80;
		screenData[2] |= 0x80;
		screenData[3] |= 0x80;
		screenData[4] |= 0x80;
		screenData[5] |= 0x80;
	}
}


void DisplayOut::setDisplayBootUp(uint8_t num)
{
	screenData[0] = bootUpDisplayData[num][0];
	screenData[1] = bootUpDisplayData[num][1];
	screenData[2] = bootUpDisplayData[num][2];
	screenData[3] = bootUpDisplayData[num][3];
	screenData[4] = bootUpDisplayData[num][4];
	screenData[5] = bootUpDisplayData[num][5];
}



void DisplayOut::animationTickCounter()
{
	animationTicks[0]++;
	animationTicks[1]++;
	animationTicks[2]++;
	animationTicks[3]++;
	animationTicks[4]++;
	animationTicks[5]++;
}

void DisplayOut::animationRoutin()
{
	for (uint8_t i = 0; i < 6; i++)
	{
		if (screenData[i] & 0x80)
		{
			segDataBuffer[i] |= 0x80;
		}
		else
		{
			segDataBuffer[i] &= 0x7f;
		}
		if (animationTicks[i] > animationRate[i])
		{
			animationTicks[i] = 0;
			if ((segDataBuffer[i]&0x7f) != (screenData[i]&0x7f))
			{
				if (animationCount[i] < 8)
				{
					segDataBuffer[i] &= ~(1<<(animationCount[i]));
				}
				if (animationCount[i] >2)
				{
					segDataBuffer[i] |= (screenData[i]&0x7f)&(1<<(animationCount[i]-3));
				}
				animationCount[i]++;
				if (animationCount[i] == 10)
				{
					animationCount[i] = 0;
				}
			}
			else
			{
				animationCount[i] = 0;
			}
		}
	}
}
