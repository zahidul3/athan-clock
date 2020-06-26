SIMPLELINK_CC26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/imports.mak


SYSCONFIG_PRODUCT := $(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/.metadata/sdk.json
BOARD := /ti/boards/.meta/CC26X2R1_LAUNCHXL

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
LNK = "$(IAR_ARMCOMPILER)/bin/ilinkarm"

OBJECTS = Board.obj main_nortos.obj watchdog.obj CC26X2R1_LAUNCHXL_fxns.obj ccfg.obj

NAME = watchdog

CFLAGS = -I.. \
    -I. \
    -DDeviceFamily_CC26X2 \
    "-I$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/kernel/nortos" \
    "-I$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/kernel/nortos/posix" \
    --cpu=Cortex-M4F \
    --debug \
    --silent \
    -e \
    --aeabi \
    --thumb \
    --diag_suppress=Pa050

LFLAGS = "$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/source/ti/display/lib/display.arm4f" \
    "$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/source/ti/grlib/lib/iar/m4f/grlib.a" \
    "$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/source/third_party/spiffs/lib/iar/m4f/spiffs_cc26xx.a" \
    "$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/source/ti/drivers/rf/lib/rf_multiMode_cc26x2_v1.arm4f" \
    "$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/source/ti/drivers/lib/drivers_cc26x2_v1.arm4f" \
    "$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/kernel/nortos/lib/nortos_cc26x2_v1.arm4f" \
    "$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/source/ti/devices/cc13x2_cc26x2_v1/driverlib/bin/iar/driverlib.lib" \
    --config ../iar/CC26X2R1_LAUNCHXL_NoRTOS.icf \
    --map "$(NAME).map" \
    --silent \
    --cpu=Cortex-M4F \
    --redirect _Printf=_PrintfSmall \
    --redirect _Scanf=_ScanfSmall \
    --semihosting=iar_breakpoint

all: $(NAME).out

Board.c: ../watchdog.syscfg
	@ echo Generating $@...
	@ $(NODE_JS) $(SYSCONFIG_TOOL) --product $(SYSCONFIG_PRODUCT) --board $(BOARD) --output $(@D) $<
Board.obj: Board.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

main_nortos.obj: ../main_nortos.c Board.obj
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

watchdog.obj: ../watchdog.c Board.obj
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

CC26X2R1_LAUNCHXL_fxns.obj: ../CC26X2R1_LAUNCHXL_fxns.c Board.obj
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

ccfg.obj: ../ccfg.c Board.obj
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

$(NAME).out: $(OBJECTS)
	@ echo linking...
	@ $(LNK)  $(OBJECTS) $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RM) Board.*
