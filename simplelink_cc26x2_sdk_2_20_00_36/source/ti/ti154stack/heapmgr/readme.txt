March 14, 2016

This is an experimental modification to the original heapmgr.h file located at:
C:\GIT\lprf-sd-common-components\applib\src\heap\heapmgr.h

This version will allow auto-sizing the heap based on the available unused RAM, while remaining backward compatible.

To use:
Set HEAPMGR_SIZE=0 in the App project Preprocessor defines.

If HEAPMGR_SIZE is not defined, the value of HEAPMGR_SIZE will be set in the heapmgr.h as before.

If HEAPMGR_SIZE is defined, but it's value is not zero, then that value of HEAPMGR_SIZE will be used as before.
