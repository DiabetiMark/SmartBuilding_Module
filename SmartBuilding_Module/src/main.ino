#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
const bool DEBUG = true;

// SENSORY VARIABLES
// Sensor mapping
const int SENSPIN_DHT = 5;
const int SENSPIN_REED = 16;
const int SENSPIN_METHANE = 17;
const int SENSPIN_MOTION = 18;

//Sensor Defining
#define DHTTYPE DHT22
DHT dht(SENSPIN_DHT, DHTTYPE);
// Available sensor pins [19,21,22,23,24,34,35]
const int sPinsSize = 10;
int sensorPins[sPinsSize] = {SENSPIN_DHT, SENSPIN_REED, SENSPIN_METHANE, SENSPIN_MOTION};

// connected sensor check
bool connectedPins[sPinsSize];

// Multicore variables
const int TASK_PRIO = 1;  // 1 > 0 <- Priority rating.
const int TASK_CORE = 0;
SemaphoreHandle_t batton;


// WiFI & MQTT VARIABLES
const char* NET_SSID = ".";
const char* NET_PASS = "1234567890";

const char* mqtt_host = "217.122.39.160";
const char* mqtt_user   = "SmartBuilding";
const char* mqtt_pass   = "AareonStenden2018";
const int   mqtt_port   = 1883;

// JSON VARIABLES
// Array Matrix Variables
const int DATA_NAME = 0;
const int DATA_FORMAT = 1;
const int DATA_VALUE = 2;

const int DATA_VARIABLE_COUNT = 3;

// Network variables
WiFiClient CLIENT;
PubSubClient MQTT(CLIENT);

// JSON Buffer variable
DynamicJsonBuffer jsonBuffer;


void setup() {
  // Create task for defined core.
  Serial.begin(115200);
  if(DEBUG){
      while(!Serial){/* Wait for the Serial Monitor. */}
      printMsg("Booting...");
  }
  // Run sensor connection check.
  sensorMapper();
  readSensors();
  // MultiCore initialization
/*
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
   */
}

void loop() {
  // Primary core tasks, such as communications here, to avoid unwanted behaviour.
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
  if(DEBUG)
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
    }else{
      // Not connected.
      printMsg("[MQTT] Couldn't connect to: ''" + (String)mqtt_user + "'.");
      delay(5000);
    }
  }
}

//TODO: FINISH THIS METHOD!
void publishMsg(String topic, String payload){
  if(!MQTT.connected())
    connectBroker();
  //MQTT.publish();
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
          // Set required variable if pin is connected.
          if(pin == SENSPIN_DHT) {
            dht.begin();
          }
        } else {
          // Set pinmode to OUTPUT to conserve energy.
          pinMode(i, OUTPUT);
        }
    }
  }
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);
  printMsg("=================");
  for(int i = 0; i < sPinsSize; i++){
    if(connectedPins[i]){
      printMsg("[SensorConnectionCheck] Pin [" + (String)sensorPins[i] + "] is connected to the module!");
    }
  }
}

