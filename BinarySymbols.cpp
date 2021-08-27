#include <Arduino.h>
#include "../include/BinarySymbols.h"

BinarySymbols::BinarySymbols()
{
  // no intialisation actions necessary.
}

byte BinarySymbols::convertCharToSymbol(char input) {
  // Accepts all numbers, some letters (AbCdEFr), spaces, hyphens.
  switch(input) {
    case ' ':
      return blank;
      break;
    case '0':
      return zero;
      break;
    case ')':
      // 0 with dot.
      return addDot(zero);
      break;
    case '1':
      return one;
      break;
    case '!':
      return addDot(one);
      break;
    case '2':
      return two;
      break;
    case '@':
      return addDot(two);
      break;
    case '3':
      return three;
      break;
    case '#':
      return addDot(three);
      break;
    case '4':
      return four;
      break;
    case '$':
      return addDot(four);
      break;
    case '5':
      return five;
      break;
    case '%':
      return addDot(five);
      break;
    case '6':
      return six;
      break;
    case '^':
      return addDot(six);
      break;
    case '7':
      return seven;
      break;
    case '&':
      return addDot(seven);
      break;
    case '8':
      return eight;
      break;
    case '*':
      return addDot(eight);
      break;
    case '9':
      return nine;
      break;
    case '(':
      return addDot(nine);
      break;
    case 'a':
    case 'A':
      return letter_A;
      break;
    case 'b':
    case 'B':
      return letter_b;
      break;
    case 'c':
    case 'C':
      return letter_C;
      break;
    case 'd':
    case 'D':
      return letter_d;
      break;
    case 'e':
    case 'E':
      return letter_E;
      break;
    case 'f':
    case 'F':
      return letter_F;
      break;
    case 'r':
    case 'R':
      return letter_r;
      break;
    case '-':
      return hyphen;
      break;
    default:
      Serial.print("error in BinarySymbols::convertCharToDisplayDigit(): invalid input char: ");Serial.println(input);
      return zero;
      break;
  }

}

byte BinarySymbols::addDot(byte input)
{
  // set bit that controls dot to 0 to activate led (0 = on).
  return input & 0b11111110;
}