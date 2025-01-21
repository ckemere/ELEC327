import serial
import re

with serial.Serial('ls', 115200) as ser: # Open the serial port
  with open('pulselist.txt', 'w') as datafile:
    while (True):
      # Read data from the port
      data = ser.readline()

      # Decode the data (if necessary)
      data = data.decode('utf-8')
      match = re.search('=\s*(\d*)', data)
      
      if match:
        pulselength = match.group(1)
        # Print the received data
        print(pulselength)
        datafile.write('{}\n'.format(pulselength))
      else:
        print(data, end='') # Should print the no pulse detected message

