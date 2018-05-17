#!/usr/bin/env python
          
      
import time
import serial
          
      
ser = serial.Serial(

port='/dev/ttyAMA0',
  baudrate = 9600,
  parity=serial.PARITY_NONE,
  stopbits=serial.STOPBITS_ONE,
  bytesize=serial.EIGHTBITS,
  timeout=1
)
counter=50

ser.write('ELO\n');
time.sleep(1);

      
while 1:
  trig = [1,2,3,4,5,6,7,8]
  ser.write([counter])
  time.sleep(1)
  counter += 1
