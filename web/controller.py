#!/usr/bin/python
from SimpleWebSocketServer import WebSocket, SimpleWebSocketServer
import math
import serial

ser = serial.Serial()

def setupSerial():
  ser.port='/dev/ttyUSB0'
  ser.baudrate=115200
  ser.parity=serial.PARITY_NONE
  ser.stopbits=serial.STOPBITS_ONE
  ser.bytesize=serial.EIGHTBITS
  ser.open()

def calculateChecksum(cmd):
  csum = 0
  for i in cmd:
    csum = csum + ord(i)

  csum = ~csum & 0xFF
  return csum

def updateBot(lspeed, rspeed, pan, tilt):
  cmd_id = 1
  cmd = chr(cmd_id) + ' ' + chr(lspeed) + chr(rspeed) + chr(pan) + chr(tilt)
  print chr(cmd_id)
  csum = calculateChecksum(cmd)
  cmd = cmd + ' ' + chr(csum)
  #ser.write(cmd)

def normalizeSpeed(speed):
  if(speed < -255):
    speed = -255
  elif(speed > 255):
    speed = 255
  return (speed + 255)/2

def processMessage(data):
  data = data.split()
  data = map(int, data)
  data[0] = data[0] * 5
  data[1] = data[1] * 5
  speed = math.sqrt(math.pow(data[0], 2) + math.pow(data[1], 2))
  if((data[0] == 0) and (data[1] == 0)):
    return

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

  print 'Data: %d %d' %(lspeed, rspeed)
  updateBot(lspeed, rspeed, 0, 0)

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
  #setupSerial()
  startServer()

main()
