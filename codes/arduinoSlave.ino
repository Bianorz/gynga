#include <Wire.h>;
#define SLAVE_ADDRESS 0x04 
union testeUnion {
  float v;
  byte b[4];
};
testeUnion op;
void setup()
{
Wire.begin(SLAVE_ADDRESS);
Wire.onRequest(requestEvent);
}
 
void loop()
{
delay(100);
}
 
void requestEvent()
{
op.v = 5.2; 
Wire.write(op.b, 4);
}
