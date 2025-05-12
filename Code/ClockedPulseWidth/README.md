# ClockedPulseWidth

To use this code, load the ClockedPulseWidth.out firmware onto your measurement Launchpad. Connect the following pins:
Measurement Launchpad Pin -> Morse Code Launchpad Pin

  - PB16  ->  PB26 (or which ever LED pin is displaying the morse code message)
  - PB0   ->  PA31 (LFCLK output pin)
  - PB6   ->  NRST (this is on the outer row of the right pin header next to PB20)
  -  GND   ->  GND

If both the morse code launchpad and the pulse measurement launchpad are plugged into the same computer, you'll need to 
figure out what serial port your measurement launchpad is connected to. I suggest running the code with the -l option to 
figure out your options. Each launchpad creates 2 serial ports; the one that gives the data is usually assigned a lower
number. If you plug in the measurement launchpad first, and look at the available ports, then you'll know which 
option to select when both launchpads are powered and you're ready to run the code to test!


### Programming a different serial number on the Launchpad
If you program a different serial number on the Launchpad, this will facilitate uploading code.
(I suspect there will be a way to also know the proper serial port programatically based on
serial number??)

This requires using the program `xdsdfu` (the onboard debugger is an XDS110). In my
installation, I find this program in this path:
```
/Applications/ti/ccs1220/ccs/ccs_base/common/uscif/xds110
```

The default serial number is "MG350001". To change the serial number to, e.g., "MG350002",
run the following commands in sequence - `./xdsdfu -m; ./xdsdfu -s MG350002 -r`. You
should get output that looks like this:

```
/Applications/ti/ccs1220/ccs/ccs_base/common/uscif/xds110 % ./xdsdfu -m            

USB Device Firmware Upgrade Utility
Copyright (c) 2008-2019 Texas Instruments Incorporated.  All rights reserved.

Scanning USB buses for supported XDS110 devices...


<<<< Device 0 >>>>

VID: 0x0451    PID: 0xbef3
Device Name:   XDS110 Embed with CMSIS-DAP
Version:       3.0.0.32
Manufacturer:  Texas Instruments
Serial Num:    MG350001
Mode:          Runtime
Configuration: Standard

Switching device into DFU mode.
/Applications/ti/ccs1220/ccs/ccs_base/common/uscif/xds110 % ./xdsdfu -s MG350002 -r

USB Device Firmware Upgrade Utility
Copyright (c) 2008-2019 Texas Instruments Incorporated.  All rights reserved.

Scanning USB buses for supported XDS110 devices...

Setting serial number to "MG350002"...

```
