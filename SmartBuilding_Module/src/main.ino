#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
const bool DEBUG = true;


// WiFI & MQTT VARIABLES
const char* NET_SSID = "Public_AareonNL";
const char * NET_PASS = "5iaspGEM!";

const char* MQTT_HOST = "217.122.38.126";
const char* MQTT_USER   = "aareon";
const char* MQTT_PASS   = "AareonStenden2018";
const int   MQTT_PORT   = 8883;

String mqtt_topic = "";
String  MAC;

// TIMING Variables
unsigned long currentMillis, previousMillis;
const int updateInterval = 5000;

// SENSORY VARIABLES
// Sensor mapping

const int SENSPIN_DHT = 5;
const int SENSPIN_REED = 16;
const int SENSPIN_METHANE = 17;
const int SENSPIN_MOTION = 18;

// Default states
bool SENSOR_MOTION_STATE = false;

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

String statusUpdateMessage = "";
bool hasNewMessage = false;

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
      printMsg("Device ID: " + MAC);
  }
  // Run sensor connection check.
  sensorMapper();
  // MultiCore initialization

  // WiFI & MQTT Initialization
  WiFi.begin(NET_SSID, NET_PASS);
  MAC = WiFi.macAddress();
  printMsg("Device ID: " + MAC);
  mqtt_topic = "SB/API/" + MAC;
  connectWiFi();
  MQTT.setServer(MQTT_HOST, MQTT_PORT);
  connectBroker();

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
   //readSensors();
}

void loop() {
  // Primary core tasks, such as communications here, to avoid unwanted behaviour.
  checkConnectivity();
  xSemaphoreTake(batton, portMAX_DELAY);
  if(hasNewMessage) {
      printMsg("[UpdateCycle] Update publish status: " + (String)MQTT.publish(stringToChar(mqtt_topic), stringToChar(statusUpdateMessage)));
      hasNewMessage = false;
      /// === Expendability for listening to information ===
      //MQTT.subscribe("");
  }
  xSemaphoreGive(batton);
  MQTT.loop();
}

void coreTask_II( void * pvParameters ){
// Repeating core cycle
while(true){
  currentMillis = millis();


  // Check whether motion has been detected.
  if(digitalRead(SENSPIN_MOTION)) {
    SENSOR_MOTION_STATE = true;
  }

  /*
  *   ============================================
  *   ============ UPDATE CYCLE TIMER ============
  *   ============================================
  */
  if(currentMillis - previousMillis >= updateInterval) {
    //Execute sensor read
    previousMillis = currentMillis;
    //TODO: Include readSensor method here. Also clear the PIR's state to 'false'.
    xSemaphoreTake(batton, portMAX_DELAY);
    statusUpdateMessage = readSensors();
    hasNewMessage = true;
    xSemaphoreGive(batton);

    // Reset the motion detection cycle for the last update cycle.
    SENSOR_MOTION_STATE = false;
  }
  vTaskDelay(10 / portTICK_PERIOD_MS);
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

void checkConnectivity(){
  if(WiFi.status() != WL_CONNECTED)
    connectWiFi();
  if(!MQTT.connected())
    connectBroker();
}

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
    printMsg("[MQTT] Attempt connecting to: ''" + (String)MQTT_USER + "'.");
    if(MQTT.connect(stringToChar(MAC), MQTT_USER, MQTT_PASS)){
      // Connected
      printMsg("[MQTT] Connected to the broker aswell!");
    }else{
      // Not connected.
      printMsg("[MQTT] Couldn't connect to: ''" + (String)MQTT_USER + "'.");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
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

String readSensors(){
  // ==== JSON HEADER ====
  JsonObject& root = jsonBuffer.createObject();
  JsonArray& SENSORS = root.createNestedArray("SENSORS");
  StaticJsonBuffer<2000> globalBuffer;

  // ====  START DUMMY TEST SECTION, REMOVE ON DISTRIBUTION ====
  for(int i = 0; i < 4; i++){
    connectedPins[i] = true;
  }
  // ==== STOP DUMMY TEST SECTION, REMOVE ON DISTRIBUTION ====

  for(int i = 0; i < 4; i++){
    // Check whether the pin mapper found a connection to read.
    if(connectedPins[i] != 0){
      int sPin = sensorPins[i];
        if(sPin == SENSPIN_DHT) {

            // ==== START ACQUIRE DATASET ====
            String dataSet[2][DATA_VARIABLE_COUNT];
            dataSet[0][DATA_NAME] = "Temperature";
            dataSet[0][DATA_FORMAT] = "DECIMAL";
            // Read data
            float temp = dht.readTemperature();

            dataSet[0][DATA_VALUE] = isnan(temp) ? -1 : temp;

            dataSet[1][DATA_NAME] = "Humidity";
            dataSet[1][DATA_FORMAT] = "DECIMAL";
            // Read data
            float humid = dht.readHumidity();

            dataSet[1][DATA_VALUE] =  isnan(humid) ? -1 : humid;
            // ==== STOP ACQUIRE DATASET ====

            JsonObject& obj = globalBuffer.parseObject(JSON_SensorObject("DHT_01", "DHT22", 2, dataSet));
            SENSORS.add(obj);
        }else if(sPin == SENSPIN_REED){
          // ==== START ACQUIRE DATASET ====
          String dataSet1[1][DATA_VARIABLE_COUNT];
          dataSet1[0][DATA_NAME] = "Status";
          dataSet1[0][DATA_FORMAT] = "BOOL";
          dataSet1[0][DATA_VALUE] =  digitalRead(SENSPIN_REED);
          JsonObject& obj1 = globalBuffer.parseObject(JSON_SensorObject("REED_01", "Reed", 1, dataSet1));
          SENSORS.add(obj1);
        }else if(sPin == SENSPIN_METHANE){
          String dataSet2[1][DATA_VARIABLE_COUNT];
          // ==== START ACQUIRE DATASET ====
          float partsPerThousand = analogRead(SENSPIN_METHANE);
          dataSet2[0][DATA_NAME] = "Status";
          dataSet2[0][DATA_FORMAT] = "BOOL";
          dataSet2[0][DATA_VALUE] =  (String)partsPerThousand;

          // ==== STOP ACQUIRE DATASET ====
          JsonObject& obj2 = globalBuffer.parseObject(JSON_SensorObject("MQ4_01", "Methaan", 1, dataSet2));
          SENSORS.add(obj2);
        }else if(sPin == SENSPIN_MOTION){
          String dataSet3[1][DATA_VARIABLE_COUNT];
          // ==== START ACQUIRE DATASET ====
          dataSet3[0][DATA_NAME] = "Status";
          dataSet3[0][DATA_FORMAT] = "BOOL";
          dataSet3[0][DATA_VALUE] = (String)SENSOR_MOTION_STATE;
          // ==== STOP ACQUIRE DATASET ====
          JsonObject& obj3 = globalBuffer.parseObject(JSON_SensorObject("HC-SR501_01", "Beweging", 1, dataSet3));
          SENSORS.add(obj3);
        }
    }
  }
  // DEBUG PRINTING
  if(DEBUG)
    root.prettyPrintTo(Serial);

  String payload;
  root.printTo(payload);
  return payload;
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

char* stringToChar(String msg){
  if(msg.length() != 0) {
    char *output = const_cast<char*>(msg.c_str());
    return output;
  }
}
