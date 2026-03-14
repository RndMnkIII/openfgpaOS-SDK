/*
 * of.h -- openfpgaOS Application API
 *
 * One header. Everything you need to make a game on Analogue Pocket.
 * Compile with -DOF_PC and link of_sdl2.c to build and test on PC.
 *
 *   #include "of.h"
 *
 *   int main(void) {
 *       of_video_init();
 *       while (1) {
 *           of_input_poll();
 *           if (of_btn(BTN_A)) shoot();
 *           of_video_clear(0);
 *           draw_world();
 *           of_video_flip();
 *       }
 *   }
 *
 * Platform: Analogue Pocket FPGA, RISC-V (VexRiscv) @ 100 MHz
 * Video:    320x240, 8-bit indexed color, double-buffered
 * Audio:    YM2151 FM synthesis (8 channels) + 48 kHz PCM FIFO
 * Input:    2 controllers, d-pad + ABXY + L/R + sticks + triggers
 * Memory:   64 MB SDRAM, 16 MB CRAM0, 16 MB CRAM1, 256 KB SRAM
 */

#ifndef OF_H
#define OF_H

#include <stdint.h>

/* ======================================================================
 * Platform Constants
 * ====================================================================== */

#define OF_SCREEN_W     320
#define OF_SCREEN_H     240
#define OF_CPU_HZ       100000000
#define OF_AUDIO_RATE   48000
#define OF_AUDIO_FIFO   4096
#define OF_OPM_CHANNELS 8
#define OF_MAX_PLAYERS  2

/* ======================================================================
 * Button Masks
 * ====================================================================== */

#define BTN_UP      (1 << 0)
#define BTN_DOWN    (1 << 1)
#define BTN_LEFT    (1 << 2)
#define BTN_RIGHT   (1 << 3)
#define BTN_A       (1 << 4)
#define BTN_B       (1 << 5)
#define BTN_X       (1 << 6)
#define BTN_Y       (1 << 7)
#define BTN_L1      (1 << 8)
#define BTN_R1      (1 << 9)
#define BTN_L2      (1 << 10)
#define BTN_R2      (1 << 11)
#define BTN_L3      (1 << 12)
#define BTN_R3      (1 << 13)
#define BTN_SELECT  (1 << 14)
#define BTN_START   (1 << 15)

/* ======================================================================
 * Shared Types
 * ====================================================================== */

typedef struct {
    uint32_t buttons;
    uint32_t buttons_pressed;
    uint32_t buttons_released;
    int16_t  joy_lx, joy_ly;
    int16_t  joy_rx, joy_ry;
    uint16_t trigger_l, trigger_r;
} of_input_state_t;

typedef struct {
    uint8_t  enabled;
    uint8_t  video_mode;
    uint8_t  snac_type;
    uint8_t  snac_assignment;
    int8_t   h_offset;
    int8_t   v_offset;
} of_analogizer_state_t;

/* ======================================================================
 * Tile / Sprite Constants
 * ====================================================================== */

#define OF_TILE_COLS    64
#define OF_TILE_ROWS    32
#define OF_TILE_SIZE    8
#define OF_MAX_TILES    256
#define OF_MAX_SPRITES  64

#ifndef OF_PC
/* ======================================================================
 * Internal: Syscall Mechanism (FPGA / RISC-V only)
 * ====================================================================== */

static inline long __of_syscall0(long n) {
    register long a7 __asm__("a7") = n;
    register long a0 __asm__("a0");
    __asm__ volatile("ecall" : "=r"(a0) : "r"(a7) : "memory");
    return a0;
}

static inline long __of_syscall1(long n, long arg0) {
    register long a7 __asm__("a7") = n;
    register long a0 __asm__("a0") = arg0;
    __asm__ volatile("ecall" : "+r"(a0) : "r"(a7) : "memory");
    return a0;
}

