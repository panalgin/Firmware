#include "MotorController.h"

MotorController::MotorController() {

}

void MotorController::Initialize() {
  this->UseRamp = true;
}

void MotorController::Move(long steps, Motor& motor) {
  long absolute = motor.GetCurrentPosition() + steps;
  this->MoveTo(absolute, motor);
}

void MotorController::MoveTo(long absolute, Motor& motor) {
  long curPos = motor.GetCurrentPosition();

  if (curPos != absolute) {
    long delta = abs(curPos - absolute);
    motor.PrepareTo(absolute);

    long accelIndex = 0;

    while(motor.StepsRemaining > 0) {
      if (motor.UseRamping)
        this->CalculateRamp(delta, accelIndex, motor);

      motor.Step();
      accelIndex++;
    }

    motor.SetSpeed(motor.MaxSpeed);
  }
}

void MotorController::LinearMove(long steps1, long steps2, Motor& first, Motor& second) {
  long pos1 = first.GetCurrentPosition() + steps1;
  long pos2 = second.GetCurrentPosition() + steps2;

  this->LinearMoveTo(pos1, pos2, first, second);
}

void MotorController::LinearMoveTo(long pos1, long pos2, Motor& first, Motor& second) {  
  first.PrepareTo(pos1);
  second.PrepareTo(pos2);

  long m_DeltaX = first.StepsRemaining;
  long m_DeltaY = second.StepsRemaining;

  long m_Index;
  long m_Over = 0;

  if (m_DeltaX > m_DeltaY) {    
    for(m_Index = 0; m_Index < m_DeltaX; ++m_Index) { 
      if (first.UseRamping)     
        this->CalculateRamp(m_DeltaX, m_Index, first);

      first.Step();
      m_Over += m_DeltaY;

      if (m_Over >= m_DeltaX) {
        m_Over -= m_DeltaX;
        second.Step();
      }
    }

    first.SetSpeed(first.MaxSpeed);
  }
  else {
    for(m_Index = 0; m_Index < m_DeltaY; ++m_Index) { 
      if (second.UseRamping)     
        this->CalculateRamp(m_DeltaY, m_Index, second);

      second.Step();
      m_Over += m_DeltaX;

      if (m_Over >= m_DeltaY) {
        m_Over -= m_DeltaY;
        first.Step();
      }
    }

    second.SetSpeed(second.MaxSpeed);
  }
}

void MotorController::BackOffset() {
  for(uint8_t i = 0; i < MOTORS_COUNT; i++) {
    Motor & t_Motor = this->Motors[i];
    
    t_Motor.PrepareConstant(-1); // reverse

    uint8_t t_MinSwitchState = LOW;
    t_MinSwitchState = digitalRead(t_Motor.MinPin);
    
    bool t_IsOffsetted = false;
    unsigned long t_AccelIndex = 0;

    if (t_MinSwitchState == HIGH) {
      while(t_IsOffsetted == false) {
        if (t_IsOffsetted == false) {
          if (t_MinSwitchState == HIGH) {
            t_MinSwitchState = digitalRead(t_Motor.MinPin);
            this->CalculateRamp(t_AccelIndex, t_Motor);
            t_Motor.Step();
            t_AccelIndex++;
          }
          else {
            t_IsOffsetted = true;
            t_Motor.RampDown(); 
          }
        }
      } 
    }    

    t_Motor.BackOffset();
  }
}

void MotorController::ForwardOffset() {
  for(uint8_t i = 0; i < MOTORS_COUNT; i++) {
    Motor & t_Motor = this->Motors[i];
    
    t_Motor.PrepareConstant(1); // reverse

    uint8_t t_MaxSwitchState = LOW;
    t_MaxSwitchState = digitalRead(t_Motor.MaxPin);
    
    bool t_IsOffsetted = false;
    unsigned long t_AccelIndex = 0;

    if (t_MaxSwitchState == HIGH) {
      while(t_IsOffsetted == false) {
        if (t_IsOffsetted == false) {
          if (t_MaxSwitchState == HIGH) {
            t_MaxSwitchState = digitalRead(t_Motor.MaxPin);
            this->CalculateRamp(t_AccelIndex, t_Motor);
            t_Motor.Step();
            t_AccelIndex++;
          }
          else {
            t_IsOffsetted = true;
            t_Motor.RampDown(); 
          }
        }
      } 
    }    

    t_Motor.ForwardOffset();
  }
}

void MotorController::CalculateRamp(unsigned long index, Motor& motor) {
  this->CalculateRamp(50000000, index, motor);
}

void MotorController::CalculateRamp(unsigned long delta, unsigned long index, Motor& motor) {
  if (this->UseRamp) {
    float currentRpm = 1.0;
    long accelerationEndsAt = motor.ShortDistance / 2; //(long)(m_DeltaY * 0.10);
    long decelerationStartsAt = delta - (motor.ShortDistance / 2); //(long)(m_DeltaY * 0.90);
    int maxRpm = max(1, motor.MaxSpeed);
    int minRpm = max(1, motor.RampStartsAt);

    if (delta < motor.ShortDistance) {
      if (index <= 0) { // performance reasons
        motor.SetSpeed(motor.DwellSpeed); 
      }
    }
    else {
      if (index <= accelerationEndsAt || index >= decelerationStartsAt) {
        if (index <= accelerationEndsAt) {
          currentRpm = max(1, minRpm + ((maxRpm - minRpm) * ((float)index / (float)accelerationEndsAt)));
          motor.SetSpeed(currentRpm);
        }
        else if (index >= decelerationStartsAt) {
          currentRpm = max(1, minRpm + ((maxRpm - minRpm) * (1 - ((float)index / (float)delta)) / (1 - ((float)decelerationStartsAt / (float)delta))));
          motor.SetSpeed(currentRpm);
        }
        else
          motor.SetSpeed(maxRpm);
      }
    }
  }
}

