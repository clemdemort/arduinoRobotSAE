#include <SimpleTimer.h>
#define BT false
SimpleTimer       timer;
SimpleTimer       timer2;
float  periode = 100;
float  consigneVit = 150;
int    consignePos = 1000;
float    time = 0;
//-----Moteur Gauche-----------
float  PvitG = 1.2;
float  IvitG = 0.005;
float  PposG = 0.7;
float  IposG = 0;
int    consigneVitG = 0;
float  compteurG = 0;
float  compteurPosG = 0;
float  compteurGdelta = 0;
float  vitesseG = 0;
float  sommeErreurVitG = 0;
float  sommeErreurPosG = 0;
//-----Moteur Droite-----------
float  PvitD = 0.9;
float  IvitD = 0.005;
float  PposD = 0.7;
float  IposD = 0;
int    consigneVitD = 0;
float  compteurD = 0;
float  compteurPosD = 0;
float  compteurDdelta = 0;
float  vitesseD = 0;
float  sommeErreurVitD = 0;
float  sommeErreurPosD = 0;
//-----------------------------

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);
  //while(!Serial1){;}
  //Serial1.setTimeout(2000);
  timer.setInterval(periode, Prog_INT);
  RWheel(0);
  LWheel(0);
  pinMode(12, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  Serial.println("#CLEAR");
  Serial.println("posG posD vitG vitD");
  attachInterrupt(digitalPinToInterrupt(2), interruptG, RISING);
  attachInterrupt(digitalPinToInterrupt(3), interruptD, FALLING);
  delay(2000);
}

void loop() {
  timer.run();
  delay(periode);
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

void interruptG() {
  compteurGdelta = compteurG;
  if (digitalRead(2) == digitalRead(4)) {

    compteurG++;
    compteurPosG++;
  }
  else {
    compteurG--;
    compteurPosG--;
  }
}

void interruptD() {
  compteurDdelta = compteurD;
  if (digitalRead(3) == digitalRead(13)) {
    compteurD++;
    compteurPosD++;
  }
  else {
    compteurD--;
    compteurPosD--;
  }
}

void Prog_INT(){
  time = time + periode/1000;
  compteurGdelta = compteurG;
  compteurDdelta = compteurD;
  compteurG = 0;
  compteurD = 0;
  vitesseG = compteurGdelta / 0.1;
  vitesseD = compteurDdelta / 0.1;
  consignePos = getConsigne(consignePos);
  consigneVitD = regulateurPos(PposD, IposD , consignePos, compteurPosD, sommeErreurPosD);
  consigneVitG = regulateurPos(PposG, IposG , consignePos, compteurPosG, sommeErreurPosG);
  RWheel(regulateurVit(PvitD, IvitD, consigneVitD, vitesseD, sommeErreurVitD));
  LWheel(regulateurVit(PvitG, IvitG, consigneVitG, vitesseG, sommeErreurVitG));
  if (BT == true) {BTsendGraph(compteurPosG, compteurPosD, vitesseG, vitesseD);}
  if (BT == false){
    Serial.println(consignePos);
    Serial.print(compteurPosG);
    Serial.print(", ");
    Serial.print(compteurPosD);
    Serial.print(", ");
    Serial.print(vitesseG);
    Serial.print(", ");
    Serial.print(vitesseD);
    Serial.println(" ");
  }
}

float erreur(float mesure, int consigne) {
  return consigne - mesure;
}

int regulateurVit(float P, float I, int consigne, float vitesse, float & sommeErreur) {
  float e = erreur(vitesse, consigne);
  sommeErreur += e * periode;
  int nouvelleConsigne = P * e + I * sommeErreur;
  if (nouvelleConsigne > 255) {
    nouvelleConsigne = 255;
  } else if (nouvelleConsigne < -255) {
    nouvelleConsigne = -255;
  }
  /*
    if(Serial.available()){
    Serial.print(e);
    Serial.print(", ");
    Serial.print(sommeErreur);
    Serial.print(", ");
    Serial.println(nouvelleConsigne);
    }*/
  return nouvelleConsigne;
}

int regulateurPos(float P, float I, int consigne, float position, float & sommeErreur) {
  float e = erreur(position, consigne);
  sommeErreur += e * periode;
  int nouvelleConsigne = P * e + I * sommeErreur;
  /*Serial.print(e);
    Serial.print(", ");
    Serial.print(sommeErreur);
    Serial.print(", ");
    Serial.println(nouvelleConsigne);*/
  if (nouvelleConsigne > 255){nouvelleConsigne=150;}
  return nouvelleConsigne;
}

int getConsigne(int consigne){
  char buffer = Serial.read();
  Serial.print("test");
  char * bufferConsigne = new char[1];
  
  int newConsigne = 0;
  int i = 0;
  if (buffer == 'A'){
    while(buffer != 'B'){
      buffer = Serial.read();
      if(isdigit(buffer)){charappend(bufferConsigne,i,buffer);}
      i++;  
    }
    sscanf(bufferConsigne,"%d",&newConsigne);
    free(bufferConsigne);
    return newConsigne;
  }else{
    return consigne;
  }
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

void BTsendGraph(float A, float B, float C, float D) {
  char buffer[80];
  char As[50];
  char Bs[50];
  char Cs[50];
  char Ds[50];
  char times[50];
  dtostrf(A,4,2,As);
  dtostrf(B,4,2,Bs);
  dtostrf(C,4,2,Cs);
  dtostrf(D,4,2,Ds);
  dtostrf(time,4,2,times);
  sprintf(buffer, "*HX%sY%s,X%sY%s,X%sY%s,X%sY%s*", times, As, times, Bs, times, Cs, times, Ds);
  Serial1.println(buffer);
  free(buffer);
}
