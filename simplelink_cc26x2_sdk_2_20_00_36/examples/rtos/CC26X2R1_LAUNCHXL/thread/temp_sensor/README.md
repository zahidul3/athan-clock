# Temperature Sensor Example Application

## Table of Contents

* [Introduction](#intro)
* [Hardware Prerequisites](#hardware-prereqs)
* [Software Prerequisites](#software-prereqs)
* [Functional Description](#functional-desc)
  * [Software Overview](#software-overview)
    * [Application Files](#application)
    * [Example Application](#example-application)
* [Usage](#usage)
  * [Buttons](#usage-buttons)
  * [Display](#usage-display)
  * [Setting up the Thread Network](#usage-setup-nwk)
  * [Interfacing with the Example Application](#usage-control)

# <a name="intro"></a> Introduction

This document discusses how to use the Temperature Sensor Example Application
and the different parts that compose it. Thermostat Example Application is a
standalone CoAP server example running on Thread.

Some of the areas explored are:

- Setting up a network.

- Bringing up the device as a Minimum Thread Device (MTD).

- Initialization and use of the Constrained Application Protocol (CoAP) APIs.


# <a name="hardware-prereqs"></a> Hardware Prerequisites


## Device Reporting example

- 3 x CC2652 LaunchPads

- 1 x BeagleBone Black.

- (optional) 1 x Sharp96 LCD boosterpack.


## Basic CoAP usage

- 2 x CC2652 LaunchPads

- (optional) 1 x Sharp96 LCD boosterpack.


# <a name="software-prereqs"></a> Software Prerequisites

- Code Composer Studio(CCS) v7.3 or higher


# <a name="functional-desc"></a> Functional Description


## <a name="software-overview"></a> Software Overview

This section describes software components and the corresponding source file.


### <a name="application"></a> Application Files

- `tempsensor.[ch]`: Contains the application's event loop, CoAP callback
  functions, device initialization function calls, and all temperature sensor
  specific logic.

- `otstack.[ch]`: OpenThread stack processing, instantiation and network
  parameters.

- `task_config.h`: This file contains the definitions of the RTOS task
  priorities and stack sizes.

If the application is compiled with the predefined symbol,
`ALLOW_PRECOMMISSIONED_NETWORK_JOIN`, following parameter should be verified in
`otstack.h`.

- `OT_NWK_PAN_ID` This is the Personal Area Network (PAN) identifier of the
  network. The default value is set to 0xface. Please make sure this matches
  the PAN ID set in the border router, otherwise change it accordingly.

- `OT_NWK_CHANNEL` This sets the channel on which the device will be operating
  in the 2.4 GHz spectrum as per the IEEE 802.15.4 specification. The default
  value is channel 14.


### <a name="example-application"></a> Example Application

This application provides an example implementation of a temperature sensor
using the Thread wireless protocol and CoAP as the application layer protocol.
The temperature sensor application is configured as a minimum thread Device
(MTD) which supports CoAP commands to read the temperature. The temperature
sensor example also includes basic reporting functionality to a known IPv6
address, in this case the thermostat example application.


# <a name="usage"></a> Usage

This section describes how to set up and run the Temperature Sensor Example
Application.


## <a name="usage-buttons"></a> Buttons

- `BTN-1` + `BTN-2` at boot: A factory reset of the non-volatile storage is
  performed.  This must be pressed at the start of the `OtStack_task` function.

- BTN-2: Start the joining process. This may be pressed after the hold image
  appears on-screen.

## <a name="usage-display"></a> Display

The temperature sensor events will be displayed through UART to a serial
terminal emulator. To enable the serial terminal in CCS press `ctrl + shift +
alt + T`, select `Serial Terminal` under `Choose terminal`, select `115200` for
Baud Rate and click `OK`


## <a name="usage-setup-nwk"></a> Setting up the Thread Network

### Basic CoAP usage

This section describes how to set up a Thread network. The application supports
the ability to be commissioned into a Thread network. Commissioning may be
bypassed by compiling with the `ALLOW_PRECOMMISSIONED_NETWORK_JOIN` predefined
symbol.

1. Set up a LaunchPad as a CLI FTD device by following the READMEs files in
   the respective application folder.

2. Load and run the Temperature Sensor example on a second LaunchPad.

3. The Temperature Sensor will print out the device's EUI64 and the
   application's PSKd (pre-shared key device identifier) over the UART
   terminal. If the device was already commissioned or was built with the
   `ALLOW_PRECOMMISSIONED_NETWORK_JOIN` symbol, commissioning will not be
   necessary, skip to step 8.

   ```
   pskd: TMPSENS1
   EUI64: 0x00124b000f6e6113
   ```

4. Start a commissioner on the CLI FTD by issuing the following command.
   `commissioner start` It will display `Done` if it is successful in starting
   the network.

5. Add Temperature Sensor LaunchPad device as a joiner device by providing the
   EUI64 and pskd as credentials to the commissioner. `commissioner joiner add
   00124b000f6e6113 TMPSENS1` It will display `Done` if it is successful in
   adding the joiner entry.

6. Now on the Temperature Sensor LaunchPad, press `BTN-2` to start the joining
   process. The UART will print `Joining Nwk ...`.

7. Once the joining process has successfully completed, the UART will print
   `Joined Nwk`. The green LED should turn on on the shade LaunchPad once it
   has joined the network.

8. Next we need to get the IPv6 address of the temperature sensor LaunchPad.
   Use the command `ping ff03::1` to send an ICMP echo request to the
   realm-local all nodes multicast address. All devices on Thread network will
   respond with an ICMP echo response. You will see in the terminal a response
   like the one below.

   ```
   8 bytes from fd00:db7:0:0:0:ff:fe00:b401: icmp_seq=1 hlim=64 time=11ms
   ```


### Running the example with reporting

The Temperature Sensor example has a basic reporting feature. When connected to
a network with a Globally Unique Address, the temperature sensor will attempt
to post the temperature it reads to the thermostat. To enable this feature you
need to setup a Thread network with an NCP connected to a BeagleBone Black and
a LaunchPad with the Thermostat Example. Consult the NCP example's README for
information on setting up a BeagleBone Black based border router.

*NOTE*: This kind of static addressing is a hack of SLAAC. Proper discovery
mechanisms are being explored.


## <a name="usage-control"></a> Interfacing with the Temperature Sensor Example Application

The temperature sensor application hosts a simple CoAP server with one
registered resource for the current temperature. This resource supports CoAP
GET commands. Any device with scope of the temperature sensor's IPv6 address
can send commands to the temperature sensor application.

Temperature Sensor Attribute URI:

- Temperature value: `tempsensor/temperature`

Open up the serial terminal to the `cli_ftd` application and also to the
temperature sensor application.


### Starting the CoAP client

In the CLI FTD serial terminal, type `coap start` at the prompt to start the
CoAP service. It will display the following message if it successful in
starting the CoAP service. ` Coap service started: Done`


### Getting status from the Temperature Sensor

To get the temperature sensor's current temperature, type the following command
into the CLI FTD terminal.

```
coap get fd00:db7:0:0:0:ff:fe00:b401 tempsensor/temperature
```

*NOTE*: The IPv6 address will be different for your setup

The temperature sensor should respond, and the `cli_ftd` will print a message
like the following.

```
Received coap response with payload: 3635
```

Converting the payload from hex to ascii we get `65` which is the temperature
in degrees Fahrenheit.
