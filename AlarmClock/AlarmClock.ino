/* ------------------------------------------------------------------- *
*  This project is a part of CSS332 Microcontrollers and Applications. *
*  Project Name: Alarm Clock                                           *
*  Section: 1                                                          *
*  Members: Archawit Changtor     6422770121                           *
*           Pakapon Rattanasrisuk 6422780062                           *
* -------------------------------------------------------------------- */

#include <LiquidCrystal_I2C.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <SoftwareSerial.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include "pitches.h"

// lcd
LiquidCrystal_I2C lcd(0x27,  16, 2);

// real-time clock module
ThreeWire myWire(10, 11, 12); // DAT, CLK, RST
RtcDS1302<ThreeWire> Rtc(myWire);
RtcDateTime now;

// pin numbers
int changePagePin = 2;
int stopAlarmPin = 3;
int changeMelodyPin = 4;
int speakerPin = 8;

// screens
int currentPage = 0;
int timePage = 0;
int setAlarmPage = 1;
int setMelodyPage = 2;
int setAlarmPage2 = 3;
int setMelodyPage2 = 4;
int totalPage = 5;
bool isPageChanged = false;

// alarm 1
int alarmClock[] = {0, 0, 0, 0};
int currentUnit = 0;
int alarmHour = 0;
int alarmMinute = 0;
bool alarmOn = false;
bool isAlarmPlayed = false;

// alarm 2
int alarmClock2[] = {0, 0, 0, 0};
int currentUnit2 = 0;
int alarmHour2 = 0;
int alarmMinute2 = 0;
bool alarmOn2 = false;
bool isAlarmPlayed2 = false;

// keypad
int nextKey = -1;         // '#'
int clearAlarmKey = -2;   // '*'   
int toggleAlarmKey = -3;  // 'D'

// alarm sound
int currentMelody = 0;
int currentMelody2 = 0;
int totalMelody = 4;

// alarm volume
bool settingVolume = false;
int currentVolume = 30;

/* ----------------------------------------- */

/***********
* MELODIES *
************/
// melody 1
int melodyPinkPanther[] = {
  // notes
  // rest means no sound
  REST, REST, REST, NOTE_DS4, 
  NOTE_E4, REST, NOTE_FS4, NOTE_G4, REST, NOTE_DS4,
  NOTE_E4, NOTE_FS4,  NOTE_G4, NOTE_C5, NOTE_B4, NOTE_E4, NOTE_G4, NOTE_B4,   
  NOTE_AS4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_D4, 
  NOTE_E4, REST, REST, NOTE_DS4,
  
  NOTE_E4, REST, NOTE_FS4, NOTE_G4, REST, NOTE_DS4,
  NOTE_E4, NOTE_FS4,  NOTE_G4, NOTE_C5, NOTE_B4, NOTE_G4, NOTE_B4, NOTE_E5,
  NOTE_DS5,   
  NOTE_D5, REST, REST, NOTE_DS4, 
  NOTE_E4, REST, NOTE_FS4, NOTE_G4, REST, NOTE_DS4,
  NOTE_E4, NOTE_FS4,  NOTE_G4, NOTE_C5, NOTE_B4, NOTE_E4, NOTE_G4, NOTE_B4,   
  
  NOTE_AS4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_D4, 
  NOTE_E4, REST,
  REST, NOTE_E5, NOTE_D5, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4,
  NOTE_AS4, NOTE_A4, NOTE_AS4, NOTE_A4, NOTE_AS4, NOTE_A4, NOTE_AS4, NOTE_A4,   
  NOTE_G4, NOTE_E4, NOTE_D4, NOTE_E4, NOTE_E4, NOTE_E4
};
int durationsPinkPanther[] = {
  // note velocity
  2, 4, 8, 8, 
  4, 8, 8, 4, 8, 8,
  8, 8,  8, 8, 8, 8, 8, 8,   
  2, 16, 16, 16, 16, 
  2, 4, 8, 4,
  
  4, 8, 8, 4, 8, 8,
  8, 8,  8, 8, 8, 8, 8, 8,
  1,   
  2, 4, 8, 8, 
  4, 8, 8, 4, 8, 8,
  8, 8,  8, 8, 8, 8, 8, 8,   
  
  2, 16, 16, 16, 16, 
  4, 4,
  4, 8, 8, 8, 8, 8, 8,
  16, 8, 16, 8, 16, 8, 16, 8,   
  16, 16, 16, 16, 16, 2
};
void playPinkPanther() {
  for (int thisNote = 0; thisNote < 88; thisNote++) {
    if(isAlarmPlayed || isAlarmPlayed2){
      
      // compute note duration from current note note velocity
      int noteDuration = 1000 / durationsPinkPanther[thisNote];
      
      // sound current note
      tone(speakerPin, melodyPinkPanther[thisNote], noteDuration);
      
      // set interval between notes
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing:
      noTone(speakerPin);

    }
    else  return;
  }
}

