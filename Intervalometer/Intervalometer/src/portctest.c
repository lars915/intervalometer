#include <avr/io.h>
#include <util/delay.h>


int main (void)
{
	// Configure LCD
	DDRC = 0xFF;
	
	
	while(1)
	{
	PORTC |= 1<<PORTC0;
	_delay_ms(200);
	PORTC &= ~(1<<PORTC0);
	_delay_ms(500);
	}
}
	
