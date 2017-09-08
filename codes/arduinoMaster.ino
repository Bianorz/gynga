#include <Wire.h>;
#define SLAVE_ADDRESS 0x04 
union testeUnion {
  float v;
  byte b[4];
};
testeUnion op;
void setup()
{
Wire.begin();
  
Serial.begin(9600);

}
 
void loop()
{
delay(1000);
Wire.requestFrom(SLAVE_ADDRESS,4);

  for( int i = 0; i<=3; i++)
{
  op.b[i] = Wire.read();
} 

Serial.print(op.v);
Serial.print("\n");
}
