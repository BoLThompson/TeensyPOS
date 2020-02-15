#include "global.h"
#include "keypad.h"
#include "SdFs.h"

namespace {
  void (*drawRoutine)();
  void (*buttonRoutine)(uint8_t);

  #define DRAWERPIN 8
  #define SCREENSAVER_TIMEOUT 30000000

  IntervalTimer drawerTimer;
  IntervalTimer screenTimer;

  volatile bool drawerIsOpen;
};

//SD CARD CARGO CODE=========================================================
#define SD_FAT_TYPE 3

const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
#define SD_CONFIG SdioConfig(FIFO_SDIO)
SdFs sd;
FsFile file;

//CARGO CODE TO GET THE SCREEN RUNNING=======================================
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

  display.setCursor(0,SCREEN_HEIGHT-(LINE_HEIGHT*2));  //additional button display
  display.setTextSize(1);
  display.println(F("Press D to Continue"));
  display.println(F("# for Backspace"));

  display.display();
}

void drawSDError() {
  display.clearDisplay();

  display.setTextColor(WHITE);        // instructions
  display.setTextWrap(false);
  display.setCursor(0,LINE_HEIGHT*2);
  display.setTextSize(2);
  display.println(F("ERROR!"));
  display.println(F("SD CARD?"));

  display.display();
}

void drawDrawer() {
  display.clearDisplay();

  display.setTextColor(WHITE);        // instructions
  display.setTextWrap(false);
  display.setCursor(0,LINE_HEIGHT*2);
  display.setTextSize(2);
  display.println(F("HOLLA HOLLA"));
  display.println(F("$GET DOLLA$"));

  display.display();
}

void drawTypeEntry() {
  display.clearDisplay();

  display.setTextColor(WHITE);        // instructions
  display.setTextWrap(false);
  display.setCursor(0,LINE_HEIGHT*2);
  display.setTextSize(2);
  display.println(F("A:Sale"));
  display.println(F("B:Refund"));

  display.display();
}

void drawDocEntry() {
  display.clearDisplay();

  display.setTextColor(WHITE);        // instructions
  display.setTextWrap(false);
  display.setCursor(0,0);
  display.setTextSize(1);
  display.println(F("Enter Document ID:"));

  display.setCursor(0,(LINE_HEIGHT*1.5));
  display.setTextSize(2);

  if (Entry::docType == SA) display.print(F("SA#"));
  else display.print(F("I#"));

  display.println(Entry::docId,DEC); //print the cash amount

  display.setCursor(0,SCREEN_HEIGHT-(LINE_HEIGHT*4));  //additional button display
  display.setTextSize(1);
  display.println(F("A: Invoice"));
  display.println(F("B: Service Agreement"));
  display.println(F("Press D to Continue"));
  display.println(F("# for Backspace"));

  display.display();
}

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

void drawScreenSaver() {
  display.clearDisplay();
  display.display();
}

//LOGICAL SHIT AND ARITHMETIC MANIPULATION===================================
//false if shift would cause rollover
//true if successful
bool decimalShiftLeft(uint32_t& input) {
  uint32_t newVal = input;

    for (byte i = 0; i < 9; i++) {
      if ((newVal+input) < input) return false;
      newVal += input;
    }

    input = newVal;
    return true;
}

//returns false if another digit would cause rollover
//otherwise enters that digit and returns true
bool attemptDigitEntry(uint32_t& input, uint8_t val) {
  if (decimalShiftLeft(input)) {
    input+=val;
    return true;
  }
  else return false;
}

//I'm pretty sure this will never return false but w/e
bool attemptDigitRemoval(uint32_t& input) {
  if (input < 1) return false;
  input/=10;
  return true;
}

//BUTTON ROUTINES===========================================================
void cashEntry(uint8_t);
void docEntry(uint8_t);
void employeeEntry(uint8_t);
void typeEntry(uint8_t);
void screenSaver();
void reset();
bool saveEntry();
void openDrawer();

