#include "Volume.h"

Volume vol;

void setup() {
  vol.begin(); //default pin = D5; alternate = D6

  Serial.begin(9600);
}

void loop() {
  
  //scaling from 0 - 1023 to 0 - 255
  int testVolume = (analogRead(A1) / 1023.0) * 255; 

  Serial.println(testVolume);

  vol.tone(440, testVolume);

  delay(1);
}
