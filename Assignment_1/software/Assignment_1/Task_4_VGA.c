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
#define SecondColStart 32
#define ThirdColStart 54
#define FirstRowHeight 40
#define SecRowHeight 43
#define ThirdRowHeight 46
#define FourthRowHeight 49
#define FifthRowHeight 52


double valueArray[18] = { 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50 };
double rocArray[18];
int statsReceiveArray[8];
char outputBuffer[4];
int tempTimeHours, tempTimeMin;

int tickCountStart, tickCountEnd;

void task_4_VGA_Controller(void* pvParameters) {
    //printf("VGA Controller Started\n");
    // Init Task
    //const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;

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

    /*    Testing bars
    for (int i = 0; i < 80; i++) {
        alt_up_pixel_buffer_dma_draw_vline(pixel_buf, i * 8, 0, 480, ColorCyan, 0);
    }
    for (int i = 0; i < 60; i++) {
        alt_up_pixel_buffer_dma_draw_hline(pixel_buf, 0, 640, i * 8, ColorCyan, 0);
    }
    alt_up_pixel_buffer_dma_draw_rectangle(pixel_buf, 20, 20, 640 - 20, 280, ColorRed, 0); // Red Top
    alt_up_pixel_buffer_dma_draw_rectangle(pixel_buf, 20, 280, 640 - 20, 480 - 20, ColorRed, 0); // Red Bottom

    */alt_up_pixel_buffer_dma_draw_rectangle(pixel_buf, 0, 0, 639, 479, ColorRed, 0);

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
        alt_up_pixel_buffer_dma_draw_hline(pixel_buf, GraphXStart - 10, GraphXStart, RoCYStart + 10 + (i * 10), ColorWhite, 0);
    }
    alt_up_char_buffer_string(char_buf, " -4 Sec       -3 Sec       -2 Sec        -1 Sec        0 Sec", 12, RoCTextStart + 14);
    alt_up_char_buffer_string(char_buf, "Rate of Change (dF/dt Hz/S)", 3, RoCTextStart);
    alt_up_char_buffer_string(char_buf, "8", 10 - 1, RoCTextStart + 3); // TODO: These roc numbers are wrong
    alt_up_char_buffer_string(char_buf, "6", 10 - 1, RoCTextStart + 5); // the whole 10-1 thing is about moving the start back
    alt_up_char_buffer_string(char_buf, "4", 10 - 1, RoCTextStart + 8); // based on the number of digits in the number
    alt_up_char_buffer_string(char_buf, "2", 10 - 1, RoCTextStart + 10);
    alt_up_char_buffer_string(char_buf, "0", 10 - 1, RoCTextStart + 13);

    alt_up_char_buffer_string(char_buf, "System Status:", FirstColStart, FirstRowHeight);
    alt_up_char_buffer_string(char_buf, "Freq Threshold:", FirstColStart, SecRowHeight);
    alt_up_char_buffer_string(char_buf, "RoC Threshold:", FirstColStart, ThirdRowHeight);
    alt_up_char_buffer_string(char_buf, "Total Time Active:   :  :", FirstColStart, FourthRowHeight);
    alt_up_char_buffer_string(char_buf, "Average Time:", SecondColStart, FirstRowHeight);
    alt_up_char_buffer_string(char_buf, "Max Time:", SecondColStart, SecRowHeight);
    alt_up_char_buffer_string(char_buf, "Min Time:", SecondColStart, ThirdRowHeight);
    alt_up_char_buffer_string(char_buf, "Display Refreshed @ 4Hz", ThirdColStart, FourthRowHeight);

    //alt_up_pixel_buffer_dma_draw_rectangle(pixel_buf, 20, 20, 640-20, 480-20, ColorRed, 0); // Red Border
    //alt_up_char_buffer_draw(char_buf, '!', 51, 30);

    while (1) {
        //printf("VGA Loop Started\n");
        //tickCountStart = xTaskGetTickCount();
        xQueueReceive(statsQueue, &statsReceiveArray, (TickType_t)0);

        for (int i = 17; i > 0; i--) {
            valueArray[i] = valueArray[i - 1];
        }
        valueArray[0] = 16000 / (double)statsReceiveArray[0];
        for (int i = 0; i < 17; i++) {
            rocArray[i] = fabs(valueArray[i] - valueArray[i + 1]) / vgaRefreshSec;
        }

        for (int i = 0; i < 17; i++) {
            int tempStart = GraphXEnd - 25 - i * 26;
            double tempY = 50 + 90 - (valueArray[i] - 48) * 20;
            double tempY2 = 50 + 90 - (valueArray[i + 1] - 48) * 20;
            alt_up_pixel_buffer_dma_draw_box(pixel_buf, tempStart, FreqYStart + 1, tempStart + 26, FreqYEnd - 1, ColorBlack, 0);
            alt_up_pixel_buffer_dma_draw_line(pixel_buf, tempStart, tempY2, tempStart + 26, tempY, ColorBlue, 0);
        }

        for (int i = 0; i < 17; i++) {
            int tempStart = GraphXEnd - 25 - i * 26;
            double tempY = RocYEnd - 1 - (rocArray[i]) * 10;
            alt_up_pixel_buffer_dma_draw_box(pixel_buf, tempStart, RoCYStart + 1, tempStart + 26, RocYEnd - 1, ColorBlack, 0);
            alt_up_pixel_buffer_dma_draw_box(pixel_buf, tempStart, tempY, tempStart + 26, RocYEnd - 1, ColorBlue, 0);
        }

        // freq, Stability Bool, Freq Thresh x10, RoC Thresh x10, total time in seconds, average time in ms, max time in ms, min time in ms
        if (statsReceiveArray[1]) {
            alt_up_char_buffer_string(char_buf, "Stable  ", FirstColStart + 15, FirstRowHeight);
            alt_up_pixel_buffer_dma_draw_box(pixel_buf, (FirstColStart + 15) * 8 - 5, FirstRowHeight * 8 - 5, (FirstColStart + 23) * 8 + 4, (FirstRowHeight + 1) * 8 + 4, ColorBlack, 0);
            alt_up_pixel_buffer_dma_draw_box(pixel_buf, (FirstColStart + 15) * 8 - 5, FirstRowHeight * 8 - 5, (FirstColStart + 21) * 8 + 4, (FirstRowHeight + 1) * 8 + 4, ColorGreen, 0);
        } else {
            alt_up_char_buffer_string(char_buf, "UnStable", FirstColStart + 15, FirstRowHeight);
            alt_up_pixel_buffer_dma_draw_box(pixel_buf, (FirstColStart + 15) * 8 - 5, FirstRowHeight * 8 - 5, (FirstColStart + 23) * 8 + 4, (FirstRowHeight + 1) * 8 + 4, ColorRed, 0);
        }

        // Live frequency
        sprintf(outputBuffer, "%6.3f Hz", (16000 / (double)statsReceiveArray[0]));
        alt_up_char_buffer_string(char_buf, outputBuffer, 69, 11);
        // Live RoC
        sprintf(outputBuffer, "%3.1f Hz/Sec", rocArray[0]);
        alt_up_char_buffer_string(char_buf, outputBuffer, 69, RoCTextStart + 7);
        // Freq Threshold
        sprintf(outputBuffer, "%4.1f Hz", (float)statsReceiveArray[2] / 10);
        alt_up_char_buffer_string(char_buf, outputBuffer, FirstColStart + 16, SecRowHeight);
        //RoC threshold
        sprintf(outputBuffer, "%3.1f Hz/Sec", (float)statsReceiveArray[3] / 10);
        alt_up_char_buffer_string(char_buf, outputBuffer, FirstColStart + 15, ThirdRowHeight);
        // Total Hours
        tempTimeHours = statsReceiveArray[4] / 3600;
        sprintf(outputBuffer, "%02d", tempTimeHours);
        alt_up_char_buffer_string(char_buf, outputBuffer, FirstColStart + 19, FourthRowHeight);
        // Total Min
        tempTimeMin = (statsReceiveArray[4] - (3600 * tempTimeHours)) / 60;
        sprintf(outputBuffer, "%02d", tempTimeMin);
        alt_up_char_buffer_string(char_buf, outputBuffer, FirstColStart + 22, FourthRowHeight);
        // Total sec
        sprintf(outputBuffer, "%02d", (statsReceiveArray[4] - (3600 * tempTimeHours) - (60 * tempTimeMin)));
        alt_up_char_buffer_string(char_buf, outputBuffer, FirstColStart + 25, FourthRowHeight);
        // Average time
        sprintf(outputBuffer, "%03d ms", statsReceiveArray[5]);
        alt_up_char_buffer_string(char_buf, outputBuffer, SecondColStart + 14, FirstRowHeight);
        // Max time
        sprintf(outputBuffer, "%03d ms", statsReceiveArray[6]);
        alt_up_char_buffer_string(char_buf, outputBuffer, SecondColStart + 10, SecRowHeight);
        // Min time
        sprintf(outputBuffer, "%03d ms", statsReceiveArray[7]);
        alt_up_char_buffer_string(char_buf, outputBuffer, SecondColStart + 10, ThirdRowHeight);


        //tickCountEnd = (xTaskGetTickCount() - tickCountStart);
        //printf("VGA Completed in %f Seconds\n", (float)tickCountEnd/configTICK_RATE_HZ);
        vTaskSuspend(t4Handle);
    }
}

