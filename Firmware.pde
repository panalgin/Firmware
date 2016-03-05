#include "MotorController.h"
#include "Motor.h"

MotorController m_Controller;

Motor x1_Motor(9, 31, 'X');
Motor y_Motor(5, 25, 'Y');
Motor z1_Motor(6, 35, 'Z');
Motor x2_Motor(10, 33, 'A');
Motor z2_Motor(77, 29, 'B');

int X1StartSensor = A5;
int X1EndSensor = A11;

int X2StartSensor = A13;
int X2EndSensor = A7;

int Z1StartSensor = A8;
int Z1EndSensor = A12;

int Z2StartSensor = A6;
int Z2EndSensor = A4;

int YStartSensor = A9;
int YEndSensor = A10;

int TestSensor = 49;

char PosInfo[64];

void setup() {
  Serial.begin(115200);
  delay(20);

  pinMode(TestSensor, INPUT);

  m_Controller.Initialize();

  x1_Motor.IsInverted = true;
  x1_Motor.SetDwellSpeed(5);
  x1_Motor.ShortDistance = 20000;
  x1_Motor.RampStartsAt = x1_Motor.MaxSpeed * 0.1;
  x1_Motor.MinPin = X1StartSensor;
  x1_Motor.MaxPin = X1EndSensor;
  x1_Motor.Initialize();

  x2_Motor.IsInverted = false;
  x2_Motor.SetDwellSpeed(5);
  x2_Motor.ShortDistance = 20000;
  x2_Motor.RampStartsAt = x2_Motor.MaxSpeed * 0.1;
  x2_Motor.MinPin = X2StartSensor;
  x2_Motor.MaxPin = X2EndSensor;
  x2_Motor.Initialize();

  z1_Motor.IsInverted = false;
  z1_Motor.SetDwellSpeed(5);
  z1_Motor.ShortDistance = 20000;
  z1_Motor.RampStartsAt = z1_Motor.MaxSpeed * 0.1;
  z1_Motor.MinPin = Z1StartSensor;
  z1_Motor.MaxPin = Z1EndSensor;
  z1_Motor.Initialize();

  z2_Motor.IsInverted = false;
  z2_Motor.SetDwellSpeed(50);
  z2_Motor.ShortDistance = 20000;
  z2_Motor.RampStartsAt = z2_Motor.MaxSpeed * 0.1;
  z2_Motor.MinPin = Z2StartSensor;
  z2_Motor.MaxPin = Z2EndSensor;
  z2_Motor.Initialize();

  y_Motor.IsInverted = false;
  y_Motor.SetDwellSpeed(5);
  y_Motor.ShortDistance = 40000;
  y_Motor.RampStartsAt = 10;
  y_Motor.MinPin = YStartSensor;
  y_Motor.MaxPin = YEndSensor;
  y_Motor.Initialize();

  x1_Motor.SetMaxSpeed(200);
  x2_Motor.SetMaxSpeed(200);
  z1_Motor.SetMaxSpeed(400);
  z2_Motor.SetMaxSpeed(300);
  y_Motor.SetMaxSpeed(150);

  x1_Motor.SetSpeed(200);
  x2_Motor.SetSpeed(200);
  z1_Motor.SetSpeed(400);
  z2_Motor.SetSpeed(300);
  y_Motor.SetSpeed(150);

  m_Controller.Motors[0] = z1_Motor;
  m_Controller.Motors[1] = z2_Motor;
  m_Controller.Motors[2] = x1_Motor;
  m_Controller.Motors[3] = x2_Motor;
  m_Controller.Motors[4] = y_Motor;

  /*x1_Motor.SetSpeed(200);
   x2_Motor.SetSpeed(200);
   z1_Motor.SetSpeed(300);
   z2_Motor.SetSpeed(200);
   y_Motor.SetSpeed(300);*/

  delay(1000);

  //m_Controller.LinearMove(90000, 60000, z2_Motor, x2_Motor);
  //m_Controller.LinearMove(-90000, -60000, z2_Motor, x2_Motor);


  //m_Controller.ForwardOffset();
  //m_Controller.BackOffset();

  //attachCoreTimerService(MyCallback);
}

String incomingData = "";
void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    incomingData += c;
  }
  if (incomingData.endsWith(";")) {
    incomingData.replace(";", "");
    Serial.println(incomingData); 
    incomingData = "";
  }
}




