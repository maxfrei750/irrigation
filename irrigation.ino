// Includes -------------------------------------------------------------------------------------
#include <LiquidCrystal.h>
#include <Encoder.h>


// Constants and global variables ---------------------------------------------------------------

// LCD
const byte pinLcdRs = 11;
const byte pinLcdE = 10;
const byte pinLcdD4 = 4;
const byte pinLcdD5 = 5;
const byte pinLcdD6 = 6;
const byte pinLcdD7 = 7;

const byte numLcdRows = 2;
const byte numLcdColumns = 16;
LiquidCrystal lcd(pinLcdRs, pinLcdE, pinLcdD4, pinLcdD5, pinLcdD6, pinLcdD7);

// Section control
const byte numIrrigationSections = 3; // Number of irrigation sections
byte currentIrrigationSection = 0;
const byte pinSectionSwitch = 9;
byte stateSectionSwitch = 0;

// Override button
const byte pinOverrideSwitch = 8;
byte stateOverrideSwitch = 0;
bool isOverrideActive = false;

// Target value control
int irrigationSectionTargets[numIrrigationSections];
const byte pinTargetValueRotaryEncoderDt = 3;
const byte pinTargetValueRotaryEncoderClk = 2;
Encoder targetValueRotaryEncoder(pinTargetValueRotaryEncoderClk, pinTargetValueRotaryEncoderDt);
long posTargetValueRotaryEncoderOld = -999;
long posTargetValueRotaryEncoderNew = -999;

// Miscellaneous variables and constants
const byte pinLed = 13;
const int delayButtons = 250;

// Setup routine -------------------------------------------------------------------------------
void setup() {
  lcd.begin(numLcdColumns, numLcdRows);

  pinMode(pinSectionSwitch, INPUT_PULLUP);

  updateDisplay();
}


// Main loop -----------------------------------------------------------------------------------
void loop() {
  if (!isOverrideActive) {
    checkSectionSwitch();
    updateTargetValueRotaryEncoder();
  }

  checkOverrideSwitch();
}

void checkOverrideSwitch(){
  stateOverrideSwitch = digitalRead(pinOverrideSwitch);
  if (stateOverrideSwitch == HIGH) {
    toggleOverride();
  }
}

void checkSectionSwitch(){
  stateSectionSwitch = !digitalRead(pinSectionSwitch);
  if (stateSectionSwitch) {
    switchIrrigationSection();
  }
}

void updateTargetValueRotaryEncoder()
{
  posTargetValueRotaryEncoderNew = targetValueRotaryEncoder.read();

  if (posTargetValueRotaryEncoderNew != posTargetValueRotaryEncoderOld) {
    if (posTargetValueRotaryEncoderNew > posTargetValueRotaryEncoderOld) {
      irrigationSectionTargets[currentIrrigationSection]++;
    }
    else {
      irrigationSectionTargets[currentIrrigationSection]--;
    }

    if (irrigationSectionTargets[currentIrrigationSection] > 100) {
      irrigationSectionTargets[currentIrrigationSection] = 100;
    }

    if (irrigationSectionTargets[currentIrrigationSection] < 0) {
      irrigationSectionTargets[currentIrrigationSection] = 0;
    }

    posTargetValueRotaryEncoderOld = posTargetValueRotaryEncoderNew;
    updateDisplay();
  }
}

void switchLed(bool state) {
  if (state) {
    digitalWrite(pinLed, HIGH);
  } else {
    digitalWrite(pinLed, LOW);
  }
}

void switchIrrigationSection() {
  currentIrrigationSection++;
  currentIrrigationSection %= numIrrigationSections;
  updateDisplay();
  delay(delayButtons);
}

void toggleOverride() {
  isOverrideActive = !isOverrideActive;
  switchLed(isOverrideActive);
  delay(delayButtons);
  updateDisplay();
}

void updateDisplay() {
  lcd.clear();
  String displayRow1 = String("");
  String displayRow2 = String("");

  if (isOverrideActive) {
    displayRow1 = String("!!!TESTMODUS!!!");
  } else {
    displayRow1 = String("Sektion ") + String(currentIrrigationSection + 1);
    displayRow2 = String("Soll: ") + String(irrigationSectionTargets[currentIrrigationSection]);
  }


  lcd.setCursor(0, 0);
  lcd.print(displayRow1);
  lcd.setCursor(0, 1);
  lcd.print(displayRow2);
}
