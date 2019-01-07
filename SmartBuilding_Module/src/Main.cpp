#include "Globals.h"
#include "Util.h"
#include "SensorMapper.h"
#include "Communications.h"
#include "Multicore.h"

SensorMapper sensorMapper;
Communications communications;
Multicore multiCore;

void MulticoreCallback(void *pvParameters)
{
    while(true)
    {
        currentMillis = millis();

        // Check whether motion has been detected
        if(digitalRead(SENSPIN_MOTION))
        {
            sensorMapper.SetSensorMotionState(true);
        }

        // Update Cycle
        if(currentMillis - previousMillis >= updateInterval)
        {
            previousMillis = currentMillis;
            
            // TODO - Include readSensor method here. Also clear the PIR's state to 'false'.
            multiCore.Lock();
            communications.SetStatusUpdateMessage(sensorMapper.readSensors());
            communications.HasNewMessage(true);
            multiCore.Unlock();

            sensorMapper.SetSensorMotionState(false);
        }
        multiCore.TaskDelay();
    }
}

void setup()
{
    Serial.begin(115200);
    if(DEBUG)
    {
        while(!Serial){ /* Wait for Serial Monitor. */ }
        Util::PrintMessage("Booting...");
        Util::PrintMessage("Device ID: " + MAC);
    }

    // Run sensor connection check.
    sensorMapper.Map();
    communications.ConnectWifi();
    communications.ConnectBroker();
    multiCore.CreateTask(MulticoreCallback, "MulticoreCallback");   
}

void loop()
{
    communications.CheckConnectivity();
    if(communications.HasNewMessage())
    {
        Util::PrintMessage("[UpdateCycle] Update publish status: " + (String)communications.MQTTPublish(Util::StringToChar(mqtt_topic), 
            Util::StringToChar(communications.GetStatusUpdateMessage())));
        communications.HasNewMessage(false);
    }
    communications.MQTTLoop();
}