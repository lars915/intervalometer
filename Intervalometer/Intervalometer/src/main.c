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
//#include "menu.h"
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
volatile int enterCnt = 0;
volatile int cancelCnt = 0;
volatile int upCnt = 0;
volatile int dnCnt = 0;

volatile int LEDFlag = 0;
volatile int LEDCnt = 0;
volatile unsigned char c = 0;
volatile int rxflag = 0;

volatile char enterbutton = 0;
volatile char preventerbutton = 0;
volatile char cancelbutton = 0;
volatile char prevcancelbutton = 0;
volatile char upbutton = 0;
volatile char prevupbutton = 0;
volatile char downbutton = 0;
volatile char prevdownbutton = 0;

typedef enum {
	OFF,
	ON,
} flagstates;

volatile flagstates enterflag = OFF;
volatile flagstates cancelflag = OFF;
volatile flagstates upflag = OFF;
volatile flagstates dnflag = OFF;


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
	//int count = 0;
	//char buffer[] = "";
	

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
			PORTD ^= 1<<PORTD7;
			LEDFlag = 0;
		}
		
		
		if (enterflag == ON){
			serialSendText("ENTER\n\r");
		}
		enterflag = OFF;
				
		if (cancelflag == ON){
			serialSendText("CANCEL\n\r");
		}
		cancelflag = OFF;
		
		serialSendText("IDLE\n\r");
		//handleMenu();
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
	if (bit_is_clear(PINB, 0)){
		enterCnt++;
		if (enterCnt >= 20)
			enterbutton = 1;
	}
	else{
		enterbutton = 0;
		enterCnt = 0;
	}
	
	if ((preventerbutton == 0) & (enterbutton == 1)){
		enterflag = ON;
	}
	preventerbutton = enterbutton;
	

	// Cancel button flag handling
	if (bit_is_clear(PINB, 1)){
		cancelCnt++;
		if (cancelCnt >= 20)
		cancelbutton = 1;
	}
	else{
		cancelbutton = 0;
		cancelCnt = 0;
	}
	
	if ((prevcancelbutton == 0) & (cancelbutton == 1)){
		cancelflag = ON;
	}
	prevcancelbutton = cancelbutton;
	
	/*
	// Up button flag handling
	if (bit_is_clear(PINB, 2)){
		upCnt++;
		if (upCnt >= 20){
			if (buttons[2] == 0){
				buttons[2] = 1;
			}
		}
	}
	else{
		if (upCnt > 0){
			upCnt--;
		}
	}

	// Down button flag handling
	if (bit_is_clear(PINB, 3)){
		dnCnt++;
		if (dnCnt >= 20){
			if (buttons[3] == 0){
				buttons[3] = 1;
			}
		}
	}
	else{
		if (dnCnt > 0){
			dnCnt--;
		}
	}
	*/

}

ISR(USART_RXC_vect)
{
	c = UDR;
	rxflag = 1;;
}