// molody 2
int melodyPirates[] = {
  NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, REST,
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, REST,
  NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, REST,
  NOTE_A4, NOTE_G4, NOTE_A4, REST,
  
  NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, REST,
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, REST,
  NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, REST,
  NOTE_A4, NOTE_G4, NOTE_A4, REST,
  
  NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, REST,
  NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5, REST,
  NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, REST,
  NOTE_E5, NOTE_D5, NOTE_E5, NOTE_A4, REST,
  
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, REST,
  NOTE_D5, NOTE_E5, NOTE_A4, REST,
  NOTE_A4, NOTE_C5, NOTE_B4, NOTE_B4, REST,
  NOTE_C5, NOTE_A4, NOTE_B4, REST,
  
  NOTE_A4, NOTE_A4,
};
int durationsPirates[] = {
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 8,
  
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 8,
  
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 8, 4, 8,
  
  8, 8, 4, 8, 8,
  4, 8, 4, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 4,
  
  4, 8,
};
void playPirates() {
  for (int thisNote = 0; thisNote < 78; thisNote++) {
    if(isAlarmPlayed || isAlarmPlayed2) {
      int noteDuration = 1000 / durationsPirates[thisNote];
      tone(speakerPin, melodyPirates[thisNote], noteDuration);

      // set interval between notes
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing
      noTone(speakerPin);
    }
    else  return;
  }
}

// molody 3
int melodyHarryPotter[] = {
  REST, NOTE_D4,
  NOTE_G4, NOTE_AS4, NOTE_A4,
  NOTE_G4, NOTE_D5,
  NOTE_C5, 
  NOTE_A4,
  NOTE_G4, NOTE_AS4, NOTE_A4,
  NOTE_F4, NOTE_GS4,
  NOTE_D4, 
  NOTE_D4,
  
  NOTE_G4, NOTE_AS4, NOTE_A4,
  NOTE_G4, NOTE_D5,
  NOTE_F5, NOTE_E5,
  NOTE_DS5, NOTE_B4,
  NOTE_DS5, NOTE_D5, NOTE_CS5,
  NOTE_CS4, NOTE_B4,
  NOTE_G4,
  NOTE_AS4,
   
  NOTE_D5, NOTE_AS4,
  NOTE_D5, NOTE_AS4,
  NOTE_DS5, NOTE_D5,
  NOTE_CS5, NOTE_A4,
  NOTE_AS4, NOTE_D5, NOTE_CS5,
  NOTE_CS4, NOTE_D4,
  NOTE_D5, 
  REST, NOTE_AS4,  
  
  NOTE_D5, NOTE_AS4,
  NOTE_D5, NOTE_AS4,
  NOTE_F5, NOTE_E5,
  NOTE_DS5, NOTE_B4,
  NOTE_DS5, NOTE_D5, NOTE_CS5,
  NOTE_CS4, NOTE_AS4,
  NOTE_G4
};
int durationsHarryPotter[] = {
  2, 4,
  4, 8, 4,
  2, 4,
  2, 
  2,
  4, 8, 4,
  2, 4,
  1, 
  4,
  
  4, 8, 4,
  2, 4,
  2, 4,
  2, 4,
  4, 8, 4,
  2, 4,
  1,
  4,
   
  2, 4,
  2, 4,
  2, 4,
  2, 4,
  4, 8, 4,
  2, 4,
  1, 
  4, 4,  
  
  2, 4,
  2, 4,
  2, 4,
  2, 4,
  4, 8, 4,
  2, 4,
  1
};
void playHarryPotter() {
  for (int thisNote = 0; thisNote < 62; thisNote++) {
    if(isAlarmPlayed || isAlarmPlayed2) {
      int noteDuration = 1000 / durationsHarryPotter[thisNote];
      tone(speakerPin, melodyHarryPotter[thisNote], noteDuration);

      // set interval between notes
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing
      noTone(speakerPin);
    }
    else  return;
  }
}

