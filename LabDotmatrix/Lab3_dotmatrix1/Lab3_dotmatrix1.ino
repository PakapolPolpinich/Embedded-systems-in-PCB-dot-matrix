#include <util/delay.h>
void Spi_init() {
  DDRB |= (1 << 2) | (1 << 3) | (1 << 5);  //set spi pin output
  SPCR = 0b01010000;//set 8Mhz 
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
const uint8_t Name[7][8]{
  { 0xff, 0x81, 0x81, 0xff, 0x80, 0x80, 0x80, 0x80 },  //p
  { 0x18, 0x24, 0x42, 0x81, 0xff, 0x81, 0x81, 0x81 },  //a
  { 0x82, 0x84, 0x88, 0x90, 0xe0, 0xa0, 0x98, 0x84 },  //k
  { 0x18, 0x24, 0x42, 0x81, 0xff, 0x81, 0x81, 0x81 },  //a
  { 0xff, 0x81, 0x81, 0xff, 0x80, 0x80, 0x80, 0x80 },  //p
  { 0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e },  //o
  { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xff }   //L
};
void setup() {
  Spi_init();
  Max7219_init();
}

void loop() {
  for (uint8_t i = 0; i <= 6; i++) {
    for (uint8_t j = 0; j < 8; j++) {
      Senddata((0x01 + j), Name[i][j]);
    }
    _delay_ms(2000);
  }
}
