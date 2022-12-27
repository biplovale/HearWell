#include <SPI.h>
#include <SD.h>
#include <map>

File myFile;
std::map<float, float> profile {};

float pi = 3.14159;
int recStep = 0;

int count = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
//  profile[0] = (sin(0) * sin(0));
  profile.insert(std::make_pair(0.1, sin(1/0.1)));
//  profile[2*pi] = (sin(2* pi) * sin(2*pi));
  profile.insert(std::make_pair(pi, sin(1/pi)));



  
}

void loop() {
  // put your main code here, to run repeatedly:
Serial.println();
  for (const auto& m : profile)
  {
      Serial.print("{");
      Serial.print(m.first); Serial.print(",");
      Serial.print(m.second);
      Serial.println("}");
  }

  recStep = 0;
  recursiveSimpson(0.1, pi, 0.000001, simpleSimpson(0.1, pi));


delay(2000);
  Serial.println();
  for (const auto& m : profile)
  {
      count ++;
      Serial.print("{");
      Serial.print(m.first); Serial.print(",");
      Serial.print(m.second);
      Serial.println("}");
  }
  Serial.println(count);
  while(Serial.available() == 0){}
}

float simpleSimpson(float a, float b){
  float xMean = (a+b)/2.0;
  float interval = b - a;
  float meanVolPoint;

  //taking input from user for vol
  Serial.print("Enter the volume for freq: ");
  Serial.println(xMean);
  meanVolPoint = sin(1/xMean);
  Serial.println(meanVolPoint);

  delay(150);

//  profile[xMean] = meanVolPoint;
  profile.insert(std::make_pair(xMean, meanVolPoint));
  
  return (interval/6.0) * (profile[a] + (4 * meanVolPoint) + profile[b]);
}

float recursiveSimpson(float xF, float xL, float eps, float intLast){
  float xMean = (xF+xL)/2.0;
  Serial.println(xMean);
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

  delay(150);
  Serial.println(abs(intTotal - intLast) < 15 * eps);
  if(abs(intTotal - intLast) < 15 * eps){
    recStep += 1;
    return ((16*intTotal) - intLast) / 15.0;
  }
  else{
    if (recStep <= 8){
      recStep += 2;
      return recursiveSimpson(xF, xMean, eps/2, intL) + recursiveSimpson(xMean, xL, eps/2, intR);
    }
    else{
      return ((16*intTotal) - intLast) / 15.0;
    }
  }
} 
