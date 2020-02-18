#ifndef KEYPAD_H
#define KEYPAD_H

#include "global.h"

namespace Keypad {
  #define b0 0
  #define b1 1
  #define b2 2
  #define b3 3
  #define b4 4
  #define b5 5
  #define b6 6
  #define b7 7
  #define b8 8
  #define b9 9
  #define bAsterisk 10
  #define bPound 11
  #define bA 12
  #define bB 13
  #define bC 14
  #define bD 15

  void init();
  void pollButtons();
  bool oneNewButtonPressed();
  bool getButton(void (*callback)(uint8_t));
  uint8_t getButtonIndex();
  uint16_t getButtonState();
};

#endif