#include <Arduino.h>

// Multicore variables
const int TASK_PRIO = 1;  // 1 > 0 <- Priority rating.
const int TASK_CORE = 0;
SemaphoreHandle_t batton;

int count = 0;

void coreTask_II( void * pvParameters ){
while(true){
  // Secondary core tasks here.
  Serial.println("Core 0 - " + (String)count);
  xSemaphoreTake(batton, portMAX_DELAY);
  count++;
  xSemaphoreGive(batton);
  delay(100);

  /*Wait for new coreTaskCycle*/}
}

void setup() {
  // Create task for defined core.
  Serial.begin(115200);
  batton = xSemaphoreCreateMutex();
  xTaskCreatePinnedToCore(
                    coreTask_II,
                    "coreTask_II",
                    10000,
                    NULL,
                    TASK_PRIO,
                    NULL,
                    TASK_CORE);

   // Further setup operations here.
}

void loop() {
  // Primary core tasks, such as communications here, to avoid unwanted behaviour.
  Serial.println("Core 1 - " + (String)count);
  xSemaphoreTake(batton, portMAX_DELAY);
  count++;
  xSemaphoreGive(batton);
  delay(250);
}
