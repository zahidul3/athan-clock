SIMPLELINK_CC26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/imports.mak


SYSCONFIG_PRODUCT := $(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/.metadata/sdk.json
BOARD := /ti/boards/.meta/CC26X2R1_LAUNCHXL

CC = "$(CCS_ARMCOMPILER)/bin/armcl"
LNK = "$(CCS_ARMCOMPILER)/bin/armcl"

OBJECTS = Board.obj main_nortos.obj sha2hash.obj CC26X2R1_LAUNCHXL_fxns.obj ccfg.obj

NAME = sha2hash

CFLAGS = -I.. \
    -I. \
    -DDeviceFamily_CC26X2 \
    "-I$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/kernel/nortos" \
    "-I$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/kernel/nortos/posix" \
    --silicon_version=7M4 \
    --code_state=16 \
    --little_endian \
    --display_error_number \
    --diag_warning=255 \
    --diag_wrap=off \
    --gen_func_subsections=on \
    --float_support=FPv4SPD16 \
    --symdebug:dwarf \
    "-I$(CCS_ARMCOMPILER)/include"

LFLAGS = "-i$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/source" \
    -lti/display/lib/display.aem4f \
    -lti/grlib/lib/ccs/m4f/grlib.a \
    -lthird_party/spiffs/lib/ccs/m4f/spiffs_cc26xx.a \
    -lti/drivers/rf/lib/rf_multiMode_cc26x2_v1.aem4f \
    -lti/drivers/lib/drivers_cc26x2_v1.aem4f \
    "-i$(SIMPLELINK_CC26X2_SDK_INSTALL_DIR)/kernel/nortos" \
    -llib/nortos_cc26x2_v1.aem4f \
    -lti/devices/cc13x2_cc26x2_v1/driverlib/bin/ccs/driverlib.lib \
    ../ccs/CC26X2R1_LAUNCHXL_NoRTOS.cmd \
    "-m$(NAME).map" \
    --warn_sections \
    --display_error_number \
    --diag_wrap=off \
    --rom_model \
    "-i$(CCS_ARMCOMPILER)/lib" \
    -llibc.a

all: $(NAME).out

Board.c: ../sha2hash.syscfg
	@ echo Generating $@...
	@ $(NODE_JS) $(SYSCONFIG_TOOL) --product $(SYSCONFIG_PRODUCT) --board $(BOARD) --output $(@D) $<
Board.obj: Board.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

main_nortos.obj: ../main_nortos.c Board.obj
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

sha2hash.obj: ../sha2hash.c Board.obj
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

CC26X2R1_LAUNCHXL_fxns.obj: ../CC26X2R1_LAUNCHXL_fxns.c Board.obj
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

ccfg.obj: ../ccfg.c Board.obj
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

$(NAME).out: $(OBJECTS)
	@ echo linking...
	@ $(LNK) $(CFLAGS) -z $(OBJECTS) $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RM) Board.*
