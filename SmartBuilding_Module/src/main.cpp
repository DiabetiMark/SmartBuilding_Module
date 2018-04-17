#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* NET_SSID = "The House";
const char* NET_PASS = "";

const char* mqtt_host = "217.122.39.160";
const char* mqtt_user   = "SmartBuilding";
const char* mqtt_pass   = "AareonStenden2018";
const int   mqtt_port   = 1883;

WiFiClient CLIENT;
PubSubClient MQTT(CLIENT);

void connectWiFi() {
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
  }
  Serial.println("Connected!");
}

void connectBroker() {
  while(!MQTT.connected()){
    // TODO: Make dynamic. Or based on unit.
    delay(500);
    Serial.println("Attempt connecting to broker " + (String)mqtt_user + "  --  " + (String)mqtt_pass);
    if(MQTT.connect("UNIT_ID_GOES_HERE", mqtt_user, mqtt_pass)){
      // Connected
      Serial.println("Connected to the broker aswell!");
      MQTT.publish("test", "Heya! FROM ESP32.");
    }else{
      // Not connected.
    }
  }
}


void setup() {
    Serial.begin(9600);
    while(!Serial)
    {
      // Wait
    }
    Serial.println("Booting");
    WiFi.begin(NET_SSID, NET_PASS);
    connectWiFi();
    MQTT.setServer(mqtt_host, mqtt_port);
    connectBroker();
}

void loop() {
  if(!MQTT.connected()){
    connectBroker();
  }
}
