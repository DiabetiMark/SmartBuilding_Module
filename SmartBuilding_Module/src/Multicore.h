#ifndef MULTICORE_H
#define MULTICORE_H

#include "Globals.h"

class Multicore
{
    private:
        // Variables
        const int TASK_PRIO = 1;  // 1 > 0 <- Priority rating.
        const int TASK_CORE = 0;
        const int updateInterval = 5000;
        unsigned long currentMillis, previousMillis;
        SemaphoreHandle_t batton;

    public:
        // Methods
        void CreateTask(TaskFunction_t callback, const char* name);
        void Lock();
        void Unlock();
        void TaskDelay();

        // Getters
        int GetUpdateInterval() { return updateInterval; }
};

#endif