static inline long __of_syscall2(long n, long arg0, long arg1) {
    register long a7 __asm__("a7") = n;
    register long a0 __asm__("a0") = arg0;
    register long a1 __asm__("a1") = arg1;
    __asm__ volatile("ecall" : "+r"(a0) : "r"(a7), "r"(a1) : "memory");
    return a0;
}

static inline long __of_syscall3(long n, long arg0, long arg1, long arg2) {
    register long a7 __asm__("a7") = n;
    register long a0 __asm__("a0") = arg0;
    register long a1 __asm__("a1") = arg1;
    register long a2 __asm__("a2") = arg2;
    __asm__ volatile("ecall" : "+r"(a0) : "r"(a7), "r"(a1), "r"(a2) : "memory");
    return a0;
}

static inline long __of_syscall4(long n, long arg0, long arg1,
                                     long arg2, long arg3) {
    register long a7 __asm__("a7") = n;
    register long a0 __asm__("a0") = arg0;
    register long a1 __asm__("a1") = arg1;
    register long a2 __asm__("a2") = arg2;
    register long a3 __asm__("a3") = arg3;
    __asm__ volatile("ecall" : "+r"(a0) : "r"(a7), "r"(a1), "r"(a2), "r"(a3) : "memory");
    return a0;
}

static inline long __of_syscall5(long n, long arg0, long arg1,
                                     long arg2, long arg3, long arg4) {
    register long a7 __asm__("a7") = n;
    register long a0 __asm__("a0") = arg0;
    register long a1 __asm__("a1") = arg1;
    register long a2 __asm__("a2") = arg2;
    register long a3 __asm__("a3") = arg3;
    register long a4 __asm__("a4") = arg4;
    __asm__ volatile("ecall" : "+r"(a0) : "r"(a7), "r"(a1), "r"(a2), "r"(a3), "r"(a4) : "memory");
    return a0;
}

/* Syscall numbers */
#define _SYS_VIDEO_INIT         0x1000
#define _SYS_VIDEO_FLIP         0x1001
#define _SYS_VIDEO_SYNC         0x1002
#define _SYS_VIDEO_PALETTE      0x1003
#define _SYS_VIDEO_SURFACE      0x1004
#define _SYS_VIDEO_DISPMODE     0x1005
#define _SYS_VIDEO_CLEAR        0x1006
#define _SYS_VIDEO_PALETTE_BULK 0x1007
#define _SYS_VIDEO_FLUSH        0x1008

#define _SYS_AUDIO_WRITE        0x1010
#define _SYS_AUDIO_FREE         0x1011
#define _SYS_OPM_WRITE          0x1012
#define _SYS_OPM_RESET          0x1013
#define _SYS_AUDIO_INIT         0x1014

#define _SYS_INPUT_POLL         0x1020
#define _SYS_INPUT_STATE        0x1021

#define _SYS_SAVE_READ          0x1030
#define _SYS_SAVE_WRITE         0x1031
#define _SYS_SAVE_FLUSH         0x1032
#define _SYS_SAVE_ERASE         0x1033

#define _SYS_ANLG_STATE         0x1040
#define _SYS_ANLG_ENABLED       0x1041

#define _SYS_TERM_PUTCHAR       0x1050
#define _SYS_TERM_CLEAR         0x1051
#define _SYS_TERM_SETPOS        0x1053

#define _SYS_LINK_SEND          0x1060
#define _SYS_LINK_RECV          0x1061
#define _SYS_LINK_STATUS        0x1062

#define _SYS_TIME_US            0x1070
#define _SYS_TIME_MS            0x1071
#define _SYS_DELAY_US           0x1072
#define _SYS_DELAY_MS           0x1073

#define _SYS_FILE_READ          0x1080
#define _SYS_FILE_SIZE          0x1081

#define _SYS_TILE_ENABLE        0x1090
#define _SYS_TILE_SCROLL        0x1091
#define _SYS_TILE_SET           0x1092
#define _SYS_TILE_LOAD_MAP      0x1093
#define _SYS_TILE_LOAD_CHR      0x1094

