#include "CommandParser.h"
#ifndef MotorController_h
  #include "MotorController.h"
#endif

extern MotorController m_Controller;

CommandParser::CommandParser() {

}

void CommandParser::Parse(String& text) {
  if (text.startsWith("Hello: ")) {
    Serial.println(text);
  }
  else if (text.startsWith("JogStart: ")) { // format JogStart: A:-6000
     text.replace("JogStart: ", "");
     
     char axis = text[0];
     char rep[3] = "";
     rep[0] = axis;
     strcat(rep, ":\0");
     
     text.replace(rep, "");
     
     unsigned int length = text.length() + 1;
     char buffer[length];
     text.toCharArray(buffer, length);
     
     char* needle;
     long value = strtol(buffer, &needle, 10);
     
     m_Controller.JogMove(axis, value);
  }
  else if (text.startsWith("JogEnd: ")) { // format JogEnd: All
    m_Controller.Halt();
  }
}
