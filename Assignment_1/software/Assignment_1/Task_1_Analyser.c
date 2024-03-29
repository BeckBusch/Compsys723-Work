#include "Task_1.h"

float RoCThreshold, FreqThreshold, RoCSaved, FreqSaved;
int tempFreqValue, curStabBool, startTickOutput;
double curFreqValue, lastFreqValue, rocValue;

void freq_relay_ISR() {
    long lHigherPriorityTaskWoken = pdFALSE;
    startTickOutput = xTaskGetTickCount();
    freqOutput = IORD(FREQUENCY_ANALYSER_BASE, 0);
    xQueueOverwriteFromISR(startTickQueue, &startTickOutput, &lHigherPriorityTaskWoken);
    //printf("start tick %d\n", startTickOutput); // PRINTER: start tick output
    xQueueSendFromISR(freqQueue, &freqOutput, &lHigherPriorityTaskWoken);
    portEND_SWITCHING_ISR(lHigherPriorityTaskWoken);
}

void ps2_isr(void* ps2_device, alt_u32 id) {
    // long lHigherPriorityTaskWoken = pdFALSE;
    unsigned char key = 0;
    decode_scancode(ps2_device, NULL, &key, NULL);
    // R: 52    E: 45    F: 46    D: 44
    switch (key) {
    case 0x2d: // Increase RoC Threshold
        RoCThreshold += 2.5;
        break;
    case 0x24: // Decrease RoC Threshold
        RoCThreshold -= 2.5;
        break;
    case 0x2b: // Increase Frequency Threshold
        FreqThreshold += 0.1;
        break;
    case 0x23: // Decrease Frequency Threshold
        FreqThreshold -= 0.1;
        break;
    }
    //portEND_SWITCHING_ISR(lHigherPriorityTaskWoken);
}

void task_1_Analyser(void* pvParameters) {
    // Init Values
    RoCThreshold = 20;
    FreqThreshold = 48.5;
    lastFreqValue = 50;
    freqDataOutput[0] = 50;
    freqDataOutput[1] = 0;

    while (1) {
        xQueueReceive(freqQueue, &tempFreqValue, (TickType_t)30); // pause task awaiting new freq value

        RoCSaved = RoCThreshold; // saving the threshold values so they aren't changed during the running of this task
        FreqSaved = FreqThreshold;


        curFreqValue = 16000.0 / (double)tempFreqValue;

        rocValue = (curFreqValue - lastFreqValue) * 2.0 * curFreqValue * lastFreqValue / (curFreqValue + lastFreqValue);

        freqDataOutput[0] = curFreqValue;
        freqDataOutput[1] = rocValue;
        xQueueOverwrite(freqDataQueue, &freqDataOutput);

        thresholdSendArray[0] = FreqSaved;
        thresholdSendArray[1] = RoCSaved;
        xQueueOverwrite(threshQueue, &thresholdSendArray);

        if ((curFreqValue < FreqSaved) || (rocValue > RoCSaved)) {
            curStabBool = 0;
        } else {
            curStabBool = 1;
        }
        xQueueOverwrite(stableStatusQueue, &curStabBool);
        vTaskResume(t2Handle); // start the load manager
        lastFreqValue = curFreqValue;
    }
}