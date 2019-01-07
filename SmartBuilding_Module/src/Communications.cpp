#include "Communications.h"
#include "Util.h"

bool hasNewMessage = false;

Communications::Communications()
{
    WiFi.begin(NET_SSID, NET_PASS);
    MAC = WiFi.macAddress();
    Util::PrintMessage("Device ID: " + MAC);
    mqtt_topic = mqtt_topic + MAC;
}

void Communications::CheckConnectivity()
{
    if(WiFi.status() != WL_CONNECTED)
    {
        ConnectWifi();
    }
    if(!MQTT.connected())
    {
        ConnectBroker();
    }
}

void Communications::ConnectWifi()
{
    Util::PrintMessage("[ConnectWifi] Attempting to connect to: '" + (String)NET_SSID + "'.");
    while(WiFi.status() != WL_CONNECTED)
    {
        Util::PrintMessage("[ConnectWifi] Couldn't connect to: '" + (String)NET_SSID + "'.");
        delay(5000);
    }
    Util::PrintMessage("[ConnectWifi] Connected!");
}

void Communications::ConnectBroker()
{
    MQTT = MQTT.setClient(CLIENT);
    MQTT.setServer(MQTT_HOST, MQTT_PORT);
    while(!MQTT.connected())
    {
        Util::PrintMessage("[MQTT] Attempt connecting to: ''" + (String)MQTT_USER + "'.");
        if(MQTT.connect(Util::StringToChar(MAC), MQTT_USER, MQTT_PASS))
        {
            Util::PrintMessage("[MQTT] Connected to the broker aswell!");
        }
        else
        {
            Util::PrintMessage("[MQTT] Couldn't connect to: ''" + (String)MQTT_USER + "'.");
            delay(5000);
        }
    }
}

bool Communications::MQTTPublish(const char* topic, const char* payload)
{
    return MQTT.publish(topic, payload);
}

bool Communications::MQTTLoop()
{
    return MQTT.loop();
}