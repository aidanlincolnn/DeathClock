#include "LedControl.h"
#include "elapsedMillis.h"
#include "binary.h"
#include "pitches.h"

int buttonOne = 2;
int buttonTwo = 3;
int buttonThree = 4;
long minutes = 0;
boolean boy = true;
unsigned long delaytime=250;
boolean firstRun = true;
boolean firstClockSet = true;
long minutesLived = 0;
// Arduino Pin 7 to DIN, 9 to Clk, 8 to LOAD, no.of devices is 1
LedControl lc = LedControl(7, 9, 8, 1);
elapsedMillis timeElapsed;

void setup()
{
  Serial.begin(9600);  
  // Define pin #1,2,3 as input and activate the internal pull-up resistor
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  // Initialize the MAX7219 device
  lc.shutdown(0, false);  // Enable display
  lc.setIntensity(0, 15); // Set brightness level (0 is min, 15 is max)
  lc.clearDisplay(0);     // Clear display register
}

void loop()
{
  int buttonValue = digitalRead(buttonOne);
  if (buttonValue == LOW || firstRun) {
    firstRun = false;
    enterDates();
    enterSex();
    calculateExpectancy(); 
  }
  updateTime();
  delay(250);
}

void updateTime() {
  if(minutes>0){
    if((timeElapsed / 60000) == 1 || firstClockSet){
      firstClockSet = false;
      minutes--;
      timeElapsed = 0;
      lc.setDigit(0, 7, (byte) (minutes / 10000000) % 10, false);
      lc.setDigit(0, 6, (byte) (minutes / 1000000) % 10, false);
      lc.setDigit(0, 5, (byte) (minutes / 100000) % 10, false);
      lc.setDigit(0, 4, (byte) (minutes / 10000) % 10, false);
      lc.setDigit(0, 3, (byte) (minutes / 1000) % 10, false);
      lc.setDigit(0, 2, (byte) (minutes / 100) % 10, false);
      lc.setDigit(0, 1, (byte) (minutes / 10) % 10, false);
      lc.setDigit(0, 0, (byte) minutes % 10, false);
      if(minutes == 0){
        youreDead();
      }
    }
  }
}

int getNumberInput(int field, String refreshType){
  boolean first = true;
  boolean exitLoop = false;

  while (!exitLoop){
    if (first == true){
      if (refreshType == "Day"){
         updateYearDisplay(NULL, NULL, field);
      }
      else if (refreshType == "Month"){
         updateYearDisplay(NULL, field, NULL);
      }
      else if (refreshType == "Year"){
         updateYearDisplay(field, NULL, NULL);
      }
      delay(delaytime*4);
      first = false;
    }
    int startButton = digitalRead(buttonOne);
    int upButton = digitalRead(buttonTwo);
    int downButton = digitalRead(buttonThree);
    
    if (startButton == LOW) {
      exitLoop = true;
    }
    else if (upButton == LOW) {
      field = field + 1;
      if(refreshType == "Day"){
        if (field > 31){
          field = 31;
        }
        updateYearDisplay(NULL, NULL, field);
      }
      else if (refreshType == "Month"){
        if(field > 12){
          field = 12;
        }
        updateYearDisplay(NULL, field, NULL);
      }
      else if (refreshType == "Year"){
        updateYearDisplay(field, NULL, NULL);
      }
    }
    else if (downButton == LOW) {
      field = field - 1;
      if (refreshType == "Day"){
        if(field < 1){
          field = 1;
        }
        updateYearDisplay(NULL, NULL, field);
      }
      else if (refreshType == "Month"){
        if(field < 1){
          field = 1;
        }
        updateYearDisplay(NULL, field, NULL);
      }
      else if (refreshType == "Year"){
        if(field < 1920){
          field = 1916;
        }
        updateYearDisplay(field, NULL, NULL);
      }
    }
    delay(100); 
  }
  return field;
}

