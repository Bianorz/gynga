

#include <avr/io.h>                        
#include <util/delay.h>                    


int main(void) {

  // Configurar PB5 (Digital 13 no arduino) como saída
  DDRB |= 0b00100000;            

  // ------ Event loop ------ //
  while (1) {

    PORTB = 0b00100000;          // Ligar PB5
    _delay_ms(250);                                           

    PORTB = 0b00000000;          // Desligar PB5
    _delay_ms(250);                                          

  }                                                  
  return 0;                            
}

