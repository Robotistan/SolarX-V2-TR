#include <Servo.h>

#define STEP_DELAY 15

//Pil Ölçüm LED'lerin Pin Tanımlamaları
#define led1 4
#define led2 5
#define led3 6
#define led4 7

int BUTTON_PIN = A2;

int buttonState = 0; 
int solar_mode = 1; //Default SolarX Modu

int TOLERANCE = 20; //İki LDR Arasındaki Tolerans

Servo servohori;
int servoh = 0;
int servohLimitHigh = 170;  //Yatay Servo Maksimum Açısı
int servohLimitLow = 20;    //Yatay Servo Minimum Açısı

Servo servoverti;
int servov = 0;
int servovLimitHigh = 170;  //Dikey Servo Maksimum Açısı
int servovLimitLow = 30;    //Dikey Servo Minimum Açısı

//LDR pinlerinin tanımlaması
int ldrtopr = A4;  //Üst R LDR
int ldrbotr = A3;  //Alt R LDR
int ldrtopl = A1;  //Üst L LDR
int ldrbotl = A0;  //Alt L LDR

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);

  servohori.attach(11); //Yatay Servo Pini
  servohori.write(90);  //Yatay Servo Açısı
  delay(1500);
  servoverti.attach(9); //Dikey Servo Pini
  servoverti.write(90); //Dikey Servo Açısı
  Serial.begin(9600); 

  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  delay(20);
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW) {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
    delay(50);
    if (solar_mode == 1) {  //Mod 1: Başlangıç Pozisyonu
      for (int i = 0; i < 3; i++) {
        digitalWrite(led1, HIGH);
        delay(300);
        digitalWrite(led1, LOW);
        delay(300);
      }
      servohori.attach(11);
      servohori.write(90);
      delay(1500);
      servoverti.attach(9);
      servoverti.write(90);
      solar_mode++;
    } else if (solar_mode == 2) {  //Mod 2: Gün Işığı Modu
      TOLERANCE = 3;
      for (int i = 0; i < 3; i++) {
        digitalWrite(led1, HIGH);
        digitalWrite(led2, HIGH);
        delay(300);
        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
        delay(300);
      }
      solar_mode++;
    } else if (solar_mode == 3) {  //Mod 3: Flash Işığı Modu
      TOLERANCE = 20;
      for (int i = 0; i < 3; i++) {
        digitalWrite(led1, HIGH);
        digitalWrite(led2, HIGH);
        digitalWrite(led3, HIGH);
        delay(300);
        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
        digitalWrite(led3, LOW);
        delay(300);
      }
      solar_mode = 1;
    }
  }

  //Serial.println("Solar Mode:");
  //Serial.println(solar_mode);

  int solarvalue = analogRead(A6);
  int batvalue = analogRead(A7);
  //Calculating Percent Of Battery
  float voltage = batvalue * (10 / 1024.0);

  //Serial.println("solarvalue:");
  //Serial.println(solarvalue);

  //Serial.println("batvalue:");
  //Serial.println(batvalue);

  //Serial.println("voltage:");
  //Serial.println(voltage);

  //LDR'lerin Okunan Değerleri
  int topl = analogRead(ldrtopl);
  int topr = analogRead(ldrtopr);
  int botl = analogRead(ldrbotl);
  int botr = analogRead(ldrbotr);

  /*
  Serial.print(topl);
  Serial.print("\t\t");
  Serial.print(topr);
  Serial.print("\t\t");
  Serial.print(botl);
  Serial.print("\t\t");
  Serial.print(botr);
  Serial.print("\t\t");
  Serial.print(servov);
  Serial.print("\t\t");
  Serial.println(servoh);
  */
  
  //LDR'lerin Ortalama Değerleri
  int avgtop = (topl + topr) / 2;    //Üstteki LDR'ler
  int avgbot = (botl + botr) / 2;    //Alttki LDR'ler
  int avgleft = (topl + botl) / 2;   //Soldaki LDR'ler
  int avgright = (topr + botr) / 2;  //Sağdaki LDR'ler
  
/*
  Serial.print(avgtop);
  Serial.print("\t\t");
  Serial.print(avgbot);
  Serial.print("\t\t");
  Serial.print(avgleft);
  Serial.print("\t\t");
  Serial.println(avgright);
*/
  
  //Pil Doluluk Değeri
  if (voltage <= 3.3) {  //%0 - %20
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, HIGH);
    delay(500);
    digitalWrite(led4, LOW);
    delay(500);
  } else if (voltage > 3.3 && voltage <= 3.5) {  //%20 - %40
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, HIGH);
  } else if (voltage > 3.5 && voltage <= 3.75) {  //%40 - %60
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, HIGH);
  } else if (voltage > 3.75 && voltage <= 4) {  //%60 - %80
    digitalWrite(led1, LOW);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, HIGH);
  } else if (voltage > 4) {  //%80 - %100
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, HIGH);
  }

  servov = servoverti.read(); //Dikey Servoların Son Pozisyonu

  if (avgbot - avgtop > TOLERANCE) {
    if (servov <= servovLimitLow) {
      servov = servovLimitLow;
    } else
      servoverti.write(servov - 1);

  } else if (avgtop - avgbot > TOLERANCE) {
    if (servov >= servovLimitHigh) {
      servov = servovLimitHigh;
    } else
      servoverti.write(servov + 1);
  } 
  else {
    delay(5);
  }
  delay(STEP_DELAY);

  //Servo Move - Right & Left
  servoh = servohori.read();  //Yatay Servoların Son Pozisyonu

  if (avgleft - avgright > TOLERANCE) {
    if (servoh >= servohLimitHigh) {
      servoh = servohLimitHigh;
    } else
      servohori.write(servoh + 1);
  } else if (avgright - avgleft > TOLERANCE) {
    if (servoh <= servohLimitLow) {
      servoh = servohLimitLow;
    } else
      servohori.write(servoh - 1);
  } else {
    delay(5);
  }
  delay(STEP_DELAY);
}
