# 4-chord Theramin

## Overview
This project is a light controlled Theramin for the 4-chord enthusiast. It is based on Project 6 in the [Arduino Starter Kit](https://www.arduino.cc/en/Main/ArduinoStarterKit?setlang=en) projects book and extended to be pitch focused rather than a continuous spectrum of frequencies.

This project uses a photoresistor, 2 buttons, a piezo, and a Potentiometer to create a Theramin which can play 4 chords (I - IV - V - vi) in any key.  Changing the amount of light reaching the light sensor (by moving a hand up/down near it) controls pitch. The buttons control the arpeggiated chord that is played:
 * - no buttons pressed:   I chord
 * - BUTTON1 pressed:      V chord
 * - BUTTON2 pressed:      IV chord
 * - both buttons pressed: vi chord

The potentiometer controls the key in half steps - 0 is A major -> 12 is G# major.

## Calibration

When the Arduino is first powered on, it enters a calibration phase for 5 seconds during which time the onboard LED will be on. To calibrate, move your hand near the light sensor in the range that you expect to use it.
