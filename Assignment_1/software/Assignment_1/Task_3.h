/** @file Task_3.h
* 
* @brief A description of the module’s purpose. 
*
*/ 
#ifndef TASK_3_H
#define TASK_3_H

#include "Main.h"

// total time in seconds, average time in ms, max time in ms, min time in ms, 5 recent values
int statsMessage[8];
void task_3_Tracker(void* pvParameters);
TickType_t startTickTime;

#endif /* TASK_3_H */
/*** end of file ***/
