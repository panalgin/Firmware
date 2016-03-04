#include "Motor.h"

Motor::Motor() {
}

Motor::Motor(uint8_t steppin, uint8_t dirpin, char axis) {
  this->StepPin = steppin;
  this->DirPin = dirpin;
  this->Axis = axis;

}

void Motor::SetDirection() {
  if (this->Direction == 1)
    digitalWrite(this->DirPin, this->IsInverted ? LOW : HIGH);
  else
    digitalWrite(this->DirPin, this->IsInverted ? HIGH : LOW);
}

void Motor::ChangeDirection(int8_t dir) {
  if (this->Direction != dir) {
    this->Direction = dir; 
    this->SetDirection();
  }
}

void Motor::Initialize() {
  this->StepsPerRevolution = 200;
  this->Direction = -1;

  this->MicroStepRatio = 50;
  this->CurrentPosition = 0;
  this->TargetPosition = 0;
  this->StepsRemaining = 0;
  this->StepsTaken = 0;
  this->LastSteppedAt = micros() - (this->StepInterval * 2); // make it ready to step instantly on first loop cycle
  this->UseRamping = true;

  pinMode(this->StepPin, OUTPUT);
  pinMode(this->DirPin, OUTPUT);

  if (this->MinPin > 0)
    pinMode(this->MinPin, INPUT);

  if (this->MaxPin > 0)
    pinMode(this->MaxPin, INPUT);

  digitalWrite(this->StepPin, LOW);
  digitalWrite(this->DirPin, LOW);
}

void Motor::Prepare(long relativeTarget) {
  this->TargetPosition = this->CurrentPosition + relativeTarget;
  this->StepsRemaining = abs(this->TargetPosition - this->CurrentPosition);
  this->StepsTaken = 0;

  if (this->TargetPosition < this->CurrentPosition)
    this->Direction = -1;
  else
    this->Direction = 1;

  this->SetDirection();
}

void Motor::PrepareTo(long absoluteTarget) {
  this->TargetPosition = absoluteTarget;
  this->StepsRemaining = abs(this->TargetPosition - this->CurrentPosition);
  this->StepsTaken = 0;

  if (this->TargetPosition < this->CurrentPosition)
    this->Direction = -1;
  else
    this->Direction = 1;

  this->SetDirection();
}

void Motor::PrepareConstant(int8_t dir) {
  this->TargetPosition = 0;
  this->StepsRemaining = 0;
  this->StepsTaken = 0;

  this->Direction = dir;
  this->SetDirection();
}

void Motor::Stop() {
  this->StepsRemaining = 0;
  this->StepsTaken = 0;
}

void Motor::Move(long relativeTarget) {
  this->Prepare(relativeTarget);

  while(this->StepsRemaining > 0) {
    this->Step();
  }
}

void Motor::MoveTo(long absoluteTarget) {
  this->PrepareTo(absoluteTarget);

  while(this->StepsRemaining > 0) {
    this->Step();
  }
}

void Motor::Step() {
  while(true) {
    if (micros() - this->LastSteppedAt >= this->StepInterval) {
      this->LastSteppedAt = micros();

      digitalWrite(this->StepPin, HIGH);
      this->StepsTaken++;
      delayMicroseconds(1);
      digitalWrite(this->StepPin, LOW);
      this->CurrentPosition += this->Direction;
      this->StepsRemaining--;

      break;
    }
  }
}

void Motor::JoyStep() {
  if (micros() - this->LastSteppedAt >= this->StepInterval) {
    this->LastSteppedAt = micros();

    digitalWrite(this->StepPin, HIGH);
    delayMicroseconds(1);
    digitalWrite(this->StepPin, LOW);
    this->CurrentPosition += this->Direction;
  }
}

long Motor::GetCurrentPosition() {
  return this->CurrentPosition;
}

void Motor::SetCurrentPosition(long pos) {
  this->CurrentPosition = pos;
}

long Motor::GetSpeed() {
  return this->Speed;
}

void Motor::SetSpeed(long whatSpeed) {
  if (this->Speed != whatSpeed) {
    this->StepInterval = 60000000 / (this->StepsPerRevolution * this->MicroStepRatio) / whatSpeed;
    this->Speed = whatSpeed;
  }
}

void Motor::SetDwellSpeed(long whatSpeed) {
  if (this->DwellSpeed != whatSpeed)
    this->DwellSpeed = whatSpeed;
}

void Motor::SetMaxSpeed(long whatSpeed) {
  if (this->MaxSpeed != whatSpeed) 
    this->MaxSpeed = whatSpeed;
}

void Motor::RampDown() {
  long rampIndex = 2800;
  long currentRpm = 1;
  uint8_t minRpm = 5;
  uint8_t rampTheresold = 14;

  if (this->Axis == 'Y') {
    rampIndex = 5000;
    rampTheresold = 34;
  }
  
  if (this->Axis == 'Z') {
    rampTheresold = 7;
  }
  if (this->Axis == 'B')
    rampTheresold = 10;

  while(rampIndex > 0) {
    rampIndex--;

    currentRpm = max(minRpm, rampIndex / rampTheresold);

    if (this->Speed >= currentRpm) // prevent slow to high brake
      this->SetSpeed(currentRpm);

    this->Step();

    if (this->Speed < minRpm) { // it can go reverse now
      delay(500);
      break;
    }
  }
}

void Motor::BackOffset() {
  int SwitchState = digitalRead(this->MinPin);
  this->ChangeDirection(1);
  this->SetSpeed(this->DwellSpeed);

  while(SwitchState == LOW) {
    this->Step();

    SwitchState = digitalRead(this->MinPin);
  }

  this->SetCurrentPosition(0);
}

void Motor::ForwardOffset() {
  int SwitchState = digitalRead(this->MaxPin);
  this->ChangeDirection(-1);
  this->SetSpeed(this->DwellSpeed);

  while(SwitchState == LOW) {
    this->Step();

    SwitchState = digitalRead(this->MaxPin);
  }

  Serial.print(this->Axis);
  Serial.print(" Motoru: ");
  Serial.println(this->GetCurrentPosition());
  
  //this->SetCurrentPosition(0);
}

