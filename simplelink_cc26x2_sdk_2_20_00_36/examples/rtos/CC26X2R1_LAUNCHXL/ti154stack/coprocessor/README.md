TI 15.4-Stack Coprocessor Example
============================

Example Summary
---------------

The coprocessor example application demonstrates how to implement a Media Access Control (MAC) coprocessor device designed to work with a host processor in a two-chip scenario. The coprocessor provides an interface to the TI 15.4-Stack protocol stack with full-function MAC capability through a serial interface. Through this interface the application, running on the host, allows for the addition of TI 15.4-Stack wireless functionality to systems that are not suited to single-chip solutions.

> The project names for CC1310 and C1350 platforms are referred to as CC13x0. Replace x with either **1** or **5** depending on the specific wireless MCU being used.

> See `features.h` for the default application features that are enabled.

Peripherals Exercised
---------------------

* `Board_UART_TX` `Board_UART_RX` - UART transmit and receive lines available through the backchannel CC13x0 Launchpad USB connection. This is used as the serial connection to the host processor.

Resources & Jumper Settings
---------------------------
The following hardware is required to run the TI 15.4-Stack Out of Box (OOB) example applications:
* One [**CC13x0 Launchpad development kits**](http://www.ti.com/tool/launchxl-cc1310)
* One host device running the collector application. See the [**TI 15.4-Stack Gateway SDK**](http://www.ti.com/tool/ti-15.4-stack-gateway-linux-sdk) for an example of how to use the Sitara AM335x processor running the collector application to enable a Sensor to Cloud solution.

> If you're using an IDE (such as CCS or IAR), please refer to `Board.html` in
your project directory for resources used and board-specific jumper settings.
Otherwise, you can find `Board.html` in the directory
&lt;SDK_INSTALL_DIR&gt;/source/ti/boards/&lt;BOARD&gt;.

Example Usage
-------------
This example project implements a coprocessor device and as such does not directly provide a user interface.
After flashing the coprocessor application to the target device, the host collector application will output information
regarding the status of the coprocessor.

Analyzing Network Traffic 
-------------------------

TI 15.4-Stack provides the means to analyze over-the-air traffic by including a packet sniffer firmware image. With an additional CC13x0 Launchpad, users can set up a packet sniffer with the software provided in the SDK. More information about this can be found in the TI 15.4-Stack documentation under **Packet Sniffer**.


Application Design Details
--------------------------
A detailed description of the application architecture can be found within the TI 15.4-Stack Coprocessor Interface Guide
within the [**TI 15.4-Stack Gateway SDK**](http://www.ti.com/tool/ti-15.4-stack-gateway-linux-sdk) installation.
&lt;LINUX_SDK_INSTALL_DIR&gt;/docs/TI-15.4 Stack_CoP_Interface_Guide.pdf.

An overview of the application can be found in your installation within the
TI-15.4 Stack Getting Started Guide's Application Overview section.

&lt;SDK_INSTALL_DIR&gt;/docs/ti154stack/ti154stack-getting-started-guide.html.

> For IAR users: When using the CC13x0DK, the TI XDS110v3 USB Emulator must
be selected. For the CC13x0_LAUNCHXL, select TI XDS110 Emulator. In both cases,
select the cJTAG interface.