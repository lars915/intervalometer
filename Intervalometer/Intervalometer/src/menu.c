/*
 * menu.c
 *
 * Created: 6/6/2015 7:52:56 AM
 *  Author: Larry
 */
#include "menu.h"
#include "lcd.h"
#include "serial.h"

#include <util/delay.h>

typedef enum{
	HOME,
	RUN,
	STOP,
	RESET,
	TIME,
} menustates;

menustates menustate = HOME;
static runstatesenum runstate = RS_DONTRUN;
int counter = 0;
int time = 5;

runstatesenum handleMenu(int button_pressed)
{
	LCDSetPos(3,0);
	LCDSendText("                  ");
	LCDSetPos(3,0);
	switch (menustate)
	{
		case HOME:
		LCDSendText("Home");
		switch (button_pressed){
			case BTN_ENTER:
			menustate = RUN;
			break;

			default:
			menustate = HOME;
		}
		break;

		case RUN:
		LCDSendText("Run");
		switch (button_pressed){
			case BTN_ENTER:
			runstate = RS_RUN;
			//TBD start timer
			break;

			case BTN_CANCEL:
			menustate = HOME;
			break;

			case BTN_DOWN:
			menustate = STOP;
			break;

			default:
			menustate = RUN;
		}
		break;

		case STOP:
		LCDSendText("Stop");
		switch (button_pressed){
			case BTN_ENTER:
			//TBD stop timer
			runstate = RS_DONTRUN;
			break;

			case BTN_CANCEL:
			menustate = HOME;
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
		LCDSendText("Reset");
		switch (button_pressed){
			case BTN_ENTER:
			//TBD reset counter
			counter = 0;
			break;

			case BTN_CANCEL:
			menustate = HOME;
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
		LCDSendText("Time");
		switch (button_pressed){

			case BTN_CANCEL:
			menustate = HOME;
			break;

			case BTN_UP:
			// Increment time
			time++;
			break;

			case BTN_DOWN:
			// Decrement time
			time--;
			break;

			default:
			menustate = TIME;
		}
		break;
	}
	return runstate;
}


