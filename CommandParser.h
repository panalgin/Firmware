#ifndef CommandParser_h
#define CommandParser_h

#ifndef WProgram_h
#include "WProgram.h"
#endif
class CommandParser { 
private:

public:
  CommandParser();

  void Parse(String& text);
};

#endif

