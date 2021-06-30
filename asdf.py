import serial
import time
import os

# Open grbl serial port
s = serial.Serial('COM3',115200)

time.sleep(2)

while(1):
  if s.readline() == b'v0.0.1\r\n':
    break

ok = 'ok\n'

while(1):
  gcode_line = s.readline()
  if gcode_line:
    print(gcode_line)
    s.write(ok.encode())
    print(ok)
    if gcode_line == b'v0.0.1\r\n':
      break
    gcode_line = None

# Close file and serial port
s.close()