#include "CommandParser.h"

CommandParser::CommandParser() {

}

void CommandParser::Parse(String& text) {
  if (text.startsWith("Hello: ")) {
    Serial.println(text);
  }
  if (text.startsWith("JogStart: ")) { // format JogStart: A:-6000
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
  }
}
