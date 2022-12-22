#include <ArxContainer.h>

arx::map<float, float> profile {};
float pi = 3.14159;
int recStep = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
<<<<<<< HEAD
  profile[0] = (sin(0) * sin(0));
  profile[2*pi] = (sin(2* pi) * sin(2*pi));
//  profile[500.0] = 100;
=======
  profile[125.0] = 100;
  profile[250.0] = 100;
  profile[500.0] = 100;
>>>>>>> f50eaa9096417b62c9324df521e0432e0d1f8c34
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
  
<<<<<<< HEAD
  recursiveSimpson(0, 2*pi, 0.000001, simpleSimpson(0, 2*pi));

delay(2000);
=======
  recursiveSimpson(125.0, 250.0, 0.000001, simpleSimpson(125.0, 250.0));

>>>>>>> f50eaa9096417b62c9324df521e0432e0d1f8c34
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

  meanVolPoint = sin(xMean) * sin(xMean);
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
<<<<<<< HEAD
    recStep += 1;
    return ((16*intTotal) - intLast) / 15.0;
  }
  else{
    if (recStep <= 8){
      recStep += 2;
      return recursiveSimpson(xF, xMean, eps/2, intL) + recursiveSimpson(xMean, xL, eps/2, intR);
    }
    else{
      return intLast;
    }
=======
    return ((16*intTotal) - intLast) / 15.0;
  }
  else{
     return recursiveSimpson(xF, xMean, eps/2, intL) + recursiveSimpson(xMean, xL, eps/2, intR);
>>>>>>> f50eaa9096417b62c9324df521e0432e0d1f8c34
//    Serial.println("leftRecursion Start");
//    float recurLeft = recursiveSimpson(xF, xMean, eps/2, intL);
//    Serial.println("leftRecursion End");
//    Serial.println("rightRecursion Start");
//    float recurRight = recursiveSimpson(xMean, xL, eps/2, intR);
//    Serial.println("leftRecursion End");
//
//    return recurLeft + recurRight;
  }
} 