#define _SYS_SPRITE_ENABLE      0x10A0
#define _SYS_SPRITE_SET         0x10A1
#define _SYS_SPRITE_MOVE        0x10A2
#define _SYS_SPRITE_LOAD_CHR    0x10A3
#define _SYS_SPRITE_HIDE        0x10A4
#define _SYS_SPRITE_HIDE_ALL    0x10A5

/* ======================================================================
 * Video
 *
 * 320x240 framebuffer, 8-bit indexed color, 256-entry palette.
 * Double-buffered: draw to the back buffer, flip to display.
 * ====================================================================== */

/* Switch to framebuffer mode and clear both buffers. */
static inline void of_video_init(void) {
    __of_syscall0(_SYS_VIDEO_INIT);
}

/* Get pointer to the current draw buffer. Write pixels here. */
static inline uint8_t *of_video_surface(void) {
    return (uint8_t *)__of_syscall0(_SYS_VIDEO_SURFACE);
}

/* Swap draw/display buffers (returns immediately). */
static inline void of_video_flip(void) {
    __of_syscall0(_SYS_VIDEO_FLIP);
}

/* Block until the last flip is complete. */
static inline void of_video_sync(void) {
    __of_syscall0(_SYS_VIDEO_SYNC);
}

/* Fill the draw buffer with a palette index. */
static inline void of_video_clear(uint8_t color) {
    __of_syscall1(_SYS_VIDEO_CLEAR, color);
}

/* Set one palette entry. rgb = 0x00RRGGBB. */
static inline void of_video_palette(uint8_t index, uint32_t rgb) {
    __of_syscall2(_SYS_VIDEO_PALETTE, index, rgb);
}

/* Load up to 256 palette entries from an array of 0x00RRGGBB values. */
static inline void of_video_palette_bulk(const uint32_t *pal, int count) {
    __of_syscall2(_SYS_VIDEO_PALETTE_BULK, (long)pal, count);
}

/* Flush the D-cache for the draw buffer (rarely needed). */
static inline void of_video_flush(void) {
    __of_syscall0(_SYS_VIDEO_FLUSH);
}

/* Draw a single pixel (bounds-checked). */
static inline void of_video_pixel(int x, int y, uint8_t color) {
    if ((unsigned)x < OF_SCREEN_W && (unsigned)y < OF_SCREEN_H) {
        uint8_t *fb = of_video_surface();
        fb[y * OF_SCREEN_W + x] = color;
    }
}

/* ======================================================================
 * Input
 *
 * Call of_input_poll() once per frame, then use of_btn() to check buttons.
 * ====================================================================== */

/* Cached input state (updated by of_input_poll). */
static of_input_state_t __of_p0, __of_p1;

/* Read all controllers. Call once per frame. */
static inline void of_input_poll(void) {
    __of_syscall0(_SYS_INPUT_POLL);
    __of_syscall2(_SYS_INPUT_STATE, 0, (long)&__of_p0);
    __of_syscall2(_SYS_INPUT_STATE, 1, (long)&__of_p1);
}

/* Is button held right now? (player 0) */
static inline int of_btn(uint32_t mask) {
    return (__of_p0.buttons & mask) != 0;
}

/* Was button just pressed this frame? (player 0) */
static inline int of_btn_pressed(uint32_t mask) {
    return (__of_p0.buttons_pressed & mask) != 0;
}

/* Was button just released this frame? (player 0) */
static inline int of_btn_released(uint32_t mask) {
    return (__of_p0.buttons_released & mask) != 0;
}

/* Is button held right now? (player 1) */
static inline int of_btn_p2(uint32_t mask) {
    return (__of_p1.buttons & mask) != 0;
}

/* Was button just pressed this frame? (player 1) */
static inline int of_btn_pressed_p2(uint32_t mask) {
    return (__of_p1.buttons_pressed & mask) != 0;
}

/* Was button just released this frame? (player 1) */
static inline int of_btn_released_p2(uint32_t mask) {
    return (__of_p1.buttons_released & mask) != 0;
}

