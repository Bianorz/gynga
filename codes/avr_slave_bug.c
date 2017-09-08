#include <avr/interrupt.h>
#include <avr/io.h>
#define SLAVE_ADDRESS 0x04 
union testeUnion {
	float v;
	byte b[4];
};
testeUnion op;

ISR(TWI_vect)
{
	// Disable Global Interrupt
	cli();
	// Turn on Led
	PORTB = 0b00100000;
	// Enable Global Interrupt
	sei();
}

void setup() {
	// Set PB 5 as output
	DDRB |= 0b00100000;
	// Load slave adress in TWAR, bit 7 to 1
	TWAR = SLAVE_ADDRESS << 1;
	// Set SCL speed
	TWBR = 72;
	// Clear data register
	TWDR = 0x00;
	// start slave listening
	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
	// Enable Global Interrupt
	sei();
}

void loop() {
	delay(100);
}

