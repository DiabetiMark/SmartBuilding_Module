#ifndef SENSORMAPPER_H
#define SENSORMAPPER_H

#include "Globals.h"
#include <DHT.h>
#include <ArduinoJson.h>

#define DHTTYPE DHT22
#define SENSPIN_DHT 5
#define SENSPIN_REED 16
#define SENSPIN_METHANE 17
#define SENSPIN_MOTION 18

#define DATA_NAME 0
#define DATA_FORMAT 1
#define DATA_VALUE 2
#define DATA_VARIABLE_COUNT 3

class SensorMapper
{
    private:
        // Variables
        static const int sPinsSize = 10;
        bool SENSOR_MOTION_STATE = false;
        int sensorPins[sPinsSize] = {SENSPIN_DHT, SENSPIN_REED, SENSPIN_METHANE, SENSPIN_MOTION};
        bool connectedPins[sPinsSize];
        DHT dht = DHT(SENSPIN_DHT, DHTTYPE);
        DynamicJsonBuffer jsonBuffer;

        // Methods
        String JSON_SensorObject(String sensorID, String sensorName, int dataCount, String dataSet[][DATA_VARIABLE_COUNT]);
    public:
        // Methods
        void Map();
        String readSensors();
        void SetSensorMotionState(bool state) { SENSOR_MOTION_STATE = state; }
};

#endif