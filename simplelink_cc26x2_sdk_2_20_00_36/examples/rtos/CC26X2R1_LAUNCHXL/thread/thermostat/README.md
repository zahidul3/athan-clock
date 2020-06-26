# Thermostat Example Application

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

This document discusses how to use the Thermostat Example Application and the
different parts that compose it. Thermostat Example Application is a
standalone CoAP server example running on Thread.

Some of the areas explored are:

- Setting up a network.

- Bringing up the device as a Full Thread Device(FTD).

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

- `thermostat.[ch]`: Contains the application's event loop, CoAP callback
  functions, device initialization function calls, and all thermostat specific
  logic.

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

This application provides an example implementation of a thermostat using the
Thread wireless protocol and CoAP as the application layer protocol.  The
thermostat application is configured as a full thread Device (FTD) which
supports CoAP commands to control the temperature and setpoint. These are
integers in the range of [10 .. 99].


# <a name="usage"></a> Usage

This section describes how to set up and run the Thermostat Example
Application.


## <a name="usage-buttons"></a> Buttons

- `BTN-1` + `BTN-2` at boot: A factory reset of the non-volatile storage is
  performed.  This must be pressed at the start of the `OtStack_task` function.

- BTN-2: Start the joining process. This may be pressed after the hold image
  appears on-screen.


## <a name="usage-display"></a> Display

There are 2 ways that this application will display information to the user
which can be used simultaneously and are described below:

1. Serial terminal: The thermostat events will be displayed through UART to a
   serial terminal emulator.  To enable the serial terminal in CCS press `ctrl
   + shift + alt + T`, select `Serial Terminal` under `Choose terminal`, select
   `115200` for Baud Rate and click `OK`

2. `Sharp96 LCD boosterpack`: There is no extra configuration needed to use the
   LCD boosterpack other than plugging it to the LaunchPads running the example
   application.


## <a name="usage-setup-nwk"></a> Setting up the Thread Network

This section describes how to set up a Thread network. The application supports
the ability to be commissioned into a Thread network. Commissioning may be
bypassed by compiling with the `ALLOW_PRECOMMISSIONED_NETWORK_JOIN` predefined
symbol. The Temperature Sensor example has the ability to report to the
Thermostat, consult its README for details on this functionality.

1. Set up a LaunchPad as a CLI FTD device by following the READMEs files in
   the respective application folder.

2. Load and run the Thermostat example on a second LaunchPad.

3. The Thermostat will print out the device's EUI64 and the application's PSKd
   (pre-shared key device identifier) over the UART terminal. If the device was
   not already commissioned or was built without the
   `ALLOW_PRECOMMISSIONED_NETWORK_JOIN` symbol, it will display this
   information on the LCD screen. If the device has already been commissioned,
   skip to step 8.

   ```
   pskd: THERMSTAT1
   EUI64: 0x00124b000f6e6113
   ```

4. Start a commissioner on the CLI FTD by issuing the following command.
   `commissioner start` It will display `Done` if it is successful in starting
   the network.

5. Add Thermostat LaunchPad device as a joiner device by providing the EUI64
   and pskd as credentials to the commissioner. `commissioner joiner add
   00124b000f6e6113 THERMSTAT1` It will display `Done` if it is successful in
   adding the joiner entry.

6. Now on the Thermostat LaunchPad, press `BTN-2` to start the joining process.
   The display will show `Joining Nwk ...`.

7. Once the joining process has successfully completed, the LCD will display
   `Joined Nwk` before switching to the thermostat image. The green LED should
   turn on on the shade LaunchPad once it has joined the network.

8. Next we need to get the IPv6 address of the thermostat LaunchPad. Use the
   command `ping ff03::1` to send an ICMP echo request to the realm-local all
   nodes multicast address. All devices on Thread network will respond with an
   ICMP echo response. You will see in the terminal a response like the one
   below.

   ```
   8 bytes from fd00:db7:0:0:0:ff:fe00:b401: icmp_seq=1 hlim=64 time=11ms
   ```


## <a name="usage-control"></a> Interfacing with the Thermostat Example Application

The thermostat application hosts a simple CoAP server with two registered
resources for the current temperature and setpoint. These resources support CoAP
GET and POST commands. Any device with scope of the thermostat's IPv6 address send
commands to the thermostat application.

Thermostat Attribute URI:

- Temperature value: `thermostat/temperature`

- Temperature setpoint: `thermostat/setpoint`

Open up the serial terminal to the `cli_ftd` application and also to the
thermostat application.

### Starting the CoAP client

In the CLI FTD serial terminal, type `coap start` at the prompt to start the
CoAP service. It will display the following message if it successful in
starting the CoAP service. ` Coap service started: Done`


### Getting status from the Thermostat

To get the thermostat's current temperature, type the following command into
the CLI FTD terminal.

```
coap get fd00:db7:0:0:0:ff:fe00:b401 thermostat/temperature
```

*NOTE*: The IPv6 address will be different for your setup

The thermostat should respond, and the `cli_ftd` will print a message like the
following.

```
Received coap response with payload: 3635
```

Converting the payload from hex to ascii we get `65` which is the default
temperature in degrees Fahrnenheit.

The above process can be repeated with the endpoint `thermostat/setpoint` in
place of `thermostat/temperature` to get the current setpoint.


### Controlling the Thermostat

The thermostat temperature can be changed by sending the appropriate payload in
a CoAP confirmable (con) POST command message, to the IPv6 address of the
thermostat and the resource URI attribute.

Use the following command in the CLI FTD terminal to set the thermostat's
temperature to 90 degrees Fahrenheit.

```
coap post fd00:db7:0:0:0:ff:fe00:b401 thermostat/temperature con 90
```

*NOTE*: The IPv6 address will be different for your setup

The initial command will result in the message `Sending coap request: Done`.
The thermostat will respond, and the CLI FTD will print the following message.

```
Received coap response with payload: 3930
```

If the POST was successful, the thermostat will change the image displayed on
the LCD boosterpack and print its state change over the serial port.

The above process can be repeated with the endpoint `thermostat/setpoint` to
change the current setpoint of the thermostat.
