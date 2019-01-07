#include "Globals.h"

const bool DEBUG = false;
String mqtt_topic = "SB/API/";
String MAC;

// TIMING Variables
unsigned long currentMillis, previousMillis;
const int updateInterval = 5000;