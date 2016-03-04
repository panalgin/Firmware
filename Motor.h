#ifndef Motor_h
#define Motor_h

#ifndef WProgram_h
  #include "WProgram.h"
#endif

class Motor { 
  private:
    uint8_t StepPin;
    uint8_t DirPin;
    uint8_t EnablePin;

    
    int StepsPerRevolution;
    long LastSteppedAt;
    
    void SetDirection();
    
  public:
    Motor();
    Motor(uint8_t steppin, uint8_t dirpin, char axis);
    
    int MinPin;
    int MaxPin;
    
    float MicroStepRatio;
    long CurrentPosition;
    long TargetPosition;
    long StepsRemaining;
    long StepsTaken;
    long StepInterval;
    int8_t Direction;
    bool IsInverted;
    bool UseRamping;
    
    long DwellSpeed;
    long Speed;
    long MaxSpeed;
    
    long ShortDistance;
    int RampStartsAt;
    
    
    char Axis;
    
    void Initialize();
    void Prepare(long relativeTarget);
    void PrepareTo(long absoluteTarget);
    void PrepareConstant(int8_t dir);
    
    void Move(long relativeTarget);
    void MoveTo(long absoluteTarget);
    void Step();
    void JoyStep();
    void SetMaxSpeed(long whatSpeed);
    void SetDwellSpeed(long whatSpeed);
    void SetSpeed(long whatSpeed);
    long GetSpeed();
    long GetCurrentPosition();
    void SetCurrentPosition(long pos);
    void Stop();
    void ChangeDirection(int8_t dir);
    void RampDown();
    void BackOffset();
    void ForwardOffset();
};

#endif
