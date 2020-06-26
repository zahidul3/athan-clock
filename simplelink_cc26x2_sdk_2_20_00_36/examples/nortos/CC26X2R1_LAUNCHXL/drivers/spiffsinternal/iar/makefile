SIMPLELINK_CC26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../..)

include $(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/imports.mak

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
LNK = "$(IAR_ARMCOMPILER)/bin/ilinkarm"

OBJECTS = main_nortos.obj spiffsinternal.obj CC26X2R1_LAUNCHXL.obj CC26X2R1_LAUNCHXL_fxns.obj ccfg.obj

NAME = spiffsinternal

CFLAGS = -I.. \
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

main_nortos.obj: ../main_nortos.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

spiffsinternal.obj: ../spiffsinternal.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

CC26X2R1_LAUNCHXL.obj: ../CC26X2R1_LAUNCHXL.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

CC26X2R1_LAUNCHXL_fxns.obj: ../CC26X2R1_LAUNCHXL_fxns.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

ccfg.obj: ../ccfg.c
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
