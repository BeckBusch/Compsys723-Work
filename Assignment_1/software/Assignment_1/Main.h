/** @file Main.h
* 
* @brief A description of the module’s purpose. 
*
*/ 
#ifndef MAIN_H
#define MAIN_H

/* Standard includes. */
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/* Altera includes*/
#include <unistd.h>
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"
#include "sys/alt_irq.h"
#include "io.h"
#include "altera_up_avalon_ps2.h"
#include "altera_up_ps2_keyboard.h"

/* Scheduler includes. */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "FreeRTOS/timers.h"



#define vgaRefreshMs 250 // dont change tbh the x axis is hard coded
#define vgaRefreshSec 0.25

TaskHandle_t t1Handle;
TaskHandle_t t2Handle;
TaskHandle_t t3Handle;
TaskHandle_t t4Handle;

TimerHandle_t refreshTimer;

QueueHandle_t statsQueue;
QueueHandle_t threshQueue;

#endif /* MAIN_H */
/*** end of file ***/
