#include "SensorMapper.h"
#include "Util.h"

void SensorMapper::Map()
{
    Util::PrintMessage("[SensorMapper] Commencing sensor map!");

    // Loop through each sensorPin
    for(int i = 0; i < sPinsSize; i++)
    {
        int pin = sensorPins[i];

        // Check whether an actual pin as been assigned. (0 isn't a pin)
        if(pin != 0)
        {
            Util::PrintMessage("[SensorMapper] Checking listed pin: " + (String)pin);
            pinMode(pin, INPUT_PULLUP);
            delay(50);
            if(!digitalRead(pin))
            {
                // Connected
                connectedPins[i] = true;
                pinMode(i, INPUT);

                // Set required variable if pin is connected.
                if(pin == SENSPIN_DHT){
                    dht.begin();
                }
            }
            else
            {
                // Set pinmode to OUTPUT to conserve energy.
                pinMode(i, OUTPUT);
            }
        }
    }

    pinMode(22, OUTPUT);
    digitalWrite(22, HIGH);
    Util::PrintMessage("=================");
    for(int i = 0; i < sPinsSize; i++)
    {
        if(connectedPins[i])
        {
            Util::PrintMessage("[SensorMapper] Pin [" + (String)sensorPins[i] + "] is connected to the module!");
        }
    }
}

String SensorMapper::readSensors()
{
    // JSON header
    JsonObject& root = jsonBuffer.createObject();
    JsonArray& SENSORS = root.createNestedArray("SENSORS");
    StaticJsonBuffer<2000> globalBuffer;

    for(int i = 0; i < 4; i++)
    {
        // Check whether the pin mapper found a connection to read
        if(connectedPins[i] != 0)
        {
            int sPin = sensorPins[i];
            switch(sPin){
                case SENSPIN_DHT:
                    {
                        // Read Temperature
                        String dataSet[2][DATA_VARIABLE_COUNT];
                        dataSet[0][DATA_NAME] = "Temperature";
                        dataSet[0][DATA_FORMAT] = "DECIMAL";
                        float temp = dht.readTemperature();
                        dataSet[0][DATA_VALUE] = isnan(temp) ? -1 : temp;

                        // Read Humidity
                        dataSet[1][DATA_NAME] = "Humidity";
                        dataSet[1][DATA_FORMAT] = "DECIMAL";
                        float humid = dht.readHumidity();
                        dataSet[1][DATA_VALUE] = isnan(humid) ? -1 : humid;

                        JsonObject& obj = globalBuffer.parseObject(JSON_SensorObject("DHT_01", "DHT22", 2, dataSet));
                        SENSORS.add(obj);
                    }
                    break;
                case SENSPIN_REED:
                    {
                        // Read reed sensor
                        String dataSet1[1][DATA_VARIABLE_COUNT];
                        dataSet1[0][DATA_NAME] = "Status";
                        dataSet1[0][DATA_FORMAT] = "BOOL";
                        dataSet1[0][DATA_VALUE] = digitalRead(SENSPIN_REED);

                        JsonObject& obj1 = globalBuffer.parseObject(JSON_SensorObject("REED_01", "Reed", 1, dataSet1));
                        SENSORS.add(obj1);
                    }
                    break;
                case SENSPIN_METHANE:
                    {
                        // Read methane sensor in parts per thousand
                        String dataSet2[1][DATA_VARIABLE_COUNT];
                        float partsPerThousand = analogRead(SENSPIN_METHANE);
                        dataSet2[0][DATA_NAME] = "Status";
                        dataSet2[0][DATA_FORMAT] = "BOOL";
                        dataSet2[0][DATA_VALUE] = (String)partsPerThousand;

                        JsonObject& obj2 = globalBuffer.parseObject(JSON_SensorObject("MQ4_01", "Methaan", 1, dataSet2));
                        SENSORS.add(obj2);
                    }
                    break;
                case SENSPIN_MOTION:
                    {
                        // Read motion sensor
                        String dataSet3[1][DATA_VARIABLE_COUNT];
                        dataSet3[0][DATA_NAME] = "Status";
                        dataSet3[0][DATA_FORMAT] = "BOOL";
                        dataSet3[0][DATA_VALUE] = (String)SENSOR_MOTION_STATE;

                        JsonObject& obj3 = globalBuffer.parseObject(JSON_SensorObject("HC-SR501_01", "Beweging", 1, dataSet3));
                        SENSORS.add(obj3);
                    }
                    break;
            }
        }
    }

    if(DEBUG)
    {
        root.prettyPrintTo(Serial);
    }
    String payload;
    root.printTo(payload);
    return payload;
}

String SensorMapper::JSON_SensorObject(String sensorID, String sensorName, int dataCount, String dataSet[][DATA_VARIABLE_COUNT])
{
    // Sensor Component
    StaticJsonBuffer<2000> sensorBuffer;
    JsonObject& SENSOR = sensorBuffer.createObject();
    SENSOR["SENSOR_ID"] = sensorID;
    SENSOR["SENSOR_NAME"] = sensorName;
    JsonArray& DATA_ARRAY = SENSOR.createNestedArray("DATA");

    // Component
    StaticJsonBuffer<1000> dataBuffer;
    for(int y = 0; y < dataCount; y++)
    {
        JsonObject& DATA = dataBuffer.createObject();
        DATA["NAME"] = dataSet[y][DATA_NAME];
        DATA["FORMAT"] = dataSet[y][DATA_FORMAT];
        DATA["VALUE"] = dataSet[y][DATA_VALUE];
        DATA_ARRAY.add(DATA);
    }

    String generatedJson;
    SENSOR.printTo(generatedJson);
    return generatedJson;
}