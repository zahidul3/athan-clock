---
# mpu

---

## Example Summary

This application serves as an example program showing how to program
the MPU hardware using SYS/BIOS APIs.

## Example Usage

* Run the application, the string `hello world` will be printed to
  the SysMin output buffer and the CPU will be in an exception handler
  routine executing a loop forever. Exception decoding in Hwi module's
  ROV view will show the cause of the exception.

## Application Design Details
* This application programs the MPU to map the FLASH, SRAM and GPRAM.
  The SRAM and GPRAM are marked as non-executable. The application
  then attempts to jump to a function in SRAM. This is expected to
  cause a memory fault exception as SRAM memory is not executable.

## References

* For more help, search the SYS/BIOS User Guide.