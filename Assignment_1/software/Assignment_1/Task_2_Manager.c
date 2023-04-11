#include "Task_2.h"

#define loadCount 6

#define mantState 3
#define waitState 1
#define manageState 2

#define actionShed 0 // action shed means the controller is trying to turn loads off
#define actionLoad 1 // action load mean the controller is turning loads on

#define unstable 0
#define stable 1

int newStability, loadStatus[loadCount], fsmState, switchInput, currentStability, greenLedOutput, redLedOutput, switchState[loadCount];
int finishTickOutput;

void manageLoads(int manageAction);
void manageTimerCallback(xTimerHandle manageTimer);

void manageTimerCallback(xTimerHandle manageTimer) {
    // if the timer ends and current stability is unstable, we are sheding loads
    if (currentStability == unstable) {
        manageLoads(actionShed);
        xTimerReset(manageTimer, 0); // restart the timer
    } else if (currentStability == stable) {
        manageLoads(actionLoad);
        xTimerReset(manageTimer, 0); // restart the timer
    }
    if (loadStatus[0] == 1) { // if the lowest prio load is active, then we have moved back into the waiting state
        xTimerStop(manageTimer, 0);
        fsmState = waitState;
    }
}

void manageLoads(int manageAction) {
    if (manageAction == actionShed) { // we are sheding a load
        for (int i = 0; i < loadCount; i++) { // iterate through the loads, starting at the lowest prio
            if (loadStatus[i] == 1) { // if the load is on, 
                loadStatus[i] = 0; // turn the load off
                break; // leave the loop
            }
        }
    } else if (manageAction == actionLoad) {
        for (int i = loadCount - 1; i >= 0; i--) { // iterate through the loads, starting at the highest prio
            if (loadStatus[i] == 0) { // if the load is off,
                loadStatus[i] = 1; // turn the load on
                break;
            }
        }
    }
}

void task_2_Manager(void* pvParameters) {
    fsmState = waitState;
    currentStability = stable;

    while (1) {
        xQueuePeek(stableStatusQueue, &newStability, (TickType_t)0);
        switchInput = IORD_ALTERA_AVALON_PIO_DATA(SLIDE_SWITCH_BASE);


        if (fsmState == waitState) { // we are not managing loads, and waiting for a change in stability
            if (newStability == unstable) { // if the system goes unstable
                manageLoads(actionShed); // shed one load
                finishTickOutput = xTaskGetTickCount();
                //printf("finish tick %d\n", finishTickOutput);
                xQueueOverwrite(finishTickQueue, &finishTickOutput);
                xTimerStart(manageTimer, 0); // start the 500ms countdown
                currentStability = unstable; // save new stability
                fsmState = manageState; // switch to management mode
            }
        } else if (fsmState == manageState) { // we are managing loads, resetting the timer if stability changes
            if (newStability != currentStability) { // if stability has changed
                xTimerReset(manageTimer, 0); // restart the timer
                currentStability = newStability; // save new stability
            }
        } else if (fsmState == mantState) {

        }

        redLedOutput = 0;
        greenLedOutput = 0;
        for (int i = 0; i < loadCount; i++) {
            redLedOutput += ((loadStatus[i]) << i);
            greenLedOutput += (loadStatus[i] << i);
        }
        IOWR_ALTERA_AVALON_PIO_DATA(GREEN_LEDS_BASE, greenLedOutput);
        IOWR_ALTERA_AVALON_PIO_DATA(RED_LEDS_BASE, redLedOutput);

        vTaskSuspend(t2Handle); // suspend the task to be awoken by something else
    }
}