bool mode = 0;
uint8_t now = 4;
uint8_t prestate = 6;
uint16_t time = 3125;
float speed = 200.0;
char str[20];
bool print = 0;
ISR(INT0_vect) {
   mode = !mode;
   print = 1;  
}

ISR(INT1_vect) {
  print = 1;
  if(time < 3125*5){
    time += 3125;
    speed+=200;
    dtostrf(speed,4,0,str);
    
  }else{
    time = 3125;
    speed = 200;
    dtostrf(speed,4,0,str);
  }
}

ISR(TIMER1_OVF_vect){
  if(mode == 0){
    PORTD = (1<< now)| PORTD & ~(1<< prestate);
    prestate = now;
    now++;
    if(now >= 7){
      now = 4;
    }
  }
  if(mode == 1){
    PORTD = (1<< prestate)| PORTD & ~(1<< now);
    now = prestate;
    prestate--;
    if(prestate < 4){
      prestate = 6;
    }
  }
  TCNT1 = 65536-time;
}

void External_init() {
  EICRA = 0x0A;  //set falling INT0,1
  EIMSK = 0x03;  //enable INT0,1
}

void Timer_init(){
  TCCR1A = 0x00;
  TCCR1B = 0x05;
}
void Serial_begin() {
  UBRR0 = 207; // set baud rate for x2
  UCSR0A = (1 << U2X0); // enable double speed
  UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0); // enable RX interrupt, RX and TX
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // set character size to 8-bit
  sei(); 
}
void Put_Char(char data) {
  while (!(UCSR0A & (1 << UDRE0))); // wait until buffer is empty
  UDR0 = data; // send data
}

void Serial_PutString(const char *data) {
  while (*data) {
    Put_Char(*data++);
  }
}
void setup() {
  Serial_begin();
  DDRD |= (0x7 << PD4);
  External_init();
  Timer_init();
  sei();
  TIMSK1 |= (1 << TOIE1);
  TCNT1 = 65536-time;
  dtostrf(speed,4,0,str);
  Serial_PutString("Red -> Yellow -> Green  :");
  Serial_PutString(str);
  Serial_PutString("ms");
  Serial_PutString("\r\n");
}

void loop() {
 if(print == 1){
  if(mode == 0){
    Serial_PutString("Red -> Yellow -> Green  :");
    Serial_PutString(str);
    Serial_PutString("ms");
    Serial_PutString("\r\n");
  }else if (mode ==1){
    Serial_PutString("Green -> Yellow -> Red  :");
    Serial_PutString(str);
    Serial_PutString("ms");
    Serial_PutString("\r\n");
  }
  print = 0;
  }

}
