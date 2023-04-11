#include "Main.h"

#include "Task_1.h"
#include "Task_2.h"
#include "Task_3.h"
#include "Task_4.h"

/* The parameters passed to the reg test tasks.  This is just done to check
 the parameter passing mechanism is working correctly. */
 /*#define task_1_PARAMETER    ( ( void * ) 0x12345678 )
 #define task_2_PARAMETER    ( ( void * ) 0x12345678 )
 #define task_3_PARAMETER    ( ( void * ) 0x12345678 )
 #define task_4_PARAMETER    ( ( void * ) 0x12345678 )*/

#define task_1_PRIORITY       ( tskIDLE_PRIORITY + 4)
#define task_2_PRIORITY       ( tskIDLE_PRIORITY + 3)
#define task_3_PRIORITY       ( tskIDLE_PRIORITY + 2)
#define task_4_PRIORITY       ( tskIDLE_PRIORITY + 1)

TaskHandle_t t1Handle = NULL;
TaskHandle_t t2Handle = NULL;
TaskHandle_t t3Handle = NULL;
TaskHandle_t t4Handle = NULL;

int main(void) {
	startTickQueue = xQueueCreate(1, sizeof(int));
	finishTickQueue = xQueueCreate(1, sizeof(int));

	startTickTime = xTaskGetTickCount();
	alt_up_ps2_dev* ps2_device = alt_up_ps2_open_dev(PS2_NAME);

	if (ps2_device == NULL) {
		printf("can't find PS/2 device\n");
		return 1;
	}

	alt_up_ps2_enable_read_interrupt(ps2_device);
	alt_irq_register(PS2_IRQ, ps2_device, ps2_isr);

	alt_irq_register(FREQUENCY_ANALYSER_IRQ, 0, freq_relay_ISR);
	freqQueue = xQueueCreate(1, sizeof(freqOutput));
	freqDataQueue = xQueueCreate(1, sizeof(freqDataOutput));

	threshQueue = xQueueCreate(1, sizeof(thresholdSendArray));
	statsQueue = xQueueCreate(1, sizeof(statsMessage));
	stableStatusQueue = xQueueCreate(1, sizeof(stabilityOutput));

	refreshTimer = xTimerCreate("Refresh Timer", pdMS_TO_TICKS(vgaRefreshMs), pdTRUE, NULL, refreshTimerCallback);
	if (xTimerStart(refreshTimer, 0) != pdPASS) {
		printf("Cannot start timer");
	}
	manageTimer = xTimerCreate("Management Timer", pdMS_TO_TICKS(500), pdTRUE, NULL, manageTimerCallback);

	xTaskCreate(task_1_Analyser, "Freq_Analyser", configMINIMAL_STACK_SIZE, NULL, task_1_PRIORITY, &t1Handle);
	xTaskCreate(task_2_Manager, "Load_Manager", configMINIMAL_STACK_SIZE, NULL, task_2_PRIORITY, &t2Handle);
	xTaskCreate(task_3_Tracker, "Stats_Tracker", configMINIMAL_STACK_SIZE, NULL, task_3_PRIORITY, &t3Handle);
	xTaskCreate(task_4_VGA_Controller, "VGA_Controller", configMINIMAL_STACK_SIZE, NULL, task_4_PRIORITY, &t4Handle);

	/* Finally start the scheduler. */
	vTaskStartScheduler();

	/* Will only reach here if there is insufficient heap available to start
	 the scheduler. */
	for (;;);
}

