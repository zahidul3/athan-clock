# invoke SourceDir generated makefile for ble_debug.pem4f
ble_debug.pem4f: .libraries,ble_debug.pem4f
.libraries,ble_debug.pem4f: package/cfg/ble_debug_pem4f.xdl
	$(MAKE) -f C:\workspace_v8\ble5_project_zero_cc26x2r1lp_app\TOOLS/src/makefile.libs

clean::
	$(MAKE) -f C:\workspace_v8\ble5_project_zero_cc26x2r1lp_app\TOOLS/src/makefile.libs clean

