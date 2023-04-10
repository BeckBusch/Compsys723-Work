/** @file Task_3.h
* 
* @brief A description of the module’s purpose. 
*
*/ 
#ifndef TASK_3_H
#define TASK_3_H

#include "Main.h"

// Stability Bool, Freq Thresh x10, RoC Thresh x10, total time in seconds, average time in ms, max time in ms, min time in ms
int statsMessage[8];
void refreshTimerCallback(xTimerHandle refreshTimer);
void task_3_Tracker(void* pvParameters);

#endif /* TASK_3_H */
/*** end of file ***/
