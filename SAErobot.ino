#define SOUND 5

float  nbTourG = 0;
float  nbTourD = 0;
float  compteurG = 0;
float  compteurD = 0;
char   message;
String commande= "";




void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);
 // while(!Serial1){;}
  Serial.begin(9600);
 // while(!Serial){;}
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
      Serial.println(message);
      commande="";
  }
    action();
    message=' ';
}

void Script(bool(*inter)(void),int distanceR,int distanceL,int Rspeed,int Lspeed)
{
    float Rdist = distanceR;
    float Ldist = distanceL;
    compteurD = 0;
    compteurG = 0;
    bool cond = true;
    while(cond)
    {
      float dist = getDist();
      Serial.println(dist);
      if(dist < 20)
      {
        cond = inter();
      }
      if(cond){
        RWheel(Rspeed);
        LWheel(Lspeed);
      }
      if(Rdist < abs(compteurD) && Ldist < abs(compteurG))
      {
        cond = false;  
      }
    }
    
}
bool StopSTP()
{
  RWheel(0);
    LWheel(0); 
  float distance = getDist();
  while(distance < 20)
  {
    distance = getDist();
    Serial.println(distance);
    RWheel(-255);
    LWheel(-255);  
  }
  return false;  
}
bool StoppasSTP()
{
  return true;  
}
void action(){
  compteurG = 0;
  compteurD = 0;
  //Serial.println(commande);
    if(message == 'A'){
      Script(StopSTP,200,200,255,255);
    }else if(message == 'R'){
      Script(StoppasSTP,200,200,-255,-255);
    }else if(message == 'G'){
      Script(StoppasSTP,200,200,-255,255);
    }else if(message == 'D'){
      Script(StoppasSTP,200,200,255,-255);
  }else{
        LWheel(0);
        RWheel(0);
    }
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
