# openfpgaOS SDK

Build games and apps for the Analogue Pocket using the openfpgaOS operating system.

**Platform:** RISC-V (VexRiscv) @ 100 MHz, 320x240 indexed color, 48 kHz stereo audio, YM2151 FM synthesis.

## Prerequisites

A RISC-V cross-compiler targeting `rv32imafc`:

- **Arch:** `pacman -S riscv64-elf-gcc`
- **macOS:** `brew install riscv64-elf-gcc`
- **Ubuntu:** `apt install gcc-riscv64-unknown-elf`

Optional: SDL2 for PC testing (`sdl2-config` must be in PATH).

## Quick Start

```bash
./setup.sh          # Check toolchain
make                # Build app.elf
make pc && ./app_pc # Test on PC (needs SDL2)
```

## Project Structure

```
openfpgaOS-SDK/
├── Makefile        # Build system -- add your .c files to SRCS
├── app.ld          # Linker script (don't modify)
├── main.c          # Your app (edit this!)
├── os.bin          # Pre-built OS kernel (download from releases)
├── sdk/            # SDK headers and runtime (don't modify)
│   ├── of.h        #   Main API -- #include "of.h"
│   ├── of_libc.h   #   libc jump table (used by wrapper headers)
│   ├── of_sdl2.c   #   SDL2 backend for PC testing
│   ├── crt/        #   C runtime startup
│   └── libc_include/   Standard C headers (string.h, math.h, etc.)
└── dist/           # Analogue Pocket core packaging
```

## Writing Your App

```c
#include "of.h"
#include <string.h>  // memcpy, memset, etc. (provided by OS)
#include <math.h>    // sinf, cosf, etc. (provided by OS)

int main(void) {
    of_video_init();

    while (1) {
        of_input_poll();
        if (of_btn_pressed(BTN_A)) { /* ... */ }

        of_video_clear(0);
        // draw your game here
        of_video_flip();
    }
}
```

Add more `.c` files to `SRCS` in the Makefile.

## API Overview

| Subsystem | Functions |
|-----------|-----------|
| **Video** | `of_video_init`, `of_video_surface`, `of_video_flip`, `of_video_sync`, `of_video_clear`, `of_video_palette`, `of_video_palette_bulk`, `of_video_pixel` |
| **Input** | `of_input_poll`, `of_btn`, `of_btn_pressed`, `of_btn_released`, `of_btn_p2`, `of_input_state` |
| **Audio PCM** | `of_audio_init`, `of_audio_write`, `of_audio_free` |
| **Audio FM** | `of_opm_write`, `of_opm_reset` |
| **Timer** | `of_time_us`, `of_time_ms`, `of_delay_us`, `of_delay_ms` |
| **Save** | `of_save_read`, `of_save_write`, `of_save_flush`, `of_save_erase` |
| **File I/O** | `of_file_read`, `of_file_size` |
| **Terminal** | `of_print`, `of_print_char`, `of_print_clear`, `of_print_at` |
| **Tiles** | `of_tile_enable`, `of_tile_scroll`, `of_tile_set`, `of_tile_load_map`, `of_tile_load_chr` |
| **Sprites** | `of_sprite_enable`, `of_sprite_set`, `of_sprite_move`, `of_sprite_load_chr`, `of_sprite_hide` |
| **Link** | `of_link_send`, `of_link_recv`, `of_link_status` |
| **System** | `of_exit` |

See `sdk/of.h` for full documentation.

## Standard C Library

The OS provides 52 libc functions via a jump table -- no static linking needed. Use standard headers:

- `<string.h>` -- memcpy, memset, strlen, strcmp, ...
- `<stdlib.h>` -- malloc, free, rand, qsort, ...
- `<stdio.h>`  -- snprintf, printf
- `<math.h>`   -- sinf, cosf, sqrtf, atan2f, ...

## Loading Data Files

Place assets in data slot 3. Edit `dist/instances/My App.json`:

```json
{
    "instance": {
        "magic": "APF_VER_1",
        "variant_select": { "id": 666, "select": false },
        "data_slots": [
            { "id": 1, "filename": "os.bin" },
            { "id": 2, "filename": "app.elf" },
            { "id": 3, "filename": "mydata.dat" }
        ]
    }
}
```

Then in code: `of_file_read(3, offset, buffer, length);`

## Adding Save Support

Add a save slot to your instance JSON:

```json
{
    "id": 10,
    "name": "Saves",
    "required": false,
    "parameters": "0x84",
    "extensions": ["sav"],
    "nonvolatile": true,
    "address": "0x03C00000",
    "size_maximum": "0x00060000",
    "filename": "MyGame.sav"
}
```

## Deploying to Pocket

1. Get `bitstream.rbf_r` and `os.bin` from the openfpgaOS releases
2. Place `os.bin` in this directory
3. Run: `make install SDCARD=/path/to/sdcard`
4. Copy `bitstream.rbf_r` to `Cores/ThinkElastic.openfpgaOS/` on the SD card

## Button Mapping

Edit `dist/input.json` to customize button names shown on the Pocket.

## PC Testing

The SDL2 backend maps keys: Arrow keys = D-pad, Z = A, X = B, A = X, S = Y, Q/W = L1/R1, Enter = Start, RShift = Select.

## Updating the SDK

```bash
./update.sh          # Latest release
./update.sh v0.2     # Specific version
```

This replaces `sdk/`, `app.ld`, `os.bin`, and core dist files. Your source files (`main.c`, `Makefile`, `dist/input.json`, instance JSONs) are never overwritten.

## Platform Specs

| Feature | Value |
|---------|-------|
| CPU | VexRiscv RV32IMAFC @ 100 MHz |
| Display | 320x240, 8-bit indexed, 256-color palette |
| Audio | 48 kHz stereo PCM + YM2151 FM (8 channels) |
| Input | 2 controllers (d-pad, ABXY, L/R, sticks, triggers) |
| SDRAM | 64 MB |
| Saves | 6 x 64 KB slots (nonvolatile) |
