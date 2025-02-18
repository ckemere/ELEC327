import re

import argparse
import serial
import serial.tools.list_ports


# To use this code, load the ClockedPulseWidth.out firmware onto your measurement Launchpad. Connect the following pins:
# Measurement Launchpad Pin -> Morse Code Launchpad Pin
# PB16  ->  PB26 (or which ever LED pin is displaying the morse code message)
# PB0   ->  PA31 (LFCLK output pin)
# PB6   ->  NRST (this is on the outer row of the right pin header next to PB20)
# GND   ->  GND
#
# If both the morse code launchpad and the pulse measurement launchpad are plugged into the same computer, you'll need to 
# figure out what serial port your measurement launchpad is connected to. I suggest running the code with the -l option to 
# figure out your options. Each launchpad creates 2 serial ports; the one that gives the data is usually assigned a lower
# number. If you plug in the measurement launchpad first, and look at the available ports, then you'll know which 
# option to select when both launchpads are powered and you're ready to run the code to test!

def list_serial_ports():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        print(f"{port.device}: {port.description}")

# Under linux, we can check by serial number - (/dev/serial/by-id/usb-Texas_Instruments_XDS110__03.00.00.32__Embed_with_CMSIS-DAP_MG350002-if00)
def read_serial_data(port, count):
    with serial.Serial(port, 921600, timeout=3) as ser: # Open the serial port 
      with open('pulselist.txt', 'w') as datafile:
        ser.write('r'.encode())
        got_response = False
        timed_out = False
        while (not got_response) and (not timed_out):
          # Read data from the port
          try:
            data = ser.readline()
          except serial.SerialTimeoutException:
            timed_out = True

          if not timed_out:
            data = data.decode('utf-8')
            if (data == 'Received r\r\n'):
              got_response = True
            else:
              print(f'Waiting for response {data}', end='') # Should print the no pulse detected message

        if timed_out:
          print('Timed out waiting for response!')

        else:
          counter = 0

          while (counter < count) and (not timed_out):
            # Read data from the port
            try:
              data = ser.readline()
            except serial.SerialTimeoutException:
              timed_out = True

            if not timed_out:
            # Decode the data (if necessary)
              data = data.decode('utf-8')
              match = re.search('>\s*(\d*), *(\d*)', data)
              if match:
                edgeDirection = match.group(1)
                timestamp = match.group(2)
                # Print the received data
                print(edgeDirection, timestamp)
                datafile.write('{}, {}\n'.format(edgeDirection, timestamp))
                counter += 1
              else:
                print(data, end='') # Should print the no pulse detected message


def main():
    parser = argparse.ArgumentParser(description="Read characters from a serial port.")
    parser.add_argument('-p', '--port', help='Serial port to connect to (e.g., COM3 or /dev/ttyUSB0 or /dev/tty.usbmodemMG3500011)')
    parser.add_argument('-n', '--num_pulses', type=int, default=50, help='Number of puslses to read')
    parser.add_argument('-l', '--list_ports', action='store_true', help='List available serial ports')

    args = parser.parse_args()


    if args.list_ports:
        list_serial_ports()
    elif args.port and args.num_pulses:
        read_serial_data(args.port, args.num_pulses)
    else:
        print("Error: Please specify a port to read from or use the --list_ports option to list available ports.")
        parser.print_help()
   


if __name__ == '__main__':
    main()
