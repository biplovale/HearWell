#include <ArxContainer.h>

arx::map<float, int> profile {};
float pi = 3.14159;
int recStep = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  profile[0] = (sin(0) * sin(0));
  profile[2*pi] = (sin(pi*2) * sin(pi*2));
//  profile[1000] = 100;
//  profile[2000] = 100;
//  profile[3000] = 100;
//  profile[4000] = 100;
//  profile[8000] = 100;

  
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
  recursiveSimpson(0, 2*pi, 0.0001, simpleSimpson(0, 2*pi));

  Serial.println();
  for (const auto& m : profile)
  {
      Serial.print("{");
      Serial.print(m.first); Serial.print(",");
      Serial.print(m.second);
      Serial.println("}");
  }

  while(Serial.available() == 0){}
}

float simpleSimpson(float a, float b){
  float xMean = (a+b)/2.0;
  float interval = b - a;
  float meanVolPoint;

  //taking input from user for vol
  Serial.print("Enter the volume for freq: ");
  Serial.println(xMean);
//  while(Serial.available() == 0){
//  }
  meanVolPoint = (sin(xMean) * sin(xMean));
  Serial.println(meanVolPoint);
  delay(1000);
  Serial.end();
  Serial.begin(9600);

  profile[xMean] = meanVolPoint;
  
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

  delay(1000);
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
