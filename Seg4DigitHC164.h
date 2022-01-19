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

#define NUM_OF_DISPLAY_DIGITS 4
#define BUFFER_LENGTH 16

/*
  NOTES ABOUT NUMBER OF DIGITS:

  The number of display digits is defined in the header file. Default is 4.
  When you change this value (especially to something larger than 4), you
  should also update the init() method so you can tell the program which
  Arduino pins the display is connected to.
*/

/*
  NOTES ABOUT INPUT BUFFER SIZE:

  Size of the input buffer is set in the header file. The default value
  is set to 16, with a display of 4 digits in mind. This buffer size
  value affects the inputBuffer as well as the displayBuffer.

  When modifying the buffer size, consider the following aspects.

  A buffersize of 16 offers room for:
  - 15 chars.
  - 1 null terminator.

  When the number of input characters is larger than the amount of
  display digits, a scrolling functionality is activated. An animation
  is created by showing the display buffer frame by frame (one frame is
  the same length as the display). However, blank spaces need to be added
  to the display buffer to make the output scroll in and out of view.
  These blank spaces also use the available buffer size. 
  
  So, in case of a 4-digit display, a buffersize of 16 offers room for:
  - 3 blank spaces to scroll the output into view.
  - 9 characters for the actual output.
  - 4 blank spaces to scroll the output out of view.
  (When building a displaybuffer for scrolling, the null terminator is discarded).

  The maximum input length in this case is 9 characters.
  
  The program calculates this in the init() method, using the buffersize
  and the number of display digits constant.
*/

/*
  NOTES ABOUT REFRESH RATE:

  You can only send one display value to the segment display. By (de)activating
  the separate digits you can decide which digit shows the input value.

  To show more than 1 digit, the program very quickly switches from one digit 
  to another, at the same time changing the output value. This means we have to
  take in account a timing interval between the digits switching on and off.
  This delay between the off and on state of one digit should not be greater
  than 12 milliseconds to avoid noticeable flickering when looking straight
  at the display. (This is based on my own observations with a 4-digit segment 
  display). There is some flickering visible from the corner of your eye.

  When there are more than 2 digits, the off-time is longer than the on-time.
  So, the off-time (the longest interval) should not exceed 12 milliseconds.
  
  Interval ratio with 4 digits:
    If on-time + off-time equals 1, than for each digit:
      - on-time equals 1/4.
      - off-time equals 3/4.
    Off-time is the longest, and should not be greater than 12 milliseconds.
    If off-time = 12 ms (3/4 of total time), on-time would be 4 ms (1/4).

  This means the minimum refresh rate equals 4 ms per digit.
  Which is 1 / 0,004 = 250 hz.

  Because my current configuration uses a non-latching bit shift register,
  high refresh rates (like 1500) cause all led segments to light up because
  all the 'bits' are constantly being 'shoved through' the whole display.
*/


class Seg4DigitHC164 {
  
  private:

    // shift register pins, led segment digit pins.
    byte _dataPin;
    byte _clockPin;
    byte _digitPins[NUM_OF_DISPLAY_DIGITS];

    // current input data.
    int currentInputInt;
    float currentInputFloat;
    unsigned long currentInputLong;
    int currentInputLength;

    // input buffer data.
    char inputBuffer[BUFFER_LENGTH];
    byte displayBuffer[BUFFER_LENGTH];
    int maxInputLength;

    // output data.
    byte currentFrame[NUM_OF_DISPLAY_DIGITS];
    byte currentFrameCopy[NUM_OF_DISPLAY_DIGITS];

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

    // methods.
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
    void init(byte dataPin, byte clockPin, byte* digitPins);
    void loop();
    
    // interfaces.
    void showInt(int input);
    void showFloat(float input, int decimalPlaces);
    void showHex(unsigned long input);
    void showError();
};

#endif