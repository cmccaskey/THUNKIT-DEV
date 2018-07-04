/*
atmega16u2
*/
#define F_CPU 800000UL //MCU runs at 8MHz

#include <avr/io.h>
#include <util/delay.h>

int main() {
	DDRD |= (1 << DDD0); //PD0 output
	while(1){
		PORTD |= (1 << PD0); //turn PD0 on
		_delay_ms(1000);
		PORTD &= ~(1 << PD0); //turn PD0 off
		_delay_ms(1000);
	}
}
