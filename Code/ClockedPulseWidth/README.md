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
