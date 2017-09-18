#include <avr/io.h>
#include <util/delay.h>
#include <compat/twi.h>
#include <avr/interrupt.h>

#define BUFFSIZE 0x04
#define SLAVEADDRESS 0X05

// TASK: add comment about `union` (c++) vs `typedef union` (c)
typedef union{
  float v;
  uint8_t b[4];
} Union;

Union var;
uint8_t dataRead[BUFFSIZE] = {0}; // buff onde será armazenado o dado recebido
//uint8_t dataWrite[BUFFSIZE] = {0}; // buff onde será armazenado o dado enviado
uint8_t i=0,j=0; // contador


// TASK: document ISR, e.g. its use cases, sources to read about it
// document also TWI_vect
ISR(TWI_vect) {
	static unsigned char i2c_state;
	unsigned char twi_status;

	// Disable Global Interrupt
	// TASK: why exactly is it needed? what could happen if not used?
	cli();

	// Get TWI Status Register, mask the prescaler bits (TWPS1,TWPS0)
	// only the 5 firsts bits are used for twi_status (0xF8 = b11111000)
	// TASK: explain the twi_status (in the context of TWI)
	//
	// TASK: explain in detail each case
	twi_status = TWSR & 0xF8;
   switch (twi_status) {
	// case TW_SR_SLA_ACK:      // 0x60: SLA+W received, ACK returned
	// 	//i2c_state = 0;          // Start I2C State for Register Address required
	// 	break;

	case TW_SR_DATA_ACK:     // 0x80: data received, ACK returned
		//PORTB = TWDR;
		if (i < BUFFSIZE) {
			dataRead[i] = TWDR;
			i = i + 1;
		}
		break;

	case TW_SR_STOP: // 0xA0: stop or repeated start condition received while selected
		i = 0;
		j = 0;
		break;

	case TW_ST_SLA_ACK:      // 0xA8: SLA+R received, ACK returned
  // TASK: explain! could it be commented out?
	case TW_ST_DATA_ACK:     // 0xB8: data transmitted, ACK received
		if (j < BUFFSIZE) {
		  TWDR = var.b[j];      // Store data in TWDR register
		  j = j +1;
		}
		//i2c_state = 0;	      // Reset I2C State
		break;

	// case TW_ST_DATA_NACK:    // 0xC0: data transmitted, NACK received
	// case TW_ST_LAST_DATA:    // 0xC8: last data byte transmitted, ACK received
	// case TW_BUS_ERROR:       // 0x00: illegal start or stop condition

  TWCR |= (1 << TWINT);

	// Enable Global Interrupt
	sei();
}


void enableI2c(uint8_t slave_address) {
	uint8_t gen_call = 0; // gen_call desabilitada
	TWAR = ((slave_address << 1) | gen_call << 1);
	TWDR = 0x00;                 // Default Initial Value
  // TASK: explain the use of each register
	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
	sei();
}


// TASK: explain the logical flow necessary to run with i2c/twi
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
