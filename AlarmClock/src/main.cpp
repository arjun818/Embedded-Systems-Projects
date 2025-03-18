#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

static void Timer1_setup(void){
  TCCR1A= 0;         //CTC mode
  TCCR1B= B00001100; //Prescalar=256, CTC mode
  TIMSK1|=B00000010; //Interrupt enable for OCR14 compare match
  OCR1A=  6250-1;    //OC match value for 100ms time base generation
}
void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}