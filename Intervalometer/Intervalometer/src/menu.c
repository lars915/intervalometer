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

volatile int buttonCnt = 0;

typedef enum menu_states
{
	IDLE,
	RUN,
	STOP,
	RESET,
	TIME
} menu_State;

menu_State menuState = IDLE;

volatile uint8_t buttons[] = {0,0,0,0};

void handleMenu(void)
{
	_delay_ms(100);
	serialSendChar(0xA);
	serialSendChar(0xD);
	for (int x = 0; x <4; x++)
		serialSendChar(0x30 + buttons[x]);
	serialSendChar(0xA);
	serialSendChar(0xD);
	switch (menuState)
	{
		case IDLE:
			buttons[0] = 0;
			menuState = RUN;
			serialSendChar(0x49);
			LCDSetPos(3,0);
			LCDSendText("Idle");
			break;

		case RUN:
			if (buttons[0] == 1){
				buttons[0]= 0;
				LCDClearHome();
				LCDSetPos(0,0);
				LCDSendText("Start timer");
				//TBD start timer
				serialSendChar(0x52);
			}
			else if (buttons[3] == 1) {
				buttons[3]= 0;
				menuState = STOP;
			}
			else if (buttons[1] == 1){
				buttons[1]= 0;
				menuState = IDLE;
			}
			break;

		case STOP:
			if (buttons[0] == 1){
				buttons[0]= 0;
				LCDClearHome();
				LCDSetPos(0,0);
				LCDSendText("Stop timer");
				//TBD stop timer
				serialSendChar(0x53);
			}
			else if (buttons[2] == 1) {
				buttons[2]= 0;
				menuState = RUN;
			}
			else if (buttons[3] == 1) {
				buttons[3]= 0;
				menuState = RESET;
			}
			else if (buttons[1] == 1){
				buttons[1]= 0;
				menuState = IDLE;
			}
			break;

		case RESET:
			if (buttons[0] == 1){
				buttons[0]= 0;
				LCDClearHome();
				LCDSetPos(0,0);
				LCDSendText("Reset timer");
				//TBD reset timer
				serialSendChar(0x45);
			}
			else if (buttons[2] == 1) {
				buttons[2]= 0;
				menuState = STOP;
			}
			else if (buttons[3] == 1) {
				buttons[3]= 0;
				menuState = TIME;
			}
			else if (buttons[1] == 1){
				buttons[1]= 0;
				menuState = IDLE;
			}
			break;

		case TIME:
			if (buttons[0] == 1){
				buttons[0]= 0;
				LCDClearHome();
				LCDSetPos(0,0);
				LCDSendText("Set time");
				serialSendChar(0x54);
			}
			else if (buttons[2] == 1) {
				buttons[2]= 0;
				// TBD increment time
			}
			else if (buttons[3] == 1) {
				buttons[3]= 0;
				// TBD decrement time
			}
			else if (buttons[1] == 1){
				buttons[1]= 0;
				menuState = IDLE;
			}
			break;
		
		default:{}
	}
}


