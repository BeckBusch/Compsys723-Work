#include "Task_4.h"

// Colors
#define ColorWhite ((0x3ff << 20) + (0x3ff << 10) + (0x3ff))
#define ColorBlack (0)
#define ColorGreen (0x3ff)
#define ColorRed (0x3ff << 10)
#define ColorBlue (0x3ff << 20)
#define ColorCyan ((0x3ff << 20) + (0x3ff))
// Graph Spots    17 values with 26 pixels per x value
#define FreqYStart (20+30) // Start of Freq y axis, 20 border plus 30
#define FreqYEnd (20+30+90) // end of Freq y axis, start plus 90
#define FreqTextStart 4 // Char start row
#define GraphXStart 100 // Start of both graph x axis
#define GraphXEnd (640-20-30-48) // end of both graph x axis
#define RoCYStart (20+30+90+30+16) // start of roc y axis
#define RocYEnd (20+30+90+30+90+16) // end of roc y axis
#define RoCTextStart 21
// Stat String spots    Char buffer coords are 80x60
#define FirstColStart 5
#define SecondColStart 34
#define ThirdColStart 58
#define FirstRowHeight 40
#define SecRowHeight 43
#define ThirdRowHeight 46
#define FourthRowHeight 49
#define FifthRowHeight 52
#define SixthRowHeight 55


double valueArray[18] = { 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50 };
double rocArray[18], freqDataInput[2];
float thresholdReceiveArray[2];
char outputBuffer[4];
int tempTimeHours, tempTimeMin, tempROC, timeStatCount, stableStatsInput, statsReceiveArray[8];

int tickCountStart, tickCountEnd;

void refreshTimerCallback(xTimerHandle refreshTimer) {
    vTaskResume(t4Handle);
}



