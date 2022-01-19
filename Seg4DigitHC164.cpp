#include <Arduino.h>
#include "../include/Seg4DigitHC164.h"
#include "../include/BinarySymbols.h"

// add helper class for converting input chars to led display bytes.
BinarySymbols displaySymbols;

/*
  -----------------
  CONSTRUCTOR
  -----------------
*/

Seg4DigitHC164::Seg4DigitHC164()
{
  // empty constructor, initialisation is done with init() method.
}



/*
  -----------------
  PUBLIC METHODS
  -----------------
*/

void Seg4DigitHC164::init(byte dataPin, byte clockPin, byte* digitPins)
{
  int i = 0;

  // assign pins.
  _dataPin = dataPin;
  _clockPin = clockPin;
  pinMode(_dataPin, OUTPUT);
  pinMode(_clockPin, OUTPUT);

  for (i = 0; i < NUM_OF_DISPLAY_DIGITS; i++)
  {
    _digitPins[i] = digitPins[i];
    pinMode(_digitPins[i], OUTPUT);
  }

  /*
    Calculate maximum input length. When the input length exceeds the number
    of display digits, scrolling functionality is activated. The maximum
    input length depends on the input buffer size and the number of empty
    whitespaces needed to build a scrolling animation:

    maximum input length = BUFFER_LENGTH - required blank spaces.

    Required blank spaces: scrolling view starts with one symbol visible, so 
    we need (number of digits - 1) blank paces. Scrolling ends with a blank 
    display, for this we need (number of digits) blank spaces.

    required blank spaces = (NUM_OF_DISPLAY_DIGITS - 1) + NUM_OF_DISPLAY_DIGITS.
  */

  maxInputLength = BUFFER_LENGTH - (NUM_OF_DISPLAY_DIGITS - 1) - NUM_OF_DISPLAY_DIGITS;

  // set current frame (value shown on display) to all zeroes.
  for (i = 0; i < NUM_OF_DISPLAY_DIGITS; i++)
  {
    currentFrame[i] = displaySymbols.zero;
  }

  // initialize current frame copy.
  for (i = 0; i < NUM_OF_DISPLAY_DIGITS; i++)
  {
    currentFrameCopy[i] = displaySymbols.zero;
  }

  // initialize variables used for scrolling.
  scrolling = false;
  numOfscrollingFrames = 0;
  scrollingInterval = 300; // milliseconds between frames.
  currentScrollingFrame = 0;
  timeStampFrame = 0;

  // initialize variables used in display loop method.
  timeStampDigit = 0;
  currentDigit = 0; // 0 = first digit.
  previousDigit = NUM_OF_DISPLAY_DIGITS - 1; // index of last digit.
  errorShown = false;
  timeStampError = 0;
  errorDuration = 3000;
  refreshRate = 250; // Hz.
  refreshRateMillis = 1000 / refreshRate;

  // debug.
  Serial.print(F("BUFFER_LENGTH: "));Serial.println(BUFFER_LENGTH);
}

void Seg4DigitHC164::loop()
/*
  - alternates between the digits.
  - overrides output with error message (if necessary).
  - calls scrolling loop method (if necessary).
*/
{
  if (millis() - timeStampDigit >= refreshRateMillis)
  // quickly alternate between digits, using the refresh rate set in init().
  {
    previousDigit = currentDigit; // used to switch off the previous digit.
    currentDigit++;

    if (currentDigit == NUM_OF_DISPLAY_DIGITS)
    {
      // currentDigit is zero indexed. So when currendDigit equals 
      // the number of display digits, the index is pointing one 
      // digit 'outside' of the available display digits and should
      // be reset to index 0.
      
      currentDigit = 0;
    }

    digitalWrite(_digitPins[previousDigit], 0);
    digitalWrite(_digitPins[currentDigit], 1);
    shiftOut(_dataPin, _clockPin, LSBFIRST, currentFrame[currentDigit]);
    timeStampDigit = millis();
  }
  
  if (errorShown) // error overrides scrolling.
  {
    if (millis() - timeStampError > errorDuration)
    {
      removeError();
    }
  }
  else if (scrolling) // call looping method that updates frames.
  {   
    updateScrollingFrame();
  }
}

void Seg4DigitHC164::showInt(int input)
// store input, build input buffer, build and process display buffer.
{ 
  currentInputInt = input;
  buildInputBuffer('i');
  buildDisplayBuffer();
  processDisplayBuffer();
}

