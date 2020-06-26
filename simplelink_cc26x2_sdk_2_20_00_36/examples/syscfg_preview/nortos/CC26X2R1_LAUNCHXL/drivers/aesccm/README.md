---
# aesccm

---

Example Summary
---------------
An interactive demo of the AESCCM driver. The target performs encryption or decryption of a
given input using the AESCCM driver. The demo uses a serial port to get input and display
the result. The user is asked to enter all the information required to perform encryption
or decryption. This includes additional authentication data (AAD), text to encrypt/decrypt
and a nonce. In the case of decryption, the user will also need to enter the message authentication code (MAC).

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

The console application needs to be setup to send the carriage-return (CR) character
when 'Enter' is pressed. This because it is used as the termination character in this demo.
If the demo is not working properly, check the default configuration of the console
application in question and make sure that it is sending the carriage-return character
as expected.

* Run the example.

* Select either encryption or decryption action by following the prompted instructions.

* Enter the AAD, plain/cipher text, nonce and MAC as requested depending on the chosen action.

* The target will perform either encryption or decryption of the provided input and display back the results.

Application Design Details
--------------------------
This examples shows how to use the AESCCM driver in single step encryption/decryption mode.

The length of the entered nonce must be between 7-13 characters long.
The length of the MAC in this example is fixed to 16 bytes, but can be changed to be
any value between 0 - 16 bytes. This is done by adjusting the size of the 'macHex' and
'macAlpha' buffers.

When prompted to enter a message to decrypt or to enter the MAC, a HEX coded string is expected as input (e.g. 'A3B4B1').

> The example expects the input to be encoded in ASCII binary format. The user must make sure that the
character binary representation used by the host side serial tool corresponds to that of the ASCII binary format.
