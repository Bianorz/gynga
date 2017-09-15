#include <avr/io.h>
#include <util/delay.h>
#include <compat/twi.h>
#include <avr/interrupt.h>

#define BUFFSIZE 0x04
#define SLAVEADDRESS 0X05

typedef union{
  float v;
  uint8_t b[4];
} Union;

Union var;
uint8_t dataRead[BUFFSIZE] = {0}; // buff onde será armazenado o dado recebido
//uint8_t dataWrite[BUFFSIZE] = {0}; // buff onde será armazenado o dado enviado
uint8_t i=0,j=0; // contador

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
