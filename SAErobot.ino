#define SOUND 5

float  nbTourG = 0;
float  nbTourD = 0;
float  compteurG = 0;
float  compteurD = 0;
char   message;
int    vitesseRoue = 255;
bool   IU = true;
bool   secu = true;

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);
  // while(!Serial1){;}
  Serial.begin(9600);
  // while(!Serial){;}
  pinMode(12, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), interruptG, RISING);
  attachInterrupt(digitalPinToInterrupt(3), interruptD, FALLING);
}

void loop() {
  // put your main code here, to run repeatedly
  TestInput();
  action();
}

void TestInput()
{
    if (Serial1.available()) {
    message = (char)Serial1.read();
    //Serial.println(message);
    }
}

float gradiant(float edge0, float edge1, float x)
{
   if (x < edge0)
      return 0;

   if (x >= edge1)
      return 1;
      
   x = (x - edge0) / (edge1 - edge0);

   return x * x * (3 - 2 * x);
}

void Script(bool(*inter)(void), int distanceR, int distanceL, int Rspeed, int Lspeed)
{
  float Rdist = distanceR;
  float Ldist = distanceL;
  compteurD = 0;
  compteurG = 0;
  bool cond = true;
  char message2 = message;
  while (cond)
  {
    float dist = getDist();
    //Serial.println(dist);
    if(IU)sendBT(dist);
    TestInput();
    Serial.println("message un");
    Serial.println(message);
    Serial.println(message2);
    if(message2 != message)
    {
      cond = false;
    }
    if (dist < 10)
    {
      Serial1.println("le mur mec");
      float tempG = compteurG;
      float tempD = compteurD;
      cond = inter();
      Serial1.println(cond);
      if (cond == true)
      {
        compteurG = tempG;
        compteurD = tempD;
      }else
      {
        message = ' ';  
      }
    }
    if (cond) {
      RWheel(Rspeed*(0.5*(1-gradiant(0,Rdist,abs(compteurD)))+0.5));
      LWheel(Lspeed*(0.5*(1-gradiant(0,Ldist,abs(compteurG)))+0.5));
    }
    if (Rdist < abs(compteurD) && Ldist < abs(compteurG))
    {
      cond = false;
      message = ' ';
    }
    Serial1.println(cond);
  }

}

bool StopSTP()
{
  Serial.println("wallah ta vu le mur");
  RWheel(0);
  LWheel(0);
  for (int i = 0; i < 5; i++) {
    Serial.print("mec ya un mur je teste numero: ");
    Serial.println(i);
    float distance = getDist();
    while (distance < 15)
    {
      distance = getDist();
      Serial.println(distance);
      RWheel(-255);
      LWheel(-255);
    }

  }
  return false;
}

bool StoppasSTP()
{
  return true;
}

void action() {
  compteurG = 0;
  compteurD = 0;
  Serial.println(message);
  if (message == 'A') {
    if(secu == true){
      Script(StopSTP, 200, 200, vitesseRoue, vitesseRoue);
    }else{
      Script(StoppasSTP, 200, 200, vitesseRoue, vitesseRoue);
    }
    
  } else if (message == 'R') {
    Script(StoppasSTP, 200, 200, -vitesseRoue, -vitesseRoue);
  } else if (message == 'G') {
    Script(StoppasSTP, 93, 93, vitesseRoue, -vitesseRoue);
  } else if (message == 'D') {
    Script(StoppasSTP, 93, 93, -vitesseRoue, vitesseRoue);
  } else if (message == 'E') {
    Script(StoppasSTP, 50, 50, vitesseRoue, -vitesseRoue);
  } else if (message == 'O') {
    Script(StoppasSTP, 50, 50, -vitesseRoue, vitesseRoue);
  } else if (message == 'e') {
    Script(StoppasSTP, 50, 50, vitesseRoue, -vitesseRoue);
  } else if (message == 'o') {
    Script(StoppasSTP, 50, 50, -vitesseRoue, vitesseRoue);
  } else if (message == 'S') {  
    LWheel(0);
    RWheel(0);
    sequence();
  } else if (message == 'P') {
    LWheel(0);
    RWheel(0);
    delay(500);
  } else if (message == 'C') {
    secu=true;
  } else if (message == 'c') {
    secu=false;
  } else if (message == 'U') {
    IU = true;
  } else if (message == 'u') {
    IU = false;
  } else {
    LWheel(0);
    RWheel(0);
  }
}

void sequence(){
  char * sequence = new char[1];
  message=' ';
  int i=0;
  while(message != 'Q'){
    message=' ';
    while(message == ' '){
      TestInput();
    }
    charappend(sequence,i,message);
    Serial.print(message);
    Serial.print(i);
    i++;
  }
  for(int j = 0; j<i;j++){
    TestInput();
    if(message !='P'){
      message = sequence[j];
      Serial.print(message);
      action();
      message=' ';
    }else{
      action();
    }
  }
  free(sequence);
}

void interruptG() {
  if (digitalRead(2) == digitalRead(4)) {
    compteurG++;
  }
  else {
    compteurG--;
  }
}

void interruptD() {
  if (digitalRead(3) == digitalRead(13)) {
    compteurD++;
  }
  else {
    compteurD--;
  }
}

void sendBT(float number) {
  Serial1.print(number);
  Serial1.print("\n");
}

float getDist()
{
  pinMode(SOUND, OUTPUT);
  digitalWrite(SOUND, HIGH);
  delayMicroseconds(8);
  digitalWrite(SOUND, LOW);
  pinMode(SOUND, INPUT);
  float dist = pulseIn(SOUND, HIGH) / 58.0;
  return dist;
}

void RWheel(int vitesse)
{
  if (vitesse >= 0) {
    digitalWrite(12, HIGH);
  } else {
    digitalWrite(12, LOW);
  }
  analogWrite(10, abs(vitesse));
}

void LWheel(int vitesse)
{
  if (vitesse >= 0) {
    digitalWrite(8, LOW);
  } else {
    digitalWrite(8, HIGH);
  }
  analogWrite(9, abs(vitesse));
}

void charappend(char *& list,int listsize, char letter)
{
    char * list2 = new char[listsize+1];
    memcpy(list2,list,listsize);
    list2[listsize] = letter;
    free(list);
    list = new char[listsize+1];
    memcpy(list,list2,listsize+1);
    free(list2);
}
