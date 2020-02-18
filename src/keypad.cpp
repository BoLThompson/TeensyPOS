#include "keypad.h"

namespace {
  #define PinRow4 0
  #define PinRow3 1
  #define PinRow2 2
  #define PinRow1 3
  #define PinCol4 4
  #define PinCol3 5
  #define PinCol2 6
  #define PinCol1 7

  const byte PinCol[] = {PinCol1, PinCol2, PinCol3, PinCol4};
  const byte PinRow[] = {PinRow1, PinRow2, PinRow3, PinRow4};

  #define KeyPad_1           0B1000000000000000
  #define KeyPad_4           0B0100000000000000
  #define KeyPad_7           0B0010000000000000
  #define KeyPad_asterisk    0B0001000000000000
  #define KeyPad_2           0B0000100000000000
  #define KeyPad_5           0B0000010000000000
  #define KeyPad_8           0B0000001000000000
  #define KeyPad_0           0B0000000100000000
  #define KeyPad_3           0B0000000010000000
  #define KeyPad_6           0B0000000001000000
  #define KeyPad_9           0B0000000000100000
  #define KeyPad_pound       0B0000000000010000
  #define KeyPad_A           0B0000000000001000
  #define KeyPad_B           0B0000000000000100
  #define KeyPad_C           0B0000000000000010
  #define KeyPad_D           0B0000000000000001

  const uint32_t keyNumbers[] = {
    KeyPad_0,
    KeyPad_1,
    KeyPad_2,
    KeyPad_3,
    KeyPad_4,
    KeyPad_5,
    KeyPad_6,
    KeyPad_7,
    KeyPad_8,
    KeyPad_9,
    KeyPad_asterisk,
    KeyPad_pound,
    KeyPad_A,
    KeyPad_B,
    KeyPad_C,
    KeyPad_D,
  };

  uint16_t buttonState;
  uint16_t prevButtonState;
};

void Keypad::init() {
  buttonState = 0;
  prevButtonState = 0;

  for (byte i = 0; i < 4; i++) {
    digitalWriteFast(PinCol[i], LOW);
    pinMode(PinCol[i],OUTPUT);
    pinMode(PinCol[i],HIGH);
    pinMode(PinRow[i],INPUT_PULLUP);
  }
}

void Keypad::pollButtons() {
  prevButtonState = buttonState;
  buttonState = 0;

  for (byte c = 0; c < 4; c++) {
    digitalWriteFast(PinCol[c],LOW);
    delay(1);
    for (byte r = 0; r < 4; r++) {
      buttonState <<= 1;
      if ( digitalReadFast(PinRow[r]) == LOW ) 
        buttonState |= 1;
    }
    digitalWriteFast(PinCol[c],HIGH);
  }
}

bool Keypad::oneNewButtonPressed() {
        // (x xor y) returns a bit pattern of all the flipped bits
            // ((x xor y) and x) returns a bit pattern of all set bits in n which are unset in y
  if (( ( (buttonState^prevButtonState) & buttonState ) > 0 ) //test for freshly pressed buttons
            // ((x-1) and x) == 0 iff x has only one bit set (x is a power of 2)
  and ( ( (buttonState - 1) & buttonState) == 0)) //make sure multiple buttons are not pressed
            //the second test is kind of redundant because the switch statement only handles powers of 2
    return true;
  return false;
}

bool Keypad::getButton(void (*callback)(uint8_t)) {
  if ( ( (buttonState^prevButtonState) & buttonState ) == 0 ) return false;  //test for freshly pressed buttons
  if ( ((buttonState-1) & buttonState) != 0 ) return false; //make sure multiple buttons are not pressed

  if (callback == NULL) return true;

  uint8_t buttonIndex = getButtonIndex();

  (callback)(buttonIndex);

  return true;
}

uint8_t Keypad::getButtonIndex() {
  uint8_t buttonIndex = 0;

  while (keyNumbers[buttonIndex] != buttonState) buttonIndex++;

  return (buttonIndex);
}

uint16_t Keypad::getButtonState() {
  return buttonState;
}