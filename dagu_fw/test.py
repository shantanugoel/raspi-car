#!/usr/bin/python
import serial

ser = serial.Serial(0)
while (1):
    val = raw_input('Enter: ')
    num = val.split()
    #print num
    csum = 1+32+int(num[0])+int(num[1])+32
    #print csum
    csum = ~csum & 255
    #print csum
    msg = chr(1) + ' ' + chr(int(num[0])) + chr(int(num[1])) + chr(0) + chr(0) + ' ' + chr(csum)
    print msg
    ser.write(msg)
    resp = ser.readline()
    print resp

