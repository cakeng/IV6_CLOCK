/*
*	DisplayOut.cpp
*
*	Created: 2018-07-10 오전 6:43:41
*	Author: Cakeng (PARK JONG SEOK)
*
*	NO LICENCE INCLUDED
*	Contact cakeng@naver.com to
*	use, modify, or share the software for any purpose
*	other than personal use.
*
*/
#define F_CPU 16000000
#include "Basic.h"
#include "DisplayOut.h"
#include "util/delay.h"

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

DisplayOut::DisplayOut(uint16_t freq)
{
	cathodeNum = 4;
	
	segDataBuffer[0] = 0;
	segDataBuffer[1] = 0;
	segDataBuffer[2] = 0;
	segDataBuffer[3] = 0;
	
	screenData[0] = 0;
	screenData[1] = 0;
	screenData[2] = 0;
	screenData[3] = 0;
	
	animationTicks[0] = 0;
	animationTicks[1] = 0;
	animationTicks[2] = 0;
	animationTicks[3] = 0;
	
	uint16_t buf = freq/100;
	animationConst[0] = 30*buf/10;
	animationConst[1] = 10*buf/10;
	animationConst[2] = 1*buf/10;
	animationConst[3] = 5*buf/10;
	
	animationRate[0] = animationConst[0];
	animationRate[1] = animationConst[0];
	animationRate[2] = animationConst[0];
	animationRate[3] = animationConst[0];
	
	animationCount[0] = 0;
	animationCount[1] = 0;
	animationCount[2] = 0;
	animationCount[3] = 0;
}

void DisplayOut::refreshDisplay()
{
	cathodeNum++;
	if (cathodeNum == 8)
	{
		cathodeNum = 0;
	}
	PORTD &= 0b10000000;
	PORTB &= 0b00000111;
	if (cathodeNum%2 == 0)
	{
		PORTD |= (0b01111111&segDataBuffer[cathodeNum/2]);
		PORTB |= (((segDataBuffer[cathodeNum/2]&0x80)>>4)|(1<<(cathodeNum/2+4)));
	}
	
	//PORTD |= 0b01000000;
	
}

void DisplayOut::setDisplay(ClockWorks& clockObj, bool hour12Mode)
{
	
	screenData[0] &= 0x80;
	screenData[1] &= 0x80;
	screenData[2] &= 0x80;
	screenData[3] &= 0x80;
	
	if (hour12Mode)
	{
		uint8_t buf = clockObj.getHour();
		if (buf > 11)
		{
			screenData[1] |= 0x80; //PM
		}
		else
		{
			screenData[1] &= 0x7f; //AM
		}
		if (buf > 12)
		{
			buf -= 12;
		}
		else if (buf == 0)
		{
			buf = 12;
		}
		screenData[0] |= numDataBox[buf/10];
		screenData[1] |= numDataBox[buf%10];
	}
	else
	{
		screenData[0] |= numDataBox[clockObj.getHour()/10];
		screenData[1] |= numDataBox[clockObj.getHour()%10];
	}
	screenData[2] |= numDataBox[clockObj.getMin()/10];
	screenData[3] |= numDataBox[clockObj.getMin()%10];

	
	/*screenData[0] |= numDataBox[clockObj.getMin()/10];
	screenData[1] |= numDataBox[clockObj.getMin()%10];
	screenData[2] |= numDataBox[clockObj.getSec()/10];
	screenData[3] |= numDataBox[clockObj.getSec()%10];*/
	
}
void DisplayOut::setDisplay(uint8_t h, uint8_t m, uint8_t s)
{
	screenData[0] &= 0x80;
	screenData[1] &= 0x80;
	screenData[2] &= 0x80;
	screenData[3] &= 0x80;
	
	screenData[0] |= numDataBox[h/10];
	screenData[1] |= numDataBox[h%10];
	screenData[2] |= numDataBox[m/10];
	screenData[3] |= numDataBox[m%10];
}
void DisplayOut::setDisplay(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
	screenData[0] = numDataBox[a];
	screenData[1] = numDataBox[b];
	screenData[2] = numDataBox[c];
	screenData[3] = numDataBox[d];
}
void DisplayOut::setDisplayNoAnimation(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
	
	
	segDataBuffer[0] = numDataBox[a];
	segDataBuffer[1] = numDataBox[b];
	segDataBuffer[2] = numDataBox[c];
	segDataBuffer[3] = numDataBox[d];
}
void DisplayOut::setDisplayNoAnimation(ClockWorks& clockObj)
{
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

void DisplayOut::setDisplayBootUp(uint8_t num)
{
	screenData[0] = bootUpDisplayData[num][0];
	screenData[1] = bootUpDisplayData[num][1];
	screenData[2] = bootUpDisplayData[num][2];
	screenData[3] = bootUpDisplayData[num][3];
}


void DisplayOut::animationTickCounter()
{
	animationTicks[0]++;
	animationTicks[1]++;
	animationTicks[2]++;
	animationTicks[3]++;
}

void DisplayOut::animationRoutin()
{
	for (uint8_t i = 0; i < 4; i++)
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
