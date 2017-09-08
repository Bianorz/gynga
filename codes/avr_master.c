#define SLAVE_ADDRESS 0x04
union testeUnion {
	float v;
	byte b[4];
};
testeUnion op;
void initI2C(void) {
	TWBR = 72;
	TWCR |= (1 << TWEN);
}
void i2cWaitForComplete(void) {
	loop_until_bit_is_set(TWCR, TWINT);
}
void i2cStart(void) {
	TWCR = (_BV(TWINT) | _BV(TWEN) | _BV(TWSTA));
	i2cWaitForComplete();
}
void i2cStop(void) {
	TWCR = (_BV(TWINT) | _BV(TWEN) | _BV(TWSTO));
}
uint8_t i2cReadAck(void) {
	TWCR = (_BV(TWINT) | _BV(TWEN) | _BV(TWEA));
	i2cWaitForComplete();
	return (TWDR);
}
uint8_t i2cReadNoAck(void) {
	TWCR = (_BV(TWINT) | _BV(TWEN));
	i2cWaitForComplete();
	return (TWDR);
}

void i2cSend(uint8_t data) {
	TWDR = data;
	TWCR = (_BV(TWINT) | _BV(TWEN));
	i2cWaitForComplete();
}
byte address, c, d, e, first;
void setup() {
	initI2C();
	Serial.begin(9600);
}

void loop() {
	i2cStart();
	i2cSend((SLAVE_ADDRESS << 1) | 1);
	i2cStart();
	address = i2cReadAck();
	for (int i = 0; i <= 3; i++) {
		op.b[i] = i2cReadAck();
	}
	if (op.b[0] != 255) {
		Serial.print(op.v);
		Serial.print('\n');
	}
	delay(500);
}

