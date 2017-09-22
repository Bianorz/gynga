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

	// Disable Global Interrupt, to assure that this interruption won't be stopped by other interruptions
	cli();

/*TWSR & 0xF8 is used to get TWI Status Register, only the 5 MSB 0b11111000, used to know what's happening in the i2c peripheral
We will be interested in three different states, when we receive data (status = 0x80), when we transmitt 
data(status = 0xA0) and when the master stops the communication (status = 0xA0). A complete description
off all status codes can be found in pg 218,219,221,224,227 and 228 from the AtMega328P datasheet
*/	twi_status = TWSR & 0xF8;
   switch (twi_status) {
	case 0x80:     // 0x80: data received, ACK returned
		if (i < BUFFSIZE) {
			dataRead[i] = TWDR;
			i = i + 1;
		}
		break;

	case 0xA0: // 0xA0: stop or repeated start condition received while selected
		i = 0;
		j=0;
		break;

	case 0xB8:     // 0xB8: data transmitted, ACK received
		if (j < BUFFSIZE){
		TWDR = var.b[j];      // Store data in TWDR register
		j = j +1;
		}
		break;	
	}
	// Every time a byte is transfered across the I2C line, the TWINT bit is set by hardware, we need to clear it to became available for a new connection. We clear it, by putting '1' in the TWINT bit (clear by putting '1' is confusing but it's like AVR implemented this operation)
	TWCR |= (1 << TWINT);    // Clear TWINT Flag
	// Enable Global Interrupt
	sei();
}
/*

To enable the I2C peripheral as slave device we must do the following operations:
gen_call = 0; To make the device responds only by its address
TWAR = ((slave_address << 1) | gen_call << 1); Load the slave address and the gen_call in TWAR
TWDR = 0; TWDR is the data register, it's cleared just to assure that all data from previous communications are erased
1<<TWINT; To clear the TWINT register and let the I2C lines available
1<<TWEA; To enable an ack message in response of every received data
1<<TWEN; To enable the I2C peripheral
1<<TWIE; To enable the i2c interruption
sei(); to enable the global interruptions
*/

void enableI2c(uint8_t slave_address) {
	uint8_t gen_call = 0; // gen_call desabilitada
	TWAR = ((slave_address << 1) | gen_call << 1);
	TWDR = 0x00;                 // Default Initial Value
	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
	sei();
}
/*
In the main loop we enable the i2c using the function 'enableI2c(SLAVEADDRESS)', this means that every time the master starts a communication for SLAVEADDRESS it will trigger a interruption. In the for loop we just flash the data sent by the master in PORTB.
var.v = 5.2; Float number that is sent when the master request data



*/
int main(void) {
	var.v = 5.2;
	DDRB = 0xFF;     
	PORTB = 0x00;
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
