#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins

LiquidCrystal lcd(2, 3, 8, 9, 10, 11); /// REGISTER SELECT PIN,ENABLE PIN,D4 PIN,D5 PIN, D6 PIN, D7 PIN

const byte numIrrigationSections = 3; // Number of irrigation sections
byte currentIrrigationSection = 0;

const byte pinSectionSwitch = 5;
byte stateSectionSwitch = 0; 

const byte pinLed = 13;

void setup(){
  // set up the LCD’s number of columns and rows:
  lcd.begin(16, 2);
  
  // initialize the pushbutton pin as an input:
  pinMode(pinSectionSwitch, INPUT);

  updateIrrigationSection();
}

 

void loop(){

  stateSectionSwitch = digitalRead(pinSectionSwitch);
  if (stateSectionSwitch == HIGH){
    switchIrrigationSection();
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
  updateIrrigationSection();
  delay(250);
}

void updateIrrigationSection(){
  String displayRow1 = String("Sektion ") + String(currentIrrigationSection+1);
  
  lcd.setCursor(0, 0); // set the cursor to column 0, line 2
  lcd.print(displayRow1);
}
