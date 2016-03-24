#include "DmaController.h"
#ifndef MotorController_h
  #include "MotorController.h"
#endif
#ifndef Settings_h
  #include "Settings.h"
#endif

extern MotorController m_Controller;
extern DmaController m_DmaController;

unsigned long oldx2Pos = 0;

uint32_t Callback(uint32_t currentTime) {
  if (DCH0CONbits.CHBUSY == 0) {
    Motor *x2 = &m_Controller.Motors[3];
    
    unsigned long x2Pos = x2->GetCurrentPosition();

    if (x2Pos != oldx2Pos) {
      sprintf(RAMBUFF, "Pos: %c: %u\n", x2->Axis, x2Pos);
      oldx2Pos = x2Pos;
      m_DmaController->RunChannel();
    }
  }

  return (currentTime + CORE_TICK_RATE * 40); // 40 ms
}

inline unsigned int __attribute__((always_inline)) _VirtToPhys(const void* p)
{
  return (int)p<0?((int)p&0x1fffffffL):(unsigned int)((unsigned char*)p+0x40000000L);
}

//////////// INSTANCE 

DmaController::DmaController() {

}

void DmaController::Initialize() {
  attachCoreTimerService(Callback);
}

static void DmaController::RunChannel() {
  IEC1CLR = 0x00010000; // disable DMA channel 0 interrupts
  IFS1CLR = 0x00010000; // clear any existing DMA channel 0 interrupt flag
  DMACONSET = 0x00008000; // enable the DMA controller

  DCH0CON = 0x03; // channel off, priority 3, no chaining
  //DCH0ECON = 0; // no start irqs, no match enabled
  DCH0ECON= (28 <<8)| 0x30; // start irq is UART1 TX, pattern match enabled

  //TODO MATCH TERMINATION PATTERN
  //_UART1_TX_IRQ TX BUFFER EMPTY IRQ
  DCH0DAT = 0x00; //0x00;

  // program the transfer
  DCH0SSA=_VirtToPhys((void*)RAMBUFF); // transfer source physical address
  DCH0DSA=_VirtToPhys((void*)&U1TXREG); // transfer destination physical address
  DCH0SSIZ= sizeof(RAMBUFF); // source size at most 200 bytes
  DCH0DSIZ= 1; // dst size is 1 byte
  DCH0CSIZ= 1; // one byte per UART transfer request

  DCH0INTCLR=0x00ff00ff; // clear existing events, disable all interrupts
  DCH0INTSET=0x00090000; // enable Block Complete and error interrupts

  DCRCCON = 0;                // crc module off
  //DCH0INT = 0;                // interrupts disabled

  IPC9CLR=0x0000001f; // clear the DMA channel 0 priority and sub-priority
  //IPC9SET=0x00000016; // set IPL 5, sub-priority 2
  IPC9bits.DMA0IP = 0; // low priority
  IPC9bits.DMA0IS = 0; // low subpriorty

  IEC1SET=0x00010000; // enable DMA channel 0 interrupt

  //DCH0CONbits.CHAEN = 1; //auto enable again
  DCH0CONSET=0x80; // turn channel on
  DCH0ECONbits.CFORCE = 1; //force to run 
}
