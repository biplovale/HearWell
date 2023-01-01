//Libraries
#include <Arduino.h>
#include <math.h>
#include <map>

//Adaptive Tester parameters
#define EPS 100
#define MAX_REC_STEPS 2

//Initial setup 
int buttonPin = 2;                                                                        //pull-down resistor with button; unpressed -> 0 and pressed -> 1
int freqArrSize = 8;
int interestedFreq[8] = {125, 250, 500, 1000, 2000, 3000, 4000, 8000};
std::map<float, float> profile {};
portMUX_TYPE synch = portMUX_INITIALIZER_UNLOCKED;

//Audio variables
int PWM_FREQUENCY = 0;                                                                    // this variable is used to select the frequency of pwm                                                    
int PWM_CHANNEL = 0;                                                                      // this variable is used to select the channel number
int PWM_RESOUTION = 16;                                                                   // this will define the resolution of the signal which is 16 in this case
int GPIOPIN = 5 ;                                                                         // GPIO to which we want to attach this channel signal
volatile int dutyCycle = 0;                                                               // it will define the width of signal or also the one time

//Other variables
float curFreq;
volatile int recordedVol;                                                                 //ranges 0 to 255
bool volRecordState = false;                                                              //true = recording is still in progress
volatile bool isVolRecordDown = false;                                                    //true = recording run from higher vol to lower vol
volatile bool isVolRecordUpDone = false;                                                  //true = vol up run is done => vol up run record done
volatile bool isVolRecordDownDone = false;                                                //true = vol down run is done => vol down run record done
volatile int count = 0;
int curFreqIndex = 0;
int stopValue = 0;
int recStep = 0; 
bool testerState = true;                                                                  //True = initialFreqTestRun, False = recursiveFreqTestRun
unsigned long tonePreviousMillis = 0;                                                     // stores last recorded timestamp for audio change in ms
const long tonePeriod = 40;                                                              // period at which to change audio in ms
unsigned long buttonPreviousMicros = 0;                                                   //stores last recorded timestamp for button pressed in micro-sec

//Helper function definations
int getCurrentFrequency(int freqIndex);
void playFreqAndRecordVol(float freq);                                                    //plays the tone and record the tone's cut-off volume (tone's volume that can't hear anymore)
void stopPlayFreq();                                                                      //stops the tone playing
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
  for(int i = 0; i < (freqArrSize - 2); i++){
    recStep = 0;
    recursiveSimpson(interestedFreq[i], interestedFreq[i+1], EPS * (i+1), NULL);
  }

  int count = 0;
  for (const auto& m : profile)
  {
      Serial.print("{");
      Serial.print(m.first); Serial.print(",");
      Serial.print(m.second);
      Serial.println("}");
      count++;
  }
  Serial.println(count);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Helper function implementations
int getCurrentFrequency(int freqIndex){
  int result = interestedFreq[freqIndex];

  return result;
}

void playFreqAndRecordVol(float freq){
  //set the new freq
  PWM_FREQUENCY = (int)freq;
  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOUTION);
  ledcAttachPin(GPIOPIN, PWM_CHANNEL);
  delay(3000);
  Serial.println();
  Serial.print("       presenting the freq: "); Serial.println(freq);
  ledcWrite(PWM_CHANNEL, 255);
  delay(2000);
  ledcWrite(PWM_CHANNEL, 0);
  delay(1000);
  
  //play the tone in a loop until the button is pressed
  dutyCycle = 0;
  int volSample1;
  int volSample2;
  while(volRecordState == true){
      unsigned long currentMillis = millis();
      if(currentMillis - tonePreviousMillis >= tonePeriod){
        Serial.println(dutyCycle);
        ledcWrite(PWM_CHANNEL, dutyCycle);

        if (isVolRecordDown == false){
          if (dutyCycle >= 0 and dutyCycle < 255){
            dutyCycle++;
          }
          else{
            recordedVol = 255;
            count++;
            isVolRecordDown = true;
            isVolRecordUpDone = true;
            Serial.println("max db for the speaker is noted as volume");
          }       
        }
        else{
          if (dutyCycle > 0 and dutyCycle <= 255){
            dutyCycle--;
          }
          else{
            recordedVol = 0;
            count++;
            isVolRecordDown = false;
            isVolRecordDownDone = true;
            Serial.println("max db for the speaker is noted as volume");
          }
        }

        if (isVolRecordUpDone == true){
          volSample1 = recordedVol;
      
          //setting up for down vol run
          delay(3000);
          dutyCycle = 2 * dutyCycle;
          if (!(dutyCycle <= 255)){
            dutyCycle = 255;
          }

          isVolRecordUpDone = false;
        }

        if (isVolRecordDownDone == true){
          volSample2 = recordedVol;
          volRecordState = false;
          isVolRecordDownDone = false;
        }
        
        if(testerState == true and count >= 2){
          curFreqIndex += 1;
          stopValue += 1;
          count = 0; 
        }
        
        tonePreviousMillis = currentMillis;
      }    
    }

    //when button is pressed
    if(volRecordState == false){
      Serial.print("volsample1"); Serial.println(volSample1);
      Serial.print("volsample2"); Serial.println(volSample2);
      recordedVol = (int)((volSample1 + volSample2)/2.0);
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
  if (micros() - buttonPreviousMicros >= 200000){
    if (volRecordState == true){
      recordedVol = dutyCycle;

      count++;

      if (isVolRecordDown == false){
        isVolRecordDown = true;
        isVolRecordUpDone = true;
      }
      else{
        isVolRecordDown = false;
        isVolRecordDownDone = true;
      }
    
      Serial.println("buttonPressed");
    }
    buttonPreviousMicros = micros();
  }
  portEXIT_CRITICAL(&synch);
}

float simpleSimpson(float a, float b){
  float xMean = (a+b)/2.0;
  float interval = b - a;
  int meanVolPoint;
  
  curFreq = xMean;
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
    if (recStep <= MAX_REC_STEPS){
      return recursiveSimpson(xF, xMean, eps/2, intL) + recursiveSimpson(xMean, xL, eps/2, intR);
    }
    else{
      return ((16*intTotal) - intLast) / 15.0;
    } 
  }
} 
