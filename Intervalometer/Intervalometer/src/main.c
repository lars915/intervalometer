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
#include "support.h"
#include "lcd.h"
#include "menu.h"
#include "serial.h"


#define BAUD 38400
#define UBRR (F_CPU/(16*BAUD))-1

#define ENTER PINB0
#define CANCEL PINB1
#define UP PINB2
#define DN PINB3
#define BUTTONS PORTB
#define BUTTON_DIR DDRB

extern int buttons[];
volatile int btnCnt = 0;
volatile int cancelCnt = 0;
volatile int upCnt = 0;
volatile int dnCnt = 0;

volatile int LEDFlag = 0;
volatile int LEDCnt = 0;
volatile unsigned char c = 0;
volatile int rxflag = 0;

volatile int prevbutton = 0;

typedef enum {
	OFF,
	ON,
} flagstates;


typedef enum{
	RS_RUN,
	RS_DONTRUN,
} runstates;

typedef enum{
	NOTEXPIRED,
	EXPIRED,
} triggerstates;

// Flags to monitor trigger state timers
triggerstates trigger = NOTEXPIRED;
triggerstates halfclick = NOTEXPIRED;
triggerstates click = NOTEXPIRED;

volatile runstates runstate = RS_DONTRUN; // If DONTRUN, camera will not be triggered

volatile int triggerinit = 200;		// Delay counter for triggering
volatile int halfclickinit = 10;	// On time for half click
volatile int clickinit = 10;		// On time for full click
volatile int triggercount = 0;		// Delay counter for triggering
volatile int halfclickcount = 0;	// On time for half click
volatile int clickcount = 0;		// On time for full click
volatile char prevclick = 0;
volatile char prevhalfclick = 0;

volatile flagstates enterflag = OFF;
volatile flagstates cancelflag = OFF;
volatile flagstates upflag = OFF;
volatile flagstates downflag = OFF;

volatile btnpressedenum btnpressed = BTN_NONE;


int main(void)
{
	MCUCSR = (1<<JTD); // Disable JTAG
	MCUCSR = (1<<JTD); // Disable JTAG (yes, 2 are required)

	// Configure serial
	UBRRH = (uint8_t)(UBRR>>8);
	UBRRL = (uint8_t)(UBRR);
	UCSRB |= (1<<TXEN) | (1<<RXEN) | (1<<RXCIE);
	UCSRC |= (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0);

	// Set up buttons
	DDRB &= ~(1<<ENTER | 1<<CANCEL | 1<<UP | 1<<DN);
	PORTB |= 0xF; // Configure ports 0-3 as inputs with pull-ups

	// Timer interrupt, 1mS
	TCCR1B |= (1<<CS12) | (1<<WGM12);
	OCR1A = 625; // 100Hz (10mS) with div by 256
	TIMSK |= 1<<OCIE1A;
	sei();

	DDRD |= 1<<PIND7;
	LCDInit();
	//LCDSendText("Intervalometer");
	//LCDSetPos(1,0);
	//LCDSendText("Copyright LAS 2015");
	//LCDSetPos(2,0);
	//LCDSendText("Enter to continue");
	//int count = 0;
	//char buffer[] = "";

	triggercount = triggerinit;		// Delay counter for triggering
	halfclickcount = halfclickinit;	// On time for half click
	clickcount = clickinit;		// On time for full click

	while(1)
	{
		_delay_ms(500);
		if (rxflag)
		{
			LCDSetPos(0,15);
			LCDSendChar(c);
		}
		if (LEDFlag)
		{
			LEDFlag = 0;
		}

		switch(btnpressed){
			case(BTN_ENTER):
				serialSendText("Enter\n\r");
				break;
			case(BTN_CANCEL):
				serialSendText("Cancel\n\r");
				break;
			case(BTN_UP):
				serialSendText("Up\n\r");
				break;
			case(BTN_DOWN):
				serialSendText("Down\n\r");
				break;
			default:
				serialSendText("None\n\r");
		}
		handleMenu(btnpressed);
		btnpressed = BTN_NONE;

	}
}

ISR(TIMER1_COMPA_vect)
{
	LEDCnt++;
	if (LEDCnt >= 10){
		LEDFlag = 1;
		LEDCnt = 0;
			PORTD ^= 1<<PORTD7;
	}

	if (runstate == RS_RUN){
		triggercount--;
		if (triggercount <= 0){
			// fire halfclick
			halfclickcount--;
			if (prevhalfclick == 0){
				serialSendText("Halfclick\n\r");
				prevhalfclick = 1;
			}
		}
		if (halfclickcount <= 0){
			// fire click
			clickcount --;
			if (prevclick == 0){
				serialSendText("Click\n\r");
				prevclick = 1;
			}
		}
		if (clickcount <= 0){
			// release click
			// release halfclick
			triggercount = triggerinit;		// Delay counter for triggering
			halfclickcount = halfclickinit;	// On time for half click
			clickcount = clickinit;		// On time for full click
			prevhalfclick = 0;
			prevclick = 0;
			serialSendText("Done\n\r");
		}
	}

	if ((~PINB & 0x0F) > 0){
		btnCnt++;
		if (btnCnt >= 5){
			if (prevbutton == 0){
				switch(~PINB & 0x0F){
					case(1):
						btnpressed = BTN_ENTER;
						break;
					case(2):
						btnpressed = BTN_CANCEL;
						break;
					case(4):
						btnpressed = BTN_UP;
						break;
					case(8):
						btnpressed = BTN_DOWN;
						break;
					default:
						btnpressed = BTN_NONE;
				}
				prevbutton = 1;
			}
		}
	}
	else{
		btnCnt = 0;
		prevbutton = 0;
	}
}

ISR(USART_RXC_vect)
{
	c = UDR;
	rxflag = 1;;
}

