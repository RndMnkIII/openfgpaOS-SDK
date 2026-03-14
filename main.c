/*
 * openfpgaOS SDK Example App
 *
 * Draws a color gradient, waits for button presses.
 * Press A to randomize, START to exit.
 */

#include "of.h"
#include <string.h>

int main(void) {
    of_video_init();

    /* Set up a gradient palette */
    for (int i = 0; i < 256; i++)
        of_video_palette(i, (i << 16) | ((255 - i) << 8) | 128);

    /* Draw gradient */
    uint8_t *fb = of_video_surface();
    for (int y = 0; y < OF_SCREEN_H; y++)
        memset(&fb[y * OF_SCREEN_W], y, OF_SCREEN_W);

    of_video_flip();

    of_print("Hello from openfpgaOS SDK!\n");
    of_print("Press A to randomize, START to exit.\n");

    while (1) {
        of_input_poll();

        if (of_btn_pressed(BTN_START))
            of_exit();

        if (of_btn_pressed(BTN_A)) {
            fb = of_video_surface();
            uint8_t base = (uint8_t)(of_time_ms() & 0xFF);
            for (int y = 0; y < OF_SCREEN_H; y++)
                memset(&fb[y * OF_SCREEN_W], (y + base) & 0xFF, OF_SCREEN_W);
            of_video_flip();
        }
    }

    return 0;
}
