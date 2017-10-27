#include "HX711.h"

//dinh nghia chan cho 3 led
#define LedVang 5
#define LedDo 4
#define LedXanh 6

//dat thong so cho can
int dat = 17781;
int kl;
int Weight;
int khoiluongmax = 1000;//1kg loadcell
int check = 0;

//che do sleep
#include <avr/sleep.h>
unsigned long time;//kiem tra thoi gian => vao sleepmode
unsigned long timesave;//kiem tra thoi gian => vao sleepmode
int TimeToSleep = 10000; //thoi gian toi da de vao sleep mode (milliseconds)
int wakePin = 2; // pin used for waking up

//mode 3: tang trong luong len 1 nguong moi
int tangkhoiluong = 700; //nguong moi = 700g => bat led xanh
boolean checktangkhoiluong = false;

boolean hieuchinh = false;
//dua can ve so 0
void hieuchinhcan() {

  Serial.println("Dang hieu chinh can");

  do {
    Serial.print(".");
    Weight = Get_Weight();
    dat = Weight;

    kl = dat - Weight;

    Serial.print("Khoi luong dang hieu chinh: ");
    Serial.println(kl);

    if (kl == 0) hieuchinh = true;
  } while (!hieuchinh);

  Serial.println("");
  Serial.println(dat);
}

void wakeUpNow() {
}

void setup()
{
  pinMode(LedDo, OUTPUT);
  pinMode(LedVang, OUTPUT);
  pinMode(LedXanh, OUTPUT);

  //tat 3 led
  digitalWrite(LedDo, LOW);
  digitalWrite(LedVang, LOW);
  digitalWrite(LedXanh, LOW);

  Serial.begin(9600);
  delay(100);

  pinMode(wakePin, INPUT_PULLUP);
  attachInterrupt(0, wakeUpNow, LOW); // use interrupt 0 (pin 2) and run function wakeUpNow when pin 2 gets LOW

  //khoi tao can
  Init_Hx711();
  Get_Maopi();

  //hieu chinh can => dua can ve vi tri 0
  hieuchinhcan();
  Serial.println("Vui long dat vat the can do");
}

void sleepNow() {
  Serial.println("Sleep");
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);// sleep mode is set here  
  sleep_enable();// enables the sleep bit in the mcucr register  
  attachInterrupt(0, wakeUpNow, LOW); // use interrupt 0 (pin 2) and run function  
  sleep_mode();// here the device is actually put to sleep!!  
  // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
  sleep_disable();// first thing after waking from sleep: disable sleep...  
  detachInterrupt(0);// disables interrupt 0 on pin 2 so the wakeUpNow code will not be executed during normal running time.
}


void loop()
{
  //doc can nang
  Weight = Get_Weight();
  kl = dat - Weight;

  /* ==============================================================================
     MODE 3:
     Neu Khoi luong do duoc = Tang khoi luong (khoi luong dat) => bat led xanh la cay.
    =================================================================================*/
  while (kl >= tangkhoiluong && kl < khoiluongmax && checktangkhoiluong == true) {
    timesave = millis();
    time = millis();

    while (((time - timesave) < TimeToSleep) && kl >= tangkhoiluong && kl < khoiluongmax) {
      time = millis(); //bat dau tinh thoi gian vao mode 1
      Serial.println(time);

      Serial.println("Mode 3");

      Weight = Get_Weight();
      delay(100);
      kl = dat - Weight;

      //giam khoi luong
      if (kl < tangkhoiluong) checktangkhoiluong = false;

      digitalWrite(LedXanh, HIGH);
      digitalWrite(LedVang, LOW);

      if ((time - timesave) >= TimeToSleep) {
        sleepNow();// sleep function called here
      }
    }
  }

  /* ========================================
     MODE 2
     Co vat the dat tren can => bat led Vang.
    =========================================*/
  while (kl != 0 && kl < tangkhoiluong) {
    timesave = millis();
    time = millis();

    while (((time - timesave) < TimeToSleep) && kl != 0 && kl < tangkhoiluong) {
      time = millis(); //bat dau tinh thoi gian vao mode 1
      Serial.println(time);

      Serial.println("mode 2");
      digitalWrite(LedXanh, LOW);
      digitalWrite(LedVang, HIGH);

      Weight = Get_Weight();
      kl = dat - Weight;

      checktangkhoiluong = true;

      if (kl != check) {
        check = kl;
        Serial.print("Weight:");
        Serial.print(kl);
        Serial.println("g");
      }

      if ((time - timesave) >= TimeToSleep) {
        sleepNow();// sleep function called here
      }
    }
  }

  /* =====================================================
     MODE 1:
     Khi khong co vat the dat tren can => led do nhap nhay
    ========================================================*/
  while (kl == 0) {
    timesave = millis();
    time = millis(); //bat dau tinh thoi gian vao mode 1

    while (((time - timesave) < TimeToSleep) && kl == 0) {
      time = millis(); //bat dau tinh thoi gian vao mode 1
      Serial.println(time);

      digitalWrite(LedDo, HIGH);
      digitalWrite(LedVang, LOW);
      digitalWrite(LedXanh, LOW);
      delay(100);
      digitalWrite(LedDo, LOW);
      delay(100);

      Weight = Get_Weight();
      kl = dat - Weight;

      if ((time - timesave) >= TimeToSleep) {
        sleepNow();// sleep function called here
      }
    }
  }

  /* ==============================================
      MODE 4: Truong hop can qua tai
     3 led vang, xanh la cay, do nhap nhay lien tuc
    ================================================*/
  while (kl >= khoiluongmax) {
    Serial.println("Qua Tai");

    Weight = Get_Weight();
    kl = dat - Weight;
    digitalWrite(LedDo, HIGH);
    digitalWrite(LedVang, HIGH);
    digitalWrite(LedXanh, HIGH);
    delay(100);
    digitalWrite(LedDo, LOW);
    digitalWrite(LedVang, LOW);
    digitalWrite(LedXanh, LOW);
    delay(100);
  }
}

