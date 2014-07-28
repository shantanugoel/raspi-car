#!/usr/bin/python
from SimpleWebSocketServer import WebSocket, SimpleWebSocketServer
import math
import serial
import time

ser = serial.Serial()
last_data = [0, 0, 0, 0]

def setupSerial():
  ser.port='/dev/ttyUSB0'
  ser.baudrate=115200
  ser.parity=serial.PARITY_NONE
  ser.stopbits=serial.STOPBITS_ONE
  ser.bytesize=serial.EIGHTBITS
  ser.open()
  time.sleep(11)

def calculateChecksum(cmd):
  csum = 0
  for i in cmd:
    csum = csum + ord(i)

  csum = ~csum & 0xFF
  return csum

def updateBot(lspeed, rspeed, pan, tilt):
  cmd_id = 1
  cmd = chr(cmd_id) + ' ' + chr(9) + chr(int(lspeed)) + chr(int(rspeed)) + chr(pan) + chr(tilt)
  #print chr(cmd_id)
  csum = cmd_id + 32 + 9 + lspeed + rspeed + pan + tilt + 32
  csum = ~csum & 255
  #csum = calculateChecksum(cmd)
  #print "%d %d %d %d" %(cmd_id, lspeed, rspeed, csum)
  cmd = cmd + ' ' + chr(csum)
  ser.write(cmd)
  msg = ser.readline()
  #print msg

def normalizeSpeed(speed):
  if(speed < -255):
    speed = -255
  elif(speed > 255):
    speed = 255
  return (speed + 255)/2

def calculatePan(pan):
  pan = pan/50.0
  pan = pan * -1
  pan = pan + 1
  pan = int(pan*90)
  if(pan < 0):
    pan = 0
  elif(pan > 180):
    pan = 180
  return pan

def calculateTilt(tilt):
  tilt = tilt/50.0
  #tilt = tilt * -1
  tilt = tilt + 1
  tilt = int(tilt*90)
  if(tilt < 0):
    tilt = 0
  elif(tilt > 180):
    tilt = 180
  return tilt

def processMessage(data):
  data = data.split()
  data = map(int, data)
  data[0] = data[0] * 3 #should be 5 but limiting speed for now
  data[1] = data[1] * 3
  speed = math.sqrt(math.pow(data[0], 2) + math.pow(data[1], 2))
  if((data[0] == 0) and (last_data[0] == 0) and  (data[1] == 0) and (last_data[1] == 0) and (data[2] == 0) and (last_data[2] == 0) and (data[3] == 0) and (last_data[3] == 0)):
    return

  last_data[0] = data[0]
  last_data[1] = data[1]
  last_data[2] = data[2]
  last_data[3] = data[3]

  if(data[0] < 0):
    lwt =  -1 * data[1]/255.0
    rwt = -1.0 * cmp(data[1], 0)
  elif (data[0] > 0):
    lwt = -1.0 * cmp(data[1], 0)
    rwt = -1.0 * data[1]/255.0
  else:
    lwt = -1.0 * cmp(data[1], 0)
    rwt = -1.0 * cmp(data[1], 0)
   
  lspeed = int(lwt * speed)
  rspeed = int(rwt * speed)

  lspeed = normalizeSpeed(lspeed)
  rspeed = normalizeSpeed(rspeed)

  pan = calculatePan(data[2])
  tilt = calculateTilt(data[3])

  #print 'Data: %d %d' %(pan, tilt)
  updateBot(lspeed, rspeed, pan, tilt)

class SimpleEcho(WebSocket):

  def handleMessage(self):
    #print 'Received ' + self.data
    if self.data is None:
      self.data = ''
    else:
      processMessage(self.data)

  def handleConnected(self): 
    print self.address, 'connected'

  def handleClose(self):
    print self.address, 'closed'

def startServer():
  server = SimpleWebSocketServer('', 12345, SimpleEcho)
  server.serveforever()

def main():
  setupSerial()
  startServer()

main()
