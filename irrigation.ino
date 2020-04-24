// Includes -------------------------------------------------------------------------------------
#include <LiquidCrystal.h>
#include <Encoder.h>
#include <EEPROM.h>


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

// Relay Board
int pinsRelays[] = {19, 20, 21, 22};

// Section control
const byte numIrrigationSections = 4; // Number of irrigation sections
byte currentIrrigationSection = 0;
const byte pinSectionSwitch = 9;
byte stateSectionSwitch = 0;

// Save settings button
const byte pinSaveSettingsSwitch = 12;
byte stateSaveSettingsSwitch = 0;

// Override button
const byte pinOverrideSwitch = 8;
byte stateOverrideSwitch = 0;
bool isOverrideActive = false;

// Target value control
byte irrigationSectionTargets[numIrrigationSections];
const byte pinTargetValueRotaryEncoderDt = 3;
const byte pinTargetValueRotaryEncoderClk = 2;
Encoder targetValueRotaryEncoder(pinTargetValueRotaryEncoderClk, pinTargetValueRotaryEncoderDt);
long posTargetValueRotaryEncoderOld = -999;
long posTargetValueRotaryEncoderNew = -999;

//Moisture sensors
int pinsSensors[] = {23, 24, 25, 26};
int moistureValues[numIrrigationSections];
int readEveryNthIteration = 100;
int sensorRawMin = 190;
int sensorRawMax = 700;

// Miscellaneous variables and constants
const int delayButtons = 250;
const int delayLoop = 10;
int iLoop = 0;

// Setup routine -------------------------------------------------------------------------------
void setup() {
  lcd.begin(numLcdColumns, numLcdRows);

  pinMode(pinSectionSwitch, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  for (byte iRelayPin = 0; iRelayPin < 4; iRelayPin++) {
    pinMode(pinsRelays[iRelayPin], OUTPUT);
  }

  readEeprom();
  readMoistureSensors();
  updateDisplay();
}

// Main loop -----------------------------------------------------------------------------------
void loop() {
  if (!isOverrideActive) {
    checkSectionSwitch();
    checkSaveSettingsSwitch();
    updateTargetValueRotaryEncoder();

    if (!(iLoop % readEveryNthIteration)) {
      readMoistureSensors();
      setRelays();
    }
  }

  checkOverrideSwitch();


  delay(delayLoop);
  iLoop++;
}

// other routines
void readEeprom() {
  for (byte iAdress = 0; iAdress < numIrrigationSections; iAdress++) {
    irrigationSectionTargets[iAdress] = EEPROM.read(iAdress);
  }
}

void updateEeprom() {
  for (byte iAdress = 0; iAdress < numIrrigationSections; iAdress++) {
    EEPROM.update(iAdress, irrigationSectionTargets[iAdress]);
  }
}

void checkOverrideSwitch() {
  stateOverrideSwitch = digitalRead(pinOverrideSwitch);
  if (stateOverrideSwitch == HIGH) {
    toggleOverride();
  }
}

void checkSaveSettingsSwitch() {
  stateSaveSettingsSwitch = digitalRead(pinSaveSettingsSwitch);
  if (stateSaveSettingsSwitch == HIGH) {
    updateEeprom();
    digitalWrite(LED_BUILTIN, HIGH);
    delay(delayButtons);
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void checkSectionSwitch() {
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

    if (irrigationSectionTargets[currentIrrigationSection] == 101) {
      irrigationSectionTargets[currentIrrigationSection] = 100;
    }

    if (irrigationSectionTargets[currentIrrigationSection] == 255) {
      irrigationSectionTargets[currentIrrigationSection] = 0;
    }

    posTargetValueRotaryEncoderOld = posTargetValueRotaryEncoderNew;
    updateDisplay();
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
  delay(delayButtons);
  updateDisplay();
}

void updateDisplay() {
  lcd.clear();
  char displayRow1[numLcdColumns];
  char displayRow2[numLcdColumns];

  if (isOverrideActive) {
    sprintf(displayRow1, "!!!TESTMODUS!!!");
    sprintf(displayRow2, "");
  } else {
    sprintf(displayRow1, "Sektion %d", currentIrrigationSection + 1);
    sprintf(displayRow2, "Soll:%3d Ist:%3d", irrigationSectionTargets[currentIrrigationSection], moistureValues[currentIrrigationSection]);
  }

  lcd.setCursor(0, 0);
  lcd.print(displayRow1);
  lcd.setCursor(0, 1);
  lcd.print(displayRow2);
}

void readMoistureSensors() {
  bool doDisplayUpdate = false;

  for (byte iSensor = 0; iSensor < numIrrigationSections; iSensor++) {
    //int newValue = analogRead(A0+iSensor);
    int newValue = (1 - (analogRead(pinsSensors[iSensor]) - sensorRawMin) / float(sensorRawMax - sensorRawMin)) * 100.0;
    if (newValue != moistureValues[iSensor]) {
      moistureValues[iSensor] = newValue;
      doDisplayUpdate = true;
    }
  }

  if (doDisplayUpdate) {
    updateDisplay();
  }
}

void setRelays() {
  for (byte iRelay = 0; iRelay < 4; iRelay++) {
    if (irrigationSectionTargets[iRelay] > moistureValues[iRelay]) {
      digitalWrite(pinsRelays[iRelay], HIGH);
    } else {
      digitalWrite(pinsRelays[iRelay], LOW);
    }
  }
}