void readSensors(){
  // ==== JSON HEADER ====
  JsonObject& root = jsonBuffer.createObject();
  JsonArray& SENSORS = root.createNestedArray("SENSORS");
  StaticJsonBuffer<2000> globalBuffer;

  // ====  START DUMMY TEST SECTION, REMOVE ON DISTRIBUTION ====
  /*for(int i = 0; i < sPinsSize; i++){
    connectedPins[i] = true;
  }
// DHT22
  String dataSet[2][DATA_VARIABLE_COUNT];
  dataSet[0][0] = "Temperatuur";
  dataSet[0][1] = "DECIMAL";
  dataSet[0][2] = 20.5;

  dataSet[1][0] = "Luchtvochtigheid";
  dataSet[1][1] = "DECIMAL";
  dataSet[1][2] = 68.5;

  // REED
  String dataSet1[2][DATA_VARIABLE_COUNT];
  dataSet1[0][0] = "Status";
  dataSet1[0][1] = "BOOL";
  dataSet1[0][2] = "true";

  // MQ4
  String dataSet2[2][DATA_VARIABLE_COUNT];
  dataSet2[0][0] = "Methaan";
  dataSet2[0][1] = "DECIMAL";
  dataSet2[0][2] = 5.5;

  // Motion
  String dataSet3[2][DATA_VARIABLE_COUNT];
  dataSet3[0][0] = "Beweging";
  dataSet3[0][1] = "BOOL";
  dataSet3[0][2] = "false";

  // ==== STOP DUMMY TEST SECTION, REMOVE ON DISTRIBUTION ====
  */
  for(int i = 0; i < 4; i++){
    // Check whether the pin mapper found a connection to read.
    if(connectedPins[i] != 0){
      int sPin = sensorPins[i];
        if(sPin == SENSPIN_DHT) {
            // ==== START ACQUIRE DATASET ====
            String dataSet[2][DATA_VARIABLE_COUNT];
            dataSet[0][0] = "Temperature";
            dataSet[0][1] = "DECIMAL";
            float temp = dht.readTemperature();
            dataSet[0][2] = isnan(temp) ? -1 : temp;

            dataSet[1][0] = "Humidity";
            dataSet[1][1] = "DECIMAL";
            float humid = dht.readHumidity();
            dataSet[1][2] =  isnan(humid) ? -1 : humid;
            // ==== STOP ACQUIRE DATASET ====
            JsonObject& obj = globalBuffer.parseObject(JSON_SensorObject("DHT_01", "DHT22", 2, dataSet));
            SENSORS.add(obj);
        }else if(sPin == SENSPIN_REED){
          // ==== START ACQUIRE DATASET ====
          String dataSet[1][DATA_VARIABLE_COUNT];
          dataSet[0][0] = "Status";
          dataSet[0][1] = "BOOL";
          dataSet[0][2] =  digitalRead(SENSPIN_REED);
          JsonObject& obj1 = globalBuffer.parseObject(JSON_SensorObject("REED_01", "Reed", 1, dataSet));
          SENSORS.add(obj1);
        }else if(sPin == SENSPIN_METHANE){
          String dataSet[2][DATA_VARIABLE_COUNT];
          //TODO: Make function that returns dataSet for the given sensor.
          // ==== START ACQUIRE DATASET ====

          // ==== STOP ACQUIRE DATASET ====
          JsonObject& obj2 = globalBuffer.parseObject(JSON_SensorObject("MQ4_01", "Methane", 1, dataSet));
          SENSORS.add(obj2);
        }else if(sPin == SENSPIN_MOTION){
          String dataSet[2][DATA_VARIABLE_COUNT];
          //TODO: Make function that returns dataSet for the given sensor.
          // ==== START ACQUIRE DATASET ====

          // ==== STOP ACQUIRE DATASET ====
          JsonObject& obj3 = globalBuffer.parseObject(JSON_SensorObject("HC-SR501_01", "Motion", 1, dataSet));
          SENSORS.add(obj3);
        }
    }
  }
  // DEBUG PRINTING
  if(DEBUG)
    root.prettyPrintTo(Serial);
  String payload;
  root.printTo(payload);
  //MQTT.publish("/SB/Module/" + "MODULE_ID_GOES_HERE" + "/Data", payload);
}

String JSON_SensorObject(String sensorID, String sensorName, int dataCount, String dataSet[][DATA_VARIABLE_COUNT]) {
  // ==== JSON START SENSOR COMPONENT ====
  StaticJsonBuffer<2000> sensorBuffer;
  JsonObject& SENSOR = sensorBuffer.createObject();
  SENSOR["SENSOR_ID"]= sensorID;
  SENSOR["SENSOR_NAME"]= sensorName;
  JsonArray& DATA_ARRAY = SENSOR.createNestedArray("DATA");

  // ==== JSON START DATA ====
  StaticJsonBuffer<1000> dataBuffer;
  for(int y = 0; y < dataCount; y++) {
  JsonObject& DATA = dataBuffer.createObject();
  DATA["NAME"] = dataSet[y][DATA_NAME];
  DATA["FORMAT"] = dataSet[y][DATA_FORMAT];
  DATA["VALUE"] = dataSet[y][DATA_VALUE];
  DATA_ARRAY.add(DATA);
  }
  // ==== JSON STOP DATA ====

  String generatedJson;
  SENSOR.printTo(generatedJson);
  return generatedJson;
}
