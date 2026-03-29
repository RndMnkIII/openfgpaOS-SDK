# src/RayTracingTheNextWeek/Makefile

SDK_DIR = ../sdk

# ── Fuentes de la app ─────────────────────────────────────────────
SRCS_CXX = main.cpp

# Extra flags for the compat/ headers path
CXXFLAGS += -I.

include $(SDK_DIR)/sdk.mk

app.elf: $(OBJS) $(APP_LD)
	$(LD) $(ALL_LDFLAGS) -o $@ $(OBJS) $(LIBGCC)

$(CRT_DIR)/start.o: $(CRT_DIR)/start.S
	$(AS) $(ASFLAGS) -c -o $@ $<

all: app.elf
	$(SIZE) app.elf

# ── PC / host build ───────────────────────────────────────────────────────
PC_CC  ?= c++
PC_OUT  = app_pc

app_pc: main.cpp
	$(PC_CC) -DOF_PC -I. -O2 -Wall -Wextra -std=c++14 \
	    $(CXXFLAGS) main.cpp -lm -o $(PC_OUT)

.PHONY: all app_pc clean-pc clean

clean-pc:
	rm -f $(PC_OUT)

sdk-clean: clean-pc

clean: sdk-clean
