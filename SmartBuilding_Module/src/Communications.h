#ifndef COMMUNICATIONS_H
#define COMMUNICATIONS_H

#include "Globals.h"
#include <WiFi.h>
#include <PubSubClient.h>

class Communications
{
    private:
        // Variables
        const char* NET_SSID = "Public_AareonNL";
        const char* NET_PASS = "5iaspGEM!";
        const char* MQTT_HOST = "217.122.38.126";
        const char* MQTT_USER = "aareon";
        const char* MQTT_PASS = "AareonStenden2018";
        const int   MQTT_PORT = 8883;
        String statusUpdateMessage = "";
        bool hasNewMessage = false;
        WiFiClient CLIENT;
        PubSubClient MQTT;

    public:
        // Methods
        Communications();
        void CheckConnectivity();
        void ConnectWifi();
        void ConnectBroker();
        bool MQTTPublish(const char* topic, const char* payload);
        bool MQTTLoop();

        // Getters
        String GetStatusUpdateMessage() { return statusUpdateMessage; }
        bool HasNewMessage() { return hasNewMessage; }

        // Setters
        void SetStatusUpdateMessage(String msg) { statusUpdateMessage = msg; }
        void HasNewMessage(bool value) { hasNewMessage = value; }
};

#endif