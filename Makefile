# openfpgaOS SDK -- Application Makefile
#
# Usage:
#   make              Build app.elf for Analogue Pocket
#   make pc           Build for PC (requires SDL2)
#   make install      Copy to SD card (set SDCARD= path)
#   make clean        Remove build artifacts

# ── Configuration ──────────────────────────────────────────────────────
APP_NAME ?= My App

# ── Toolchain (auto-detect) ───────────────────────────────────────────
CROSS ?= $(shell which riscv64-unknown-elf-gcc >/dev/null 2>&1 && echo riscv64-unknown-elf- || echo riscv64-elf-)
CC      = $(CROSS)gcc
LD      = $(CROSS)gcc
AS      = $(CROSS)gcc
OBJDUMP = $(CROSS)objdump
SIZE    = $(CROSS)size

# ── Architecture ──────────────────────────────────────────────────────
ARCH = rv32imafc
ABI  = ilp32f

# ── Paths ─────────────────────────────────────────────────────────────
SDK_DIR = sdk
CRT_DIR = $(SDK_DIR)/crt

# ── Sources (add your .c files here) ─────────────────────────────────
SRCS = main.c

# ── Compiler flags ────────────────────────────────────────────────────
CFLAGS  = -march=$(ARCH) -mabi=$(ABI) -O2 -Wall -Wextra
CFLAGS += -ffreestanding -nostdlib -nostartfiles
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fno-builtin
CFLAGS += -nostdinc -I$(SDK_DIR)/libc_include -I$(SDK_DIR) -I.
CFLAGS += -isystem $(shell $(CC) -print-file-name=include)

LDFLAGS  = -march=$(ARCH) -mabi=$(ABI)
LDFLAGS += -nostdlib -nostartfiles -static
LDFLAGS += -T app.ld -Wl,--gc-sections

ASFLAGS = -march=$(ARCH)_zicsr -mabi=$(ABI)

LIBGCC = $(shell $(CC) -march=$(ARCH) -mabi=$(ABI) -print-libgcc-file-name)

# ── Objects ───────────────────────────────────────────────────────────
CRT_START = $(CRT_DIR)/start.o
OBJS      = $(CRT_START) $(SRCS:.c=.o)

# ── Targets ───────────────────────────────────────────────────────────
all: app.elf
	$(SIZE) $<

app.elf: $(OBJS) app.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBGCC)

$(CRT_DIR)/%.o: $(CRT_DIR)/%.S
	$(AS) $(ASFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# ── PC build (SDL2) ──────────────────────────────────────────────────
PC_CC ?= cc
SDL_CFLAGS := $(shell sdl2-config --cflags 2>/dev/null || pkg-config --cflags sdl2 2>/dev/null)
SDL_LIBS   := $(shell sdl2-config --libs 2>/dev/null || pkg-config --libs sdl2 2>/dev/null)

pc: app_pc
	@echo "Built app_pc -- run with ./app_pc"

app_pc: $(SRCS) $(SDK_DIR)/of_sdl2.c $(SDK_DIR)/of.h
	$(PC_CC) -DOF_PC -I$(SDK_DIR) -I. -O2 -Wall -Wextra \
		$(SRCS) $(SDK_DIR)/of_sdl2.c \
		$(SDL_CFLAGS) $(SDL_LIBS) -lm -o $@

# ── Install to SD card ───────────────────────────────────────────────
SDCARD ?= /mnt/sdcard

install: app.elf
	@echo "Installing to $(SDCARD)..."
	@mkdir -p "$(SDCARD)/Cores/ThinkElastic.openfpgaOS"
	@cp dist/core.json       "$(SDCARD)/Cores/ThinkElastic.openfpgaOS/"
	@cp dist/audio.json      "$(SDCARD)/Cores/ThinkElastic.openfpgaOS/"
	@cp dist/video.json      "$(SDCARD)/Cores/ThinkElastic.openfpgaOS/"
	@cp dist/input.json      "$(SDCARD)/Cores/ThinkElastic.openfpgaOS/"
	@cp dist/data.json       "$(SDCARD)/Cores/ThinkElastic.openfpgaOS/"
	@cp dist/interact.json   "$(SDCARD)/Cores/ThinkElastic.openfpgaOS/"
	@cp dist/variants.json   "$(SDCARD)/Cores/ThinkElastic.openfpgaOS/"
	@cp dist/icon.bin        "$(SDCARD)/Cores/ThinkElastic.openfpgaOS/"
	@mkdir -p "$(SDCARD)/Cores/ThinkElastic.openfpgaOS/Platforms/_images"
	@cp dist/platforms/openfpgaos.json       "$(SDCARD)/Cores/ThinkElastic.openfpgaOS/Platforms/"
	@cp dist/platforms/_images/openfpgaos.bin "$(SDCARD)/Cores/ThinkElastic.openfpgaOS/Platforms/_images/"
	@mkdir -p "$(SDCARD)/Assets/openfpgaos/common"
	@cp os.bin    "$(SDCARD)/Assets/openfpgaos/common/"
	@cp app.elf   "$(SDCARD)/Assets/openfpgaos/common/"
	@cp "dist/instances/$(APP_NAME).json" "$(SDCARD)/Assets/openfpgaos/common/"
	@echo "Done. Copy your bitstream.rbf_r to $(SDCARD)/Cores/ThinkElastic.openfpgaOS/"

# ── Clean ─────────────────────────────────────────────────────────────
clean:
	rm -f $(OBJS) app.elf app_pc

.PHONY: all pc install clean
