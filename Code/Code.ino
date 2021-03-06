#include "Volume.h"
#include <math.h>
#include <ArxContainer.h>
#define window_size 10

//Initial setup 
int window[window_size] = {0};
//int volProfile[8] = {125, 250, 500, 1000, 2000, 3000, 4000, 8000};
arx::vector<int> interestedFreq {};
arx::map<int, int> profile {};
int noOfOctaves = 6;
int volumePot = A0;
int frequencyPot = A1;
int buttonPin = 7;
Volume vol;
int buttonNew;
//int freqIndex = 0;
int curVol;
int curFreq;
int stopValue = 0;

//Helper function definations
void push(int val);
int take_avg();
int scaleToOctaves(int octaves);
int playFrequency(int frequency);           //plays the frequency with a volume control and returns the vol

void setup() {
  Serial.begin(9600);
  vol.begin(); //default pin = D5; alternate = D6
  pinMode(buttonPin, INPUT);
}

void loop() {
  buttonNew = digitalRead(buttonPin);

  while (buttonNew == 1 && stopValue <= noOfOctaves){ //stops the hearing test if the profiling is done 
    curFreq = scaleToOctaves(noOfOctaves);
    curVol = playFrequency(curFreq);
    buttonNew = digitalRead(buttonPin);
    Serial.println(buttonNew);
    //executes when button is pressed
    if (buttonNew == 0 ){
      Serial.println("Button Pressed ");
      Serial.print("freq: ");
      Serial.print(curFreq);
      Serial.print(" vol: ");
      Serial.print(curVol);
      interestedFreq.push_back(curFreq);
      profile[curFreq] = curVol;
      stopValue += 1;
    }
  }

  Serial.println(stopValue);
  //prints the profile
  if (stopValue >= noOfOctaves){
    vol.noTone();
    Serial.println();
    for (int i = 0; i <= noOfOctaves; i++){
      Serial.print(interestedFreq[i]);
      Serial.print(": ");
      Serial.print(profile[interestedFreq[i]]);
      Serial.println();
    }
  }
 
  
  delay(20000);
}

//Helper function implementation
void push(int val){
  int i = 0;
  for(i = 0; i < window_size; i++){
    window[i-1] = window[i];
  }
  window[window_size - 1] = val; 
}

int take_avg(){
  long sum = 0;
  int i = 0;
  for(i = 0; i < window_size; i++){
    sum += window[i];
  }
  return sum/window_size;
}

int scaleToOctaves(int octaves){
  //potentiometer stabalized inputs
  int cur_sample = 0;
  cur_sample = (250.0 * pow(2.0, (octaves *analogRead(frequencyPot)) / 1023.0));
  Serial.print(cur_sample);
  Serial.print(" avg freq: ");
  push(cur_sample);
  int result = take_avg();
  Serial.println(result);

  return result;
}

int playFrequency(int frequency){
  //potentiometer stabalized inputs
  int cur_sample = 0;
  cur_sample = (255.0 / 1023.0) * analogRead(volumePot);
  Serial.print(cur_sample);
  Serial.print(" avg: ");
  push(cur_sample);
  int result = take_avg();
  Serial.println(result);

  //generates an audio frequency
  vol.tone(frequency, result);

  return result;
}
