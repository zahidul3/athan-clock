SIMPLELINK_CC26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../../..)

include $(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/imports.mak

KERNEL_BUILD := $(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/kernel/tirtos/builds/CC26X2R1_LAUNCHXL/release
SYSCONFIG_PRODUCT := $(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/.metadata/sdk.json
BOARD := /ti/boards/.meta/CC26X2R1_LAUNCHXL

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
LNK = "$(IAR_ARMCOMPILER)/bin/ilinkarm"

OBJECTS = Board.obj spiffsexternal.obj main_spiffs_tirtos.obj CC26X2R1_LAUNCHXL_fxns.obj ccfg.obj

CONFIGPKG = $(KERNEL_BUILD)/iar

NAME = spiffsexternal

CFLAGS = -I../.. \
    -I. \
    -DDeviceFamily_CC26X2 \
    "-I$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/source/ti/posix/iar" \
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
    "$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/source/ti/drivers/pdm/lib/pdm_cc26x2_v1.arm4f" \
    "$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages/ti/dpl/lib/dpl_cc26x2_v1.arm4f" \
    -f "$(KERNEL_BUILD)/iar/linker.cmd" \
    "$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/source/ti/devices/cc13x2_cc26x2_v1/driverlib/bin/iar/driverlib.lib" \
    --config ../../tirtos/iar/CC26X2R1_LAUNCHXL_TIRTOS.icf \
    --map "$(NAME).map" \
    --silent \
    --cpu=Cortex-M4F \
    --redirect _Printf=_PrintfSmall \
    --redirect _Scanf=_ScanfSmall \
    --semihosting=iar_breakpoint

all: $(NAME).out

$(CONFIGPKG)/linker.cmd $(CONFIGPKG)/compiler.opt:
	@ $(ECHOBLANKLINE)
	@ echo $(CONFIGPKG) is not built.
	@ echo You can build it by issuing $(MAKE) in $(CONFIGPKG).
	@ $(ECHOBLANKLINE)

Board.c: ../../spiffsexternal.syscfg
	@ echo Generating $@...
	@ $(NODE_JS) $(SYSCONFIG_TOOL) --product $(SYSCONFIG_PRODUCT) --board $(BOARD) --output $(@D) $<

Board.obj: Board.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

spiffsexternal.obj: ../../spiffsexternal.c Board.obj $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

main_spiffs_tirtos.obj: ../../tirtos/main_spiffs_tirtos.c Board.obj $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

CC26X2R1_LAUNCHXL_fxns.obj: ../../CC26X2R1_LAUNCHXL_fxns.c Board.obj $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

ccfg.obj: ../../ccfg.c Board.obj $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

$(NAME).out: $(OBJECTS) $(CONFIGPKG)/linker.cmd
	@ echo linking...
	@ $(LNK)  $(OBJECTS) $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RM) Board.*
