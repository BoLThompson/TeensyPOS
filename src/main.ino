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
  drawButtonState();
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