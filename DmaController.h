#ifndef DmaController_h
#define DmaController_h

#ifndef WProgram_h
#include "WProgram.h"
#endif
class DmaController { 
private:

public:
  DmaController();

  void Initialize();
  void RunChannel();
};

#endif

