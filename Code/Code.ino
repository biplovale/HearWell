#include "Volume.h"
#include <math.h>
#include <ArxContainer.h>

//Initial setup 
int freqArrSize = 8;
arx::array<int,8> interestedFreq {125, 250, 500, 1000, 2000, 3000, 4000, 8000};
arx::map<int, int> profile {};

int buttonPin = 2;
int buttonNew;

Volume tonePlayer;
int curFreq;

int curVol = 0;//ranges 0 to 255
int curFreqIndex = 0;
int stopValue = 0;

//Helper function definations
int getCurrentFrequency(int freqIndex);
void playFrequency(int frequency, int volume);           //plays the frequency with a volume control and returns the vol

void setup() {
  Serial.begin(9600);
  tonePlayer.begin(); //default pin = D5; alternate = D6
  pinMode(buttonPin, INPUT);
}

void loop() {
  buttonNew = digitalRead(buttonPin);

  while (buttonNew == 1 && stopValue < freqArrSize){ //stops the hearing test if the profiling is done 
    curFreq = getCurrentFrequency(curFreqIndex);
    playFrequency(curFreq, curVol);
    buttonNew = digitalRead(buttonPin);
    tonePlayer.delay(3000);
    Serial.println(curVol);
    //executes when button is pressed
    if (buttonNew == 0 ){
      Serial.println("Button Pressed ");
      Serial.print("freq: ");
      Serial.print(curFreq);
      Serial.print(" vol: ");
      Serial.print(curVol);
      profile[curFreq] = curVol;
      curFreqIndex += 1;
      curVol = 0;
      stopValue += 1;
      Serial.println(stopValue);
    }

    if (curVol >= 0 && curVol < 255){
      curVol = curVol + 5;
    }
    else{
      curVol = 0;
    }
  }
  
  //prints the profile
  if (stopValue >= freqArrSize){
    tonePlayer.end();
    Serial.println();
    for (int i = 0; i < freqArrSize; i++){
      Serial.print(interestedFreq[i]); Serial.print(": "); Serial.println(profile[interestedFreq[i]]);
    }
  }
}

int getCurrentFrequency(int freqIndex){
  int result = interestedFreq[freqIndex];

  return result;
}

void playFrequency(int freq, int vol){
  //generates an audio frequency
  tonePlayer.tone(freq, vol);
}
