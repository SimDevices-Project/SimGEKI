#################################################################
# AUTO GENERATE AT 2025-03-10 17:00:17 BY unify_builder v3.9.2.0
#################################################################

# Usage:
#  - Build:	make COMPILER_DIR=<dir path>
#  - Clean:	make clean

# Targets Dependences Chain:
#  all -> postbuild -> bin -> elf -> prebuild, *.o ...

# Use 'make V=1' to see the full commands
ifdef V
	Q = 
else
	Q = @
endif

COMPILER_DIR ?= ./tools/wch_riscv_gcc_8_2_0
_PATH_TMP:=$(COMPILER_DIR)/bin:$(PATH)
export PATH=$(_PATH_TMP)

EXE?=.exe
CC=$(COMPILER_DIR)/bin/riscv-none-embed-gcc$(EXE)
AS=$(COMPILER_DIR)/bin/riscv-none-embed-gcc$(EXE)
CXX=$(COMPILER_DIR)/bin/riscv-none-embed-g++$(EXE)
LD=$(COMPILER_DIR)/bin/riscv-none-embed-gcc$(EXE)
AR=$(COMPILER_DIR)/bin/riscv-none-embed-ar$(EXE)

###########################
# targets
###########################

COLOR_END="\e[0m"
COLOR_ERR="\e[31;1m"
COLOR_WRN="\e[33;1m"
COLOR_SUC="\e[32;1m"
COLOR_INF="\e[34;1m"

.PHONY : all postbuild bin elf prebuild clean

all: postbuild
	@echo ==========
	@echo $(COLOR_SUC)"ALL DONE."$(COLOR_END)
	@echo ==========

