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
  #define bs 10
  #define bh 11
  #define ba 12
  #define bb 13
  #define bc 14
  #define bd 15

  void init();
  void pollButtons();
  bool oneNewButtonPressed();
  void getButton(void (*callback)(uint8_t));
};

#endif