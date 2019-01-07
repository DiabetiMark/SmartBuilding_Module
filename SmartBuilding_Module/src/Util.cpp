#include "Util.h"

// This method prints a message to the Serial when debug mode is enabled.
void Util::PrintMessage(String msg){
    if(DEBUG)
    {
        Serial.println(msg);
    }
}

char* Util::StringToChar(String msg)
{
    if(msg.length() != 0) {
        char *output = const_cast<char*>(msg.c_str());
        return output;
    }
}