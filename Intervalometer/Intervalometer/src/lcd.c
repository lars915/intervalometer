/*
 * CFile1.c
 *
 * Created: 5/30/2015 6:46:57 AM
 *  Author: Larry
 */

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include "lcd.h"

#define LCDREADWRITE PIND2 // LCD read/write
#define LCDDATA PORTC
#define LCDDATADIR DDRC
#define LCDCONTROL PORTD
#define LCDCONTROLDIR DDRD
#define READWRITE PORTD2
#define ENABLE PORTD5 // LCD Enable.
#define REGSEL PORTD6 // LCD Register select. 0 = command

void LCDInit()
{	// Configure LCD
	LCDDATADIR = 0xFF;
	LCDCONTROLDIR |= (1<<ENABLE | 1<<REGSEL | 1<<READWRITE);
	LCDCONTROL &= ~(1<<READWRITE);
	_delay_ms(50);
	LCDSendCommand(0x1);  // Home
	_delay_ms(2);
	LCDSendCommand(0x38); // 8-bit commands
	_delay_us(50);
	LCDSendCommand(0b00001110); // Display ON/OFF
	_delay_us(50);
}

void LCDSendCommand(unsigned char command)
{
	LCDWaitWhileBusy();
	LCDCONTROL &= ~(1<<REGSEL);
	asm volatile ("nop");
	asm volatile ("nop");
	LCDDATA = command;
	_delay_us(50);
	LCDToggleEnable();
	LCDDATA = 0;
	_delay_us(50);
}

void LCDSendChar(unsigned char character)
{
	LCDWaitWhileBusy();
	LCDCONTROL |= 1<<REGSEL;
	asm volatile ("nop");
	asm volatile ("nop");
	LCDDATA = character;
	_delay_us(50);
	LCDToggleEnable();
	LCDDATA = 0;
	_delay_us(50);
}

void LCDToggleEnable()
{
	LCDCONTROL |= 1<<ENABLE;
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	LCDCONTROL &= ~(1<<ENABLE);
}

void LCDSendText(char *text)
{
	while (*text > 0)
	{
		LCDSendChar(*text++);
	}
}

void LCDSetPos(unsigned char row, unsigned char col)
{
	unsigned char rowaddr[] = {0, 64, 20, 84};
	int pos = rowaddr[row] + col;
	LCDSendCommand(0x80 + pos);
	_delay_us(50);
}

void LCDWaitWhileBusy()
{
	LCDDATADIR = 0;
	LCDCONTROL |= 1<<READWRITE;
	LCDCONTROL &= ~(1<<REGSEL);

	while(LCDDATA >= 0x80)
	{
		LCDToggleEnable();
	}
	LCDDATADIR = 0xff;
	LCDCONTROL &= ~(1<<READWRITE);
}
