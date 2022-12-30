//Libraries
#include <Arduino.h>
#include <math.h>
#include <map>

//#define pi 3.14159

//Initial setup 
int buttonPin = 2;//pull-up resistor with button; pressed -> 0
int freqArrSize = 8;
int interestedFreq[8] = {125, 250, 500, 1000, 2000, 3000, 4000, 8000};
std::map<float, float> profile {};
portMUX_TYPE synch = portMUX_INITIALIZER_UNLOCKED;

//Audio variables
int PWM_FREQUENCY = 0; // this variable is used to define the time period 
int PWM_CHANNEL = 0; // this variable is used to select the channel number
int PWM_RESOUTION = 16; // this will define the resolution of the signal which is 16 in this case
int GPIOPIN = 5 ; // GPIO to which we want to attach this channel signal
volatile int dutyCycle = 0; // it will define the width of signal or also the one time

//Other variables
volatile int curFreq;
volatile int recordedVol;//ranges 0 to 255
volatile bool volRecordState = false;//true = recording is still in progress
volatile int curFreqIndex = 0;
volatile int stopValue = 0;
int recStep = 0; 
bool testerState = true;//True = initialFreqTestRun, False = recursiveFreqTestRun
unsigned long previousMillis = 0;  //
const long period = 250;         // period at which to blink in ms

//Helper function definations
int getCurrentFrequency(int freqIndex);
void playFreqAndRecordVol(int freq);//plays the tone and record the tone's cut-off volume (tone's volume that can't hear anymore)
void stopPlayFreq(); //stops the tone playing
void IRAM_ATTR buttonPressedAction();
float simpleSimpson(float a, float b);
float recursiveSimpson(float xF, float xL, float eps, float intLast);

//Event functions definations
void initialFreqTestRun();
void recursiveAdaptiveTestRun();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//main()
void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(buttonPin, buttonPressedAction, FALLING);

//  profile.insert(std::make_pair(0.1, sin(1/0.1)));
//  profile.insert(std::make_pair(pi, sin(1/pi)));

  //audio initial setup [a tone of 0 Hz with 0 volume; volume ranges 0 to 255]
  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOUTION);
  ledcAttachPin(GPIOPIN, PWM_CHANNEL);
}

void loop() {
  if(testerState){
    initialFreqTestRun();
  }
  else{
    recursiveAdaptiveTestRun();
    stopPlayFreq();

    while(Serial.available() ==0){};
  }

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Event functions implementations
void initialFreqTestRun(){
  while (stopValue < freqArrSize){ //stops the hearing test if the initial profiling is done 
    curFreq = getCurrentFrequency(curFreqIndex);
    volRecordState = true;
    playFreqAndRecordVol(curFreq);
  }
  if (stopValue >= freqArrSize){
    testerState = false;
    stopPlayFreq();
    
    delay(3000);

    //prints the profile
    Serial.println();
    for (const auto& m : profile){
      Serial.print("{");
      Serial.print(m.first); Serial.print(",");
      Serial.print(m.second);
      Serial.println("}");
    }
  }
}

void recursiveAdaptiveTestRun(){
  for(int i = 0; i < (freqArrSize - 1); i++){
    recStep = 0;
    recursiveSimpson(interestedFreq[i], interestedFreq[i+1], 0.000001, NULL);
  }

//  recStep = 0;
//  recursiveSimpson(interestedFreq[0], interestedFreq[1], 0.000001, NULL);

  for (const auto& m : profile)
  {
      Serial.print("{");
      Serial.print(m.first); Serial.print(",");
      Serial.print(m.second);
      Serial.println("}");
  }

//  recStep = 0;
//  recursiveSimpson(0.1, pi, 0.000001, simpleSimpson(0.1, pi));
//
//  Serial.println();
//  for (const auto& m : profile)
//  {
//      Serial.print("{");
//      Serial.print(m.first); Serial.print(",");
//      Serial.print(m.second);
//      Serial.println("}");
//  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Helper function implementations
int getCurrentFrequency(int freqIndex){
  int result = interestedFreq[freqIndex];

  return result;
}

void playFreqAndRecordVol(int freq){
  //set the new freq
  PWM_FREQUENCY = freq;
  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOUTION);
  ledcAttachPin(GPIOPIN, PWM_CHANNEL);
  delay(100);
  
  //play the tone in a loop until the button is pressed
  dutyCycle = 0;
  while(volRecordState == true){
      unsigned long currentMillis = millis();
      if(currentMillis - previousMillis >= period){
        Serial.println(dutyCycle);
        ledcWrite(PWM_CHANNEL, dutyCycle);

        if (dutyCycle >= 0 and dutyCycle <= 255){
          dutyCycle++;
        }
        else{
          dutyCycle = 0;
        }
        
        previousMillis = currentMillis;
      }    
    }

    //when button is pressed
    if(volRecordState == false){
      profile.insert(std::make_pair(curFreq, recordedVol));
      Serial.print("The volume for freq "); Serial.print(curFreq); Serial.print(" : "); Serial.println(recordedVol);
    }
}

void stopPlayFreq(){
  PWM_FREQUENCY = 0;
  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOUTION);
  ledcAttachPin(GPIOPIN, PWM_CHANNEL);
  delay(100);
  
  ledcWrite(PWM_CHANNEL, 0);
}

void IRAM_ATTR buttonPressedAction(){
  portENTER_CRITICAL(&synch);
  recordedVol = dutyCycle;
//  recordedVol = sin(1/curFreq);

  volRecordState = false;

  if(testerState == true){
    curFreqIndex += 1;
    stopValue += 1;
  }
  Serial.println("buttonPressed");
  portEXIT_CRITICAL(&synch);
}

float simpleSimpson(float a, float b){
  float xMean = (a+b)/2.0;
  float interval = b - a;
  int meanVolPoint;
  
  curFreq = (int)xMean;
  volRecordState = true;
  playFreqAndRecordVol(curFreq);
  meanVolPoint = recordedVol;

  return (interval/6.0) * (profile.find(a)->second + (4 * meanVolPoint) + profile.find(b)->second);
}

float recursiveSimpson(float xF, float xL, float eps, float intLast){
  recStep += 1;
  float xMean = (xF+xL)/2.0;
  float intL;
  float intR;
  float intTotal;
  if(intLast == NULL){
    intLast = simpleSimpson(xF, xL);
  }

  intL = simpleSimpson(xF, xMean);
  Serial.print("intL = "); Serial.println(intL);
  intR = simpleSimpson(xMean, xL);
  Serial.print("intR = "); Serial.println(intR);
  intTotal = intL + intR;
  Serial.print("intTotal = "); Serial.println(intTotal);

  delay(150);
  Serial.println(abs(intTotal - intLast) < 15 * eps);
  if(abs(intTotal - intLast) < 15 * eps){
    return ((16*intTotal) - intLast) / 15.0;
  }
  else{
    if (recStep <= 8){
      return recursiveSimpson(xF, xMean, eps/2, intL) + recursiveSimpson(xMean, xL, eps/2, intR);
    }
    else{
      return ((16*intTotal) - intLast) / 15.0;
    } 
  }
} 
