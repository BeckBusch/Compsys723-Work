#include "Task_3.h"


#define AverageTime 0
#define MaxTime 1
#define MinTime 2
#define MeasureOne 3
#define MeasureTwo 4
#define MeasureThree 5
#define MeasureFour 6
#define MeasureFive 7


int tickCountStart, tickCountEnd, tickTimeDiff, currentTime;
double threshReceiveArray[2];

void task_3_Tracker(void* pvParameters) {
    statsMessage[AverageTime] = 0;
    statsMessage[MaxTime] = 0;
    statsMessage[MinTime] = 0;
    statsMessage[MeasureOne] = 0;
    statsMessage[MeasureTwo] = 0;
    statsMessage[MeasureThree] = 0;
    statsMessage[MeasureFour] = 0;
    statsMessage[MeasureFive] = 0;

    while (1) {
        xQueueReceive(finishTickQueue, &tickCountEnd, portMAX_DELAY);
        xQueueReceive(startTickQueue, &tickCountStart, 0);

        currentTime = (tickCountEnd - tickCountStart) / configTICK_RATE_HZ + 5;

        if (currentTime > statsMessage[MaxTime]) { statsMessage[MaxTime] = currentTime; }
        if (currentTime < statsMessage[MinTime]) { statsMessage[MinTime] = currentTime; }

        statsMessage[MeasureFive] = statsMessage[MeasureFour];
        statsMessage[MeasureFour] = statsMessage[MeasureThree];
        statsMessage[MeasureThree] = statsMessage[MeasureTwo];
        statsMessage[MeasureTwo] = statsMessage[MeasureOne];
        statsMessage[MeasureOne] = currentTime;

        //statsMessage[AverageTime] = ;
        //printf("putput %d\n", statsMessage[AverageTime]);


        xQueueOverwrite(statsQueue, (void*)&statsMessage);
    }
}