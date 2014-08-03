#!/usr/bin/python
import serial
import time

ser = serial.Serial()
ser.port='/dev/ttyUSB0'
ser.baudrate=115200
ser.parity=serial.PARITY_NONE
ser.stopbits=serial.STOPBITS_ONE
ser.bytesize=serial.EIGHTBITS
ser.open()
time.sleep(11)
ser.flushInput()
ser.flushOutput()
while (1):
    val = raw_input('Enter: ')
    num = val.split()
    #print num
    csum = 1+32+9+int(num[0])+int(num[0])+int(num[0])+int(num[0])+32
    print csum
    csum = ~csum & 255
    #print csum
    msg = chr(1) + ' ' + chr(9) + chr(int(num[0])) + chr(int(num[0])) + chr(int(num[0])) + chr(int(num[0])) + ' ' + chr(csum)
    print csum
    num = ser.write(msg)
    ser.flush()
    print num
    resp = ser.readline()
    ser.flushInput()
    ser.flushOutput()
    print resp

