#include <SPI.h>
#include<nRF24L01.h>
#include <RF24.h>
#include <printf.h>
#include <Servo.h>

#define driveMotorsEn 7
#define driveMotorsIn1 6
#define driveMotorsIn2 5
#define gearMotorIn3 4
#define gearMotorIn4 3
#define gearMotorEn 2

RF24 radio(8,10);

const byte address[6] = "00001";
unsigned long lastReceiveTime = 0;
unsigned long currentTime =0;

Servo servo1;

struct Data_Package{
  byte leftJoyX;
  byte leftJoyY;
  byte buttonUp;
  byte buttonDown;
  };

Data_Package data;

int leftJoyXValue;
int leftJoyYValue;
int buttonUpValue;
int buttonDownValue;
int motorSpeedDriveMotors=0;
int motorSpeedGearMotor=0;
int servo1value;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
radio.begin();
printf_begin();
radio.openReadingPipe(0,address);
radio.setPALevel(RF24_PA_MIN);
radio.startListening();
radio.printDetails();
radio.enableDynamicPayloads();
radio.setDataRate(RF24_250KBPS);
radio.setAutoAck(false);

servo1.attach(9);
pinMode(driveMotorsEn, OUTPUT);
pinMode(driveMotorsIn1, OUTPUT);
pinMode(driveMotorsIn2, OUTPUT);
pinMode(gearMotorIn3, OUTPUT);
pinMode(gearMotorIn4, OUTPUT);
pinMode(gearMotorEn, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  /*if (radio.available()){
  char text[32] = "";
  radio.read(&text, sizeof(text));
  Serial.println(text);
  }*/
  
currentTime = millis();
if (currentTime - lastReceiveTime > 1000){
  resetData();
  }

if (radio.available()){
  radio.read(&data, sizeof(Data_Package));
  lastReceiveTime = millis();
  }
  
leftJoyXValue = data.leftJoyX;
leftJoyYValue = data.leftJoyY;
buttonUpValue = data.buttonUp;
buttonDownValue = data.buttonDown;

/*Serial.println(leftJoyXValue);*/
Serial.println(buttonUpValue);
Serial.println(buttonDownValue);

servo1value=map(data.leftJoyY, 0, 255, 0, 200);
servo1.write(servo1value);

if (leftJoyXValue < 110){
   digitalWrite(driveMotorsIn1, HIGH);
   digitalWrite(driveMotorsIn2, LOW);
   motorSpeedDriveMotors = map(leftJoyXValue, 110, 0, 0, 255);
  }

else if (leftJoyXValue>140){
   digitalWrite(driveMotorsIn1, LOW);
   digitalWrite(driveMotorsIn2, HIGH);
   motorSpeedDriveMotors = map(leftJoyXValue, 140, 255, 0, 255);
  }

else {
  motorSpeedDriveMotors=0;
  }

if (motorSpeedDriveMotors < 20) {
  motorSpeedDriveMotors = 0;
  }
analogWrite(driveMotorsEn, motorSpeedDriveMotors);

if (buttonUpValue == 0 && buttonDownValue == 1){
  digitalWrite(gearMotorIn3, HIGH);
  digitalWrite(gearMotorIn4, LOW);
  motorSpeedGearMotor=200;
  }
else if (buttonUpValue == 1 && buttonDownValue == 0){
  digitalWrite(gearMotorIn3, LOW);
  digitalWrite(gearMotorIn4, HIGH);
  motorSpeedGearMotor=200;
  }
 else{
    motorSpeedGearMotor=0;
  }

analogWrite(gearMotorEn, motorSpeedGearMotor);  

}

void resetData(){
  data.leftJoyX=127;
  data.leftJoyY=127;
  data.buttonUp=1;
  data.buttonDown=1;
  }
