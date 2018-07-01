/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include<Servo.h>

extern "C" {
#include "user_interface.h"
}

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "8bb1146283ff47b7a6931b48f8c60f91";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "PSTORM_C";
char pass[] = "1234asdf";

#define RESETPOS 0
#define TRAY1POS 1
#define TRAY2POS 2
#define TRAY3POS 3

#define WIFI_RETRIES 100
Servo mimiServo;

BlynkTimer timer;
int timerID;

int servo_pin=D1;

WidgetLED status_led(V3);

#define TRAYANGLE 10
#define TRAYANGLEOFFSET 110
#define SEELP_SEC 5
#define WAKEUP_SEC 10 
#define WORK_SEC 60
#define BUILTIN_LED D0
int angle;
bool tray_flag=false;
bool first=true;

BLYNK_WRITE(V1){
  if(!first){
    if(param.asInt()){
      if(!tray_flag){
        goTray1();
        Serial.println("tray1");
        tray_flag=true;
        timer.deleteTimer(timerID);
        timerID= timer.setInterval(WORK_SEC*1000L, goToSleep); //wifi off after 60 seconds 
      }
      
    }else{
      if(tray_flag){
        goTray2();
        Serial.println("tray2");
        tray_flag=false;
        timer.deleteTimer(timerID);
        timerID= timer.setInterval(WORK_SEC*1000L, goToSleep); //wifi off after 60 seconds 
      }
    }
  first=false; 
  }
}

BLYNK_CONNECTED() {     
  Blynk.syncAll(); // Request Blynk server to re-send latest values for all pins
  if(!status_led.getValue())status_led.on();
  LedBlink(2);
  
}

void setup() {
  // put your setup code here, to run once:  
   Serial.begin(9600);
   //resetPosition();
   Blynk.begin(auth, ssid, pass);
  timerID= timer.setInterval(WORK_SEC*1000L, goToSleep); //wifi off after 60 seconds   
  status_led.on();
  pinMode(BUILTIN_LED,OUTPUT);
  LedBlink(10);
  first=false;

}

void loop() {
  // put your main code here, to run repeatedly:

   Blynk.run();
   timer.run();
   
   
}

void goTray1(){
  mimiServo.attach(servo_pin);
  delay(200);
  angle=TRAYANGLE;
  mimiServo.write(angle);
  delay(1000);
  shakyshaky();
  mimiServo.detach();
}
void goTray2(){
  mimiServo.attach(servo_pin);
  delay(200);
  angle=TRAYANGLE+TRAYANGLEOFFSET;
  mimiServo.write(angle);
  delay(1000);
  shakyshaky();
  mimiServo.detach();  
}

void shakyshaky(){

  mimiServo.write(angle+10);
  delay(200);  
  mimiServo.write(angle-10);
  delay(200);   
  mimiServo.write(angle+10);
  delay(200);  
  mimiServo.write(angle-10);
  delay(200);    
}

void wakeup(){

  Serial.println("wake up");
  timer.deleteTimer(timerID); 
  WiFi_On(); // wake up and connect need at least 3 seconds
  timerID=timer.setInterval(WAKEUP_SEC*1000L, goToSleep); //goto sleep after 10 seconds
   
}
void goToSleep(){

  Serial.println("goto deep sleep");
  if(status_led.getValue())status_led.off();
  LedBlink(1);
  WiFi_Off();
  timer.deleteTimer(timerID);
  timerID=timer.setInterval(SEELP_SEC*1000L, wakeup); //wakeup after 5 seconds 

}

bool WiFi_Off() {
  
  int conn_tries = 0;
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  wifi_set_sleep_type(MODEM_SLEEP_T);

  while ((WiFi.status() == WL_CONNECTED) && (conn_tries++ < WIFI_RETRIES)) {
    delay(100); 
  }
  if (WiFi.status() != WL_CONNECTED)
    return (true);
  else
    return (false);


    
}

bool WiFi_On() {
  
  int conn_tries = 0;

  //WiFi.forceSleepWake();
  WiFi.mode(WIFI_STA);
  wifi_station_connect();
  WiFi.begin(ssid, pass);

  while ((WiFi.status() != WL_CONNECTED) && (conn_tries++ < WIFI_RETRIES)) {
  delay(100);

  }
  if (WiFi.status() == WL_CONNECTED)
    return (true);
  else
    return (false);
    

  return true;
}

void LedBlink(int num){
  for(int i=0;i<num;i++){
    LedOn();
    delay(50);
    LedOff();
    delay(50);
  }
}
void LedOn(){
  digitalWrite(BUILTIN_LED,LOW);
}
void LedOff(){
  digitalWrite(BUILTIN_LED,HIGH);
  
}

