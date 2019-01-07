#include "Multicore.h"

void Multicore::CreateTask(TaskFunction_t callback, const char* name)
{
    batton = xSemaphoreCreateMutex();
    xTaskCreatePinnedToCore(callback, name, 10000, NULL, TASK_PRIO, NULL, TASK_CORE);
}

void Multicore::Lock()
{
    xSemaphoreTake(batton, portMAX_DELAY);
}

void Multicore::Unlock()
{
    xSemaphoreGive(batton);
}

void Multicore::TaskDelay()
{
    vTaskDelay(10 / portTICK_PERIOD_MS);
}