import serial
import re

with serial.Serial('/dev/tty.usbmodemMG3500011', 921600) as ser: # Open the serial port
  with open('pulselist.txt', 'w') as datafile:
    while (True):
      # Read data from the port
      data = ser.readline()

      # Decode the data (if necessary)
      data = data.decode('utf-8')
      match = re.search('=\s*(\d*), *(\d*)', data)
      
      if match:
        space_length = match.group(1)
        pulse_length = match.group(2)
        # Print the received data
        print(space_length, pulse_length)
        datafile.write('{}, {}\n'.format(space_length, pulse_length))
      else:
        print(data, end='') # Should print the no pulse detected message

