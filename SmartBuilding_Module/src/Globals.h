#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>

extern const bool DEBUG;
extern String mqtt_topic;
extern String MAC;
extern unsigned long currentMillis, previousMillis;
extern const int updateInterval = 5000;

#endif