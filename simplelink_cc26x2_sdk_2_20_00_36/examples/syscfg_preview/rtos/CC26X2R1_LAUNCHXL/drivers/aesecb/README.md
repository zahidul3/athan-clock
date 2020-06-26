---
# aesecb

---

Example Summary
---------------
An example demo of the AESECB driver, the target performs encryption and decryption of an pre-defined input using the AESECB driver.
The demo uses a serial port to display the results.

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

* Run the example.

* The target will perform an ecryption and decryption of the defined input and displays the results via the serial port.

Application Design Details
--------------------------
This examples shows how to use the AESECB driver in single step encryption/decryption mode.
