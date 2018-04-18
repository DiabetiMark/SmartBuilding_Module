#include <WiFi.h>
#include <PubSubClient.h>

const bool DEBUG = true;

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
  delay(5000);
  }
}


// This method prints a message to the Serial when debug mode is enabled.
void printMsg(String msg){
  Serial.println(msg);
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
