#include <Arduino.h>
#include "../include/Seg4DigitHC164.h"

byte dataPin = 2;
byte clockPin = 3;
byte digitPins[] = {8, 9, 10, 11};

Seg4DigitHC164 display;

void switch_to_timer(unsigned long* time_stamp, int* current, int* previous);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  display.init(dataPin, clockPin, digitPins);
}

void loop() {
  int timer_mode = 7;
  static int current_demo_mode = 1;
  static int previous_demo_mode = 0;
  static unsigned long time_stamp = 0;
  int interval = 10000;
  
  // put your main code here, to run repeatedly:
  display.loop();

  switch (current_demo_mode) {
    case 1:
      Serial.println("case 1");
      display.showInt(1234);
      switch_to_timer(&time_stamp, &current_demo_mode, &previous_demo_mode);
      break;
    case 2:
      Serial.println("case 2");
      display.showInt(12345);
      switch_to_timer(&time_stamp, &current_demo_mode, &previous_demo_mode);
      break;
    case 3:
      Serial.println("case 3");
      display.showFloat(2.1987, 2);
      switch_to_timer(&time_stamp, &current_demo_mode, &previous_demo_mode);
      break;
    case 4:
      Serial.println("case 4");
      display.showFloat(2.1987, 4);
      switch_to_timer(&time_stamp, &current_demo_mode, &previous_demo_mode);
      break;
    case 5:
      Serial.println("case 5");
      display.showHex(429);
      switch_to_timer(&time_stamp, &current_demo_mode, &previous_demo_mode);
      break;
    case 6:
      Serial.println("case 6");
      display.showError();
      switch_to_timer(&time_stamp, &current_demo_mode, &previous_demo_mode);
      break;
    case 7:
      if (millis() - time_stamp > interval) {
        if (previous_demo_mode == (timer_mode - 1)) current_demo_mode = 1;
        else current_demo_mode = ++previous_demo_mode;
      }
      break;
    default:
      break;
  }
}

void switch_to_timer(unsigned long* time_stamp, int* current, int* previous)
{
  *time_stamp = millis();
  *previous = *current;
  *current = 7;
}