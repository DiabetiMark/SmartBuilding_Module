#include <WiFi.h>
#include <PubSubClient.h>

const bool DEBUG = true;

// SENSORY VARIABLES
// Sensor mapping
const int SENSPIN_TEMP = 5;
const int SENSPIN_REED = 16;
const int SENSPIN_METHANE = 17;
const int SENSPIN_MOTION = 18;

// Available sensor pins [19,21,22,23,24,34,35]
const int sPinsSize = 10;
int sensorPins[sPinsSize] = {SENSPIN_TEMP, SENSPIN_REED, SENSPIN_METHANE, SENSPIN_MOTION};

// connected sensor check
bool connectedPins[sPinsSize];

// Multicore variables
const int TASK_PRIO = 1;  // 1 > 0 <- Priority rating.
const int TASK_CORE = 0;
SemaphoreHandle_t batton;


// WiFI & MQTT variables
const char* NET_SSID = ".";
const char* NET_PASS = "1234567890";

const char* mqtt_host = "217.122.39.160";
const char* mqtt_user   = "SmartBuilding";
const char* mqtt_pass   = "AareonStenden2018";
const int   mqtt_port   = 1883;

WiFiClient CLIENT;
PubSubClient MQTT(CLIENT);


void setup() {
  // Create task for defined core.
  Serial.begin(115200);
  if(DEBUG){
      while(!Serial){/* Wait for the Serial Monitor. */}
      printMsg("Booting...");
  }
  // Run sensor connection check.
  sensorMapper();
  // MultiCore initialization
  batton = xSemaphoreCreateMutex();
  // Assign Core processes and variables.
  xTaskCreatePinnedToCore(
                    coreTask_II,
                    "coreTask_II",
                    10000,
                    NULL,
                    TASK_PRIO,
                    NULL,
                    TASK_CORE);

   // WiFI & MQTT Initialization
   WiFi.begin(NET_SSID, NET_PASS);
   connectWiFi();
   MQTT.setServer(mqtt_host, mqtt_port);
   connectBroker();
}

void loop() {
  // Primary core tasks, such as communications here, to avoid unwanted behaviour.
  printMsg("Running Core 1");
  delay(1000);
}

void coreTask_II( void * pvParameters ){
// Repeating core cycle
while(true){
  // Secondary core tasks here.
  // xSemaphoreTake(batton, portMAX_DELAY);
  // xSemaphoreGive(batton);
  readSensors();
  delay(5000);
  }
}


// This method prints a message to the Serial when debug mode is enabled.
void printMsg(String msg){
  Serial.println(msg);
}

/*
*   ============================================
*   ==============CONNECTION CODING=============
*   ============================================
*/

void connectWiFi() {
  printMsg("[ConnectWiFi] Attempting to connect to: '" + (String)NET_SSID + "'.");
  while(WiFi.status() != WL_CONNECTED){
    printMsg("[ConnectWiFi] Couldn't connect to: '" + (String)NET_SSID + "'.");
    delay(5000);
  }
  printMsg("[ConnectWiFi] Connected!");
}

void connectBroker() {
  while(!MQTT.connected()){
    // TODO: Make dynamic. Or based on unit.
    printMsg("[MQTT] Attempt connecting to: ''" + (String)mqtt_user + "'.");
    if(MQTT.connect("UNIT_ID_GOES_HERE", mqtt_user, mqtt_pass)){
      // Connected
      printMsg("[MQTT] Connected to the broker aswell!");
      MQTT.publish("test", "Heya! FROM ESP32.");
    }else{
      // Not connected.
      printMsg("[MQTT] Couldn't connect to: ''" + (String)mqtt_user + "'.");
      delay(5000);
    }
  }
}

/*
*   ============================================
*   ==============SENSORY CODING================
*   ============================================
*/

void sensorMapper(){
  printMsg("[SensorMapper] Commencing sensor map!");
  // Loop through each sensorPin
  for(int i = 0; i < sPinsSize; i++){
    int pin = sensorPins[i];
    // Check whether an actual pin has been assigned. (0 isn't a pin).
    if(pin != 0){
        printMsg("[SensorConnectionCheck] Checking listed pin: " + (String)pin);
        pinMode(pin, INPUT_PULLUP);
        delay(50);
        if(!digitalRead(pin)){
          // Connected
          connectedPins[i] = true;
          pinMode(i, INPUT);
        } else {
          // Set pinmode to OUTPUT to conserve energy.
          pinMode(i, OUTPUT);
        }
    }
  }
  printMsg("=================");
  for(int i = 0; i < sPinsSize; i++){
    if(connectedPins[i]){
      printMsg("[SensorConnectionCheck] Pin [" + (String)sensorPins[i] + "] is connected to the module!");
    }
  }
}

void readSensors(){
  printMsg("[ReadSensors] Attempting to read");
  for(int i = 0; i < sPinsSize; i++){
    // Check whether the pin mapper found a connection to read.
    if(connectedPins[i] != 0){
      int sPin = sensorPins[i];
        if(sPin == SENSPIN_TEMP){
            // Execute Temp / Humidity reading.
            printMsg("[ReadSensors] TEMP READING");
        }else if(sPin == SENSPIN_REED){
            // Execute Reed sensor reading.
            printMsg("[ReadSensors] LDR READING");
        }else if(sPin == SENSPIN_METHANE){
            // Execute MQ-4 reading.
            printMsg("[ReadSensors] METHANE READING");
        }else if(sPin == SENSPIN_MOTION){
            // Execute Motion reading.
            printMsg("[ReadSensors] MOTION READING");
        }
    }
  }
}
