#include "Task_3.h"


#define TotalTime 0
#define AverageTime 1
#define MaxTime 2
#define MinTime 3


int tickCountStart, tickCountEnd;
double threshReceiveArray[2];

void task_3_Tracker(void* pvParameters) {
    statsMessage[TotalTime] = 0;
    statsMessage[AverageTime] = 0;
    statsMessage[MaxTime] = 0;
    statsMessage[MinTime] = 0;



    while (1) {
        statsMessage[0] = xTaskGetTickCount() / configTICK_RATE_HZ;
        xQueueOverwrite(statsQueue, (void*)&statsMessage);
        vTaskDelay(1000);
        //vTaskSuspend(t3Handle);
    }
}