SIMPLELINK_CC26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/imports.mak

XDCPATH = $(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/source;$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages;

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
LNK = "$(IAR_ARMCOMPILER)/bin/ilinkarm"

XDCTARGET = iar.targets.arm.M4F
PLATFORM = ti.platforms.simplelink:CC2642R1F

OBJECTS = mutex.obj CC26X2R1_LAUNCHXL.obj CC26X2R1_LAUNCHXL_fxns.obj ccfg.obj

CONFIGPKG = mutex

NAME = mutex

CFLAGS = -I../.. \
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
    "$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/source/ti/devices/cc13x2_cc26x2_v1/driverlib/bin/iar/driverlib.lib" \
    --config ../../tirtos/iar/CC26X2R1_LAUNCHXL_TIRTOS.icf \
    --map "$(NAME).map" \
    --silent \
    --cpu=Cortex-M4F \
    --redirect _Printf=_PrintfSmall \
    --redirect _Scanf=_ScanfSmall \
    --semihosting=iar_breakpoint

.PRECIOUS: $(CONFIGPKG)/compiler.opt $(CONFIGPKG)/linker.cmd

all: $(NAME).out

$(CONFIGPKG)/compiler.opt: $(CONFIGPKG)/linker.cmd

$(CONFIGPKG)/linker.cmd: ../../tirtos/mutex.cfg
	@ echo Running Configuro...
	@ $(XDC_INSTALL_DIR)/xs --xdcpath="$(XDCPATH)" xdc.tools.configuro -c "$(IAR_ARMCOMPILER)" -t $(XDCTARGET) -p $(PLATFORM) --compileOptions "$(CFLAGS)" -o "$(CONFIGPKG)" ../../tirtos/mutex.cfg

mutex.obj: ../../mutex.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

CC26X2R1_LAUNCHXL.obj: ../../CC26X2R1_LAUNCHXL.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

CC26X2R1_LAUNCHXL_fxns.obj: ../../CC26X2R1_LAUNCHXL_fxns.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

ccfg.obj: ../../ccfg.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

$(NAME).out: $(OBJECTS) $(CONFIGPKG)/linker.cmd
	@ echo linking...
	@ $(LNK)  $(OBJECTS) -f $(CONFIGPKG)/linker.cmd $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RMDIR) $(NAME) > $(DEVNULL) 2>&1
