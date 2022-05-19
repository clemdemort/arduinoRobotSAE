#include <SoftwareSerial.h>
#define SOUND 5

//bjr clement

float  nbTourG = 0;
float  nbTourD = 0;
float  compteurG = 0;
float  compteurD = 0;
float  dist;
char   message;
String commande= "";

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);
  while(!Serial1){;}
  Serial.begin(9600);
  while(!Serial){;}
  pinMode(12,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(2,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);
  pinMode(13,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2),interruptG,RISING);
  attachInterrupt(digitalPinToInterrupt(3),interruptD,FALLING);
}

void loop() {
  // put your main code here, to run repeatedly
  if(Serial1.available()){
      message = (char)Serial1.read();
      commande+=message;
      Serial.println(commande);
      commande="";
  }
  dist = getDist();
  if(dist > 20){
    RWheel(255);
    LWheel(255);
  }else{
    RWheel(0);
    LWheel(0);    
    }
      Serial.println(compteurG);
  Serial.println(compteurD);
}

void interruptG(){
  if (digitalRead(2)==digitalRead(4)){
      compteurG++;
  }
  else{
      compteurG--;
  }
}

void interruptD(){
    if (digitalRead(3)==digitalRead(13)){
      compteurD++;
  }
  else{
      compteurD--;
  }
}



float getDist()
{
  pinMode(SOUND,OUTPUT);
  digitalWrite(SOUND,HIGH);
  delayMicroseconds(8);
  digitalWrite(SOUND,LOW);
  pinMode(SOUND,INPUT);
  float dist = pulseIn(SOUND,HIGH)/58.0;

  return dist;
}
void Script(float distance,int Rspeed,int Lspeed)
{
    float ratio = 0;
    float Rdist = distance;
    float Ldist = distance;
    float startR = compteurD;
    float startL = compteurG;
    if(abs(Rspeed) >= abs(Lspeed))
    {
      ratio = float(Lspeed/Rspeed);
      Ldist *= ratio;
    }else{
      ratio = float(Rspeed/Lspeed);
      Rdist *= ratio;
    }
    bool cond = true;
    while(cond)
    {
      if(Rdist <= compteurD - startR && Ldist <= compteurG - startL)
      {
        cond = false;  
      }
      Rwheel(Rspeed);
      Lwheel(Lspeed);
    }
    
}

void RWheel(int vitesse)
{
    if(vitesse >= 0){
      digitalWrite(12,HIGH);
    }else{
      digitalWrite(12,LOW);
      }
    analogWrite(10,abs(vitesse));
}

void LWheel(int vitesse)
{
    if(vitesse >= 0){
      digitalWrite(8,LOW);
    }else{
      digitalWrite(8,HIGH);
      }
    analogWrite(9,abs(vitesse));
}
