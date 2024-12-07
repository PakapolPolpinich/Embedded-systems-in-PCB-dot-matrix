#include <util/delay.h>
uint8_t Scrolltextrow[8] = {0};
void Spi_init() {
  DDRB |= (1 << 2) | (1 << 3) | (1 << 5);  //set spi pin output
  SPCR = 0b01010000;                       //set 8Mhz
  SPSR = (1 << SPI2X);
}
void Spi_tranfer(uint8_t data) {
  SPDR = data;
  while (!(SPSR & (1 << SPIF)));  //when SPIF set = 1 it finish
}
void Senddata(uint8_t address, uint8_t data) {
  PORTB &= ~(1 << 2);
  Spi_tranfer(address);
  Spi_tranfer(data);
  PORTB |= (1 << 2);
}
void Max7219_init() {
  Senddata(0x0C, 0x00);  //shutdown
  Senddata(0x09, 0x00);  //set no decode
  Senddata(0x0A, 0x01);  //set intensity  3/32
  Senddata(0x0B, 0x07);  //scanlimit display digit0-7
  Senddata(0x0C, 0x01);  //normal operation
}
const uint8_t Name[8][8] = {
  { 0xff, 0x81, 0x81, 0xff, 0x80, 0x80, 0x80, 0x80 },  //p
  { 0x18, 0x24, 0x42, 0x81, 0xff, 0x81, 0x81, 0x81 },  //a
  { 0x82, 0x84, 0x88, 0x90, 0xe0, 0xa0, 0x98, 0x84 },  //k
  { 0x18, 0x24, 0x42, 0x81, 0xff, 0x81, 0x81, 0x81 },  //a
  { 0xff, 0x81, 0x81, 0xff, 0x80, 0x80, 0x80, 0x80 },  //p
  { 0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e },  //o
  { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xff },  //L
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } // space
};
void setup() {
  Spi_init();
  Max7219_init();
}
void loop() {
  for (uint8_t k = 0; k <= 7; k++) {//change character
    for (uint8_t i = 0; i < 8; i++) {//shift bit
      for (uint8_t j = 0; j < 8; j++) {//print row
        Scrolltextrow[j] = ((Scrolltextrow[j] << 1) | (Name[k][j] >> (7 - i)));//& 0xFF
        Senddata((0x01 + j), Scrolltextrow[j]);
      }
      _delay_ms(100);
    }
  }
}

// Serial.print(Scrolltextrow[j]<<1,BIN);
// Serial.print(" | ");
// Serial.print((Name[k][j] >> (7 - i)),BIN);
        // Serial.print(" = ");
        //Serial.println(Scrolltextrow[j],BIN);
