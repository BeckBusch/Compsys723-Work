#include "Task_3.h"

#define CurrentFreq 0
#define StableBool 1
#define FreqThresh 2
#define RoCThresh 3
#define TotalTime 4
#define AverageTime 5
#define MaxTime 6
#define MinTime 7


//const TickType_t xDelay = vgaRefreshMs / portTICK_PERIOD_MS;
int tickCountStart, tickCountEnd;
int threshReceiveArray[2] = { 660, 6 };

void refreshTimerCallback(xTimerHandle refreshTimer) {
    printf("running timer callback\n");
    statsMessage[CurrentFreq] = IORD(FREQUENCY_ANALYSER_BASE, 0);
    statsMessage[StableBool] = 1;
    statsMessage[FreqThresh] = threshReceiveArray[0];
    statsMessage[RoCThresh] = threshReceiveArray[1];
    statsMessage[TotalTime] = 3723;
    statsMessage[AverageTime] = 150;
    statsMessage[MaxTime] = 300;
    statsMessage[MinTime] = 80;
    xQueueSend(statsQueue, (void*)&statsMessage, (TickType_t)0);
    vTaskResume(t4Handle);
}

void task_3_Tracker(void* pvParameters) {
    //printf("Tracker Task Started\n");
    statsMessage[CurrentFreq] = 320;
    statsMessage[StableBool] = 1;
    statsMessage[FreqThresh] = 123;
    statsMessage[RoCThresh] = 12;
    statsMessage[TotalTime] = 45296;
    statsMessage[AverageTime] = 234;
    statsMessage[MaxTime] = 345;
    statsMessage[MinTime] = 123;
    xQueueSend(statsQueue, (void*)&statsMessage, (TickType_t)0);

    while (1) {
        //printf("Tracker Loop Started\n");

        xQueuePeek(threshQueue, &threshReceiveArray, (TickType_t)0);

        vTaskSuspend(t3Handle);
    }
}