// melody 4
int melodyChristmas[] = {
  NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5,
  NOTE_E5,
  NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5,
  NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5,
  NOTE_D5, NOTE_G5
};
int durationsChristmas[] = {
  8, 8, 4,
  8, 8, 4,
  8, 8, 8, 8,
  2,
  8, 8, 8, 8,
  8, 8, 8, 16, 16,
  8, 8, 8, 8,
  4, 4
};
void playChristmas() {
  for (int thisNote = 0; thisNote < 26; thisNote++) {
    if(isAlarmPlayed || isAlarmPlayed2){
      int noteDuration = 1000 / durationsChristmas[thisNote];
      tone(speakerPin, melodyChristmas[thisNote], noteDuration);

      // set interval between notes
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing
      noTone(speakerPin);
    }
    else return;
  }
}

/* ----------------------------------------- */

/************
* UTILITIES *
*************/
int changeInputFromKeyPadToNumber(char num) {
  if(num == '0') {
    return 0;
  } else if(num == '1'){
    return 1;
  } else if(num == '2'){
    return 2;
  } else if(num == '3'){
    return 3;
  } else if(num == '4'){
    return 4;
  } else if(num == '5'){
    return 5;
  } else if(num == '6'){
    return 6;
  } else if(num == '7'){
    return 7;
  } else if(num == '8'){
    return 8;
  } else if(num == '9'){
    return 9;
  } else if(num == '#'){
    return nextKey;
  } else if(num == '*'){
    return clearAlarmKey;
  }else if(num == 'D'){
    return toggleAlarmKey;
  }
}
String songName(int index) {
  if(index == 0) {
    return "Pink Panther";
  } else if(index == 1) {
    return "Pirates";
  } else if(index == 2) {
    return "Harry Potter";
  } else if(index == 3) {
    return "Merry Christmas";
  }
}

