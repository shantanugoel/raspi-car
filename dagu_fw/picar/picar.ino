#include "ServoTimer2.h"

#define LMDIRPIN 7
#define LMSPDPIN 9
#define RMDIRPIN 8
#define RMSPDPIN 10
#define BVMONPIN A7

#define LM 0
#define RM 1

#define MINLSPEED -255
#define MAXLSPEED 255

#define MINRSPEED -255
#define MAXRSPEED 255


#define PANSERVOPIN  2
#define TILTSERVOPIN 4

#define MINPANANGLE  0
#define MAXPANANGLE  180
#define MINTILTANGLE 25
#define MAXTILTANGLE 75

/*
 * Command Structure:
 * Each part of command is separated by space
 * <CommandID: 1 byte> <Parameters/Payload: variable > <Checksum: 1 byte>
 * Commands supported:
 * 1. Move:
 * Command ID: 01
 * Parameters: LMSpeed(1b) RMSpeed(1b) PanAngle(1b) TiltAngle(1b)
 * End with a newline
 */
#define MAXRCVSIZE 9

#define CMD_ID_MOVE 01

#define ERR_SUCCESS            0
#define ERR_TOO_MANY_BYTES     1
#define ERR_CMD_VERIF_FAILED   2
#define ERR_CMD_INVALID        3
#define ERR_CMD_STRUCT_INVALID 4

uint8_t rcvBuffer[MAXRCVSIZE] = {
  0};

int debug = 0;

typedef struct cmdMove_t
{
  uint8_t cmdID;
  uint8_t pad1;
  uint8_t msgsize;
  uint8_t lspd;
  uint8_t rspd;
  uint8_t pan;
  uint8_t tilt;
  uint8_t pad5;
  uint8_t  csum;
}
cmdMove_t;

ServoTimer2 panServo;
ServoTimer2 tiltServo;

void setup()
{
  Serial.begin(115200);
  pinMode(LMDIRPIN, OUTPUT);
  pinMode(LMSPDPIN, OUTPUT);
  pinMode(RMDIRPIN, OUTPUT);
  pinMode(RMSPDPIN, OUTPUT);
  panServo.attach(PANSERVOPIN);
  tiltServo.attach(TILTSERVOPIN);
  panServo.write(1000);
  tiltServo.write(1200);
}

void motorChange(int motor, int speed)
{
  int direction = (speed > 0)?HIGH:LOW;
  speed = abs(speed);
  if(LM == motor)
  {
    digitalWrite(LMDIRPIN, direction);
    analogWrite(LMSPDPIN, speed);
    if(debug)
    {
      Serial.print("***");
      Serial.print(direction, DEC);
      Serial.print(speed, DEC);
      Serial.print("***");
    }
  }
  else
  {
    digitalWrite(RMDIRPIN, direction);
    analogWrite(RMSPDPIN, speed);
  }
}

void servoChange(int pan, int tilt)
{
  if(pan < MINPANANGLE)
  {
    pan = MINPANANGLE;
  }
  else if(pan > MAXPANANGLE)
  {
    pan = MAXPANANGLE;
  }

  if(tilt < MINTILTANGLE)
  {
    tilt = MINTILTANGLE;
  }
  else if(tilt > MAXTILTANGLE)
  {
    tilt = MAXTILTANGLE;
  }
  panServo.write(map(pan, 0, 180, 0, 2000));
  tiltServo.write(map(tilt, 0, 180, 0, 2000));
  if(debug)
  {
    Serial.print("===");
    Serial.print(pan, DEC);
    Serial.print(" ");
    Serial.print(tilt, DEC);
    Serial.print("===");
  }
}

void flagError(int status)
{
  int err = LOW;
  if(ERR_SUCCESS != status)
  {
    err = HIGH;
  }
  digitalWrite(13, err);
}

void sendAck(int status)
{
  Serial.print(status, DEC);
  Serial.print(":");
  Serial.print(analogRead(BVMONPIN), DEC);
  Serial.print("\n");
  flagError(status);
}

int verifyCommand(int numBytes)
{
  int i = 0, csum = 0;
  int ret = ERR_SUCCESS;
  if(debug)
  {
    Serial.print("CSUM: ");
  }
  for (i = 0 ; i < numBytes - 1; ++i)
  {
    csum += rcvBuffer[i];
  }
  if(debug)
  {
    Serial.print(csum);
  }
  csum = ~csum & 0xFF;
  if(debug)
  {
    Serial.print(" ");
    Serial.print(csum);
    Serial.print("||");
  }
  if(csum != rcvBuffer[numBytes - 1])
  {
    if(debug)
    {
      Serial.print(csum, DEC);
    }
    ret = ERR_CMD_VERIF_FAILED;
  }
  return ret;
}

int processMoveCommand(int numBytes)
{
  int ret = ERR_SUCCESS;
  if(sizeof(cmdMove_t) == numBytes)
  {
    motorChange(LM, rcvBuffer[3] - 128);
    motorChange(RM, rcvBuffer[4] - 128);
    servoChange(rcvBuffer[5], rcvBuffer[6]);
  }
  else
  {
    ret = ERR_CMD_STRUCT_INVALID;
  }

  return ret;
}

int processCommand(int numBytes)
{
  int ret = ERR_CMD_INVALID;
  switch(rcvBuffer[0])
  {
  case CMD_ID_MOVE:
    ret = processMoveCommand(numBytes);
    break;
  }
  return ret;
}

int receiveCommand()
{
  int ret = ERR_SUCCESS;
  int i = 0;
  int numBytes = 0;
  do
  {
    numBytes = Serial.available();
    if (numBytes > 0)
    {
      for(; numBytes > 0; --numBytes)
      {
        rcvBuffer[i] = Serial.read();
        if(debug)
        {
          Serial.print(i, DEC);
          Serial.print(" : ");
          Serial.print(rcvBuffer[i], DEC);
          Serial.print("==");
        }
        ++i;
      }
      if (i < MAXRCVSIZE)
      {
        continue;
      }
      else if (i == MAXRCVSIZE)
      {
        break;
      }
      else
      {
        ret = ERR_TOO_MANY_BYTES;
        break;
      }
    }
  } 
  while(1);

  if(ERR_SUCCESS == ret)
  {
    ret = verifyCommand(i);
  }
  if(ERR_SUCCESS == ret)
  {
    ret = processCommand(i);
  }

  return ret;
}

void loop()
{
  sendAck(receiveCommand());
}










