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

def processMessage(data):
  data = data.split()
  data = map(int, data)
  print data[0]
  speed = math.sqrt(math.pow(data[0], 2) + math.pow(data[1], 2))
  if(data[0] < 0):
    lwt =  -1 * data[1]/255.0
    rwt = 1
  elif (data[0] > 0):
    lwt = 1
    rwt = -1 * data[1]/255.0
  else:
    lwt = 1
    rwt = 1
   
  print 'Data: %d %f %f' %(speed, lwt , rwt)
  lspeed = int(lwt * speed)
  rspeed = int(rwt * speed)

class SimpleEcho(WebSocket):

  def handleMessage(self):
    print 'Received ' + self.data
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
