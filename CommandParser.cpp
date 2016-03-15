#include "CommandParser.h"

CommandParser::CommandParser() {

}

void CommandParser::Parse(String& text) {
  if (text.startsWith("Hello ")) {
    Serial.println(text);
  }
  if (text.startsWith("Jog: ")) { // format Jog: A:-6000
     text.replace("Jog: ", "");
     
     char axis = text[0];
     text.replace(axis + ":", "");
     
     unsigned long value = strtoul(text.toCharArray(), NULL, 0);
     
  }
}
