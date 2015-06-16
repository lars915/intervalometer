/*
 * serial.c
 *
 * Created: 6/6/2015 10:20:09 AM
 *  Author: Larry
 */

#include <avr/io.h>
#include "serial.h"

void serialSendChar(unsigned char c)
{
	while (!(UCSRA & (1<<UDRE))); // Wait for serial TX to be ready
	UDR = c;
}
void serialSendText(char *text)
{
	while (*text > 0)
	{
		serialSendChar(*text++);
	}
}
