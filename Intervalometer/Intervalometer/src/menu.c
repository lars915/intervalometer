/*
 * menu.c
 *
 * Created: 6/6/2015 7:52:56 AM
 *  Author: Larry
 */
#include "support.h"
#include "menu.h"
#include "lcd.h"
#include "serial.h"

#include <util/delay.h>

typedef enum{
	IDLE,
	RUN,
	STOP,
	RESET,
	TIME,
} menustates;

menustates menustate = IDLE;

void handleMenu(int button_pressed)
{
	switch (menustate)
	{
		case IDLE:
		LCDClearHome();
		LCDSetPos(0,0);
		LCDSendText("Idle");
		switch (button_pressed){
			case BTN_ENTER:
			menustate = RUN;
			break;

			default:
			menustate = IDLE;
		}
		break;

		case RUN:
		LCDClearHome();
		LCDSetPos(0,0);
		LCDSendText("Run");
		switch (button_pressed){
			case BTN_ENTER:
			//TBD start timer
			break;

			case BTN_CANCEL:
			menustate = IDLE;
			break;

			case BTN_DOWN:
			menustate = STOP;
			break;

			default:
			menustate = RUN;
		}
		break;

		case STOP:
		LCDClearHome();
		LCDSetPos(0,0);
		LCDSendText("Stop");
		switch (button_pressed){
			case BTN_ENTER:
			//TBD stop timer
			break;

			case BTN_CANCEL:
			menustate = IDLE;
			break;

			case BTN_UP:
			menustate = RUN;
			break;

			case BTN_DOWN:
			menustate = RESET;
			break;

			default:
			menustate = STOP;
		}
		break;

		case RESET:
		LCDClearHome();
		LCDSetPos(0,0);
		LCDSendText("Reset");
		switch (button_pressed){
			case BTN_ENTER:
			//TBD reset counter
			break;

			case BTN_CANCEL:
			menustate = IDLE;
			break;

			case BTN_UP:
			menustate = STOP;
			break;

			case BTN_DOWN:
			menustate = TIME;
			break;

			default:
			menustate = RESET;
		}
		break;

		case TIME:
		LCDClearHome();
		LCDSetPos(0,0);
		LCDSendText("Time");
		switch (button_pressed){

			case BTN_CANCEL:
			menustate = IDLE;
			break;

			case BTN_UP:
			// Increment time
			break;

			case BTN_DOWN:
			// Decrement time
			break;

			default:
			menustate = TIME;
		}
		break;
	}
}


