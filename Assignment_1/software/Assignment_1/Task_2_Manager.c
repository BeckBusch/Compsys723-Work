#include "Task_2.h"

int receiveArray[8];
const TickType_t delay2 = 500 / portTICK_PERIOD_MS;

void task_2_Manager(void* pvParameters) {

    while (1) {
        xQueueReceive(statsQueue, &receiveArray, (TickType_t)0);

        for (int i = 0; i < 8; i++) {
            printf("%d, ", receiveArray[i]);
        }
        printf("\n");

        vTaskDelay(delay2);
    }
}