/* Get full input state (sticks, triggers) for a given player. */
static inline uint32_t of_input_state(int player, of_input_state_t *state) {
    return (uint32_t)__of_syscall2(_SYS_INPUT_STATE, player, (long)state);
}

/* ======================================================================
 * Audio -- PCM
 *
 * 48 kHz stereo. Write interleaved int16_t pairs (L, R) to the FIFO.
 * ====================================================================== */

/* Initialize the audio subsystem. */
static inline void of_audio_init(void) {
    __of_syscall0(_SYS_AUDIO_INIT);
}

/* Write stereo sample pairs to the FIFO. Returns number of pairs written. */
static inline int of_audio_write(const int16_t *samples, int count) {
    return (int)__of_syscall2(_SYS_AUDIO_WRITE, (long)samples, count);
}

/* Get number of free entries in the audio FIFO. */
static inline int of_audio_free(void) {
    return (int)__of_syscall0(_SYS_AUDIO_FREE);
}

/* ======================================================================
 * Audio -- OPM (YM2151 FM Synthesis)
 *
 * 8 channels of FM synthesis. Write directly to YM2151 registers.
 * See YM2151 application manual for register map.
 * ====================================================================== */

/* Write a value to a YM2151 register. */
static inline void of_opm_write(uint8_t reg, uint8_t val) {
    __of_syscall2(_SYS_OPM_WRITE, reg, val);
}

/* Silence all channels and reset the OPM. */
static inline void of_opm_reset(void) {
    __of_syscall0(_SYS_OPM_RESET);
}

/* ======================================================================
 * Timer
 * ====================================================================== */

/* Microseconds since boot (wraps at ~4295 seconds). */
static inline uint32_t of_time_us(void) {
    return (uint32_t)__of_syscall0(_SYS_TIME_US);
}

/* Milliseconds since boot. */
static inline uint32_t of_time_ms(void) {
    return (uint32_t)__of_syscall0(_SYS_TIME_MS);
}

/* Busy-wait for the given number of microseconds. */
static inline void of_delay_us(uint32_t us) {
    __of_syscall1(_SYS_DELAY_US, us);
}

/* Busy-wait for the given number of milliseconds. */
static inline void of_delay_ms(uint32_t ms) {
    __of_syscall1(_SYS_DELAY_MS, ms);
}

/* ======================================================================
 * Save Files
 *
 * Persistent storage backed by APF save slots.
 * ====================================================================== */

/* Read from a save slot into buf. Returns bytes read or negative error. */
static inline int of_save_read(int slot, void *buf,
                               uint32_t offset, uint32_t len) {
    return (int)__of_syscall4(_SYS_SAVE_READ, slot,
                              (long)buf, offset, len);
}

/* Write to a save slot from buf. Returns bytes written or negative error. */
static inline int of_save_write(int slot, const void *buf,
                                uint32_t offset, uint32_t len) {
    return (int)__of_syscall4(_SYS_SAVE_WRITE, slot,
                              (long)buf, offset, len);
}

/* Ensure save data is persisted to SD card. */
static inline void of_save_flush(int slot) {
    __of_syscall1(_SYS_SAVE_FLUSH, slot);
}

/* Erase a save slot (fill with 0xFF). */
static inline void of_save_erase(int slot) {
    __of_syscall1(_SYS_SAVE_ERASE, slot);
}

/* ======================================================================
 * File I/O (APF Data Slots)
 *
 * Read game assets from data slots defined in data.json.
 * ====================================================================== */

/* Read from a data slot. Returns 0 on success, negative on error. */
static inline int of_file_read(uint32_t slot_id, uint32_t offset,
                               void *dest, uint32_t length) {
    return (int)__of_syscall4(_SYS_FILE_READ,
                              slot_id, offset, (long)dest, length);
}

/* Get the size of a data slot in bytes. Returns negative on error. */
static inline long of_file_size(uint32_t slot_id) {
    return __of_syscall1(_SYS_FILE_SIZE, slot_id);
}

/* ======================================================================
 * Link Cable
 * ====================================================================== */

