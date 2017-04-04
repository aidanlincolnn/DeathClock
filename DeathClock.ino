/*
 * Death Clock 
 * 3/11/2017
 * Aidan Fowler
 * 
 * Step 1: Input Your Birthday
 * Step 2: Input The Current Date
 * Step 3: Input Your Gender
 * Step 4: Watch Clock Count Down In Minutes Until Your Death
 * End of life: Plays A Song When You Hit 00000000s
 */
 
#include "LedControl.h"
#include "elapsedMillis.h"
#include "binary.h"
#include "pitches.h"

int buttonOne = 2;
int buttonTwo = 3;
int buttonThree = 4;
int brightNess = 15;
long minutes = 0;
boolean boy = true;
unsigned long delaytime=250;
boolean timeSet = false;
boolean firstRun = true;
boolean firstClockSet = true;
boolean playSound = false;
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
  int resetButton = digitalRead(buttonOne);
  if (resetButton == LOW || firstRun) {
    //hold button for 5 seconds to reset clock and re-enter dates
    if(!firstRun){
      delay(5000);
    }
    if(resetButton == LOW || firstRun){ 
      minutes = 0;
      boy = true;
      firstClockSet = true;
      minutesLived = 0;
      brightNess = 15;
      playSound = false;
      firstRun = false;
      enterDates();
      enterSex();
      calculateExpectancy(); 
    }
  }
 
  adjustDisplayBrightness(); 
  updateTime();
  delay(250);
}

void adjustDisplayBrightness(){
  if(digitalRead(buttonTwo)==LOW && digitalRead(buttonThree)==LOW){
    if(brightNess == -5){
      lc.setDigit(0, 7, (byte) (int) ((minutes / 10000000) % 10), false);
      lc.setDigit(0, 6, (byte) (int) ((minutes / 1000000) % 10), false);
      lc.setDigit(0, 5, (byte) (int) ((minutes / 100000) % 10), false);
      lc.setDigit(0, 4, (byte) (int) ((minutes / 10000) % 10), false);
      lc.setDigit(0, 3, (byte) (int) ((minutes / 1000) % 10), false);
      lc.setDigit(0, 2, (byte) (int) ((minutes / 100) % 10), false);
      lc.setDigit(0, 1, (byte) (int) ((minutes / 10) % 10), false);
      lc.setDigit(0, 0, (byte) (int) (minutes % 10), false);
      brightNess = 20;
    }
    brightNess -= 5;
    if(brightNess == -5){
      lc.setRow(0,0,B00000000);
      lc.setRow(0,1,B00000000);
      lc.setRow(0,2,B00000000);
      lc.setRow(0,3,B00000000);
      lc.setRow(0,4,B00000000);
      lc.setRow(0,5,B00000000);
      lc.setRow(0,6,B00000000);
      lc.setRow(0,7,B00000000);
    }
    Serial.println(brightNess);
    lc.setIntensity(0, brightNess); // Set brightness level (0 is min, 15 is max)
    delay(250);
  }
}

