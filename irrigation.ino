#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins

const byte pinLcdRs = 2;
const byte pinLcdE = 3;
const byte pinLcdD4 = 4;
const byte pinLcdD5 = 5;
const byte pinLcdD6 = 6;
const byte pinLcdD7 = 7;
LiquidCrystal lcd(pinLcdRs, pinLcdE, pinLcdD4, pinLcdD5, pinLcdD6, pinLcdD7);

const byte numIrrigationSections = 3; // Number of irrigation sections
byte currentIrrigationSection = 0;

const byte pinSectionSwitch = 8;
byte stateSectionSwitch = 0; 

const byte pinOverrideSwitch = 9;
byte stateOverrideSwitch = 0; 
bool isOverrideActive = false;

const byte pinLed = 13;

const int delayButtons = 250;

void setup(){
  // set up the LCD’s number of columns and rows:
  lcd.begin(16, 2);
  
  // initialize the pushbutton pin as an input:
  pinMode(pinSectionSwitch, INPUT);

  updateDisplay();
}

 

void loop(){
  if (!isOverrideActive){
    stateSectionSwitch = digitalRead(pinSectionSwitch);
    if (stateSectionSwitch == HIGH){
      switchIrrigationSection();
    }
  }

  stateOverrideSwitch = digitalRead(pinOverrideSwitch);
  if (stateOverrideSwitch == HIGH){
    toggleOverride();
  }
  

}

void switchLed(bool state){
  if (state) {
    digitalWrite(pinLed, HIGH);
  } else {
    digitalWrite(pinLed, LOW);
  }
}

void switchIrrigationSection(){
  currentIrrigationSection++;
  currentIrrigationSection%=numIrrigationSections;
  updateDisplay();
  delay(delayButtons);
}

void toggleOverride(){
  isOverrideActive = !isOverrideActive;
  switchLed(isOverrideActive);
  delay(delayButtons);
  updateDisplay();
}

void updateDisplay(){
  lcd.clear();
  String displayRow1 = String("");
  String displayRow2 = String("");

  if (isOverrideActive){
    displayRow1 = String("OVERRIDE ACTIVE!");
  } else {
    displayRow1 = String("Section ") + String(currentIrrigationSection+1);
  }
  
  
  lcd.setCursor(0, 0);
  lcd.print(displayRow1);
  lcd.setCursor(0, 1);
  lcd.print(displayRow2);
}
