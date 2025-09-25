# Flight Software User Guide
## Description
This details how to interface with the flight software application upon execution.
## Debugging
According to [these](http://microchip.wikidot.com/mplabx:select-a-hardware-tool) Help pages, there are ways to debug the program using MPLAB, but I have not taken the time to figure out how. It may be hard given we use a CCS compiler. IDK.

## HyperTerminal
The applications have debug statements that are sent out over one of the serial ports. To view the debug statements you need a different cable setup (compared to installing the software). For this, you will only need the USB cable.
1. Connect the USB cable into your computer and the USB port on the Development Board.
2. Identify the COM Port which the USB is connected to on your computer
   i. Device Manager->Ports
   ii. Which Communications Port is available? E.g. COM1
3. Open up HyperTerminal
4. Create a connection to the appropriate COM Port
5. Set the bit rate to 115200
6. Start the connection
7. You should see print statements appearing on the screen

## Command Line Interface
The programs have a built in command line interface. You can enter **mm** into the HyperTerminal to view its menu. Through it, you can send commands, run different tests, and show the available macros and schedules.