void cashEntry(uint8_t button) {
  if (button < 10) {  //if it's a number
    attemptDigitEntry(Entry::cash, button); //we do that to the cash value
  }

  else if (button == bPound) {
    attemptDigitRemoval(Entry::cash);
  }

  else if (button == bD) {
    drawRoutine = &drawDocEntry;
    buttonRoutine = &docEntry;
  }
}

void docEntry(uint8_t button) {
  if (button < 10) {
    attemptDigitEntry(Entry::docId, button);
  }

  else if (button == bPound) {
    attemptDigitRemoval(Entry::docId);
  }

  else if (button == bA) {
    Entry::docType = Invoice;
  }

  else if (button == bB) {
    Entry::docType = SA;
  }

  else if (button == bD) {
    saveEntry();    //save all this shit to the SD card
    openDrawer();   //open the cash drawer
  }
}

void employeeEntry(uint8_t button) {
  if (button < 10) {
    attemptDigitEntry(Entry::employee, button);
  }

  else if (button == bPound) {
    attemptDigitRemoval(Entry::employee);
  }

  else if (button == bD) {
    drawRoutine = &drawTypeEntry;
    buttonRoutine = typeEntry;
  }
}

void typeEntry(uint8_t button) {
  if (button == bA) {
    Entry::entryType = Sale;
    drawRoutine = &drawCashEntry;
    buttonRoutine = &cashEntry;
  }
  if (button == bB) {
    Entry::entryType = Refund;
    drawRoutine = &drawCashEntry;
    buttonRoutine = &cashEntry;
  }
}

void screenSaver(uint8_t button) {
  screenTimer.begin(reset, SCREENSAVER_TIMEOUT);
  drawRoutine = &drawEmployeeEntry;
  buttonRoutine = &employeeEntry;
}

//PROGRAM CONTROL===========================================================
void setup() {

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    while(true); // Don't proceed, loop forever
  }

  if (!sd.begin(SD_CONFIG)) {
    drawSDError();
    while(true);
  }

  pinMode(DRAWERPIN,OUTPUT);
  digitalWriteFast(DRAWERPIN,LOW);

  Keypad::init();

  //TODO: settable time?

  reset();
}

void loop() {
  if (Keypad::getButton(buttonRoutine)) {
    screenTimer.end();
    screenTimer.begin(reset, SCREENSAVER_TIMEOUT);
  }
  (drawRoutine)();      //pointer for drawing the screen
}

void reset() {
  Entry::init();

  buttonRoutine = &screenSaver;
  drawRoutine = &drawScreenSaver;
}

bool saveEntry() {
  if (!file.open("Ledger.csv", FILE_WRITE)) {
    //TODO: handle file errors
    return(false);
  }

  #define SPACER F(",")

  //TODO: date
  file.print(F("timestamp"));
  file.print(SPACER);

  file.print(Entry::employee,DEC);
  file.print(SPACER);

  if (Entry::entryType == Refund) file.print(F("Refund"));
  else if (Entry::entryType == Sale) file.print(F("Sale")); 
  file.print(SPACER);

  file.print(F("$"));
  uint32_t cashDollars = (Entry::cash)/100; //calculate dollars and cents
  uint8_t cashCents = (Entry::cash)-(cashDollars*100);
  file.print(cashDollars,DEC); //print the cash amount
  file.print(F("."));
  if (cashCents < 10) display.print(F("0"));
  file.print(cashCents,DEC);
  file.print(SPACER);

  if (Entry::docType == SA) file.print(F("SA"));
  else if (Entry::docType == Invoice) file.print(F("Invoice"));
  file.print(SPACER);

  file.println(Entry::docId,DEC);

  file.close();
  return(true);
}

void drawerInterrupt() {
  if (!drawerIsOpen) {
    digitalWriteFast(DRAWERPIN,HIGH);
    drawerIsOpen=true;
  }
  else {
    drawerTimer.end();
    digitalWriteFast(DRAWERPIN,LOW);
    drawerIsOpen=false;
    reset();
  }
}

void openDrawer() {
  drawerIsOpen = false;
  drawerTimer.end();
  drawerTimer.begin(drawerInterrupt, 1000000);
  drawerInterrupt();

  drawRoutine = &drawDrawer;
  buttonRoutine = NULL;
}