void updateTime() {
  if(minutes>0){
    if((timeElapsed / 60000) == 1 || firstClockSet){
      firstClockSet = false;
      minutes--;
      timeElapsed = 0;
      lc.setDigit(0, 7, (byte) (int) ((minutes / 10000000) % 10), false);
      lc.setDigit(0, 6, (byte) (int) ((minutes / 1000000) % 10), false);
      lc.setDigit(0, 5, (byte) (int) ((minutes / 100000) % 10), false);
      lc.setDigit(0, 4, (byte) (int) ((minutes / 10000) % 10), false);
      lc.setDigit(0, 3, (byte) (int) ((minutes / 1000) % 10), false);
      lc.setDigit(0, 2, (byte) (int) ((minutes / 100) % 10), false);
      lc.setDigit(0, 1, (byte) (int) ((minutes / 10) % 10), false);
      lc.setDigit(0, 0, (byte) (int) (minutes % 10), false);
      if(minutes == 0 || playSound == true){
        minutes = 0;
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
      delay(delaytime*2);
      first = false;
    }
    int enterButton = digitalRead(buttonOne);
    int upButton = digitalRead(buttonTwo);
    int downButton = digitalRead(buttonThree);
    
    if (enterButton == LOW) {
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
  birth();
  int month = getNumberInput(6, "Month");
  int day = getNumberInput(15, "Day");
  int year = getNumberInput(1990, "Year");
  
  lc.clearDisplay(0);
  delay(delaytime * 2);
  updateYearDisplay(year,month,day);
  delay(delaytime * 8);
  lc.clearDisplay(0);
  delay(delaytime * 2);

  today();
  int month2 = getNumberInput(6, "Month");
  int day2 = getNumberInput(15, "Day");
  int year2 = getNumberInput(2017, "Year");
  if(year2 == 2000 && month2 ==1 && day2 == 1){
    playSound = true;
  }

  lc.clearDisplay(0);
  delay(delaytime * 2);
  updateYearDisplay(year2,month2,day2);
  delay(delaytime * 8);
  lc.clearDisplay(0);
  delay(delaytime * 2);

  long yearDiff = year2-year;
  long monthDiff = month2-month;
  long dayDiff = day2-day;
  minutesLived = (long) (yearDiff*365 + monthDiff*30.35 + dayDiff)*24*60;
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
      int startButton = digitalRead(buttonOne);
      int upButton = digitalRead(buttonTwo);
      int downButton = digitalRead(buttonThree);
      
      if (startButton == LOW) {
        lc.clearDisplay(0);
        enterSex = false;
        break;
      }
      else if (upButton == LOW || downButton == LOW) {
        boy = !boy;
        updateGenderDisplay();
      }
      delay(50);
    }
}

void updateGenderDisplay(){
  lc.clearDisplay(0);
  lc.setDigit(0,7,5,false);
  lc.setChar(0,6,'E',false);
  lc.setRow(0,5,B00110111);
  lc.setRow(0,4,B00000001);
  if(boy){
    lc.setRow(0,1,B00110111);
    lc.setRow(0,0,B00111011);
  }
  else{
    lc.setRow(0,1,B00110111);
    lc.setRow(0,0,B00110111);
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
  //minimum give the clock 1 year of minutes
  if (minutes < 0 || minutesLived < 0){
    minutes = 525600;
  }
  timeElapsed = 0;
}

void birth(){
  Serial.println("birth");
  lc.clearDisplay(0);
  lc.setRow(0,7,B00011111);
  lc.setRow(0,6,B00110000);
  lc.setRow(0,5,B01000110);
  lc.setRow(0,4,B00001111);
  lc.setRow(0,3,B00010111);
  lc.setRow(0,2,B00000001);
  lc.setRow(0,1,B00000000);
  lc.setRow(0,0,B00000000);
  delay(delaytime * 8);
  lc.clearDisplay(0);
}

void today(){
  lc.clearDisplay(0);
  lc.setRow(0,7,B00001111);
  lc.setRow(0,6,B00011101);
  lc.setRow(0,5,B00111101);
  lc.setRow(0,4,B01111101);
  lc.setRow(0,3,B00111011);
  lc.setRow(0,2,B00000001);
  lc.setRow(0,1,B00000000);
  lc.setRow(0,0,B00000000);
  delay(delaytime * 8);
  lc.clearDisplay(0); 
}


void youreDead() {
  lc.clearDisplay(0);
  wasIt();
  int noteLengths[] = {
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 1, 4, 4, 1,
    4, 4, 2, 4, 2, 4, 3, 6, 5, 4, 4
  };

  int song[] = {
    NOTE_C4, NOTE_C4, NOTE_E5, NOTE_G5, NOTE_G5, 0, NOTE_G6, NOTE_G6,0, NOTE_E6, NOTE_E6,0,
    NOTE_C4, NOTE_C4, NOTE_E5, NOTE_G5, NOTE_G5, 0, NOTE_G6, NOTE_G6,0, NOTE_F6, NOTE_F6,0,
    NOTE_B4, NOTE_B4, NOTE_D5, NOTE_A6, NOTE_A6, 0, NOTE_A7, NOTE_A7,0, NOTE_F6, NOTE_F6,0,
    NOTE_B4, NOTE_B4, NOTE_D5, NOTE_A6, NOTE_A6, 0, NOTE_A7, NOTE_A7,0, NOTE_E6, NOTE_E6,0,
    NOTE_C4, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, 0, NOTE_C7, NOTE_C7,0, NOTE_G6, NOTE_G6,0,
    NOTE_C4, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, 0, NOTE_C7, NOTE_C7,0, NOTE_A6, NOTE_A6,0,
    NOTE_D4, NOTE_D4, NOTE_F4, NOTE_A5, NOTE_A5, NOTE_FS4, NOTE_G4,NOTE_E6,
    NOTE_C5, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_A5, NOTE_G4,NOTE_C4, 0,NOTE_C4, NOTE_C4, NOTE_C4
  };

  for (int currenNote = 0; currenNote < 91; currenNote++) {
    if (currenNote == 18){
      worth();
    }
    else if (currenNote == 30){
      it();
    }
    else if (currenNote == 42){
      question();
    }
    else if(currenNote == 88 || currenNote == 89){
      flicker();
    }
    else if(currenNote == 90){
      off();
    }
    int noteLength = 1000 / noteLengths[currenNote];
    tone(10, song[currenNote], noteLength);
    delay(noteLength * 1.25);
    noTone(10);
  }
}

void wasIt(){
    lc.clearDisplay(0);
    lc.setRow(0,7,B00011110);
    lc.setRow(0,6,B00111100);
    lc.setRow(0,5,B01111101);
    lc.setRow(0,4,B01011011);
    lc.setRow(0,3,B00000000);
    lc.setRow(0,2,B00110000);
    lc.setRow(0,1,B00001111);
    lc.setRow(0,0,B00000000);
}

void worth(){
    lc.clearDisplay(0); 
    lc.setRow(0,7,B00011110);
    lc.setRow(0,6,B00111100);
    lc.setRow(0,5,B00011101);
    lc.setRow(0,4,B01000110);
    lc.setRow(0,3,B00001111);
    lc.setRow(0,2,B00010111);
    lc.setRow(0,1,B00000000);
    lc.setRow(0,0,B00000000);
}

void it(){
    lc.clearDisplay(0); 
    lc.setRow(0,7,B00110000);
    lc.setRow(0,6,B00001111);
    lc.setRow(0,5,B00000000);
    lc.setRow(0,4,B00000000);
    lc.setRow(0,3,B00000000);
    lc.setRow(0,2,B00000000);
    lc.setRow(0,1,B00000000);
    lc.setRow(0,0,B00000000);
}

void question(){
    lc.clearDisplay(0); 
    lc.setRow(0,7,B00000000);
    lc.setRow(0,6,B00000000);
    lc.setRow(0,5,B00000000);
    lc.setRow(0,4,B00000000);
    lc.setRow(0,3,B00000000);
    lc.setRow(0,2,B00000000);
    lc.setRow(0,1,B10000000);
    lc.setRow(0,0,B01100101);
}

void flicker(){
    lc.clearDisplay(0); 
    delay(50);
    question();
    delay(50);
    lc.clearDisplay(0);
    question();
}

void off(){
    lc.clearDisplay(0); 
    lc.setRow(0,7,B00000000);
    lc.setRow(0,6,B00000000);
    lc.setRow(0,5,B00000000);
    lc.setRow(0,4,B00000000);
    lc.setRow(0,3,B00000000);
    lc.setRow(0,2,B00000000);
    lc.setRow(0,1,B00000000);
    lc.setRow(0,0,B00000000);
}


