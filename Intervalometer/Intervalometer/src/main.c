/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"

#define ENTER PINB0
#define CANCEL PINB1
#define UP PINB2
#define DN PINB3
#define BUTTONS PORTB
#define BUTTON_DIR DDRB


volatile int enterCnt = 0;
volatile int cancelCnt = 0;
volatile int upCnt = 0;
volatile int dnCnt = 0;
volatile int enterFlag = 0;
volatile int cancelFlag = 0;
volatile int upFlag = 0;
volatile int dnFlag = 0;

volatile int LEDFlag = 0;
volatile int LEDCnt = 0;


int main(void)
{
	MCUCSR = (1<<JTD); // Disable JTAG
	MCUCSR = (1<<JTD); // Disable JTAG (yes, 2 are required)

	// Set up buttons
	BUTTON_DIR &= ~(1<<ENTER | 1<<CANCEL | 1<<UP | 1<<DN);
	PORTB |= 0xF; // Configure ports 0-3 as inputs with pull-ups

	// Timer interrupt, 1mS
	TCCR1B |= (1<<CS11) | (1<<CS10) | (1<<WGM12);
	OCR1A = 250; // 1000Hz (1mS) with div by 64
	TIMSK |= 1<<OCIE1A;
	sei();

	DDRD |= 1<<PIND7;
	LCDInit();
	//LCDSendText("Intervalometer");
	//LCDSetPos(1,0);
	//LCDSendText("Copyright LAS 2015");
	//LCDSetPos(2,0);
	//LCDSendText("Enter to continue");

	while(1)
	{
		if (LEDFlag)
		{
			PORTD ^= 1<<PORTD7;
			LEDFlag = 0;
		}

		// Enter button
		if (enterFlag)
		{
			LCDSetPos(0,0);
			LCDSendText("Enter TRUE     ");
		}
		else
		{
			LCDSetPos(0,0);
			LCDSendText("Enter FALSE");
		}

		// Cancel button
		if (cancelFlag)
		{
			LCDSetPos(1,0);
			LCDSendText("Cancel TRUE     ");
		}
		else
		{
			LCDSetPos(1,0);
			LCDSendText("Cancel FALSE");
		}

		// UP flag
		if (upFlag)
		{
			LCDSetPos(2,0);
			LCDSendText("UP TRUE     ");
		}
		else
		{
			LCDSetPos(2,0);
			LCDSendText("UP FALSE");
		}

		// DOWN flag handler
		if (dnFlag)
		{
			LCDSetPos(3,0);
			LCDSendText("DOWN TRUE     ");
		}
		else
		{
			LCDSetPos(3,0);
			LCDSendText("DOWN FALSE");
		}

	}
}

ISR(TIMER1_COMPA_vect)
{
	LEDCnt++;
	if (LEDCnt >= 500)
	{
		LEDFlag = 1;
		LEDCnt = 0;
	}

	// Enter button flag handling
	if (bit_is_clear(PINB, 0))
	{
		enterCnt++;
		if (enterCnt >= 20)
		{
			enterFlag = 1;
		}
	}
	else
	{
		enterFlag = 0;
		if (enterCnt > 0)
		{
			enterCnt--;
		}
	}

	// Cancel button flag handling
	if (bit_is_clear(PINB, 1))
	{
		cancelCnt++;
		if (cancelCnt >= 20)
		{
			cancelFlag = 1;
		}
	}
	else
	{
		cancelFlag = 0;
		if (cancelCnt > 0)
		{
			cancelCnt--;
		}
	}

	// Up button flag handling
	if (bit_is_clear(PINB, 2))
	{
		upCnt++;
		if (upCnt >= 20)
		{
			upFlag = 1;
		}
	}
	else
	{
		upFlag = 0;
		if (upCnt > 0)
		{
			upCnt--;
		}
	}

	// Down button flag handling
	if (bit_is_clear(PINB, 3))
	{
		dnCnt++;
		if (dnCnt >= 20)
		{
			dnFlag = 1;
		}
	}
	else
	{
		dnFlag = 0;
		if (dnCnt > 0)
		{
			dnCnt--;
		}
	}

}