void task_4_VGA_Controller(void* pvParameters) {
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 250;

    // Init Pixel Buffer
    alt_up_pixel_buffer_dma_dev* pixel_buf;
    pixel_buf = alt_up_pixel_buffer_dma_open_dev(VIDEO_PIXEL_BUFFER_DMA_NAME);
    if (pixel_buf == NULL) {
        printf("Cannot find pixel buffer device\n");
    }
    //initialize character buffer
    alt_up_char_buffer_dev* char_buf;
    char_buf = alt_up_char_buffer_open_dev("/dev/video_character_buffer_with_dma");
    if (char_buf == NULL) {
        printf("can't find char buffer device\n");
    }
    alt_up_pixel_buffer_dma_clear_screen(pixel_buf, 0);
    alt_up_char_buffer_clear(char_buf);
    alt_up_pixel_buffer_dma_draw_box(pixel_buf, 0, 0, 640, 480, ColorBlack, 0); // Black Background

    alt_up_pixel_buffer_dma_draw_rectangle(pixel_buf, 0, 0, 639, 479, ColorRed, 0);

    // Draw Freq/Time Graph
    alt_up_pixel_buffer_dma_draw_vline(pixel_buf, GraphXStart, FreqYStart, FreqYEnd, ColorWhite, 0); // Black Freq Y axis
    alt_up_pixel_buffer_dma_draw_hline(pixel_buf, GraphXStart, GraphXEnd, FreqYEnd, ColorWhite, 0); // Black Freq X axis
    for (int i = 0; i <= 9; i += 2) {
        alt_up_pixel_buffer_dma_draw_hline(pixel_buf, GraphXStart - 10, GraphXStart, FreqYStart + (i * 10), ColorWhite, 0);
    }
    //                                   --_------------_------------_-------------_------------_
    alt_up_char_buffer_string(char_buf, " -4 Sec       -3 Sec       -2 Sec        -1 Sec       -0 Sec", 12, FreqTextStart + 14);
    alt_up_char_buffer_string(char_buf, "Frequency (Hz)", 3, FreqTextStart);
    alt_up_char_buffer_string(char_buf, "52Hz", 6, FreqTextStart + 2);
    alt_up_char_buffer_string(char_buf, "51Hz", 6, FreqTextStart + 4);
    alt_up_char_buffer_string(char_buf, "50Hz", 6, FreqTextStart + 7);
    alt_up_char_buffer_string(char_buf, "49Hz", 6, FreqTextStart + 9);
    alt_up_char_buffer_string(char_buf, "48Hz", 6, FreqTextStart + 12);


    alt_up_pixel_buffer_dma_draw_vline(pixel_buf, 100, RoCYStart, RocYEnd, ColorWhite, 0); // Black RoC Y Axis
    alt_up_pixel_buffer_dma_draw_hline(pixel_buf, GraphXStart, GraphXEnd, RocYEnd, ColorWhite, 0); // Black Freq X axis
    for (int i = 0; i <= 9; i += 2) {
        alt_up_pixel_buffer_dma_draw_hline(pixel_buf, GraphXStart - 10, GraphXStart, RoCYStart + 5 + (i * 10), ColorWhite, 0);
    }
    alt_up_char_buffer_string(char_buf, " -4 Sec       -3 Sec       -2 Sec        -1 Sec        0 Sec", 12, RoCTextStart + 14);
    alt_up_char_buffer_string(char_buf, "Rate of Change (dF/dt Hz/S)", 3, RoCTextStart);
    alt_up_char_buffer_string(char_buf, "+60", 10 - 3, RoCTextStart + 3);
    alt_up_char_buffer_string(char_buf, "+30", 10 - 3, RoCTextStart + 5); // the whole 10-1 thing is about moving the start back
    alt_up_char_buffer_string(char_buf, "0", 10 - 1, RoCTextStart + 8); // based on the number of digits in the number
    alt_up_char_buffer_string(char_buf, "-30", 10 - 3, RoCTextStart + 10);
    alt_up_char_buffer_string(char_buf, "-60", 10 - 3, RoCTextStart + 13);

    alt_up_char_buffer_string(char_buf, "System Status:", FirstColStart, FirstRowHeight);
    alt_up_char_buffer_string(char_buf, "Freq Threshold:", FirstColStart, SecRowHeight);
    alt_up_char_buffer_string(char_buf, "RoC Threshold:", FirstColStart, ThirdRowHeight);
    alt_up_char_buffer_string(char_buf, "Total Time Active:   :  :", FirstColStart, FourthRowHeight);
    alt_up_char_buffer_string(char_buf, "Average Time:", SecondColStart, FirstRowHeight);
    alt_up_char_buffer_string(char_buf, "Max Time:", SecondColStart, SecRowHeight);
    alt_up_char_buffer_string(char_buf, "Min Time:", SecondColStart, ThirdRowHeight);
    alt_up_char_buffer_string(char_buf, "Display Refreshed @ 4Hz", FirstColStart, SixthRowHeight);

    alt_up_char_buffer_string(char_buf, "Last 5 Values:", ThirdColStart, FirstRowHeight);

    while (1) { // TODO: add some vga stuff that shows the management state and current load management
        xQueuePeek(statsQueue, &statsReceiveArray, (TickType_t)0); // total, max, min, average
        xQueuePeek(freqDataQueue, &freqDataInput, (TickType_t)0); // Freq and roc
        xQueuePeek(threshQueue, &thresholdReceiveArray, (TickType_t)0); // freq and roc thresh
        xQueuePeek(stableStatusQueue, &stableStatsInput, (TickType_t)0); // stability

        timeStatCount = xTaskGetTickCount() / configTICK_RATE_HZ;

        for (int i = 17; i > 0; i--) {
            valueArray[i] = valueArray[i - 1];
        }
        valueArray[0] = freqDataInput[0];
        for (int i = 17; i > 0; i--) {
            rocArray[i] = rocArray[i - 1];
        }

        rocArray[0] = freqDataInput[1];

        for (int i = 0; i < 17; i++) {
            int tempStart = GraphXEnd - 25 - i * 26;
            double tempY = 50 + 90 - (valueArray[i] - 48) * 20;
            double tempY2 = 50 + 90 - (valueArray[i + 1] - 48) * 20;
            alt_up_pixel_buffer_dma_draw_box(pixel_buf, tempStart, FreqYStart + 1, tempStart + 26, FreqYEnd - 1, ColorBlack, 0);
            alt_up_pixel_buffer_dma_draw_line(pixel_buf, tempStart, tempY2, tempStart + 26, tempY, ColorBlue, 0);
        }

        for (int i = 0; i < 17; i++) { // TODO: maybe some stuff about getting the largest roc in each refresh period
            int tempStart = GraphXEnd - 25 - i * 26;
            double tempY = RoCYStart + 45 - (rocArray[i]) * 0.75;
            alt_up_pixel_buffer_dma_draw_box(pixel_buf, tempStart, RoCYStart + 1, tempStart + 26, RocYEnd - 1, ColorBlack, 0);
            alt_up_pixel_buffer_dma_draw_box(pixel_buf, tempStart, RoCYStart + 45, tempStart + 26, tempY, ColorBlue, 0);
        }

        // freq, Stability Bool, Freq Thresh x10, RoC Thresh x10, total time in seconds, average time in ms, max time in ms, min time in ms
        if (stableStatsInput) {
            alt_up_char_buffer_string(char_buf, "Stable  ", FirstColStart + 15, FirstRowHeight);
            alt_up_pixel_buffer_dma_draw_box(pixel_buf, (FirstColStart + 15) * 8 - 5, FirstRowHeight * 8 - 5, (FirstColStart + 23) * 8 + 4, (FirstRowHeight + 1) * 8 + 4, ColorBlack, 0);
            alt_up_pixel_buffer_dma_draw_box(pixel_buf, (FirstColStart + 15) * 8 - 5, FirstRowHeight * 8 - 5, (FirstColStart + 21) * 8 + 4, (FirstRowHeight + 1) * 8 + 4, ColorGreen, 0);
        } else {
            alt_up_char_buffer_string(char_buf, "UnStable", FirstColStart + 15, FirstRowHeight);
            alt_up_pixel_buffer_dma_draw_box(pixel_buf, (FirstColStart + 15) * 8 - 5, FirstRowHeight * 8 - 5, (FirstColStart + 23) * 8 + 4, (FirstRowHeight + 1) * 8 + 4, ColorRed, 0);
        }

        // Total Hours
        tempTimeHours = timeStatCount / 3600;
        sprintf(outputBuffer, "%02d", tempTimeHours);
        alt_up_char_buffer_string(char_buf, outputBuffer, FirstColStart + 19, FourthRowHeight);
        // Total Min
        tempTimeMin = (timeStatCount - (3600 * tempTimeHours)) / 60;
        sprintf(outputBuffer, "%02d", tempTimeMin);
        alt_up_char_buffer_string(char_buf, outputBuffer, FirstColStart + 22, FourthRowHeight);
        // Total sec
        sprintf(outputBuffer, "%02d", (timeStatCount - (3600 * tempTimeHours) - (60 * tempTimeMin)));
        alt_up_char_buffer_string(char_buf, outputBuffer, FirstColStart + 25, FourthRowHeight);

        // Live frequency
        sprintf(outputBuffer, "%6.3f Hz", freqDataInput[0]);
        alt_up_char_buffer_string(char_buf, outputBuffer, 69, 11);
        // Live RoC
        sprintf(outputBuffer, "%d Hz/Sec   ", (int)freqDataInput[1]);
        alt_up_char_buffer_string(char_buf, outputBuffer, 69, RoCTextStart + 7);
        // Freq Threshold
        sprintf(outputBuffer, "%4.1f Hz", thresholdReceiveArray[0]);
        alt_up_char_buffer_string(char_buf, outputBuffer, FirstColStart + 16, SecRowHeight);
        //RoC threshold
        sprintf(outputBuffer, "%3.1f Hz/Sec", thresholdReceiveArray[1]);
        alt_up_char_buffer_string(char_buf, outputBuffer, FirstColStart + 15, ThirdRowHeight);
        // Average time
        sprintf(outputBuffer, "%03d ms", statsReceiveArray[0]);
        alt_up_char_buffer_string(char_buf, outputBuffer, SecondColStart + 14, FirstRowHeight);
        // Max time
        sprintf(outputBuffer, "%03d ms", statsReceiveArray[1]);
        alt_up_char_buffer_string(char_buf, outputBuffer, SecondColStart + 10, SecRowHeight);
        // Min time
        sprintf(outputBuffer, "%03d ms", statsReceiveArray[2]);
        alt_up_char_buffer_string(char_buf, outputBuffer, SecondColStart + 10, ThirdRowHeight);


        sprintf(outputBuffer, "    %03d ms", statsReceiveArray[3]);
        alt_up_char_buffer_string(char_buf, outputBuffer, ThirdColStart, SecRowHeight);

        sprintf(outputBuffer, "    %03d ms", statsReceiveArray[4]);
        alt_up_char_buffer_string(char_buf, outputBuffer, ThirdColStart, ThirdRowHeight);

        sprintf(outputBuffer, "    %03d ms", statsReceiveArray[5]);
        alt_up_char_buffer_string(char_buf, outputBuffer, ThirdColStart, FourthRowHeight);

        sprintf(outputBuffer, "    %03d ms", statsReceiveArray[6]);
        alt_up_char_buffer_string(char_buf, outputBuffer, ThirdColStart, FifthRowHeight);

        sprintf(outputBuffer, "    %03d ms", statsReceiveArray[7]);
        alt_up_char_buffer_string(char_buf, outputBuffer, ThirdColStart, SixthRowHeight);

        //printf("VGA Completed in %f Seconds\n", (float)tickCountEnd/configTICK_RATE_HZ);
        vTaskSuspend(t4Handle);
    }
}

