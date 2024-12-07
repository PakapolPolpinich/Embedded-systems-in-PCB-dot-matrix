#include <util/delay.h>

uint8_t Scrolltextrow[8] = { 0 };
char belt[99];
uint8_t newd[sizeof(belt)][8] = {0};
int i =0;

void Serial_begin() {
  UBRR0 = 207; // Set baud rate for double speed
  UCSR0A = (1 << U2X0); // Enable double speed
  UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0); // Enable RX interrupt, RX and TX
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Set character size to 8-bit
  sei(); // Enable global interrupts
}

void Put_Char(char data) {
  while (!(UCSR0A & (1 << UDRE0))); // Wait until buffer is empty
  UDR0 = data; // Send data
}

void Serial_PutString(const char *data) {
  while (*data) {
    Put_Char(*data++);
  }
  // Put_Char('\r'); // Carriage return
  // Put_Char('\n'); // New line
}

ISR(USART_RX_vect) {
  if(UDR0 == '32'){
    i = 0;
  }
  belt[i] = UDR0; // Read the data to clear the RXC flag
  i++;
   // Set the flag to indicate that a character has been received
}

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

const uint8_t Name[10][8]{
  {0x3c,0x42,0x81,0x81,0x81,0x81,0x42,0x3c},  //0
  {0x10,0x30,0x50,0x10,0x10,0x10,0x10,0x10},  //1
  {0x1c,0x24,0x44,0x08,0x10,0x20,0x40,0xfc},  //2
  {0x7c,0x04,0x04,0x7c,0x04,0x04,0x04,0x7c},  //a
  {0x04,0x0c,0x14,0x3e,0x04,0x04,0x04,0x04},  //p
  {0xff,0x80,0xfc,0x04,0x04,0x04,0x04,0xfc},  //o
  {0xfc,0x80,0x80,0x80,0xfc,0x84,0x84,0xfc},
  {0xff,0x02,0x04,0x04,0x08,0x10,0x20,0x40},
  {0x7c,0x44,0x44,0x7c,0x44,0x44,0x44,0x7c},  //L
  {0x7c,0x44,0x44,0x7c,0x04,0x04,0x04,0x04}   // space
};
void setup() {
  Serial_begin();
  Spi_init();
  Max7219_init();
  for (uint8_t i = 0; i < (sizeof(belt) - 1); i++) {
    for (uint8_t j = 0; j < 8; j++) {
      if (belt[i] == '0') {
        newd[i][j] = Name[0][j];
      } else if (belt[i] == '1') {
        newd[i][j] = Name[1][j];
      } else if (belt[i] == '2') {
        newd[i][j] = Name[2][j];
      } else if (belt[i] == '3') {
        newd[i][j] = Name[3][j];
      } else if (belt[i] == '4') {
        newd[i][j] = Name[4][j];
      } else if (belt[i] == '5') {
        newd[i][j] = Name[5][j];
      } else if (belt[i] == '6') {
        newd[i][j] = Name[6][j];
      } else if (belt[i] == '7') {
        newd[i][j] = Name[7][j];
      } else if (belt[i] == '8') {
        newd[i][j] = Name[8][j];
      } else if (belt[i] == '9') {
        newd[i][j] = Name[9][j];
      }
    }
  }
}


void loop() {
  for (uint8_t k = 0; k <= (sizeof(belt) - 1); k++) {
    for (uint8_t i = 0; i < 8; i++) {
      for (uint8_t j = 0; j < 8; j++) {
        Scrolltextrow[j] = ((Scrolltextrow[j] << 1) | (newd[k][j] >> (7 - i)));  //& 0xFF
        Senddata((0x01 + j), Scrolltextrow[j]);
        //       //Serial.println(Scrolltextrow[j],BIN);
      }
      delay(100);
    }
  }
}
  // Serial.print(Scrolltextrow[j]<<1,BIN);
  // Serial.print(" | ");
  // Serial.print((Name[k][j] >> (7 - i)),BIN);
  // Serial.print(" = ");
  //Serial.println(Scrolltextrow[j],BIN);