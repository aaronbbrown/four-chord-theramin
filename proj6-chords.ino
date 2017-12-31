/* Light controlled Theramin for the 4-chord enthusiast. It is based on Project 6 
 * in the Arduino Starter Kit guide and extended to be pitch focused rather than
 * a continuous spectrum of frequencies.
 * 
 * This project uses a photoresistor, 2 buttons, a piezo, and a Potentiometer to create a
 * Theramin which can play 4 chords (I - IV - V - vi) in any key.
 * 
 * Changing the amount of light reaching the light sensor (by moving a hand up/down near it)
 * controls pitch. The buttons control the arpeggiated chord that is played:
 * - no buttons pressed:   I chord
 * - BUTTON1 pressed:      V chord
 * - BUTTON2 pressed:      IV chord
 * - both buttons pressed: vi chord
 * 
 * The potentiometer controls the key in half steps - 0 is A major -> 12 is G# major
 * 
 * When the Arduino is first powered on, it enters a calibration phase for 5 seconds during
 * which time the onboard LED will be on. To calibrate, move your hand near the light
 * sensor in the range that you expect to use it.
 */

#define LED_PIN 13     // Onboard LED
#define SENSOR_PIN A0  // Light sensor
#define POT_PIN A1     // Potentiometer
#define PIEZO_PIN 8    // Piezo
#define BUTTON1_PIN 2  // Button 1
#define BUTTON2_PIN 4  // Button 2

// 1-based note offsets into the notes[] array below. These need
// to be converted to 0 based for use.
#define NOTE_A 1
#define NOTE_A_SHARP 2
#define NOTE_B_FLAT 2
#define NOTE_B 3
#define NOTE_B_SHARP 4
#define NOTE_C 4
#define NOTE_C_SHARP 5
#define NOTE_D_FLAT 5
#define NOTE_D 6
#define NOTE_D_SHARP 7
#define NOTE_E_FLAT 7
#define NOTE_E 8
#define NOTE_F 9
#define NOTE_F_SHARP 10
#define NOTE_G_FLAT 10
#define NOTE_G 11
#define NOTE_G_SHARP 12
#define NOTE_A_FLAT 12


int sensorValue;       // Raw value from the light sensor
int sensorLow = 1023;  // used for calibrating low value of light sensor against ambient light
int sensorHigh = 0;    // used for calibrating high value of light sensor against ambient light
int lastNoteIdx = 0;   // index into the notes[] array for the last note played

// Notes on an 88 key piano.  Generated from idealized piano frequency formula https://en.wikipedia.org/wiki/Piano_key_frequencies
// Ruby for generating: (1..88).map { |n| ((2.0 ** (n-49.0)/12.0 ) * 440.0).round(1) }
const float freqs[] = { 27.5, 29.1, 30.9, 32.7, 34.6, 36.7, 38.9, 41.2, 43.7, 46.2, 49.0, 51.9,
                        55.0, 58.3, 61.7, 65.4, 69.3, 73.4, 77.8, 82.4, 87.3, 92.5, 98.0, 103.8,
                        110.0, 116.5, 123.5, 130.8, 138.6, 146.8, 155.6, 164.8, 174.6, 185.0, 196.0, 207.7, 
                        220.0, 233.1, 246.9, 261.6, 277.2, 293.7, 311.1, 329.6, 349.2, 370.0, 392.0, 415.3,
                        440.0, 466.2, 493.9, 523.3, 554.4, 587.3, 622.3, 659.3, 698.5, 740.0, 784.0, 830.6,
                        880.0, 932.3, 987.8, 1046.5, 1108.7, 1174.7, 1244.5, 1318.5, 1396.9, 1480.0, 1568.0, 1661.2,
                        1760.0, 1864.7, 1975.5, 2093.0, 2217.5, 2349.3, 2489.0, 2637.0, 2793.8, 2960.0, 3136.0, 3322.4,
                        3520.0, 3729.3, 3951.1, 4186.0 } ;

// Here are the 4 chords
int chordI[]  = {NOTE_C, NOTE_E, NOTE_G};
int chordIV[] = {NOTE_F, NOTE_A, NOTE_C}; 
int chordV[]  = {NOTE_G, NOTE_B, NOTE_D, NOTE_F};  // ok, it's actually V7
int chordvi[] = {NOTE_A, NOTE_C, NOTE_E};
int *notes; // a pointer used to index into the chord arrays as different buttons are pressed

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON1_PIN, INPUT);
  pinMode(BUTTON2_PIN, INPUT);

  // Calibrate the sensor against ambient light. Wave your hand around.
  digitalWrite(LED_PIN, HIGH);
  while (millis() < 5000 ) {
    sensorValue = analogRead(SENSOR_PIN);
    if (sensorValue > sensorHigh) {
      sensorHigh = sensorValue;
    }
    if (sensorValue < sensorLow) {
      sensorLow = sensorValue;
    }
  }
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  sensorValue = analogRead(SENSOR_PIN);

  // turn the raw sensor value into an index (+1) into the freqs[] array.
  int note = map(sensorValue, sensorLow, sensorHigh, 1, sizeof(freqs)/sizeof(float));
  int potVal = analogRead(POT_PIN);
  // turn the raw Potentiometer value into a musical key 0->12
  int key = map(potVal, 0, 1023, 0, 12);

  // normalize the note
  int baseNote = (note % 12) + 1;
  int count;

  if ( digitalRead(BUTTON1_PIN) == HIGH && digitalRead(BUTTON2_PIN) == HIGH) {
    // Both buttons pressed, play a vi chord
    notes = chordvi;
    count = sizeof(chordvi)/sizeof(int);    
  } else if ( digitalRead(BUTTON1_PIN) == HIGH ) {
    // Button 1 pressed, play the V chord
    notes = chordV;
    count = sizeof(chordV)/sizeof(int);
  } else if ( digitalRead(BUTTON2_PIN) == HIGH) {
    // Button 2 pressed, play a IV chord
    notes = chordIV;
    count = sizeof(chordiv)/sizeof(int);
  } else {
    // No buttons pressed, play a I chord
    notes = chordI;
    count = sizeof(chordI)/sizeof(int);
  }

  // See if the note being attempted is in the chord
  boolean found = false;
  for (int i = 0; i < count; i++ ) {
    if (baseNote == *(notes+i)) {
      found = true;
      break;
    }
  }

  // if the note is in the chord, select it, otherwise play the last note
  // in the chord.
  // This effectively snaps to notes in the chord and won't play any
  // other notes.
  int noteIdx = 0;
  if (found) {
    noteIdx = note - 1 + key;
    lastNoteIdx = noteIdx;
  } else {
    // keep playing the last note
    noteIdx = lastNoteIdx;
  }

  // make sure we don't exceed the range of the array.
  if (noteIdx < 0) {
    noteIdx = 0;
  }
  if (noteIdx > sizeof(freqs)/sizeof(int)) {
    noteIdx = sizeof(freqs)/sizeof(int);
  }

  // let's make some noise
  tone(PIEZO_PIN, freqs[noteIdx], 20);
  delay(10);
}
