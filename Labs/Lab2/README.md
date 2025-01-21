## Summary

This program uses the timer module capture and compare functionality to detect the duration of pulses
on an input pin (Pin PB20). It uses the TIMG12 timer module, which has a 32-bit counter register, to enable high
resolution pulse width measurements from the range of microseconds to seconds. When pulses are detected,
the duration of the pulse (in 32 MHz clock cycles from rising edge to falling edge) is reported over 
the UART connection as a string ("=     XXXXX\r\n"), and the red Launchpad LED is briefly illuminated. 
A second timer module is used to trigger a heartbeat UART message every second if no pulse is detected. In 
addition to the heartbeat, the PA15 pin is briefly pulsed. To verify functionality, the PA17 and PB20 pins
can be jumpered.

A python script, `read_data.py` is included that opens the serial port and records the values being
transmitted to a file.  


## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOA | PA15 | Standard Output |
| GPIOA | PB20 | Input to Timer Capture |
| SYSCTL |  |  |
| TIMG0 |  | Periodic interrupt wakes system to send heartbeat |
| TIMG12 |  | Capture on rising edge and falling edge |
| UART0 | PA11 | RX Pin |
| UART0 | PA10 | TX Pin |
| EVENT |  |  |
| DMA |  | Used for serial transmission |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## Example Usage
Compile, load and run the example.

When plugged into the computer, the Launchpad creates two serial ports. In our brief experimentation,
on Windows, the serial data appears to typically appear on `COM4` (Device Manager will show two new
`COM` ports when the Launchpad is connected. It will be one of them!).  On MacOS, it appears on something like
`/dev/tty.usbmodemMG3500011` (run `ls /dev/tty*` in a Terminal window to see possible choices.)

Connect to a serial terminal (i.e., PuTTY on Windows) to view the UART output or use the `read_data.py` 
python script to log data.

Note that the serial configuraiton should be:
- 115200 baud rate
- one stop bit
- no parity

