
 
#include "pitches.h"
#include <avr/sleep.h>
#include <avr/interrupt.h>

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

unsigned long tonePreviousMillis = 0;

int toneState = 0;
int thisNote = 0;

void setup(){  
  pinMode(PIEZO, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(TILT_SENSOR, INPUT);

  // unused pins
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);

  // Flash quick sequence so we know setup has started
  for (int k = 0; k < 10; k = k + 1) {
    if (k % 2 == 0) {
      digitalWrite(LED_PIN, HIGH);
    } else {
      digitalWrite(LED_PIN, LOW);
    }
    delay(250);
  } // for
}

void sleep() {
  GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
  PCMSK |= _BV(PCINT2);                   // Use PB2 as interrupt pin
  ADCSRA &= ~_BV(ADEN);                   // ADC off
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement

  sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
  sei();                                  // Enable interrupts
  sleep_cpu();                            // sleep

  cli();                                  // Disable interrupts
  PCMSK &= ~_BV(PCINT2);                  // Turn off PB2 as interrupt pin
  sleep_disable();                        // Clear SE bit
  ADCSRA |= _BV(ADEN);                    // ADC on

  sei();                                  // Enable interrupts
} // sleep

ISR(PCINT0_vect) {
  // This is called when the interrupt occurs, but I don't need to do anything in it
}

void loop(){
  if (toneState == 0) {
    sleep();
  }
  playTone();
}

void playTone() {
  toneState =1;
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
    delay(10000);
  }
}