void Seg4DigitHC164::showFloat(float input, int decimalPlaces)
// store input, convert to int, build input buffer, build and process display buffer.
{
  currentInputFloat = input;
  buildInputBuffer('f', decimalPlaces);

  // calculate index of digit which should have the decimal point.
  // formula: index of rightmost character in array - decimalPlaces.
  // (index of rightmost character = inputLength - 1).

  int pointIndex = (getInputLength() - 1) - decimalPlaces;
  buildDisplayBuffer(pointIndex);
  processDisplayBuffer();
}

void Seg4DigitHC164::showHex(unsigned long input)
// store input, build input buffer, build and process display buffer.
{ 
  currentInputLong = input;
  buildInputBuffer('h');
  buildDisplayBuffer();
  processDisplayBuffer();
}

void Seg4DigitHC164::showError()
// Store current frame in copy, temporarily set current frame to 'err'.
{
  timeStampError = millis();

  for (int i = 0; i < NUM_OF_DISPLAY_DIGITS; i++)
  {
    currentFrameCopy[i] = currentFrame[i];
  }

  currentFrame[0] = displaySymbols.letter_E;
  currentFrame[1] = displaySymbols.letter_r;
  currentFrame[2] = displaySymbols.letter_r;
  currentFrame[3] = displaySymbols.blank;

  errorShown = true;
}



/*
  -----------------
  PRIVATE METHODS
  -----------------
*/

void Seg4DigitHC164::buildInputBuffer(char outputType, int decimalPlaces) 
// write input to input buffer, using the specified output formatting.
// i int, f float, h hex.
// decimalPlaces = 0 by default (only needed for ouput type float).
{
  int writtenChars = -1;
  Serial.print("outputType: ");Serial.println(outputType);

  switch (outputType)
  {
    case 'i':
      writtenChars = snprintf(inputBuffer, (maxInputLength + 1), "%d", currentInputInt);
      break;
    case 'f':
      {
        long convertedFloat = convertFloatToLong(decimalPlaces);
        writtenChars = snprintf(inputBuffer, (maxInputLength + 1), "%ld", convertedFloat);
        break;
      }
    case 'h':
      writtenChars = snprintf(inputBuffer, (maxInputLength + 1), "%lx", currentInputLong);
      break;
    default:
      // debug
      Serial.println(F("error in Seg4DigitHC164::buildInputBuffer: unknown outputType."));
      break;
  }

  // store current input length.
  currentInputLength = getInputLength();

  // debug
  Serial.print(F("buildInputBuffer() writtenChars: "));Serial.println(writtenChars);

  if (writtenChars < 0)
  {
    // encoding error occured.
    Serial.println(F("error in Seg4DigitHC164::buildInputBuffer(): encoding error."));
  }
}

void Seg4DigitHC164::buildDisplayBuffer(int pointIndex)
// convert inputBuffer (char array) to displayBuffer (bytes representing display symbols).
// pointIndex = -1 by default, only needed when displaying a float.
{
  int i = 0;

  char charToConvert;

  for (i = 0; i < currentInputLength; i++)
  {
    charToConvert = inputBuffer[i];
    displayBuffer[i] = displaySymbols.convertCharToSymbol(charToConvert);
  }

  displayBuffer[currentInputLength] = '\0'; // add null terminator.

  if (pointIndex >= 0) // if input type is float.
  {
    // add decimal point to the digit at the specified index.
    byte digitWithPoint = displaySymbols.addDot(displayBuffer[pointIndex]);
    displayBuffer[pointIndex] = digitWithPoint;
  }
}

void Seg4DigitHC164::processDisplayBuffer()
// check display buffer length, activate scrolling if necessary.
{
  if (currentInputLength > NUM_OF_DISPLAY_DIGITS)
  {
    buildScrollingBuffer();
    scrolling = true;
  }
  else if (currentInputLength >= 0 && currentInputLength <= NUM_OF_DISPLAY_DIGITS)
  {
    updateCurrentFrame();
    scrolling = false;
  }
}

