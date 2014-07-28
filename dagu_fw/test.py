#!/usr/bin/python
import serial
import time

ser = serial.Serial()
ser.port='/dev/ttyUSB0'
ser.baudrate=57600
ser.parity=serial.PARITY_NONE
ser.stopbits=serial.STOPBITS_ONE
ser.bytesize=serial.EIGHTBITS
ser.open()
time.sleep(11)
while (1):
    val = raw_input('Enter: ')
    num = val.split()
    #print num
    csum = 1+32+int(num[0])+int(num[1])+32
    print csum
    csum = ~csum & 255
    #print csum
    msg = chr(1) + ' ' + chr(int(num[0])) + chr(int(num[1])) + chr(0) + chr(0) + ' ' + chr(csum) + '\n'
    print csum
    num = ser.write(msg)
    print num
    resp = ser.readline()
    print resp

