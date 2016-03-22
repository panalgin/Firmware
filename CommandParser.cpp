#include "CommandParser.h"

CommandParser::CommandParser() {

}

void CommandParser::Parse(String& text) {
  if (text.startsWith("Hello ")) {
    Serial.println(text);
  }
  if (text.startsWith("JogStart: ")) { // format Jog: A:-6000
     text.replace("JogStart: ", "");
     
     char axis = text[0];
     text.replace(axis + ":", "");
     unsigned int length = text.length();
     char buffer[length];
     text.toCharArray(buffer, length);
     
     unsigned long value = strtoul(buffer, 0, 0);
  }
}
