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

#define enterFlg = 0x1;
#define cancelFlg = 0x2;
#define upFlg = 0x4;
#define dnFlg = 0x8;

int enterCnt = 0;
int cancelCnt = 0;
int upCnt = 0;
int dnCnt = 0;
int buttonFlags = 0;


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
	LCDSendText("Intervalometer");
	LCDSetPos(1,0);
	LCDSendText("Copyright LAS 2015");
	LCDSetPos(2,0);
	LCDSendText("Enter to continue");

	while(1)
	{
		
		if (bit_is_clear(PINB, 0))
		{
			LCDSetPos(3,0);
			LCDSendText("Button OFF");
		}
		else
		{
			LCDSetPos(3,0);
			LCDSendText("Button ON ");
		}
		_delay_ms(100);

	}
}

ISR(TIMER1_COMPA_vect)
{
	PORTD ^= (1<<PIND7);
}
