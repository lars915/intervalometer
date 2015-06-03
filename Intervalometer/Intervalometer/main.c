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

//#ifndef F_CPU
//#define F_CPU 1000000UL
//#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//#define LCDREADWRITE PIND2 // LCD read/write
#define REGSEL PIND6 // LCD Register select. 0 = command
#define ENABLE PIND5 // LCD Enable.
#define LCDDATA PORTC
#define LCDCONTROL PORTD
#define LCDDATADIR DDRC
#define LCDCONTROLDIR DDRD

void LCDSendCommand(unsigned char command);
void LCDSendChar(unsigned char character);
void TickleLCD(void);
void SeeIfLCDBusy(void);


int count = 0;

int main (void)
{
	// Set up blinker on PD7
	DDRD |= 1<<PIND7;
	TCCR1B |= 1<<CS10 | 1<<CS12 | 1<<WGM12;
	OCR1A = 487;
	TIMSK |= 1<<OCIE1A;
	
	// Configure LCD
	LCDDATADIR = 0xFF;
	LCDCONTROLDIR |= (1<<ENABLE | 1<<REGSEL);
	//LCDCONTROL &= ~(1<<LCDREADWRITE);
	
	
	sei();
	_delay_ms(500);
	LCDDATA = 0x00;
	
	LCDSendCommand(0x1);
	_delay_ms(2);
	LCDSendCommand(0x38);
	_delay_us(50);
	LCDSendCommand(0b00001110);
	_delay_us(50);
	LCDSendChar(0x41);
	_delay_us(60);
	while(1)
	{
	}
}
	
ISR(TIMER1_COMPA_vect)
{
	count++;
	if (count>=5) {
		PORTD |= 1<<PIND7;
		_delay_ms(100);
		PORTD &= !(1<<PIND7);
		count = 0;
	}
}

void LCDSendCommand(unsigned char command)
{
	LCDDATA = command;
	LCDCONTROL &= ~(1<<REGSEL);
	TickleLCD();
	LCDDATA = 0;
}

void LCDSendChar(unsigned char character)
{
	//SeeIfLCDBusy();
	LCDDATA = character;
	//LCDCONTROL &= ~(1<<LCDREADWRITE);
	LCDCONTROL |= 1<<REGSEL;
	_delay_ms(5);
	TickleLCD();
	LCDDATA = 0;
}

void TickleLCD()
{
	//SeeIfLCDBusy();
	LCDCONTROL |= 1<<ENABLE;
	asm volatile ("nop");
	asm volatile ("nop");
	LCDCONTROL &= ~(1<<ENABLE);
}

void SeeIfLCDBusy()
{
	LCDDATADIR = 0;
	//LCDCONTROL |= (1<<LCDREADWRITE);
	LCDCONTROL &= ~(1<<REGSEL);
	
	while(LCDDATA >= 0x80)
	{
		TickleLCD();
	}
	
	//LCDCONTROL &= ~(1<<LCDREADWRITE);
	LCDDATADIR = 0xff;
	
}
	