/* Send a 32-bit word. Returns 0 on success, -1 if busy. */
static inline int of_link_send(uint32_t data) {
    return (int)__of_syscall1(_SYS_LINK_SEND, data);
}

/* Receive a 32-bit word. Returns 0 on success, -1 if empty. */
static inline int of_link_recv(uint32_t *data) {
    return (int)__of_syscall1(_SYS_LINK_RECV, (long)data);
}

/* Get link cable status flags. */
static inline uint32_t of_link_status(void) {
    return (uint32_t)__of_syscall0(_SYS_LINK_STATUS);
}

/* ======================================================================
 * Terminal (Debug Output)
 *
 * 40x30 text console, useful for debug prints.
 * ====================================================================== */

/* Print a string to the debug terminal. */
static inline void of_print(const char *s) {
    while (*s) __of_syscall1(_SYS_TERM_PUTCHAR, *s++);
}

/* Print a single character. */
static inline void of_print_char(char c) {
    __of_syscall1(_SYS_TERM_PUTCHAR, c);
}

/* Clear the debug terminal. */
static inline void of_print_clear(void) {
    __of_syscall0(_SYS_TERM_CLEAR);
}

/* Set the cursor position (0-based). */
static inline void of_print_at(int col, int row) {
    __of_syscall2(_SYS_TERM_SETPOS, col, row);
}

/* ======================================================================
 * Analogizer
 * ====================================================================== */

/* Check if Analogizer is connected. */
static inline int of_analogizer_enabled(void) {
    return (int)__of_syscall0(_SYS_ANLG_ENABLED);
}

/* Get full Analogizer state. */
static inline int of_analogizer_state(of_analogizer_state_t *state) {
    return (int)__of_syscall1(_SYS_ANLG_STATE, (long)state);
}

/* ======================================================================
 * Tile Layer
 *
 * 64x32 tilemap of 8x8 tiles. 4bpp with 16 sub-palettes.
 * Tilemap entry: tile_id (8-bit), palette (4-bit), hflip, vflip.
 * Hardware-scrolled. Shares the 256-color framebuffer palette.
 * ====================================================================== */

/* Enable/disable tile layer. priority: 0=behind FB, 1=over FB. */
static inline void of_tile_enable(int enable, int priority) {
    __of_syscall2(_SYS_TILE_ENABLE, enable, priority);
}

/* Set tile scroll position (pixels). */
static inline void of_tile_scroll(int x, int y) {
    __of_syscall2(_SYS_TILE_SCROLL, x, y);
}

/* Set a single tilemap entry at (col, row). */
static inline void of_tile_set(int col, int row, uint16_t entry) {
    __of_syscall3(_SYS_TILE_SET, col, row, entry);
}

/* Load a rectangular region of the tilemap. */
static inline void of_tile_load_map(const uint16_t *data,
                                     int x, int y, int w, int h) {
    __of_syscall5(_SYS_TILE_LOAD_MAP, (long)data, x, y, w, h);
}

/* Load tile character data (4bpp, 32 bytes/tile). */
static inline void of_tile_load_chr(int first_tile,
                                     const void *data, int num_tiles) {
    __of_syscall3(_SYS_TILE_LOAD_CHR, first_tile, (long)data, num_tiles);
}

/* ======================================================================
 * Sprite Engine
 *
 * 64 hardware sprites. 8x8 pixels, 4bpp, per-sprite flip/palette.
 * Shares the 256-color framebuffer palette.
 * ====================================================================== */

/* Enable/disable sprite engine. */
static inline void of_sprite_enable(int enable) {
    __of_syscall1(_SYS_SPRITE_ENABLE, enable);
}

/* Set a sprite's position, tile, and attributes. */
static inline void of_sprite_set(int index, int x, int y,
                                  int tile_id, int palette,
                                  int hflip, int vflip, int enable) {
    long packed = (palette & 0xF) |
                  ((hflip & 1) << 4) |
                  ((vflip & 1) << 5) |
                  ((enable & 1) << 6);
    __of_syscall5(_SYS_SPRITE_SET, index, x, y, tile_id, packed);
}