/**********
* DISPLAY *
***********/
void display() {
  // clear the screen when the screen has been changed
  if(isPageChanged) {
    lcd.clear();
    isPageChanged = false;
  }
  // wake up screen
  if(isAlarmPlayed || isAlarmPlayed2) {
    lcd.setCursor(0, 0);
    lcd.print("WAKE UP!!!");
  }
  // screen 1
  else if(currentPage == timePage) {
    lcd.setCursor(0, 0);
    lcd.print("Date: ");
    lcd.print(now.Day());
    lcd.print("/");
    lcd.print(now.Month());
    lcd.print("/");
    lcd.print(now.Year());
    lcd.setCursor(0, 1);
    lcd.print("Time: ");
    if(now.Hour() / 10 == 0)  lcd.print("0");
    lcd.print(now.Hour());
    lcd.print(":");
    if(now.Minute() / 10 == 0)  lcd.print("0");
    lcd.print(now.Minute());
    lcd.print(":");
    if(now.Second() / 10 == 0)  lcd.print("0");
    lcd.print(now.Second());
  } 
  // screen 2
  else if(currentPage == setAlarmPage) {
    // shiw settings and alarm status
    lcd.setCursor(0, 0);
    lcd.print("Settings 1");
    lcd.setCursor(13, 0);
    if(alarmOn) {
      lcd.print("ON ");
    } else {
      lcd.print("OFF");
    }
    // show current alarm
    lcd.setCursor(0, 1);
    lcd.print("Alarm 1: ");
    for(int i = 0; i < 4; i++) {
      if(i == 2) lcd.print(":");
      lcd.print(alarmClock[i]);
    }
  }
  // screen 3
  else if(currentPage == setMelodyPage) {
    // show settings and alarm status
    lcd.setCursor(0, 0);
    lcd.print("Alarm sound 1");
    lcd.setCursor(0, 1);
    lcd.print(songName(currentMelody));
  }
  // screen 4
  else if(currentPage == setAlarmPage2) {
    // shiw settings and alarm status
    lcd.setCursor(0, 0);
    lcd.print("Settings 2");
    lcd.setCursor(13, 0);
    if(alarmOn2) {
      lcd.print("ON ");
    } else {
      lcd.print("OFF");
    }
    // show current alarm
    lcd.setCursor(0, 1);
    lcd.print("Alarm 2: ");
    for(int i = 0; i < 4; i++) {
      if(i == 2) lcd.print(":");
      lcd.print(alarmClock2[i]);
    }
  }
  // screen 5
  else if(currentPage == setMelodyPage2) {
    // shiw settings and alarm status
    lcd.setCursor(0, 0);
    lcd.print("Alarm sound 2");
    lcd.setCursor(0, 1);
    lcd.print(songName(currentMelody2));
  }
}
void handlePageChange() {
  if(!digitalRead(changePagePin)){
    // change current screen from 1->2->3->4->5->1->2->...
    if(!isAlarmPlayed && !isAlarmPlayed2) {
      currentPage++;
      currentPage = currentPage % totalPage;
      isPageChanged = true;
    }
  }
}

/*****************
* ALARM SETTINGS *
******************/
void handleAlarmSettings() {
  if (Serial.available() > 0) {
    char receivedChar = Serial.read();
    // if current page is alarm 1, set the alarm 1
    if(currentPage == setAlarmPage) {
      int num = changeInputFromKeyPadToNumber(receivedChar);
      // confirm
      if(num == nextKey) {
        // save selected time
        alarmHour = alarmClock[0]*10 + alarmClock[1];
        alarmMinute = alarmClock[2]*10 + alarmClock[3];
        // switch on the alarm
        alarmOn = true;
        // reset pointer
        currentUnit = 0;
        // change screen back to main page
        currentPage = timePage;
        isPageChanged = true;
        display();

        return;
      }
      // clear alarm
      else if(num == clearAlarmKey) {
        // reset the alarm to 00.00
        for(int i = 0; i < 4; i++) {
          alarmClock[i] = 0;
        }
        currentUnit = 0;
      }
      // toggle alarm status
      else if(num == toggleAlarmKey) {
        alarmOn = !alarmOn;
      }
      // next
      else {
        // point to next index
        alarmClock[currentUnit] = num;
        if(currentUnit < 3)  currentUnit++;
      }
      Serial.print("index: ");
      Serial.println(currentUnit);
    }
    // if current page is alarm 2, set the alarm 1
    else if(currentPage == setAlarmPage2) {
      int num = changeInputFromKeyPadToNumber(receivedChar);
      // confirm
      if(num == nextKey) {
        // save selected time
        alarmHour2 = alarmClock2[0]*10 + alarmClock2[1];
        alarmMinute2 = alarmClock2[2]*10 + alarmClock2[3];
        // switch on the alarm
        alarmOn2 = true;
        // reset pointer
        currentUnit2 = 0;
        // change screen back to main page
        currentPage = timePage;
        isPageChanged = true;
        display();

        return;
      }
      // clear alarm
      else if(num == clearAlarmKey) {
        // reset the alarm to 00.00
        for(int i = 0; i < 4; i++) {
          alarmClock2[i] = 0;
        }
        currentUnit2 = 0;
      }
      // toggle alarm status
      else if(num == toggleAlarmKey) {
        alarmOn2 = !alarmOn2;
      }
      // next
      else {
        // point to next index
        alarmClock2[currentUnit2] = num;
        if(currentUnit2 < 3)  currentUnit2++;
      }
    }
  }
}

