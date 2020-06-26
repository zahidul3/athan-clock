SIMPLELINK_CC26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/imports.mak


SYSCONFIG_PRODUCT := $(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/.metadata/sdk.json
BOARD := /ti/boards/.meta/CC26X2R1_LAUNCHXL

CC = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"
LNK = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"

OBJECTS = Board.obj main_nortos.obj sdraw.obj CC26X2R1_LAUNCHXL_fxns.obj ccfg.obj

NAME = sdraw

CFLAGS = -I.. \
    -I. \
    -DDeviceFamily_CC26X2 \
    "-I$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/kernel/nortos" \
    "-I$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/kernel/nortos/posix" \
    -mcpu=cortex-m4 \
    -march=armv7e-m \
    -mthumb \
    -std=c99 \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16 \
    -ffunction-sections \
    -fdata-sections \
    -g \
    -gstrict-dwarf \
    -Wall \
    "-I$(GCC_ARMCOMPILER)/arm-none-eabi/include/newlib-nano" \
    "-I$(GCC_ARMCOMPILER)/arm-none-eabi/include"

LFLAGS = -Wl,-T,../gcc/CC26X2R1_LAUNCHXL_NoRTOS.lds \
    "-Wl,-Map,$(NAME).map" \
    "-L$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/source" \
    -l:ti/display/lib/display.am4fg \
    -l:ti/grlib/lib/gcc/m4f/grlib.a \
    -l:third_party/spiffs/lib/gcc/m4f/spiffs_cc26xx.a \
    -l:ti/drivers/rf/lib/rf_multiMode_cc26x2_v1.am4fg \
    -l:ti/drivers/lib/drivers_cc26x2_v1.am4fg \
    -eresetISR \
    "-L$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/kernel/nortos" \
    -l:lib/nortos_cc26x2_v1.am4fg \
    -l:ti/devices/cc13x2_cc26x2_v1/driverlib/bin/gcc/driverlib.lib \
    -march=armv7e-m \
    -mthumb \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16 \
    -nostartfiles \
    -static \
    -Wl,--gc-sections \
    "-L$(GCC_ARMCOMPILER)/arm-none-eabi/lib/thumb/v7e-m/fpv4-sp/hard" \
    "-L$(GCC_ARMCOMPILER)/arm-none-eabi/lib" \
    -lgcc \
    -lc \
    -lm \
    -lnosys \
    --specs=nano.specs

all: $(NAME).out

Board.c: ../sdraw.syscfg
	@ echo Generating $@...
	@ $(NODE_JS) $(SYSCONFIG_TOOL) --product $(SYSCONFIG_PRODUCT) --board $(BOARD) --output $(@D) $<
Board.obj: Board.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

main_nortos.obj: ../main_nortos.c Board.obj
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

sdraw.obj: ../sdraw.c Board.obj
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

CC26X2R1_LAUNCHXL_fxns.obj: ../CC26X2R1_LAUNCHXL_fxns.c Board.obj
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

ccfg.obj: ../ccfg.c Board.obj
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

$(NAME).out: $(OBJECTS)
	@ echo linking...
	@ $(LNK)  $(OBJECTS) $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RM) Board.*