/* Move a sprite without changing its other attributes. */
static inline void of_sprite_move(int index, int x, int y) {
    __of_syscall3(_SYS_SPRITE_MOVE, index, x, y);
}

/* Load sprite character data (same 4bpp format as tiles). */
static inline void of_sprite_load_chr(int first_tile,
                                       const void *data, int num_tiles) {
    __of_syscall3(_SYS_SPRITE_LOAD_CHR, first_tile, (long)data, num_tiles);
}

/* Hide a single sprite. */
static inline void of_sprite_hide(int index) {
    __of_syscall1(_SYS_SPRITE_HIDE, index);
}

/* Hide all sprites. */
static inline void of_sprite_hide_all(void) {
    __of_syscall0(_SYS_SPRITE_HIDE_ALL);
}

/* ======================================================================
 * System
 * ====================================================================== */

/* Exit the application and return to the OS. */
static inline void of_exit(void) {
    __of_syscall0(93); /* SYS_exit */
    __builtin_unreachable();
}

#else /* OF_PC -- SDL2 backend */
/* ======================================================================
 * PC (SDL2) Backend Declarations
 *
 * All functions are implemented in of_sdl2.c.
 * Build with: cc -DOF_PC app.c of_sdl2.c $(sdl2-config --cflags --libs) -lm
 * ====================================================================== */

/* Video */
void     of_video_init(void);
uint8_t *of_video_surface(void);
void     of_video_flip(void);
void     of_video_sync(void);
void     of_video_clear(uint8_t color);
void     of_video_palette(uint8_t index, uint32_t rgb);
void     of_video_palette_bulk(const uint32_t *pal, int count);
void     of_video_flush(void);
static inline void of_video_pixel(int x, int y, uint8_t color) {
    if ((unsigned)x < OF_SCREEN_W && (unsigned)y < OF_SCREEN_H) {
        uint8_t *fb = of_video_surface();
        fb[y * OF_SCREEN_W + x] = color;
    }
}

/* Input */
void     of_input_poll(void);
int      of_btn(uint32_t mask);
int      of_btn_pressed(uint32_t mask);
int      of_btn_released(uint32_t mask);
int      of_btn_p2(uint32_t mask);
int      of_btn_pressed_p2(uint32_t mask);
int      of_btn_released_p2(uint32_t mask);
uint32_t of_input_state(int player, of_input_state_t *state);

/* Audio */
void of_audio_init(void);
int  of_audio_write(const int16_t *samples, int count);
int  of_audio_free(void);
void of_opm_write(uint8_t reg, uint8_t val);
void of_opm_reset(void);

/* Timer */
uint32_t of_time_us(void);
uint32_t of_time_ms(void);
void     of_delay_us(uint32_t us);
void     of_delay_ms(uint32_t ms);

/* Save */
int  of_save_read(int slot, void *buf, uint32_t offset, uint32_t len);
int  of_save_write(int slot, const void *buf, uint32_t offset, uint32_t len);
void of_save_flush(int slot);
void of_save_erase(int slot);

/* File I/O */
int  of_file_read(uint32_t slot_id, uint32_t offset, void *dest, uint32_t length);
long of_file_size(uint32_t slot_id);

/* Link (stubs on PC) */
int      of_link_send(uint32_t data);
int      of_link_recv(uint32_t *data);
uint32_t of_link_status(void);

/* Terminal */
void of_print(const char *s);
void of_print_char(char c);
void of_print_clear(void);
void of_print_at(int col, int row);

/* Analogizer (stubs on PC) */
int of_analogizer_enabled(void);
int of_analogizer_state(of_analogizer_state_t *state);

/* Tile Layer */
void of_tile_enable(int enable, int priority);
void of_tile_scroll(int x, int y);
void of_tile_set(int col, int row, uint16_t entry);
void of_tile_load_map(const uint16_t *data, int x, int y, int w, int h);
void of_tile_load_chr(int first_tile, const void *data, int num_tiles);

