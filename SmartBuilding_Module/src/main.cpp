#include <Arduino.h>

// Multicore variables
const int TASK_PRIO = 1;  // 1 > 0 <- Priority rating.
const int TASK_CORE = 0;

int count = 0;

void coreTask_II( void * pvParameters ){

  // Secondary core tasks here.

  while(true){ /*Wait for new coreTaskCycle*/}
}

void setup() {
  // Create task for defined core.
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
}
