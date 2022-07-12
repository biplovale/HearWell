#include "Volume.h"
#define window_size 10

//Initial setup 
int window[window_size] = {0};
int volProfile[8] = {125, 250, 500, 1000, 2000, 3000, 4000, 8000};
int volumePot = A0;
int buttonPin = 7;
Volume vol;
int buttonNew;
int freqIndex = 0;
int curVol;

//Helper function definations
void push(int val);
int take_avg();
int playFrequency(int frequency);           //plays the frequency with a volume control and returns the vol

void setup() {
  Serial.begin(9600);
  vol.begin(); //default pin = D5; alternate = D6
  pinMode(buttonPin, INPUT);
}

void loop() {
  buttonNew = digitalRead(buttonPin);

  while (buttonNew == 1 && freqIndex <= 7){ //stops the hearing test if the profiling is done 
    curVol = playFrequency(volProfile[freqIndex]);
    buttonNew = digitalRead(buttonPin);
    Serial.println(buttonNew);
    //executes when button is pressed
    if (buttonNew == 0 ){
      Serial.println("Button Pressed ");
      volProfile[freqIndex] = curVol;
      Serial.println(freqIndex);
      freqIndex += 1;
      Serial.println(freqIndex);
    }
  }

  //prints the profile
  if (freqIndex >= 8){
    vol.noTone();
    Serial.println();
    for (int i = 0; i <= 7; i++){
      Serial.print(volProfile[i]);
      Serial.print(" ");
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

int playFrequency(int frequency){
  //potentiometer stabalized inputs
  int cur_sample = 0;
  cur_sample = (255.0 / 1023.0) * analogRead(volumePot);
  Serial.print(cur_sample);
  Serial.print(" avg: ");
  push(cur_sample);
  Serial.println(take_avg());

  //generates an audio frequency
  vol.tone(frequency, take_avg());

  return take_avg();
}