/* Sprite Engine */
void of_sprite_enable(int enable);
void of_sprite_set(int index, int x, int y, int tile_id, int palette,
                   int hflip, int vflip, int enable);
void of_sprite_move(int index, int x, int y);
void of_sprite_load_chr(int first_tile, const void *data, int num_tiles);
void of_sprite_hide(int index);
void of_sprite_hide_all(void);

/* System */
void of_exit(void);

#endif /* OF_PC */

/* ======================================================================
 * Tile entry helper (pure computation, available on both platforms)
 * ====================================================================== */
static inline uint16_t of_tile_entry(int tile_id, int palette,
                                      int hflip, int vflip) {
    return (uint16_t)(
        (tile_id & 0xFF) |
        ((palette & 0xF) << 10) |
        (hflip ? (1 << 14) : 0) |
        (vflip ? (1 << 15) : 0)
    );
}

/* ======================================================================
 * Blit Utilities (available on both platforms)
 *
 * Fast rectangular copy/fill operations on the framebuffer.
 * ====================================================================== */

/* Blit a rectangular region from src buffer to the framebuffer. */
static inline void of_blit(int dx, int dy, int w, int h,
                            const uint8_t *src, int src_stride) {
    uint8_t *fb = of_video_surface();
    for (int y = 0; y < h; y++) {
        int fy = dy + y;
        if ((unsigned)fy >= OF_SCREEN_H) continue;
        for (int x = 0; x < w; x++) {
            int fx = dx + x;
            if ((unsigned)fx >= OF_SCREEN_W) continue;
            uint8_t px = src[y * src_stride + x];
            if (px) fb[fy * OF_SCREEN_W + fx] = px;
        }
    }
}

/* Blit with a fixed palette offset (adds pal_offset to every non-zero pixel). */
static inline void of_blit_pal(int dx, int dy, int w, int h,
                                const uint8_t *src, int src_stride,
                                uint8_t pal_offset) {
    uint8_t *fb = of_video_surface();
    for (int y = 0; y < h; y++) {
        int fy = dy + y;
        if ((unsigned)fy >= OF_SCREEN_H) continue;
        for (int x = 0; x < w; x++) {
            int fx = dx + x;
            if ((unsigned)fx >= OF_SCREEN_W) continue;
            uint8_t px = src[y * src_stride + x];
            if (px) fb[fy * OF_SCREEN_W + fx] = px + pal_offset;
        }
    }
}

/* Fill a rectangle with a solid palette index. */
static inline void of_fill_rect(int x, int y, int w, int h, uint8_t color) {
    uint8_t *fb = of_video_surface();
    for (int ry = 0; ry < h; ry++) {
        int fy = y + ry;
        if ((unsigned)fy >= OF_SCREEN_H) continue;
        for (int rx = 0; rx < w; rx++) {
            int fx = x + rx;
            if ((unsigned)fx >= OF_SCREEN_W) continue;
            fb[fy * OF_SCREEN_W + fx] = color;
        }
    }
}

/* ======================================================================
 * Collision Detection (available on both platforms)
 *
 * Simple AABB and point-in-rect tests for game logic.
 * ====================================================================== */

/* Test if two axis-aligned bounding boxes overlap. */
static inline int of_collide_aabb(int x1, int y1, int w1, int h1,
                                   int x2, int y2, int w2, int h2) {
    return (x1 < x2 + w2) && (x1 + w1 > x2) &&
           (y1 < y2 + h2) && (y1 + h1 > y2);
}

/* Test if a point is inside a rectangle. */
static inline int of_collide_point(int px, int py,
                                    int rx, int ry, int rw, int rh) {
    return (px >= rx) && (px < rx + rw) &&
           (py >= ry) && (py < ry + rh);
}

/* Test if two sprites overlap (8x8). */
static inline int of_sprite_collide(int x1, int y1, int x2, int y2) {
    return of_collide_aabb(x1, y1, 8, 8, x2, y2, 8, 8);
}

#endif /* OF_H */