clean:
	-rm -fv ./build/SimGETRO_Public/*.elf ./build/SimGETRO_Public/*.axf ./build/SimGETRO_Public/*.out
	-rm -fv ./build/SimGETRO_Public/*.hex ./build/SimGETRO_Public/*.bin ./build/SimGETRO_Public/*.s19
	-rm -rfv ./build/SimGETRO_Public/.obj

prebuild:
	@echo $(COLOR_INF)"prebuild ..."$(COLOR_END)
	@echo nothing to prebuild.

./build/SimGETRO_Public/.obj/sdk/Core:
	$(Q)mkdir -p $@
-include ./build/SimGETRO_Public/.obj/sdk/Core/core_riscv.d
./build/SimGETRO_Public/.obj/sdk/Core/core_riscv.o: ./sdk/Core/core_riscv.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Core
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Core/core_riscv.o -MMD ./sdk/Core/core_riscv.c

./build/SimGETRO_Public/.obj/sdk/Debug:
	$(Q)mkdir -p $@
-include ./build/SimGETRO_Public/.obj/sdk/Debug/debug.d
./build/SimGETRO_Public/.obj/sdk/Debug/debug.o: ./sdk/Debug/debug.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Debug
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Debug/debug.o -MMD ./sdk/Debug/debug.c

./build/SimGETRO_Public/.obj/sdk/Peripheral/src:
	$(Q)mkdir -p $@
-include ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_adc.d
./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_adc.o: ./sdk/Peripheral/src/ch32v20x_adc.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Peripheral/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_adc.o -MMD ./sdk/Peripheral/src/ch32v20x_adc.c

-include ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_bkp.d
./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_bkp.o: ./sdk/Peripheral/src/ch32v20x_bkp.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Peripheral/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_bkp.o -MMD ./sdk/Peripheral/src/ch32v20x_bkp.c

-include ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_can.d
./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_can.o: ./sdk/Peripheral/src/ch32v20x_can.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Peripheral/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_can.o -MMD ./sdk/Peripheral/src/ch32v20x_can.c

-include ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_crc.d
./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_crc.o: ./sdk/Peripheral/src/ch32v20x_crc.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Peripheral/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_crc.o -MMD ./sdk/Peripheral/src/ch32v20x_crc.c

-include ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_dbgmcu.d
./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_dbgmcu.o: ./sdk/Peripheral/src/ch32v20x_dbgmcu.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Peripheral/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_dbgmcu.o -MMD ./sdk/Peripheral/src/ch32v20x_dbgmcu.c

-include ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_dma.d
./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_dma.o: ./sdk/Peripheral/src/ch32v20x_dma.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Peripheral/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_dma.o -MMD ./sdk/Peripheral/src/ch32v20x_dma.c

-include ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_exti.d
./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_exti.o: ./sdk/Peripheral/src/ch32v20x_exti.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Peripheral/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_exti.o -MMD ./sdk/Peripheral/src/ch32v20x_exti.c

-include ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_flash.d
./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_flash.o: ./sdk/Peripheral/src/ch32v20x_flash.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Peripheral/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_flash.o -MMD ./sdk/Peripheral/src/ch32v20x_flash.c

-include ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_gpio.d
./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_gpio.o: ./sdk/Peripheral/src/ch32v20x_gpio.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Peripheral/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_gpio.o -MMD ./sdk/Peripheral/src/ch32v20x_gpio.c

-include ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_i2c.d
./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_i2c.o: ./sdk/Peripheral/src/ch32v20x_i2c.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Peripheral/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_i2c.o -MMD ./sdk/Peripheral/src/ch32v20x_i2c.c

-include ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_iwdg.d
./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_iwdg.o: ./sdk/Peripheral/src/ch32v20x_iwdg.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Peripheral/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_iwdg.o -MMD ./sdk/Peripheral/src/ch32v20x_iwdg.c

-include ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_misc.d
./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_misc.o: ./sdk/Peripheral/src/ch32v20x_misc.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Peripheral/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_misc.o -MMD ./sdk/Peripheral/src/ch32v20x_misc.c

-include ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_opa.d
./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_opa.o: ./sdk/Peripheral/src/ch32v20x_opa.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Peripheral/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_opa.o -MMD ./sdk/Peripheral/src/ch32v20x_opa.c

-include ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_pwr.d
./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_pwr.o: ./sdk/Peripheral/src/ch32v20x_pwr.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Peripheral/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_pwr.o -MMD ./sdk/Peripheral/src/ch32v20x_pwr.c

-include ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_rcc.d
./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_rcc.o: ./sdk/Peripheral/src/ch32v20x_rcc.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Peripheral/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_rcc.o -MMD ./sdk/Peripheral/src/ch32v20x_rcc.c

-include ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_rtc.d
./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_rtc.o: ./sdk/Peripheral/src/ch32v20x_rtc.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Peripheral/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_rtc.o -MMD ./sdk/Peripheral/src/ch32v20x_rtc.c

-include ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_spi.d
./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_spi.o: ./sdk/Peripheral/src/ch32v20x_spi.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Peripheral/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_spi.o -MMD ./sdk/Peripheral/src/ch32v20x_spi.c

-include ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_tim.d
./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_tim.o: ./sdk/Peripheral/src/ch32v20x_tim.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Peripheral/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_tim.o -MMD ./sdk/Peripheral/src/ch32v20x_tim.c

-include ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_usart.d
./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_usart.o: ./sdk/Peripheral/src/ch32v20x_usart.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Peripheral/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_usart.o -MMD ./sdk/Peripheral/src/ch32v20x_usart.c

-include ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_wwdg.d
./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_wwdg.o: ./sdk/Peripheral/src/ch32v20x_wwdg.c Makefile | ./build/SimGETRO_Public/.obj/sdk/Peripheral/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_wwdg.o -MMD ./sdk/Peripheral/src/ch32v20x_wwdg.c

./build/SimGETRO_Public/.obj/sdk/Startup:
	$(Q)mkdir -p $@
-include ./build/SimGETRO_Public/.obj/sdk/Startup/startup_ch32v20x_D6.d
./build/SimGETRO_Public/.obj/sdk/Startup/startup_ch32v20x_D6.o: ./sdk/Startup/startup_ch32v20x_D6.S Makefile | ./build/SimGETRO_Public/.obj/sdk/Startup
	@echo assembling $< ...
	$(Q)$(CC) -c -x assembler-with-cpp -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -mcmodel=medany -mabi=ilp32 -march=rv32imac -g -Wl,-Bstatic -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -o ./build/SimGETRO_Public/.obj/sdk/Startup/startup_ch32v20x_D6.o -MMD ./sdk/Startup/startup_ch32v20x_D6.S

./build/SimGETRO_Public/.obj/src:
	$(Q)mkdir -p $@
-include ./build/SimGETRO_Public/.obj/src/ch32v20x_it.d
./build/SimGETRO_Public/.obj/src/ch32v20x_it.o: ./src/ch32v20x_it.c Makefile | ./build/SimGETRO_Public/.obj/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/ch32v20x_it.o -MMD ./src/ch32v20x_it.c

./build/SimGETRO_Public/.obj/src/lib:
	$(Q)mkdir -p $@
-include ./build/SimGETRO_Public/.obj/src/lib/cdc.d
./build/SimGETRO_Public/.obj/src/lib/cdc.o: ./src/lib/cdc.c Makefile | ./build/SimGETRO_Public/.obj/src/lib
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/lib/cdc.o -MMD ./src/lib/cdc.c

-include ./build/SimGETRO_Public/.obj/src/lib/ch422.d
./build/SimGETRO_Public/.obj/src/lib/ch422.o: ./src/lib/ch422.c Makefile | ./build/SimGETRO_Public/.obj/src/lib
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/lib/ch422.o -MMD ./src/lib/ch422.c

-include ./build/SimGETRO_Public/.obj/src/lib/data.d
./build/SimGETRO_Public/.obj/src/lib/data.o: ./src/lib/data.c Makefile | ./build/SimGETRO_Public/.obj/src/lib
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/lib/data.o -MMD ./src/lib/data.c

-include ./build/SimGETRO_Public/.obj/src/lib/hidconfig.d
./build/SimGETRO_Public/.obj/src/lib/hidconfig.o: ./src/lib/hidconfig.c Makefile | ./build/SimGETRO_Public/.obj/src/lib
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/lib/hidconfig.o -MMD ./src/lib/hidconfig.c

-include ./build/SimGETRO_Public/.obj/src/lib/hidio.d
./build/SimGETRO_Public/.obj/src/lib/hidio.o: ./src/lib/hidio.c Makefile | ./build/SimGETRO_Public/.obj/src/lib
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/lib/hidio.o -MMD ./src/lib/hidio.c

-include ./build/SimGETRO_Public/.obj/src/lib/keyscan.d
./build/SimGETRO_Public/.obj/src/lib/keyscan.o: ./src/lib/keyscan.c Makefile | ./build/SimGETRO_Public/.obj/src/lib
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/lib/keyscan.o -MMD ./src/lib/keyscan.c

-include ./build/SimGETRO_Public/.obj/src/lib/led.d
./build/SimGETRO_Public/.obj/src/lib/led.o: ./src/lib/led.c Makefile | ./build/SimGETRO_Public/.obj/src/lib
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/lib/led.o -MMD ./src/lib/led.c

-include ./build/SimGETRO_Public/.obj/src/lib/pn532.d
./build/SimGETRO_Public/.obj/src/lib/pn532.o: ./src/lib/pn532.c Makefile | ./build/SimGETRO_Public/.obj/src/lib
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/lib/pn532.o -MMD ./src/lib/pn532.c

-include ./build/SimGETRO_Public/.obj/src/lib/pn532_uart.d
./build/SimGETRO_Public/.obj/src/lib/pn532_uart.o: ./src/lib/pn532_uart.c Makefile | ./build/SimGETRO_Public/.obj/src/lib
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/lib/pn532_uart.o -MMD ./src/lib/pn532_uart.c

-include ./build/SimGETRO_Public/.obj/src/lib/roller.d
./build/SimGETRO_Public/.obj/src/lib/roller.o: ./src/lib/roller.c Makefile | ./build/SimGETRO_Public/.obj/src/lib
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/lib/roller.o -MMD ./src/lib/roller.c

-include ./build/SimGETRO_Public/.obj/src/lib/timeout.d
./build/SimGETRO_Public/.obj/src/lib/timeout.o: ./src/lib/timeout.c Makefile | ./build/SimGETRO_Public/.obj/src/lib
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/lib/timeout.o -MMD ./src/lib/timeout.c

-include ./build/SimGETRO_Public/.obj/src/main.d
./build/SimGETRO_Public/.obj/src/main.o: ./src/main.c Makefile | ./build/SimGETRO_Public/.obj/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/main.o -MMD ./src/main.c

-include ./build/SimGETRO_Public/.obj/src/system_ch32v20x.d
./build/SimGETRO_Public/.obj/src/system_ch32v20x.o: ./src/system_ch32v20x.c Makefile | ./build/SimGETRO_Public/.obj/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/system_ch32v20x.o -MMD ./src/system_ch32v20x.c

./build/SimGETRO_Public/.obj/src/usblib/config:
	$(Q)mkdir -p $@
-include ./build/SimGETRO_Public/.obj/src/usblib/config/hw_config.d
./build/SimGETRO_Public/.obj/src/usblib/config/hw_config.o: ./src/usblib/config/hw_config.c Makefile | ./build/SimGETRO_Public/.obj/src/usblib/config
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/usblib/config/hw_config.o -MMD ./src/usblib/config/hw_config.c

-include ./build/SimGETRO_Public/.obj/src/usblib/config/usb_desc.d
./build/SimGETRO_Public/.obj/src/usblib/config/usb_desc.o: ./src/usblib/config/usb_desc.c Makefile | ./build/SimGETRO_Public/.obj/src/usblib/config
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/usblib/config/usb_desc.o -MMD ./src/usblib/config/usb_desc.c

-include ./build/SimGETRO_Public/.obj/src/usblib/config/usb_endp.d
./build/SimGETRO_Public/.obj/src/usblib/config/usb_endp.o: ./src/usblib/config/usb_endp.c Makefile | ./build/SimGETRO_Public/.obj/src/usblib/config
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/usblib/config/usb_endp.o -MMD ./src/usblib/config/usb_endp.c

-include ./build/SimGETRO_Public/.obj/src/usblib/config/usb_istr.d
./build/SimGETRO_Public/.obj/src/usblib/config/usb_istr.o: ./src/usblib/config/usb_istr.c Makefile | ./build/SimGETRO_Public/.obj/src/usblib/config
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/usblib/config/usb_istr.o -MMD ./src/usblib/config/usb_istr.c

-include ./build/SimGETRO_Public/.obj/src/usblib/config/usb_prop.d
./build/SimGETRO_Public/.obj/src/usblib/config/usb_prop.o: ./src/usblib/config/usb_prop.c Makefile | ./build/SimGETRO_Public/.obj/src/usblib/config
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/usblib/config/usb_prop.o -MMD ./src/usblib/config/usb_prop.c

-include ./build/SimGETRO_Public/.obj/src/usblib/config/usb_pwr.d
./build/SimGETRO_Public/.obj/src/usblib/config/usb_pwr.o: ./src/usblib/config/usb_pwr.c Makefile | ./build/SimGETRO_Public/.obj/src/usblib/config
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/usblib/config/usb_pwr.o -MMD ./src/usblib/config/usb_pwr.c

./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src:
	$(Q)mkdir -p $@
-include ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_core.d
./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_core.o: ./src/usblib/usb_driver/src/usb_core.c Makefile | ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_core.o -MMD ./src/usblib/usb_driver/src/usb_core.c

-include ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_init.d
./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_init.o: ./src/usblib/usb_driver/src/usb_init.c Makefile | ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_init.o -MMD ./src/usblib/usb_driver/src/usb_init.c

-include ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_int.d
./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_int.o: ./src/usblib/usb_driver/src/usb_int.c Makefile | ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_int.o -MMD ./src/usblib/usb_driver/src/usb_int.c

-include ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_mem.d
./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_mem.o: ./src/usblib/usb_driver/src/usb_mem.c Makefile | ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_mem.o -MMD ./src/usblib/usb_driver/src/usb_mem.c

-include ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_regs.d
./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_regs.o: ./src/usblib/usb_driver/src/usb_regs.c Makefile | ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_regs.o -MMD ./src/usblib/usb_driver/src/usb_regs.c

-include ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_sil.d
./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_sil.o: ./src/usblib/usb_driver/src/usb_sil.c Makefile | ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src
	@echo compiling $< ...
	$(Q)$(CC) -c -xc -std=c99 -Isdk/Peripheral/inc -Isrc -Isdk/Core -Isdk/Debug -Isrc/lib -Isrc/usblib/config -Isrc/usblib/usb_driver/inc -Isrc/usblib/usb_driver/src -DTARGET_SIMGEKI -mcmodel=medany -mabi=ilp32 -march=rv32imac -O2 -Wall -g -Wl, -Bstatic -fshort-enums -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -ffunction-sections -fdata-sections -o ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_sil.o -MMD ./src/usblib/usb_driver/src/usb_sil.c

objs = ./build/SimGETRO_Public/.obj/sdk/Core/core_riscv.o ./build/SimGETRO_Public/.obj/sdk/Debug/debug.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_adc.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_bkp.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_can.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_crc.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_dbgmcu.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_dma.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_exti.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_flash.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_gpio.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_i2c.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_iwdg.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_misc.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_opa.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_pwr.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_rcc.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_rtc.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_spi.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_tim.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_usart.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_wwdg.o ./build/SimGETRO_Public/.obj/sdk/Startup/startup_ch32v20x_D6.o ./build/SimGETRO_Public/.obj/src/ch32v20x_it.o ./build/SimGETRO_Public/.obj/src/lib/cdc.o ./build/SimGETRO_Public/.obj/src/lib/ch422.o ./build/SimGETRO_Public/.obj/src/lib/data.o ./build/SimGETRO_Public/.obj/src/lib/hidconfig.o ./build/SimGETRO_Public/.obj/src/lib/hidio.o ./build/SimGETRO_Public/.obj/src/lib/keyscan.o ./build/SimGETRO_Public/.obj/src/lib/led.o ./build/SimGETRO_Public/.obj/src/lib/pn532.o ./build/SimGETRO_Public/.obj/src/lib/pn532_uart.o ./build/SimGETRO_Public/.obj/src/lib/roller.o ./build/SimGETRO_Public/.obj/src/lib/timeout.o ./build/SimGETRO_Public/.obj/src/main.o ./build/SimGETRO_Public/.obj/src/system_ch32v20x.o ./build/SimGETRO_Public/.obj/src/usblib/config/hw_config.o ./build/SimGETRO_Public/.obj/src/usblib/config/usb_desc.o ./build/SimGETRO_Public/.obj/src/usblib/config/usb_endp.o ./build/SimGETRO_Public/.obj/src/usblib/config/usb_istr.o ./build/SimGETRO_Public/.obj/src/usblib/config/usb_prop.o ./build/SimGETRO_Public/.obj/src/usblib/config/usb_pwr.o ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_core.o ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_init.o ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_int.o ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_mem.o ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_regs.o ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_sil.o
elf: prebuild $(objs) Makefile
	@echo $(COLOR_INF)"linking ./build/SimGETRO_Public/SimGEKI.elf ..."$(COLOR_END)
	$(CC) -mcmodel=medany -mabi=ilp32 -march=rv32imac -Wl,--print-memory-usage -Xlinker --gc-sections -nostartfiles -T "sdk/Ld/Link.ld" -msmall-data-limit=8 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized --specs=nano.specs --specs=nosys.specs -Wl,--gc-sections -Wl,-Map=./build/SimGETRO_Public/SimGEKI.map -o ./build/SimGETRO_Public/SimGEKI.elf ./build/SimGETRO_Public/.obj/sdk/Core/core_riscv.o ./build/SimGETRO_Public/.obj/sdk/Debug/debug.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_adc.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_bkp.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_can.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_crc.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_dbgmcu.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_dma.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_exti.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_flash.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_gpio.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_i2c.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_iwdg.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_misc.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_opa.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_pwr.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_rcc.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_rtc.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_spi.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_tim.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_usart.o ./build/SimGETRO_Public/.obj/sdk/Peripheral/src/ch32v20x_wwdg.o ./build/SimGETRO_Public/.obj/sdk/Startup/startup_ch32v20x_D6.o ./build/SimGETRO_Public/.obj/src/ch32v20x_it.o ./build/SimGETRO_Public/.obj/src/lib/cdc.o ./build/SimGETRO_Public/.obj/src/lib/ch422.o ./build/SimGETRO_Public/.obj/src/lib/data.o ./build/SimGETRO_Public/.obj/src/lib/hidconfig.o ./build/SimGETRO_Public/.obj/src/lib/hidio.o ./build/SimGETRO_Public/.obj/src/lib/keyscan.o ./build/SimGETRO_Public/.obj/src/lib/led.o ./build/SimGETRO_Public/.obj/src/lib/pn532.o ./build/SimGETRO_Public/.obj/src/lib/pn532_uart.o ./build/SimGETRO_Public/.obj/src/lib/roller.o ./build/SimGETRO_Public/.obj/src/lib/timeout.o ./build/SimGETRO_Public/.obj/src/main.o ./build/SimGETRO_Public/.obj/src/system_ch32v20x.o ./build/SimGETRO_Public/.obj/src/usblib/config/hw_config.o ./build/SimGETRO_Public/.obj/src/usblib/config/usb_desc.o ./build/SimGETRO_Public/.obj/src/usblib/config/usb_endp.o ./build/SimGETRO_Public/.obj/src/usblib/config/usb_istr.o ./build/SimGETRO_Public/.obj/src/usblib/config/usb_prop.o ./build/SimGETRO_Public/.obj/src/usblib/config/usb_pwr.o ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_core.o ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_init.o ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_int.o ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_mem.o ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_regs.o ./build/SimGETRO_Public/.obj/src/usblib/usb_driver/src/usb_sil.o

bin: elf Makefile
	@echo $(COLOR_INF)"make bin files ..."$(COLOR_END)
	$(COMPILER_DIR)/bin/riscv-none-embed-objcopy$(EXE) -O ihex ./build/SimGETRO_Public/SimGEKI.elf ./build/SimGETRO_Public/SimGEKI.hex
	$(COMPILER_DIR)/bin/riscv-none-embed-objcopy$(EXE) -O binary ./build/SimGETRO_Public/SimGEKI.elf ./build/SimGETRO_Public/SimGEKI.bin

postbuild: bin
	@echo $(COLOR_INF)"postbuild ..."$(COLOR_END)
	riscv-none-embed-objdump --source --all-headers --demangle --disassemble "build/SimGETRO_Public\SimGEKI.elf" > "build/SimGETRO_Public\SimGEKI.lst"

