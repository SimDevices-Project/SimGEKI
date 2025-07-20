#################################################################
# Makefile for SimGEKI project with multi-target support
#
# Original Makefile AUTO GENERATE AT 2025-03-10 17:00:17 BY unify_builder v3.9.2.0
# Modified for multi-target builds.
#################################################################

# Usage:
#  - Build a specific target: make TARGET=<target_name>
#    (e.g., make TARGET=Debug)
#  - Build the default target: make
#  - Build all targets:       make all_targets
#  - Clean a specific target: make clean TARGET=<target_name>
#  - Clean all targets:       make clean_all
#
# Available targets: SimGETRO_Public, SimGETRO_EarlyHardware, Debug

# Use 'make V=1' to see the full commands
ifdef V
	Q =
else
	Q = @
endif

# Target configuration
TARGET ?= SimGETRO_Public
PROJECT_NAME = SimGEKI

# Toolchain configuration
COMPILER_DIR ?= ./tools/wch_riscv_gcc_8_2_0
_PATH_TMP:=$(COMPILER_DIR)/bin:$(PATH)
export PATH=$(_PATH_TMP)

EXE?=.exe
CC=$(COMPILER_DIR)/bin/riscv-none-embed-gcc$(EXE)
AS=$(COMPILER_DIR)/bin/riscv-none-embed-gcc$(EXE)
CXX=$(COMPILER_DIR)/bin/riscv-none-embed-g++$(EXE)
LD=$(COMPILER_DIR)/bin/riscv-none-embed-gcc$(EXE)
AR=$(COMPILER_DIR)/bin/riscv-none-embed-ar$(EXE)
OBJCOPY=$(COMPILER_DIR)/bin/riscv-none-embed-objcopy$(EXE)
OBJDUMP=riscv-none-embed-objdump

# Build directories
BUILD_DIR := ./build/$(TARGET)
OBJ_DIR   := $(BUILD_DIR)/.obj

# Output files
ELF_FILE := $(BUILD_DIR)/$(PROJECT_NAME).elf
HEX_FILE := $(BUILD_DIR)/$(PROJECT_NAME).hex
BIN_FILE := $(BUILD_DIR)/$(PROJECT_NAME).bin
LST_FILE := $(BUILD_DIR)/$(PROJECT_NAME).lst
MAP_FILE := $(BUILD_DIR)/$(PROJECT_NAME).map

###########################
# Source files
###########################
C_SOURCES = \
	sdk/Core/core_riscv.c \
	sdk/Debug/debug.c \
	sdk/Peripheral/src/ch32v20x_adc.c \
	sdk/Peripheral/src/ch32v20x_bkp.c \
	sdk/Peripheral/src/ch32v20x_can.c \
	sdk/Peripheral/src/ch32v20x_crc.c \
	sdk/Peripheral/src/ch32v20x_dbgmcu.c \
	sdk/Peripheral/src/ch32v20x_dma.c \
	sdk/Peripheral/src/ch32v20x_exti.c \
	sdk/Peripheral/src/ch32v20x_flash.c \
	sdk/Peripheral/src/ch32v20x_gpio.c \
	sdk/Peripheral/src/ch32v20x_i2c.c \
	sdk/Peripheral/src/ch32v20x_iwdg.c \
	sdk/Peripheral/src/ch32v20x_misc.c \
	sdk/Peripheral/src/ch32v20x_opa.c \
	sdk/Peripheral/src/ch32v20x_pwr.c \
	sdk/Peripheral/src/ch32v20x_rcc.c \
	sdk/Peripheral/src/ch32v20x_rtc.c \
	sdk/Peripheral/src/ch32v20x_spi.c \
	sdk/Peripheral/src/ch32v20x_tim.c \
	sdk/Peripheral/src/ch32v20x_usart.c \
	sdk/Peripheral/src/ch32v20x_wwdg.c \
	src/ch32v20x_it.c \
	src/lib/cdc.c \
	src/lib/ch422.c \
	src/lib/data.c \
	src/lib/hidconfig.c \
	src/lib/hidio.c \
	src/lib/keyscan.c \
	src/lib/led.c \
	src/lib/pn532.c \
	src/lib/pn532_uart.c \
	src/lib/roller.c \
	src/lib/timeout.c \
	src/main.c \
	src/system_ch32v20x.c \
	src/usblib/config/hw_config.c \
	src/usblib/config/usb_desc.c \
	src/usblib/config/usb_endp.c \
	src/usblib/config/usb_istr.c \
	src/usblib/config/usb_prop.c \
	src/usblib/config/usb_pwr.c \
	src/usblib/usb_driver/src/usb_core.c \
	src/usblib/usb_driver/src/usb_init.c \
	src/usblib/usb_driver/src/usb_int.c \
	src/usblib/usb_driver/src/usb_mem.c \
	src/usblib/usb_driver/src/usb_regs.c \
	src/usblib/usb_driver/src/usb_sil.c

