/*
  Seg4digit.h - Library containing functions for controlling a four
  digit segment led display with a SN74HC164 bit shift register.
  Created 04-06-2021 by Tim Ruterink.

  Note:
  The bit shift register is connected to the led segments, and
  controls the symbol that is sent to the display.
  Please consult the connection diagram for details on how the
  bit shift register, led segment digits and arduino are 
  connected.
  
  Updated: 20-08-2021.

  For study purposes.
*/

#ifndef SEG4DIGITHC164_H
#define SEG4DIGITHC164_H

#include <Arduino.h>

class Seg4DigitHC164 {
  
  private:
    // define amount of display digits.
    static const int numOfDisplayDigits = 4;

    // shift register pins, led segment digit pins.
    byte _dataPin;
    byte _clockPin;
    byte _digitPins[4];

    // current input data.
    int currentInputInt;
    float currentInputFloat;
    unsigned long currentInputLong;
    int currentInputLength;

    // input buffer data.
    static const int bufferLength = 16; // define buffer size in header file.
    char inputBuffer[bufferLength];
    byte displayBuffer[bufferLength];
    int maxInputLength;

    // output data.
    byte currentFrame[numOfDisplayDigits];
    byte currentFrameCopy[numOfDisplayDigits];

    // scrolling data.
    bool scrolling;
    int numOfscrollingFrames;
    int scrollingInterval;
    int currentScrollingFrame;
    unsigned long timeStampFrame; 

    // display loop data.
    unsigned long timeStampDigit;
    byte currentDigit;
    byte previousDigit;
    bool errorShown;
    unsigned long timeStampError;
    int errorDuration;

    // refresh rate settings.
    int refreshRate;
    unsigned int refreshRateMillis;

    void buildInputBuffer(char outputType, int decimalPlaces = 0);
    void buildDisplayBuffer(int pointIndex = -1);
    void processDisplayBuffer();
    void updateCurrentFrame();

    void buildScrollingBuffer();
    void updateScrollingFrame();

    void removeError();
    long convertFloatToLong(int decimalPlaces);
    int getInputLength();

  public:
    Seg4DigitHC164();
    void init(byte dataPin, byte clockPin, byte digitPin1,
      byte digitPin2, byte digitPin3, byte digitPin4);
    void loop();
    
    // interfaces.
    void showInt(int input);
    void showFloat(float input, int decimalPlaces);
    void showHex(unsigned long input);
    void showError();
};

#endif