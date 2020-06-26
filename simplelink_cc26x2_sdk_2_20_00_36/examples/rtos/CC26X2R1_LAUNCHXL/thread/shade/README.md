# Shade Example Application

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

This document discusses how to use the Shade Example App and the different
parts that compose it. Shade Example Application is a standalone CoAP server
example running on Thread.

Some of the areas explored are:

- Setting up a network.

- Bringing up the device as a Minimum Thread Device(MTD).

- Initialization and use of the Constrained Application Protocol (CoAP) APIs.


# <a name="hardware-prereqs"></a> Hardware Prerequisites

- 2 x CC2652 LaunchPads

- (optional) 1 x Sharp96 LCD boosterpack.


# <a name="software-prereqs"></a> Software Prerequisites

- Code Composer Studio(CCS) v7.3 or higher


# <a name="functional-desc"></a> Functional Description


## <a name="software-overview"></a> Software Overview

This section describes software components and the corresponding source file.


### <a name="application"></a> Application Files

- `shade.[ch]`: Contains the application's event loop, CoAP callback functions,
  and device initialization function calls, and all shade specific logic.

- `otstack.[ch]`: OpenThread stack processing, instantiation and network
  parameters.

- `task_config.h`: This file contains the definitions of the RTOS task
  priorities and stack sizes.

- `images.[ch]`: Contains the raw binary of the images being displayed on the
  LCD screen.

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

This application provides an example implementation of a shade (blinds) using
the Thread wireless protocol and CoAP as the application layer protocol. The
shade application is configured as a minimum thread Device (MTD) which supports
CoAP commands to control the shade state. The shade can be in three states;
open, closed, and drawn.


# <a name="usage"></a> Usage

This section describes how to set up and run the Shade Example Application.


## <a name="usage-buttons"></a> Buttons

- `BTN-1` + `BTN-2` at boot: A factory reset of the non-volatile storage is
  performed.  This must be pressed at the start of the `OtStack_task` function.

- BTN-2: Start the joining process. This may be pressed after the hold image
  appears on-screen.


## <a name="usage-display"></a> Display

There are 2 ways that this application will display information to the user
which can be used simultaneously and are described below:

1. Serial terminal: The shade events will be displayed through UART to a serial
   terminal emulator.  To enable the serial terminal in CCS press `ctrl
   + shift + alt + T`, select `Serial Terminal` under `Choose terminal`, select
   `115200` for Baud Rate and click `OK`

2. `Sharp96 LCD boosterpack`: There is no extra configuration needed to use the
   LCD boosterpack other than plugging it to the LaunchPad running the example
   application.


## <a name="usage-setup-nwk"></a> Setting up the Thread Network

This section describes how to set up a Thread network. The application supports
the ability to be commissioned into a Thread network. Commissioning may be
bypassed by compiling with the `ALLOW_PRECOMMISSIONED_NETWORK_JOIN` predefined
symbol.

1. Set up a LaunchPad as a CLI FTD device by following the READMEs files in
   the respective application folder.

2. Load and run the Shade example on a second LaunchPad.

3. The Shade will print out the device's EUI64 and the application's PSKd
   (pre-shared key device identifier) over the UART terminal. If the device was
   not already commissioned or was built without the
   `ALLOW_PRECOMMISSIONED_NETWORK_JOIN` symbol, it will display this
   information on the LCD screen. If the device has already been commissioned,
   skip to step 8.

   ```
   pskd: SHADEEX1
   EUI64: 0x00124b000f6e6113
   ```

4. Start a commissioner on the CLI FTD by issuing the following command.
   `commissioner start` It will display `Done` if it is successful in starting
   the network.

5. Add Shade LaunchPad device as a joiner device by providing the EUI64 and
   pskd as credentials to the commissioner. `commissioner joiner add
   00124b000f6e6113 SHADEEX1` It will display `Done` if it is successful in
   adding the joiner entry.

6. Now on the Shade LaunchPad, press `BTN-2` to start the joining process. The
   display will show `Joining Nwk ...`.

7. Once the joining process has successfully completed, the LCD will display
   `Joined Nwk` before switching to the shade image. The green LED should
   turn on on the shade LaunchPad once it has joined the network.

8. Next we need to get the IPv6 address of the shade LaunchPad. Use the
   command `ping ff03::1` to send an ICMP echo request to the realm-local all
   nodes multicast address. All devices on Thread network will respond with an
   ICMP echo response. You will see in the terminal a response like the one
   below.

   ```
   8 bytes from fd00:db7:0:0:0:ff:fe00:b401: icmp_seq=1 hlim=64 time=11ms
   ```


## <a name="usage-control"></a> Interfacing with the Shade Example Application

The shade application hosts a simple CoAP server with one registered resource
for the blind state. This resource supports CoAP GET and POST commands.  Any
device with scope of the shade's IPv6 address send commands to the shade
application.

Shade Attribute URI:

- Shade state: `blinds/state`

Open up the serial terminal to the `cli_ftd` application and also to the shade
application.

### Starting the CoAP client

In the CLI FTD serial terminal, type `coap start` at the prompt to start the
CoAP service. It will display the following message if it successful in
starting the CoAP service. ` Coap service started: Done`


### Getting status from the Shade

To get the shade's blind state, type the following command into the CLI FTD
terminal.
```
coap get fd00:db7:0:0:0:ff:fe00:b401 /blinds/state
```

*NOTE*: The IPv6 address will be different for your setup

The shade should respond, and the `cli_ftd` will print a message like the
following.

```
Received coap response with payload: 636c6f736564
```

Converting the payload from hex to ascii we get `closed`.


### Controlling the Shade

The shade state can be changed by sending the appropriate payload in a CoAP
confirmable (con) POST command message, to the IPv6 address of the shade and
the resource URI attribute.

Use the following command in the CLI FTD terminal to set the shade's blind
state to open.

```
coap post fd00:db7:0:0:0:ff:fe00:b401 blinds/state con open
```

*NOTE*: The IPv6 address will be different for your setup

The initial command will result in the message `Sending coap request: Done`.
The shade will respond, and the CLI FTD will print the following message.

```
Received coap response with payload: 636c6f736564
```

If the POST was successful, the shade will change the image displayed on
the LCD boosterpack and print its state change over the serial port.

The above process can be repeated with the string `closed` or `drawn` in place
of `open` to set the shade example to those respective state.