void Seg4DigitHC164::updateCurrentFrame()
/*
    Update the current value the display is showing. If the input length
    is lower than the number of display digits, add blank spaces to
    the left.
*/
{
  int i = 0;
  int blankSpaces = NUM_OF_DISPLAY_DIGITS - currentInputLength;
  byte input;

  if (blankSpaces == 0) // no blank spaces needed.
  {
    for (i = 0; i < NUM_OF_DISPLAY_DIGITS; i++)
    {
      currentFrame[i] = displayBuffer[i];
    }
  }
  else // add blank spaces to the left.
  {
    for (i = 0; i < NUM_OF_DISPLAY_DIGITS; i++)
    {
      if (i - blankSpaces < 0) // insert one blank space.
      {
        input = displaySymbols.blank;
      }
      else // insert symbol from display buffer, adjust index (move to right).
      {
        input = displayBuffer[i - blankSpaces];
      }
      currentFrame[i] = input;
    }
  }

  /* 
    // Alternative option: using a switch case.

    // This results in more readable code, but needs to be changed
    // manually if the class is used for displays with a different 
    // number of digits (other than 4).

  switch (inputLength)
  {
    case 4:
      currentFrame[0] = displayBuffer[0];
      currentFrame[1] = displayBuffer[1];
      currentFrame[2] = displayBuffer[2];
      currentFrame[3] = displayBuffer[3];
      break;
    case 3:
      currentFrame[0] = displaySymbols.blank;
      currentFrame[1] = displayBuffer[0];
      currentFrame[2] = displayBuffer[1];
      currentFrame[3] = displayBuffer[2];
      break;
    case 2:
      currentFrame[0] = displaySymbols.blank;
      currentFrame[1] = displaySymbols.blank;
      currentFrame[2] = displayBuffer[0];
      currentFrame[3] = displayBuffer[1];
      break;
    case 1:
      currentFrame[0] = displaySymbols.blank;
      currentFrame[1] = displaySymbols.blank;
      currentFrame[2] = displaySymbols.blank;
      currentFrame[3] = displayBuffer[0];
      break;
    default:
      break;
  }
  */
}

void Seg4DigitHC164::buildScrollingBuffer()
// add blank spaces to displayBuffer needed for animation effect.
{
  int i = 0;

  int spacesBefore = NUM_OF_DISPLAY_DIGITS - 1; // start with one visible symbol.
  int spacesAfter = NUM_OF_DISPLAY_DIGITS; // end with blank display.
  int scrollingLength = spacesBefore + currentInputLength + spacesAfter;

  // copy input to a temporary array.
  byte inputCopy[currentInputLength];
  for (i = 0; i < currentInputLength; i++)
  {
    inputCopy[i] = displayBuffer[i];
  }

  // add blank spaces at the beginning of display buffer.
  for (i = 0; i < spacesBefore; i++)
  {
    displayBuffer[i] = displaySymbols.blank;
  }

  // add input from copy.
  for (i; i < scrollingLength - spacesAfter; i++)
  {
    displayBuffer[i] = inputCopy[i - spacesBefore];
  }

  // add ending spaces.
  for (i; i < scrollingLength; i++)
  {
    displayBuffer[i] = displaySymbols.blank;
  }

  // calculate number of frames in the scrolling animation.
  numOfscrollingFrames = scrollingLength - NUM_OF_DISPLAY_DIGITS + 1;
}

void Seg4DigitHC164::updateScrollingFrame()
{
  if (millis() - timeStampFrame >= scrollingInterval)
  {
    int i = 0;

    for (i = 0; i < NUM_OF_DISPLAY_DIGITS; i++)
    {
      currentFrame[i] = displayBuffer[i + currentScrollingFrame];
    }

    timeStampFrame = millis();

    currentScrollingFrame++;

    if (currentScrollingFrame == numOfscrollingFrames)
    {
      currentScrollingFrame = 0;
    }
  }
}

void Seg4DigitHC164::removeError()
// restores current frame to the value before the error message.
{
  for (int i = 0; i < NUM_OF_DISPLAY_DIGITS; i++)
  {
    currentFrame[i] = currentFrameCopy[i];
  }

  errorShown = false;
}

long Seg4DigitHC164::convertFloatToLong(int decimalPlaces)
// converts float to long.
{
  int i = 0;
  long convertedInput;

  // bring required decimal places to the left of the decimal point.
  for (i = 0; i < decimalPlaces; i++)
  {
    currentInputFloat *= 10;
  }

  // convert to long, discards everything to the right of the decimal point.
  convertedInput = currentInputFloat;

  return convertedInput;
}

int Seg4DigitHC164::getInputLength()
{
  int i = 0;
  int inputLength;

  for (i = 0; i < BUFFER_LENGTH; i++)
  {
    if (inputBuffer[i] == '\0')
    {
      inputLength = i; // null terminator index equals input length.

      // debug
      Serial.print(F("getInputLength() inputLength: "));Serial.println(inputLength);
      
      break;
    }
  }

  return inputLength;
}