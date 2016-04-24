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

void triggerhalf(void);
void triggerfull(void);
void triggernone(void);


volatile int btnCnt = 0;

volatile int LEDFlag = 0;
volatile int LEDCnt = 0;
volatile unsigned char c = 0;
volatile int rxflag = 0;

volatile int prevbutton = 0;

extern int counter;
extern int time;

typedef enum {
	OFF,
	ON,
} flagstates;

volatile char trigger = 0;

volatile int cnt10ms = 0;
volatile int cnt100ms = 0;
volatile int cnt1sec = 0;

volatile btnpressedenum btnpressed = BTN_NONE;
volatile runstatesenum runstate = RS_DONTRUN; // If DONTRUN, camera will not be triggered

volatile char lcdchanged = 1;
char buffer[4];
char *out;

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

	DDRD |= (1<<PIND7 | 1<<PIND3 | 1<<PIND4);

    //DDRA |= (1<<PINA0 | 1<<PINA1);
    //PORTA &= ~(1<<PORTA0 | 1<<PORTA1);
    //_delay_ms(10);
	LCDInit();

	while(1)
	{
		_delay_ms(200);
		if (rxflag)
		{
			LCDSetPos(0,15);
			LCDSendChar(c);
		}
		if (LEDFlag)
		{
			LEDFlag = 0;
    		PORTD |= 1<<PORTD7;
            _delay_ms(100);
    		PORTD &= ~(1<<PORTD7);
		}

		/*
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
        */
		runstate = handleMenu(btnpressed);

        if (runstate == RS_RUN){
			serialSendText("RUN\n\r");
		}
		else{
			serialSendText("STOP\n\r");
		}

        if ((trigger == 1) && (runstate == RS_RUN)){
            triggerhalf();
            _delay_ms(100);
            triggerfull();
            _delay_ms(100);
            triggernone();
            trigger = 0;
        }

		btnpressed = BTN_NONE;

		if (lcdchanged == 1){
			LCDSetPos(0,0);
			LCDSendText("Delay time: ");
			itoa(time, buffer, 10);
			LCDSendText(buffer);
            LCDSendText("    ");
			LCDSetPos(1,0);
			LCDSendText("Frames: ");
			itoa(counter, buffer, 10);
			LCDSendText(buffer);
            LCDSendText("      ");
			LCDSetPos(2,0);
			if (runstate == RS_RUN){
				LCDSendText("Running");
			}
			else{
				LCDSendText("Stopped");
			}
			lcdchanged = 0;
		}
	}
}

ISR(TIMER1_COMPA_vect)
{
	LEDCnt++;
	if (LEDCnt >= 400){
		LEDFlag = 1;
        LEDCnt = 0;
   	}

	cnt10ms++;
	if (cnt10ms >= 10){
		cnt10ms = 0;
		cnt100ms++;
        out = itoa(cnt1sec, buffer, 10);
        serialSendText(out);
        serialSendText("\n\r");
	}

    if (cnt100ms >= 10){
        cnt1sec++;
        cnt100ms = 0;
    }

	if (cnt1sec >= time){
        trigger = 1;
        cnt1sec = 0;
	}

	if ((~PINB & 0x0F) > 0){
		btnCnt++;
		lcdchanged = 1;
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

void triggerhalf(void){
    PORTD |= 1<<PORTD3;
    serialSendText("Halfclick\n\r");
}

void triggerfull(void){
    PORTD |= 1<<PORTD4;
    serialSendText("Click\n\r");
}

void triggernone(void){
    counter++;
    PORTD &= ~(1<<PORTD3 | 1<<PORTD4);
	serialSendText("Done\n\r");
    lcdchanged = 1;
}