#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/TI/simplelink_cc26x2_sdk_2_20_00_36/source;C:/TI/simplelink_cc26x2_sdk_2_20_00_36/kernel/tirtos/packages;C:/TI/simplelink_cc26x2_sdk_2_20_00_36/source/ti/ble5stack
override XDCROOT = C:/TI/xdctools_3_50_07_20_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/TI/simplelink_cc26x2_sdk_2_20_00_36/source;C:/TI/simplelink_cc26x2_sdk_2_20_00_36/kernel/tirtos/packages;C:/TI/simplelink_cc26x2_sdk_2_20_00_36/source/ti/ble5stack;C:/TI/xdctools_3_50_07_20_core/packages;..
HOSTOS = Windows
endif
