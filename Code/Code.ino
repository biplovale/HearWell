//Libraries
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

//variables
int curFreq;
int curVol;//ranges 0 to 255
int curFreqIndex = 0;
int stopValue = 0;

//Helper function definations
int getCurrentFrequency(int freqIndex);
void playFrequency(int frequency, int volume);           //plays the frequency with a volume control and returns the vol
float simpleSimpson(float a, float b);
void recursiveSimpson(float xF, float xL, float eps, float intLast);

//Event functions definations
void initialFreqTestRun();
void recursiveAdaptiveTestRun();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//main()
void setup() {
  Serial.begin(9600);
  tonePlayer.begin(); //default pin = D5; alternate = D6
  pinMode(buttonPin, INPUT);
}

void loop() {
  initialFreqTestRun();
  delay(5000);
  recursiveAdaptiveTestRun();

  exit(0);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Event functions implementations
void initialFreqTestRun(){
  curVol = 0;
  buttonNew = digitalRead(buttonPin);

  while (buttonNew == 1 && stopValue < freqArrSize){ //stops the hearing test if the profiling is done 
    curFreq = getCurrentFrequency(curFreqIndex);
    playFrequency(curFreq, curVol);
    buttonNew = digitalRead(buttonPin);
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

void recursiveAdaptiveTestRun(){
  for(int i = 0; i < (freqArrSize - 1); i++){
    recursiveSimpson(interestedFreq[i], interestedFreq[i+1], 0.1, NULL);
  }

  Serial.println();
  for (const auto& m : profile)
  {
      Serial.print("{");
      Serial.print(m.first); Serial.print(",");
      Serial.print(m.second);
      Serial.println("}");
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Helper function implementations
int getCurrentFrequency(int freqIndex){
  int result = interestedFreq[freqIndex];

  return result;
}

void playFrequency(int freq, int vol){
  //generates an audio frequency
  tonePlayer.tone(freq, vol);
}

float simpleSimpson(float a, float b){
  float xMean = (a+b)/2.0;
  float interval = b - a;
  int meanVolPoint;

  //taking input from user for vol
  Serial.print("The volume for freq: ");
  Serial.println(xMean);
  curVol = 0;
  buttonNew = digitalRead(buttonPin);

  while (buttonNew == 1){  
    curFreq = (int)xMean;
    playFrequency(curFreq, curVol);
    buttonNew = digitalRead(buttonPin);
    Serial.println(curVol);
    //executes when button is pressed
    if (buttonNew == 0 ){
      Serial.println("Button Pressed ");
      Serial.print("freq: ");
      Serial.print(curFreq);
      Serial.print(" vol: ");
      Serial.print(curVol);
      profile[curFreq] = curVol;
      curVol = 0;
    }

    if (curVol >= 0 && curVol < 255){
      curVol = curVol + 5;
    }
    else{
      curVol = 0;
    }
  }
  tonePlayer.end();
  Serial.println(meanVolPoint);

  profile[(int)xMean] = meanVolPoint;
  
  return (interval/6.0) * (profile[a] + (4 * meanVolPoint) + profile[b]);
}

void recursiveSimpson(float xF, float xL, float eps, float intLast){
  float xMean = (xF+xL)/2.0;
  float intL;
  float intR;
  float intTotal;
  if(intLast == NULL){
    intLast = simpleSimpson(xF, xL);
  }

  intL = simpleSimpson(xF, xMean);
  Serial.println(intL);
  intR = simpleSimpson(xMean, xL);
  Serial.println(intR);
  intTotal = intL + intR;
  Serial.println(intTotal);

  delay(1000);
  Serial.println(abs(intTotal - intLast) < 15 * eps);
  if(abs(intTotal - intLast) < 15 * eps){
//    return ((16*intTotal) - intLast) / 15.0;
      return;
  }
  else{
//    return recursiveSimpson(xF, xMean, eps/2, intL) + recursiveSimpson(xMean, xL, eps/2, intR);
    Serial.println("leftRecursion Start");
    recursiveSimpson(xF, xMean, eps/2, intL);
    Serial.println("leftRecursion End");
    Serial.println("rightRecursion Start");
    recursiveSimpson(xMean, xL, eps/2, intR);
    Serial.println("leftRecursion End");
  }
} 
