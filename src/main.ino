#include <Arduino.h>
#include "SdFs.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define PinRow4 0
#define PinRow3 1
#define PinRow2 2
#define PinRow1 3
#define PinCol4 4
#define PinCol3 5
#define PinCol2 6
#define PinCol1 7

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

enum MainState {
  CashEntry,
};

MainState mainState = CashEntry;

enum EntryType {
  Sale,
  Refund
};

enum DocType {
  SA,
  Invoice,
};

namespace Entry {
  uint8_t employee = 0;
  uint32_t cash = 0;
  EntryType entryType = Sale;
  DocType docType = SA;
  //FIXME time
};


const byte PinCol[] = {PinCol1, PinCol2, PinCol3, PinCol4};
const byte PinRow[] = {PinRow1, PinRow2, PinRow3, PinRow4};
uint16_t buttonState;
uint16_t prevButtonState;

void setup() {

  buttonState = 0;
  prevButtonState = 0;

  for (byte i = 0; i < 4; i++) {
    digitalWriteFast(PinCol[i], LOW);
    pinMode(PinCol[i],INPUT);
    pinMode(PinRow[i],INPUT_PULLUP);
  }

  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    while(true); // Don't proceed, loop forever
  }
}

void loop() {
  pollButtons();
  switch (mainState) {
    case CashEntry:


      if (( ( (buttonState^prevButtonState) & buttonState ) > 0 ) //test for freshly pressed buttons
      and ( ( (buttonState - 1) & buttonState) == 0))//make sure multiple buttons are not pressed
      { 
        switch (buttonState)
        {
        case KeyPad_1:
          cashDigitPress(1);
          break;
        case KeyPad_2:
          cashDigitPress(2);
          break;
        case KeyPad_3:
          cashDigitPress(3);
          break;
        case KeyPad_4:
          cashDigitPress(4);
          break;
        case KeyPad_5:
          cashDigitPress(5);
          break;
        case KeyPad_6:
          cashDigitPress(6);
          break;
        case KeyPad_7:
          cashDigitPress(7);
          break;
        case KeyPad_8:
          cashDigitPress(8);
          break;
        case KeyPad_9:
          cashDigitPress(9);
          break;
        case KeyPad_0:
          cashDigitPress(0);
          break;
        default:
          break;
        }
      }

      drawCashEntry();    
    break;
  }
}

void drawCashEntry() {
  display.clearDisplay();

  display.setTextColor(WHITE);        // Draw white text
  display.setTextWrap(false);
  display.setCursor(0,0);
  display.setTextSize(1);
  display.println(F("Enter Amount of Cash:"));

  display.setCursor(0,24);
  display.setTextSize(2);
  display.print(F("$"));
  
  uint32_t cashDollars = (Entry::cash)/100;
  uint8_t cashCents = (Entry::cash)-(cashDollars*100);

  display.print(cashDollars,DEC);
  display.print(F("."));
  if (cashCents < 10) display.print(F("0"));
  display.println(cashCents,DEC);

  display.setCursor(0,SCREEN_HEIGHT-16);
  display.setTextSize(1);
  display.println(F("Press D to Continue"));
  display.println(F("# for Backspace"));

  display.display();
}


void cashDigitPress(byte input) {
  uint32_t newCash = Entry::cash;

  for (byte i = 0; i < 9; i++) {
    if ((newCash+Entry::cash) < newCash) return;
    newCash+=Entry::cash;
  }


  Entry::cash=newCash;
  Entry::cash+=input;
}


void drawButtonState() {
  display.clearDisplay();

  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,16);             // Start at top-left corner
  display.setTextSize(1);             // Draw 2X-scale text
  display.setTextWrap(false);

  for (byte i = 15; i <255; i--) {
    if ( ( buttonState & _BV(i) ) == 0 ) display.print(F("0")); //display.println(0xDEADBEEF, HEX);
    else display.print(F("1"));
  }

  display.display();
}

void pollButtons() {
  prevButtonState = buttonState;
  buttonState = 0;

  for (byte c = 0; c < 4; c++) {
    pinMode(PinCol[c],OUTPUT);
    delay(10);
    for (byte r = 0; r < 4; r++) {
      buttonState <<= 1;
      if ( digitalReadFast(PinRow[r]) == LOW ) 
        buttonState |= 1;
    }
    pinMode(PinCol[c],INPUT);
  }
}