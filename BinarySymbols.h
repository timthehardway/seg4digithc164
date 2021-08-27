/*
  BinarySymbols.h - Library containing byte characters created
  for a common anode segment led display.

  Bit to led segment mapping used:
       ____
      |  2 |
    3 |____| 7
      |  1 |
    5 |____| 6  [] 8
        4
  
  Most significant bit first.
  Common anode: 0 = led on, 1 = led off.
  
  Example: 
  byte 00101001 results in '3'.

  Included characters:
  01234567890
  AbCdEFr-
  (all without the dot).
  
  Floats:
  To add a dot behind a number, use
  the following alternative input
  characters for numbers 0-9:
  ! @ # $ % ^ & * ( ) which equal
  1 2 3 4 5 6 7 8 9 0 with dot.

  This gives the user enough options to
  show decimal, hexadecimal and 
  letters for showing Celcius or
  Fahrenheit and 'Err' as an
  error symbol.
  
  Created 04-06-2021 by Tim Ruterink.
  For study purposes.
*/

#ifndef BINARYSYMBOLS_H
#define BINARYSYMBOLS_H

#include <Arduino.h>

class BinarySymbols {
  private:
  
  public:
    BinarySymbols();

    const byte blank = 0b11111111;
    const byte zero = 0b10000001;
    const byte one = 0b11111001;
    const byte two = 0b00100101;
    const byte three = 0b00101001;
    const byte four = 0b01011001;
    const byte five = 0b00001011;
    const byte six = 0b00000011;
    const byte seven = 0b10111001;
    const byte eight = 0b00000001;
    const byte nine = 0b00001001;

    const byte hyphen = 0b01111111;

    const byte letter_A = 0b00010001;
    const byte letter_b = 0b01000011;
    const byte letter_C = 0b10000111;
    const byte letter_d = 0b01100001;
    const byte letter_E = 0b00000111;
    const byte letter_F = 0b00010111;
    const byte letter_r = 0b01110111;

    byte convertCharToSymbol(char input);

    byte addDot(byte input);
};

#endif
