/** @file Task_1.h
*
* @brief A description of the module’s purpose.
*
*/
#ifndef TASK_1_H
#define TASK_1_H

#include "Main.h"

void freq_relay_ISR();
void task_1_Analyser(void* pvParameters);
void ps2_isr(void* ps2_device, alt_u32 id);

int thresholdSendArray[2];

#endif /* TASK_1_H */
/*** end of file ***/
