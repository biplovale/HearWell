#include "Volume.h"
#define window_size 10

//Initial setup 
int window[window_size] = {0};
int volProfile[8] = {125, 250, 500, 1000, 2000, 3000, 4000, 8000};
int volumePot = A0;
int buttonPin = 7;
Volume vol;
int buttonOld = 1;
int buttonNew;
int freqIndex = 0;

//Helper function definations
void push(int val);
int take_avg();
void addVolToProfile(int frequency);

void setup() {
  Serial.begin(9600);
  vol.begin(); //default pin = D5; alternate = D6
  pinMode(buttonPin, INPUT);
}

void loop() {
  buttonNew = digitalRead(buttonPin);

  while (buttonNew == 1){
    addVolToProfile(volProfile[freqIndex]);
    buttonNew = digitalRead(buttonPin);
    Serial.println(buttonNew);
  }
  
  Serial.println("Button Pressed");
  freqIndex += 1;
  buttonOld = buttonNew;
  
  delay(10000);
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

void addVolToProfile(int frequency){
  //potentiometer stabalized inputs
  int cur_sample = 0;
  cur_sample = ((15969.0 / 1023.0) * analogRead(volumePot)) + 31;
  Serial.print(cur_sample);
  Serial.print(" avg: ");
  delayMicroseconds(300);
  push(cur_sample);
  Serial.println(take_avg());

  //generates an audio frequency
//  vol.tone(frequency, take_avg());

  
}