/******************
* MELODY SETTINGS *
*******************/
void handleMelodySettings() {
  if(!digitalRead(changeMelodyPin)){
    // if the melody is set on alarm (1), then point the melody to the next of 
    // that of the alarm (1)
    if(currentPage == setMelodyPage){
      currentMelody++;
      currentMelody = currentMelody % totalMelody;
      isPageChanged = true;
      delay(200);
    }
    // if the melody is set on alarm (2), then point the melody to the next of 
    // that of the alarm (2)
    else if(currentPage == setMelodyPage2) {
      currentMelody2++;
      currentMelody2 = currentMelody2 % totalMelody;
      isPageChanged = true;
      delay(200);
    }
  }
}

/********
* ALARM *
*********/
void playAlarm() {
  // play melody based on selected melody
  while(isAlarmPlayed || isAlarmPlayed2) {
    if((isAlarmPlayed && currentMelody == 0) || (isAlarmPlayed2 && currentMelody2 == 0)) {
      playPinkPanther();
    } else if((isAlarmPlayed && currentMelody == 1) || (isAlarmPlayed2 && currentMelody2 == 1)) {
      playPirates();
    } else if((isAlarmPlayed && currentMelody == 2) || (isAlarmPlayed2 && currentMelody2 == 2)) {
      playHarryPotter();
    } else if((isAlarmPlayed && currentMelody == 3) || (isAlarmPlayed2 && currentMelody2 == 3)) {
      playChristmas();
    }
  }
}
void stopAlarm() {
  if(isAlarmPlayed || isAlarmPlayed2){
    // stop the alarm
    // change the alarm state
    isAlarmPlayed = false;
    isAlarmPlayed2 = false;
    // change back to main screen
    currentPage = timePage;
    isPageChanged = true;
  }
}
void handleAlarm() {
  // sound the alarm 1 when now is equal to alarm 1
  if(alarmOn && now.Hour() == alarmHour && now.Minute() == alarmMinute) {
    if(now.Second() == 0){
      isAlarmPlayed = true;
      isPageChanged = true;
      // display the 'wake up' screen
      display();
    }
    // play the alarm in loop
    playAlarm();
  }
  // sound the alarm 2 when now is equal to alarm 2
  else if(alarmOn2 && now.Hour() == alarmHour2 && now.Minute() == alarmMinute2) {
    if(now.Second() == 0){
      isAlarmPlayed2 = true;
      isPageChanged = true;
      // display the 'wake up' screen
      display();
    }
    // play the alarm in loop
    playAlarm();
  }
}

/*******
* MAIN *
********/
void setup() {

  // initialize serial monitor
  Serial.begin(9600);
  
  // initialize lcd screen
  lcd.init();
  lcd.backlight();

  // initialize clock and get current date and time from the PC 
  Rtc.Begin(); 
  RtcDateTime currentTime = RtcDateTime(__DATE__, __TIME__);
  Rtc.SetDateTime(currentTime);

  // initialize switches
  pinMode(changePagePin, INPUT_PULLUP);
  pinMode(stopAlarmPin, INPUT_PULLUP);
  pinMode(changeMelodyPin, INPUT_PULLUP);

  // initialize input interrupt for stopping the alarm
  attachInterrupt(digitalPinToInterrupt(stopAlarmPin), stopAlarm, RISING);

  // reset current screen
  currentPage = 0;

}
void loop() {

  // change the screen if the change screen button is pressed
  handlePageChange();

  // receive time input from another arduino
  handleAlarmSettings();

  // change melody 1 or 2 
  // if the change melody button is pressed and current page is a melody setting page
  handleMelodySettings();

  // sound the alarm when now is equal to either alarm 1 or alarm 2
  handleAlarm();

  // get current time from the real-time clock module
  now = Rtc.GetDateTime();
  
  // display current screen on lcd screen
  display();

  // some dalay
  delay(100);
}

/* ----------------------------------------- */