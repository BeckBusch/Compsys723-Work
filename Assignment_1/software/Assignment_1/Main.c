#include "Task_1.h"
#include "Task_2.h"
#include "Task_3.h"
#include "Task_4.h"

/* Standard includes. */
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* Scheduler includes. */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/* The parameters passed to the reg test tasks.  This is just done to check
 the parameter passing mechanism is working correctly. */
#define task_1_PARAMETER    ( ( void * ) 0x12345678 )
#define task_2_PARAMETER    ( ( void * ) 0x12345678 )
#define task_3_PARAMETER    ( ( void * ) 0x12345678 )
#define task_4_PARAMETER    ( ( void * ) 0x12345678 )

#define task_1_PRIORITY       ( tskIDLE_PRIORITY + 1)
#define task_2_PRIORITY       ( tskIDLE_PRIORITY + 1)
#define task_3_PRIORITY       ( tskIDLE_PRIORITY + 1)
#define task_4_PRIORITY       ( tskIDLE_PRIORITY + 1)


int main(void) {
	/* The RegTest tasks as described at the top of this file. */
	xTaskCreate(task_4_VGA_Controller, "Rreg1", configMINIMAL_STACK_SIZE, task_4_PARAMETER, task_4_PRIORITY, NULL);

	/* Finally start the scheduler. */
	vTaskStartScheduler();

	/* Will only reach here if there is insufficient heap available to start
	 the scheduler. */
	for (;;);
}

