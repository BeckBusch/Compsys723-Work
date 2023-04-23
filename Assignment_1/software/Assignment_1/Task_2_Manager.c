#include "Task_2.h"

#define loadCount 8

#define mantState 3
#define waitState 1
#define manageState 2

#define actionShed 0 // action shed means the controller is trying to turn loads off
#define actionLoad 1 // action load mean the controller is turning loads on

#define unstable 0
#define stable 1

int loadStatus[loadCount] = { 1, 1, 1, 1, 1, 1, 1, 1 };
int fsmState, newStability, currentStability, finishTickOutput, maintenanceState, timingFlag;
int switchInput, switchEffect; greenLedOutput, redLedOutput, switchState[loadCount];

void manageLoads(int manageAction);
void manageTimerCallback(xTimerHandle manageTimer);

void manageTimerCallback(xTimerHandle manageTimer) {
    // if the timer ends and current stability is unstable, we are shedding loads
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
    if (manageAction == actionShed) { // we are shedding a load
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

void button_interrupts_function(void* context, alt_u32 id) {
    int* temp = (int*)context;
    (*temp) = IORD_ALTERA_AVALON_PIO_EDGE_CAP(PUSH_BUTTON_BASE);
    //printf("bing %d\n", maintenanceState);
    maintenanceState = !maintenanceState;

    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PUSH_BUTTON_BASE, 0x7);
}

void task_2_Manager(void* pvParameters) {
    fsmState = waitState;
    currentStability = stable;
    maintenanceState = 0;

    int buttonValue = 0;
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PUSH_BUTTON_BASE, 0x4);
    IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PUSH_BUTTON_BASE, 0x4);
    alt_irq_register(PUSH_BUTTON_IRQ, (void*)&buttonValue, button_interrupts_function);

    while (1) {
        xQueuePeek(stableStatusQueue, &newStability, (TickType_t)0);
        switchInput = IORD_ALTERA_AVALON_PIO_DATA(SLIDE_SWITCH_BASE);

        if (fsmState == waitState) { // we are not managing loads, and waiting for a change in stability
            switchEffect = switchInput; // since we are not managing loads, the switches can have an effect

            if (newStability == unstable) { // if the system goes unstable
                manageLoads(actionShed); // shed one load
                /*finishTickOutput = xTaskGetTickCount(); // moving timing stop to led change as that is the actual output
                //printf("end timing tick: %d\n", finishTickOutput); // PRINTER: end timing tick
                xQueueOverwrite(finishTickQueue, &finishTickOutput);*/
                timingFlag = 1;
                xTimerStart(manageTimer, 0); // start the 500ms countdown
                currentStability = unstable; // save new stability
                fsmState = manageState; // switch to management mode

            }
        } else if (fsmState == manageState) { // we are managing loads, resetting the timer if stability changes
            if (newStability != currentStability) { // if stability has changed
                xTimerReset(manageTimer, 0); // restart the timer
                currentStability = newStability; // save new stability
            }
            // during the management state, we can alter the input from switches only to turn a switch off
            for (int i = 0; i < 8; i++) {
                if (!(switchInput & (1 << i))) {
                    switchEffect = (switchEffect & ~(1 << i));
                }
            }

        } else if (fsmState == mantState) {
            switchEffect = switchInput; // since we are not managing loads, the switches can have an effect
        }

        redLedOutput = 0;
        greenLedOutput = 0;
        if (maintenanceState) {
            redLedOutput = switchInput & 0b11111111;
        } else {
            for (int i = 0; i < loadCount; i++) {
                redLedOutput += ((1 && (switchEffect & (loadStatus[i] << i))) << i); // load power
                greenLedOutput += (!maintenanceState && (!loadStatus[i] << i)) << i; // relay forcing off
            }
        }

        if (timingFlag) {
            finishTickOutput = xTaskGetTickCount();
            //printf("end timing tick: %d\n", finishTickOutput); // PRINTER: end timing tick
            xQueueOverwrite(finishTickQueue, &finishTickOutput);
            timingFlag = 0;
        }

        alt_up_char_buffer_dev* char_buf;
         char_buf = alt_up_char_buffer_open_dev("/dev/video_character_buffer_with_dma");

        switch (fsmState) {
        case waitState:
            alt_up_char_buffer_string(char_buf, "System Status: Waiting  ", 25, 4);
            break;
        case manageState:
            alt_up_char_buffer_string(char_buf, "System Status: Managing ", 25, 4);
            break;
        case mantState:
            alt_up_char_buffer_string(char_buf, "System Status: Maintence", 25, 4);
            break;
        }

        for (int i = 0; i < 8; i++) {
            if (redLedOutput & (1 << i)) {
                alt_up_char_buffer_string(char_buf, "X", 39 + i * 4, 6);
            } else {
                alt_up_char_buffer_string(char_buf, " ", 39 + i * 4, 6);
            }

        }

        //Maintence Stable Managing

        IOWR_ALTERA_AVALON_PIO_DATA(GREEN_LEDS_BASE, greenLedOutput);
        IOWR_ALTERA_AVALON_PIO_DATA(RED_LEDS_BASE, redLedOutput);

        vTaskSuspend(t2Handle); // suspend the task to be awoken by something else
    }
}