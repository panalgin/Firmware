#include "MotorController.h"
#include "Motor.h"
#include <plib.h>


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

char __attribute__((coherent)) ramBuff[64] ;

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

  strcpy(ramBuff, "Hahaha\r\n\0");
  prepareDMA();

  ConfigIntTimer2(T2_ON | T2_INT_PRIOR_3);
  OpenTimer2(T2_ON | T2_PS_1_16, 50000); // 10 ms
}

inline unsigned int __attribute__((always_inline)) _VirtToPhys(const void* p)
{
  return (int)p<0?((int)p&0x1fffffffL):(unsigned int)((unsigned char*)p+0x40000000L);
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
  
  sprintf(ramBuff, "X:%u,Y:%u\r\n", x1_Motor.GetCurrentPosition(), y_Motor.GetCurrentPosition());
}

void prepareDMA() {
  IEC1CLR = 0x00010000; // disable DMA channel 0 interrupts
  IFS1CLR = 0x00010000; // clear any existing DMA channel 0 interrupt flag
  DMACONSET = 0x00008000; // enable the DMA controller

  DCH0CON = 0x03; // channel off, priority 3, no chaining
  //DCH0ECON = 0; // no start irqs, no match enabled
  DCH0ECON= (28 <<8)| 0x30; // start irq is UART1 TX, pattern match enabled

  //TODO MATCH TERMINATION PATTERN
  //_UART1_TX_IRQ TX BUFFER EMPTY IRQ
  DCH0DAT = '\0'; //0x00;

  // program the transfer
  DCH0SSA=_VirtToPhys((void*)ramBuff); // transfer source physical address
  DCH0DSA=_VirtToPhys((void*)&U1TXREG); // transfer destination physical address
  DCH0SSIZ= sizeof(ramBuff); // source size at most 200 bytes
  DCH0DSIZ= 1; // dst size is 1 byte
  DCH0CSIZ= 1; // one byte per UART transfer request

  DCH0INTCLR=0x00ff00ff; // clear existing events, disable all interrupts
  DCH0INTSET=0x00090000; // enable Block Complete and error interrupts

  DCRCCON = 0;                // crc module off
  //DCH0INT = 0;                // interrupts disabled

  IPC9CLR=0x0000001f; // clear the DMA channel 0 priority and sub-priority
  //IPC9SET=0x00000016; // set IPL 5, sub-priority 2
  IPC9bits.DMA0IP = 0;
  IPC9bits.DMA0IS = 0;
    
  IEC1SET=0x00010000; // enable DMA channel 0 interrupt

  DCH0CONbits.CHAEN = 1; //auto enable again
  DCH0CONSET=0x80; // turn channel on
  DCH0ECONbits.CFORCE = 1; //force to run 
}

extern "C"
{
  void __ISR(_TIMER_2_VECTOR,ipl3) playSam(void)
  {
    // initiate a transfer
    /*DCH0INTCLR=0x00ff00ff; // clear existing events disable all interrupts
     
     DCH0CONSET=0x80; // turn channel on
     DCH0ECONSET=0x00000080; // set CFORCE to 1*/

    mT2ClearIntFlag();  // Clear interrupt flag
  }
} // end extern "C"








