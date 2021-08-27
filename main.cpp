#include <Arduino.h>
#include "../include/Seg4DigitHC164.h"

byte dataPin = 2;
byte clockPin = 3;
byte digitPin1 = 8;
byte digitPin2 = 9;
byte digitPin3 = 10;
byte digitPin4 = 11;

Seg4DigitHC164 display;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  display.init(dataPin, clockPin, digitPin1, digitPin2, digitPin3, digitPin4);
  display.showInt(1234);
  // display.showInt(1234);
  // display.showFloat(2.1987, 2);
  // display.showHex(429);
}

void loop() {
  // put your main code here, to run repeatedly:
  display.loop();
}