/*

This code describes the configuration of an AtMega328P acting as a Slave Device using the I2C Protocol. A float point is sent while PORTB
flashes the data received by the master device
Details about how the AtMega deals with I2C can be found here:
* Book | Make AVR Programming, pg 359
* Online | Transforming your AVR Microcontroller to the I2C or TWI Slave I/O Expander Project, http://www.ermicro.com/blog/?p=1239
* Online | How to use I2C-bus on the Atmel AVR Microcontroller, http://www.ermicro.com/blog/?p=744
* Datasheet | AtMega328P, pg 206 
*/

#include <avr/io.h>
#include <util/delay.h>
#include <compat/twi.h>
#include <avr/interrupt.h>

/*
BUFFSIZE: Defines the size of the data that will be read/sent, in this code the packets will have 4 bytes long.
SLAVEADDRESS: Address of the slave device
*/
#define BUFFSIZE 0x04
#define SLAVEADDRESS 0X05
/*
The Union is defined to use the same memory location for different data types, in this case it's used to store a float point, more details can be found here https://www.tutorialspoint.com/cprogramming/c_unions.htm

Warning: typedef is mandatory in C and optional in  C++. Example:
C++:		
union floatByte {
  float v;
  uint8_t b[4];
};

C: 
typedef union{
  float v;
  uint8_t b[4];
} floatByte;

*/
typedef union{
  float v;
  uint8_t b[4];
} Union;
/*
var: variable where the float point to be sent is stored
dataRead: Array where the received bytes are stored
i,j: Counters used for indexing the arrays to be sent/received
*/
Union var;
uint8_t dataRead[BUFFSIZE] = {0};
uint8_t i=0,j=0;

/*
Considerations about the ISR:
ISRs are special routines that run when their interrupt flag is set, and their interrupt
vector is called. To enable it we must enable the global interrupts using the function sei(), and set the I2C interrupt TWIE
TWI_vect is the macro that refers to the I2C Interruption
More details can be found here: Book | Make AVR Programming, pg 153

*/
ISR(TWI_vect) {
	static unsigned char i2c_state;
	unsigned char twi_status;

	// Disable Global Interrupt
	cli();

	// Get TWI Status Register, mask the prescaler bits (TWPS1,TWPS0)
	twi_status = TWSR & 0xF8;
   switch (twi_status) {
	case TW_SR_SLA_ACK:      // 0x60: SLA+W received, ACK returned
		//i2c_state = 0;          // Start I2C State for Register Address required
		TWCR |= (1 << TWINT);    // Clear TWINT Flag
		break;

	case TW_SR_DATA_ACK:     // 0x80: data received, ACK returned
		//PORTB = TWDR;
		if (i < BUFFSIZE) {
			dataRead[i] = TWDR;
			i = i + 1;
		}

		TWCR |= (1 << TWINT);    // Clear TWINT Flag
		break;

	case TW_SR_STOP: // 0xA0: stop or repeated start condition received while selected
		i = 0;
		j=0;
		TWCR |= (1 << TWINT);    // Clear TWINT Flag
		break;

	case TW_ST_SLA_ACK:      // 0xA8: SLA+R received, ACK returned
	case TW_ST_DATA_ACK:     // 0xB8: data transmitted, ACK received
		if (j < BUFFSIZE){
		TWDR = var.b[j];      // Store data in TWDR register
		j = j +1;
		}
		//i2c_state = 0;	      // Reset I2C State
		TWCR |= (1 << TWINT);    // Clear TWINT Flag
		break;

	case TW_ST_DATA_NACK:    // 0xC0: data transmitted, NACK received
	case TW_ST_LAST_DATA:    // 0xC8: last data byte transmitted, ACK received
	case TW_BUS_ERROR:       // 0x00: illegal start or stop condition
	default:
		TWCR |= (1 << TWINT);    // Clear TWINT Flag
		//i2c_state = 0;         // Back to the Begining State
	}

	// Enable Global Interrupt
	sei();
}


void enableI2c(uint8_t slave_address) {
	uint8_t gen_call = 0; // gen_call desabilitada
	TWAR = ((slave_address << 1) | gen_call << 1);
	TWDR = 0x00;                 // Default Initial Value
	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
	sei();
}

int main(void) {
	var.v = 5.2;
	DDRB = 0xFF;      // Set PORTB: PB0=Input, Others as Output
	PORTB = 0x00;
	DDRD = 0xFF;      // Set PORTD to Output
	PORTD = 0x00;     // Set All PORTD to Low
	enableI2c(SLAVEADDRESS);
	uint8_t cont = 0;
	for (;;) {
		for (cont = 0;cont <=3;cont++){
		PORTB = dataRead[cont];
		_delay_ms(250);
		}
	}
	return 0;
}
