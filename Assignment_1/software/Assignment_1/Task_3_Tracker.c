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
    statsMessage[MinTime] = 723;
    statsMessage[MeasureOne] = 723;
    statsMessage[MeasureTwo] = 723;
    statsMessage[MeasureThree] = 723;
    statsMessage[MeasureFour] = 723;
    statsMessage[MeasureFive] = 723;

    while (1) {
        xQueueReceive(finishTickQueue, &tickCountEnd, portMAX_DELAY);
        //printf("captured end tick: %d\n", tickCountEnd); // PRINTER: received end tick
        xQueueReceive(startTickQueue, &tickCountStart, 0);
        //printf("captured start tick: %d\n", tickCountStart); // PRINTER: received start tick

        currentTime = (tickCountEnd - tickCountStart);// / configTICK_RATE_HZ;
        //printf("currentTime: %d\n", currentTime); // PRINTER: time recording
        if (currentTime > statsMessage[MaxTime]) { statsMessage[MaxTime] = currentTime; }
        if (currentTime < statsMessage[MinTime]) { statsMessage[MinTime] = currentTime; }

        statsMessage[MeasureFive] = statsMessage[MeasureFour];
        statsMessage[MeasureFour] = statsMessage[MeasureThree];
        statsMessage[MeasureThree] = statsMessage[MeasureTwo];
        statsMessage[MeasureTwo] = statsMessage[MeasureOne];
        statsMessage[MeasureOne] = currentTime;

        statsMessage[AverageTime] = (statsMessage[MeasureOne] + statsMessage[MeasureTwo] +
            statsMessage[MeasureThree] + statsMessage[MeasureFour] +
            statsMessage[MeasureFive]) / 5;

        xQueueOverwrite(statsQueue, (void*)&statsMessage);
    }
}