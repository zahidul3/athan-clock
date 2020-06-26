---
# sha2hash

---

Example Summary
---------------
An example demo of the SHA2 driver. The target accepts input over an serial port and displays back the hashed results. The input is limited to 256 bytes. If more then 256 bytes is entered, the application will not accept additional input and continue on hashing the intial 256 bytes received.

Peripherals Exercised
---------------------
N/A

Resources & Jumper Settings
---------------------------
> If you're using an IDE (such as CCS or IAR), please refer to Board.html in
your project directory for resources used and board-specific jumper settings.
Otherwise, you can find Board.html in the directory
&lt;SDK_INSTALL_DIR&gt;/source/ti/boards/&lt;BOARD&gt;.

Example Usage
-------------
* Open a serial session (e.g. [`PuTTY`](http://www.putty.org/ "PuTTY's
Homepage"), etc.) to the appropriate COM port.
    * The COM port can be determined via Device Manager in Windows or via
`ls /dev/tty*` in Linux.

The connection should have the following settings
```
    Baud-rate:  115200
    Data bits:       8
    Stop bits:       1
    Parity:       None
    Flow Control: None
```

The console application also needs to be setup to send the carriage-return (CR)
character when 'Enter' is pressed. This because it is used as the termination character
in this demo. If the demo is not working properly, check the default configuration of the
console application in question and make sure that it is sending the carriage-return
character as expected.

* Run the example.

* Send the string to hash over the serial port, the string is terminated by sending a carriage-return character.

* The target will hash the string using SHA2 and echo back the result.

Application Design Details
--------------------------
This examples shows how to use the SHA2 driver in single step hash mode.

> The example expects the input string to be encoded in ASCII binary format. The user must make sure that the
character binary represention used by the host side serial tool corresponds to that of the ASCII binary format.