S_SOURCES = \
	sdk/Startup/startup_ch32v20x_D6.S

# Object files
OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
OBJS += $(patsubst %.S,$(OBJ_DIR)/%.o,$(S_SOURCES))

# Dependency files
DEPS = $(OBJS:.o=.d)

###########################
# Compiler and Linker Flags
###########################

# Common flags
COMMON_FLAGS = -mcmodel=medany -mabi=ilp32 -march=rv32imac \
               -msmall-data-limit=8 -msave-restore -fmessage-length=0 \
               -fsigned-char -ffunction-sections -fdata-sections -fno-common \
               -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs

# Include paths
INCLUDES = -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib \
           -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src

# C flags
CFLAGS = -std=c99 -Wall -g $(COMMON_FLAGS) $(INCLUDES)

# Assembler flags
ASFLAGS = -g $(COMMON_FLAGS) $(INCLUDES)

# Linker flags
LDFLAGS = -Wl,--print-memory-usage -Xlinker --gc-sections -nostartfiles \
          -T "sdk/Ld/Link.ld" $(COMMON_FLAGS) -Wl,--gc-sections -Wl,-Map=$(MAP_FILE)

# Target-specific flags
ifeq ($(TARGET), SimGETRO_Public)
	TARGET_DEF = -DTARGET_SIMGEKI
	CFLAGS += -O2
else ifeq ($(TARGET), SimGETRO_EarlyHardware)
	TARGET_DEF = -DTARGET_SIMGETRO -DEARLY_HARDWARE
	CFLAGS += -O2
else ifeq ($(TARGET), Debug)
	TARGET_DEF = -DDEBUG
	CFLAGS += -Og -g3
else
    $(error Invalid TARGET specified: $(TARGET). Available targets: SimGETRO_Public, SimGETRO_EarlyHardware, Debug)
endif

CFLAGS += $(TARGET_DEF)
ASFLAGS += $(TARGET_DEF)

###########################
# Build Rules
###########################

COLOR_END="\e[0m"
COLOR_ERR="\e[31;1m"
COLOR_WRN="\e[33;1m"
COLOR_SUC="\e[32;1m"
COLOR_INF="\e[34;1m"

.PHONY: all all_targets clean clean_all postbuild bin elf prebuild

# Default target
all: postbuild

# Build all targets
all_targets:
	$(MAKE) all TARGET=SimGETRO_Public
	$(MAKE) all TARGET=SimGETRO_EarlyHardware
	$(MAKE) all TARGET=Debug

# Linking
elf: $(ELF_FILE)

$(ELF_FILE): $(OBJS) Makefile
	@echo $(COLOR_INF)"linking $(ELF_FILE) ..."$(COLOR_END)
	$(Q)$(CC) $(LDFLAGS) -o $(ELF_FILE) $(OBJS)

# Compilation
$(OBJ_DIR)/%.o: %.c Makefile
	@echo compiling $< ...
	@mkdir -p $(dir $@)
	$(Q)$(CC) -c $(CFLAGS) -o $@ -MMD $<

$(OBJ_DIR)/%.o: %.S Makefile
	@echo assembling $< ...
	@mkdir -p $(dir $@)
	$(Q)$(AS) -c $(ASFLAGS) -o $@ -MMD $<

# Create binary files
bin: $(ELF_FILE)
	@echo $(COLOR_INF)"make bin files ..."$(COLOR_END)
	$(Q)$(OBJCOPY) -O ihex $(ELF_FILE) $(HEX_FILE)
	$(Q)$(OBJCOPY) -O binary $(ELF_FILE) $(BIN_FILE)

# Post-build steps
postbuild: bin
	@echo $(COLOR_INF)"postbuild ..."$(COLOR_END)
	$(Q)$(OBJDUMP) --source --all-headers --demangle --disassemble "$(ELF_FILE)" > "$(LST_FILE)"
	@echo ==========
	@echo $(COLOR_SUC)"Target [$(TARGET)] DONE."$(COLOR_END)
	@echo ==========

# Pre-build steps (if any)
prebuild:
	@echo $(COLOR_INF)"prebuild for $(TARGET)..."$(COLOR_END)
	@echo nothing to prebuild.

# Clean build artifacts
clean:
	@echo Cleaning target: $(TARGET)
	-rm -rfv ./build/$(TARGET)

clean_all:
	@echo Cleaning all targets...
	-rm -rfv ./build

# Include dependency files
-include $(DEPS)

