#include "Task_1.h"

int tickStartCount, tickEndCount;
float RoCThreshold, FreqThreshold;
volatile unsigned int tempFreqValue;

void freq_relay_ISR() {
    long lHigherPriorityTaskWoken = pdFALSE;
    tempFreqValue = IORD(FREQUENCY_ANALYSER_BASE, 0);
    //printf("%f Hz\n", 16000 / (double)temp);
    //portYIELD_FROM_ISR( lHigherPriorityTaskWoken );
}

void ps2_isr(void* ps2_device, alt_u32 id) {
    long lHigherPriorityTaskWoken = pdFALSE;
    printf("running isr");
    unsigned char ascii;
    //alt_up_ps2_read_data_byte_timeout(ps2_device, &byte);
    decode_scancode(ps2_device, NULL, NULL, &ascii);
    // R: 52    E: 45    F: 46    D: 44
    switch (ascii) {
    case 52: // Increase RoC Threshold
        RoCThreshold += 0.1;
        break;

    case 45: // Decrease RoC Threshold
        RoCThreshold -= 0.1;
        break;

    case 46: // Increase Frequency Threshold
        FreqThreshold += 0.2;
        break;

    case 44: // Decrease Frequency Threshold
        FreqThreshold -= 0.2;
        break;
    }

    //portYIELD_FROM_ISR( lHigherPriorityTaskWoken );
}

void task_1_Analyser(void* pvParameters) {
    RoCThreshold = 3.0;
    FreqThreshold = 48.5;

    while (1) {
        thresholdSendArray[0] = FreqThreshold;
        thresholdSendArray[1] = RoCThreshold;
        xQueueOverwrite(threshQueue, (void*)&thresholdSendArray);

        vTaskDelay(1000);
    }
}