#include "Task_4.h"

#define ColorWhite ((0x3ff << 20) + (0x3ff << 10) + (0x3ff))
#define ColorBlack (0)
#define ColorGreen (0x3ff)
#define ColorRed (0x3ff << 10)
#define ColorBlue (0x3ff << 20)
#define ColorCyan ((0x3ff << 20) + (0x3ff))

#define FreqYStart (20+30)
#define FreqYEnd (20+30+90)
#define GraphXStart 100
#define GraphXEnd (640-20-30-48)
// 17 values with 26 pixels per x value

#define RoCYStart (20+30+90+30)
#define RocYEnd (20+30+90+30+90)

// Char buffer coords are 80x60

volatile double valueArray[18] = { 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50 };
volatile double rocArray[18];

void task_4_VGA_Controller(void* pvParameters) {
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

    */



    // Draw Freq/Time Graph
    alt_up_pixel_buffer_dma_draw_vline(pixel_buf, GraphXStart, FreqYStart, FreqYEnd, ColorWhite, 0); // Black Freq Y axis
    alt_up_pixel_buffer_dma_draw_hline(pixel_buf, GraphXStart, GraphXEnd, FreqYEnd, ColorWhite, 0); // Black Freq X axis
    for (int i = 0; i <= 9; i += 2) {
        alt_up_pixel_buffer_dma_draw_hline(pixel_buf, GraphXStart - 10, GraphXStart, FreqYStart + (i * 10), ColorWhite, 0);
    }
    alt_up_char_buffer_string(char_buf, "Frequency (Hz)", 3, 4);
    alt_up_char_buffer_string(char_buf, "52Hz", 6, 6);
    alt_up_char_buffer_string(char_buf, "51Hz", 6, 8);
    alt_up_char_buffer_string(char_buf, "50Hz", 6, 11);
    alt_up_char_buffer_string(char_buf, "49Hz", 6, 13);
    alt_up_char_buffer_string(char_buf, "48Hz", 6, 16);


    alt_up_pixel_buffer_dma_draw_vline(pixel_buf, 100, RoCYStart, RocYEnd, ColorWhite, 0); // Black RoC Y Axis
    alt_up_pixel_buffer_dma_draw_hline(pixel_buf, GraphXStart, GraphXEnd, RocYEnd, ColorWhite, 0); // Black Freq X axis
    for (int i = 0; i <= 9; i += 2) {
        alt_up_pixel_buffer_dma_draw_hline(pixel_buf, GraphXStart - 10, GraphXStart, RoCYStart + (i * 10), ColorWhite, 0);
    }
    alt_up_char_buffer_string(char_buf, "Rate of Change (dF/dt Hz/S)", 3, 19);
    alt_up_char_buffer_string(char_buf, "2", 10-1, 21); // TODO: These roc numbers are wrong
    alt_up_char_buffer_string(char_buf, "1.5", 10-3, 23); // the whole 10-1 thing is about moving the start back
    alt_up_char_buffer_string(char_buf, "1", 10-1, 26); // based on the number of digits in the number
    alt_up_char_buffer_string(char_buf, "0.5", 10-3, 28);
    alt_up_char_buffer_string(char_buf, "0", 10-1, 31);


    //alt_up_pixel_buffer_dma_draw_rectangle(pixel_buf, 20, 20, 640-20, 480-20, ColorRed, 0); // Red Border


    //alt_up_pixel_buffer_dma_draw_line(pixel_buf, 200, 100, 100, 200, 0x3ff << 20, 0);
    //alt_up_pixel_buffer_dma_draw_hline(pixel_buf, 50, 100, , 0x3ff << 20, 0);
    //alt_up_pixel_buffer_dma_draw_vline(pixel_buf, 110, FreqYStart, FreqYStart+10, ColorBlue, 0);
    //usleep(1000000);
    //alt_up_char_buffer_draw(char_buf, '!', 51, 30);

    while (1) {
        usleep(500000);
        unsigned int temp = IORD(FREQUENCY_ANALYSER_BASE, 0);
        for (int i = 17; i > 0; i--) {
            valueArray[i] = valueArray[i - 1];
        }
        valueArray[0] = 16000 / (double)temp;
        for (int i = 0; i < 17; i++) {
            rocArray[i] = fabs(valueArray[i] - valueArray[i + 1]);
        }

        for (int i = 0; i < 17; i++) {
            int tempStart = GraphXEnd - 25 - i * 26;
            double tempY = 50 + 90 - (valueArray[i] - 48) * 20;
            double tempY2 = 50 + 90 - (valueArray[i + 1] - 48) * 20;
            alt_up_pixel_buffer_dma_draw_box(pixel_buf, tempStart, FreqYStart + 1, tempStart + 26, FreqYEnd - 1, ColorBlack, 0);
            alt_up_pixel_buffer_dma_draw_line(pixel_buf, tempStart, tempY2, tempStart + 26, tempY, ColorBlue, 0);
        }
        alt_up_char_buffer_string(char_buf, "50.000Hz", 69, 11); // TODO: make this change lol

        for (int i = 0; i < 17; i++) {
            int tempStart = GraphXEnd - 25 - i * 26;
            double tempY = 170 + 80 - (rocArray[i]) * 20;
            //double tempY2 = 170 + 90 - (rocArray[i+1] - 48) * 20;
            alt_up_pixel_buffer_dma_draw_box(pixel_buf, tempStart, RoCYStart + 1, tempStart + 26, RocYEnd - 1, ColorBlack, 0);
            alt_up_pixel_buffer_dma_draw_line(pixel_buf, tempStart, tempY, tempStart + 26, tempY, ColorBlue, 0);
        }

        printf("%f Hz\n", 16000 / (double)temp);
    }
}