void enterDates(){
  int month = getNumberInput(6, "Month");
  int day = getNumberInput(15, "Day");
  int year = getNumberInput(1990, "Year");
  
  lc.clearDisplay(0);
  delay(delaytime * 2);
  updateYearDisplay(year,month,day);
  delay(delaytime * 8);
  lc.clearDisplay(0);
  delay(delaytime * 2);
  
  int month2 = getNumberInput(6, "Month");
  int day2 = getNumberInput(15, "Day");
  int year2 = getNumberInput(2016, "Year");

  lc.clearDisplay(0);
  delay(delaytime * 2);
  updateYearDisplay(year2,month2,day2);
  delay(delaytime * 8);
  lc.clearDisplay(0);
  delay(delaytime * 2);

  int yearDiff = year2-year;
  int monthDiff = month2-month;
  int dayDiff = day2-day;
  minutesLived = (yearDiff*365 + monthDiff*30.35 + dayDiff)*24*60;
  Serial.println(minutesLived);
}


void updateYearDisplay(int year, int month, int day){
  lc.clearDisplay(0);
  if(year!= NULL){
    lc.setDigit(0,3,(byte) (year / 1000) % 10,false);
    lc.setDigit(0,2,(byte) (year / 100) % 10,false);
    lc.setDigit(0,1,(byte) (year / 10) % 10,false);
    lc.setDigit(0,0, year % 10,false);
  }
  if(month != NULL){
    lc.setDigit(0,7,(byte) (month / 10) % 10,false);
    lc.setDigit(0,6,(byte) month % 10,false);
  }
  if(day != NULL){
    lc.setDigit(0,5,(byte) (day / 10) % 10,false);
    lc.setDigit(0,4,(byte) day % 10,false);
  }
  delay(delaytime);
}

void enterSex(){
  updateGenderDisplay();
  boolean enterSex = true;
  
  while(enterSex){
      int buttonValue = digitalRead(buttonOne);
      int button2Value = digitalRead(buttonTwo);
      int button3Value = digitalRead(buttonThree);
      
      if (buttonValue == LOW) {
        lc.clearDisplay(0);
        enterSex = false;
        break;
      }
      else if (button2Value == LOW || button3Value == LOW) {
        boy = !boy;
        updateGenderDisplay();
      }
      delay(50);
    }
}

void updateGenderDisplay(){
  lc.clearDisplay(0);
  lc.setDigit(0,6,8,false);
  lc.setDigit(0,5,0,false);
  lc.setRow(0,4,B00111011);
  if(boy){
    lc.setRow(0,0,B00111011);
  }
  else{
    lc.setRow(0,0,B00010101);
  }
  delay(delaytime);
}

void calculateExpectancy() {
  //woman age in minutes 
  long base = 41860800;
  if(boy){
   base = 39449700;
  }
  minutes = base - minutesLived;
  if (minutes < 0){
    minutes = 2;
  }
  timeElapsed = 0;
}


void youreDead() {
  // note durations: 4 = quarter note, 8 = eighth note, etc.:
  int noteDurations[] = {
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 1, 4, 4, 1,
    4, 4, 2, 4, 2, 4, 3, 6, 5, 4, 4
  };

  // notes in the melody:
  int melody[] = {
    NOTE_C4, NOTE_C4, NOTE_E5, NOTE_G5, NOTE_G5, 0, NOTE_G6, NOTE_G6,0, NOTE_E6, NOTE_E6,0,
    NOTE_C4, NOTE_C4, NOTE_E5, NOTE_G5, NOTE_G5, 0, NOTE_G6, NOTE_G6,0, NOTE_F6, NOTE_F6,0,
    NOTE_B4, NOTE_B4, NOTE_D5, NOTE_A6, NOTE_A6, 0, NOTE_A7, NOTE_A7,0, NOTE_F6, NOTE_F6,0,
    NOTE_B4, NOTE_B4, NOTE_D5, NOTE_A6, NOTE_A6, 0, NOTE_A7, NOTE_A7,0, NOTE_E6, NOTE_E6,0,
    NOTE_C4, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, 0, NOTE_C7, NOTE_C7,0, NOTE_G6, NOTE_G6,0,
    NOTE_C4, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, 0, NOTE_C7, NOTE_C7,0, NOTE_A6, NOTE_A6,0,
    NOTE_D4, NOTE_D4, NOTE_F4, NOTE_A5, NOTE_A5, NOTE_FS4, NOTE_G4,NOTE_E6,
    NOTE_C5, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_A5, NOTE_G4,NOTE_C4, 0,NOTE_C4, NOTE_C4, NOTE_C4
  };
  
  for (int thisNote = 0; thisNote < 91; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(10, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(10);
  }
}


