#include "SdFs.h"
#include "global.h"
#include "keypad.h"

//CARGO CODE TO GET THE SCREEN RUNNING=======================================
// #include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//DRAWING FUNCTIONS==========================================================
void drawEmployeeEntry() {
  display.clearDisplay();

  display.setTextColor(WHITE);        // instructions
  display.setTextWrap(false);
  display.setCursor(0,0);
  display.setTextSize(1);
  display.println(F("Enter Employee ID:"));

  display.setCursor(0,24);  //current entry
  display.setTextSize(2);
  display.print(Entry::employee,DEC);

  display.setCursor(0,SCREEN_HEIGHT-16);  //additional button display
  display.setTextSize(1);
  display.println(F("Press D to Continue"));
  display.println(F("# for Backspace"));

  display.display();
}

void drawTypeEntry() {
  display.clearDisplay();

  display.setTextColor(WHITE);        // instructions
  display.setTextWrap(false);
  display.setCursor(0,0);
  display.setTextSize(2);
  display.println(F("A:Sale"));
  display.println(F("B:Refund"));

  display.display();
}

void drawIdEntry() {
  return;
};

void drawCashEntry() {
  display.clearDisplay();

  display.setTextColor(WHITE);        // instructions
  display.setTextWrap(false);
  display.setCursor(0,0);
  display.setTextSize(1);
  display.println(F("Enter Amount of Cash:"));

  display.setCursor(0,24);  //current entry (dollar sign)
  display.setTextSize(2);
  display.print(F("$"));
  
  uint32_t cashDollars = (Entry::cash)/100; //calculate dollars and cents
  uint8_t cashCents = (Entry::cash)-(cashDollars*100);

  display.print(cashDollars,DEC); //print the cash amount
  display.print(F("."));
  if (cashCents < 10) display.print(F("0"));
  display.println(cashCents,DEC);

  display.setCursor(0,SCREEN_HEIGHT-16);  //additional button display
  display.setTextSize(1);
  display.println(F("Press D to Continue"));
  display.println(F("# for Backspace"));

  display.display();
}
//LOGICAL SHIT AND ARITHMETIC MANIPULATION===================================
bool decimalShiftLeft(uint32_t& input) {
  uint32_t newVal = input;

    for (byte i = 0; i < 9; i++) {
      if ((newVal+input) < input) return false;
      newVal += input;
    }

    input = newVal;
    return true;
}

void attemptDigitEntry(uint32_t& input, uint8_t val) {
  if (decimalShiftLeft(input)) {
    input+=val;
  }
}

//BUTTON ROUTINES===========================================================
void cashEntry(uint8_t button) {
  if (button < 10) {  //if it's a number
    attemptDigitEntry(Entry::cash, button);
  }
}

//PROGRAM CONTROL===========================================================
namespace {
  void (*drawRoutine)();
  void (*buttonRoutine)(uint8_t);
};

void setup() {

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    while(true); // Don't proceed, loop forever
  }

  Entry::init();
  Keypad::init();

  buttonRoutine = &cashEntry;
  drawRoutine = &drawCashEntry;
}

void loop() {
  Keypad::getButton(buttonRoutine);
  (drawRoutine)();      //pointer for drawing the screen
}

// void cashDigitPress(byte input) {
//   uint32_t newCash = Entry::cash;

//   for (byte i = 0; i < 9; i++) {
//     if ((newCash+Entry::cash) < newCash) return;
//     newCash+=Entry::cash;
//   }

//   Entry::cash=newCash;
//   Entry::cash+=input;
// }

// void drawButtonState() {
//   display.clearDisplay();

//   display.setTextColor(WHITE);        // Draw white text
//   display.setCursor(0,16);             // Start at top-left corner
//   display.setTextSize(1);             // Draw 2X-scale text
//   display.setTextWrap(false);

//   for (byte i = 15; i <255; i--) {
//     if ( ( buttonState & _BV(i) ) == 0 ) display.print(F("0")); //display.println(0xDEADBEEF, HEX);
//     else display.print(F("1"));
//   }

//   display.display();
// }