
 
#include "pitches.h"

// Utility macro
#define adc_disable() (ADCSRA &= ~(1<<ADEN)) // disable ADC (before power-off)

//notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_D4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4,
   NOTE_C4, NOTE_D4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4,
      NOTE_C4, NOTE_D4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_E4,
      0,
        NOTE_C4, NOTE_D4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4,
   NOTE_C4, NOTE_D4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4,
      NOTE_C4, NOTE_D4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_E4,
      0,
            NOTE_A4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4,
   NOTE_C4, NOTE_D4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4,
      NOTE_C4, NOTE_D4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_E4,  
};

// note durations: 4 = quarter note, 8 = eighth note, etc
int noteDurations[] = {
  2, 2, 4, 4, 4, 8, 4, 8, 4,
  4, 4, 4, 4, 4, 8, 4, 8, 4,
  4, 4, 4, 4, 4, 8, 4, 8, 4, 4, 4, 2,
  2, 
    2, 2, 4, 4, 4, 8, 4, 8, 4,
  4, 4, 4, 4, 4, 8, 4, 8, 4,
  4, 4, 4, 4, 4, 8, 4, 8, 4, 4, 4, 2, 
    2, 
    2, 2, 4, 4, 4, 8, 4, 8, 4,
  4, 4, 4, 4, 4, 8, 4, 8, 4,
  4, 4, 4, 4, 4, 8, 4, 8, 4, 4, 4, 2, 
};

const int LED_PIN = 3;
const int TILT_SENSOR = 2;
const int PIEZO = 4;

int tiltState = LOW;
unsigned long tonePreviousMillis = 0;

int toneState = 0;
int thisNote = 0;
int isStartup = 1;

void setup(){  
  pinMode(PIEZO, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(TILT_SENSOR, INPUT);

  // unused pins
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);

  adc_disable();
}

void loop(){
  int val = digitalRead(TILT_SENSOR);
  
  if (isStartup == 1) {
    // prevent playing tone on power up
    tiltState = val;
    isStartup = 0;
  }

  if (tiltState != val) {
    tiltState = val;
    if (toneState == 0) {
      toneState = 1;  
    }
  }

  if (toneState == 1) {
    playTone();
  }
}

void playTone() {
  if (thisNote < sizeof(melody) / sizeof(melody[0])) {
    unsigned long currentMillis = millis();
    
    /*
      to calculate the note duration, take one second divided by the note type.
      e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
     */       
    int noteDuration = 1000/noteDurations[thisNote];
    if (currentMillis - tonePreviousMillis >= noteDuration) {
      noTone(PIEZO);
      digitalWrite(LED_PIN, LOW);
    } else {
      if (melody[thisNote] == 0) {
        noTone(PIEZO);
        digitalWrite(LED_PIN, LOW);
      } else {
        tone(PIEZO, melody[thisNote]);
        digitalWrite(LED_PIN, HIGH); 
      }
    }
  
    /*
      to distinguish the notes, set a minimum time between them.
      the note's duration + 30% seems to work well:
     */
    int pauseBetweenNotes = noteDuration * 1.30;
    if (currentMillis - tonePreviousMillis >= pauseBetweenNotes) {
      // save the end time of playing a tone
      tonePreviousMillis = currentMillis;
      thisNote++;
    }
  } else {
    toneState = 0;
    thisNote = 0;
    tiltState = digitalRead(TILT_SENSOR);
  }
}
