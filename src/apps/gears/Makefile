# gears — TinyGL 3D demo for openfpgaOS Pocket
#
# This app has its own Makefile (instead of using ../app.mk) because
# it needs to compile the TinyGL library sources with special flags.
#
# Build from this directory:  make
# Or from src/apps/:          make -C gears

SDK_DIR = ../../sdk

# ── App sources ──────────────────────────────────────────────────
SRCS = main.c tinygl_compat.c

# ── TinyGL library ───────────────────────────────────────────────
TINYGL_DIR  = $(CURDIR)/tinygl
TINYGL_SRCS = $(wildcard $(TINYGL_DIR)/src/*.c)
TINYGL_OBJS = $(TINYGL_SRCS:.c=.o)
TINYGL_INC  = -I$(TINYGL_DIR)/include -I$(TINYGL_DIR)/src

# ── Pull in SDK build system ────────────────────────────────────
# sdk.mk defines: CC, ALL_CFLAGS, OBJS, the app.elf rule, etc.
include $(SDK_DIR)/sdk.mk

# ── Append TinyGL flags AFTER including sdk.mk ──────────────────
# -DNDEBUG            : disable assert()
# -DNO_DEBUG_OUTPUT    : silence TinyGL's vfprintf debug calls
#                        (vfprintf is not available in the SDK libc)
# -DM_PI=...          : POSIX constant not defined in bare-metal libc
# -Wno-unused-parameter: TinyGL has many unused params we can't fix
ALL_CFLAGS += $(TINYGL_INC) -DNDEBUG -DNO_DEBUG_OUTPUT \
              "-DM_PI=3.14159265358979323846f" \
              -Wno-unused-parameter

# ── Add TinyGL objects to the link ──────────────────────────────
OBJS += $(TINYGL_OBJS)

# ── Tell make that app.elf also depends on TinyGL objects ───────
app.elf: $(TINYGL_OBJS)

# ── Compile rule for TinyGL sources ─────────────────────────────
# The generic %.o:%.c rule in sdk.mk doesn't match paths under
# tinygl/src/, so we need an explicit pattern rule here.
$(TINYGL_DIR)/src/%.o: $(TINYGL_DIR)/src/%.c
	@echo "  CC  $@"
	$(CC) $(ALL_CFLAGS) -c -o $@ $<

# ── CRT start object (needed when not using app.mk) ────────────
$(CRT_DIR)/start.o: $(CRT_DIR)/start.S
	$(AS) $(ASFLAGS) -c -o $@ $<

# ── Targets ─────────────────────────────────────────────────────
all: app.elf
	$(SIZE) app.elf

clean: sdk-clean
	rm -f $(TINYGL_OBJS)

.PHONY: all clean
