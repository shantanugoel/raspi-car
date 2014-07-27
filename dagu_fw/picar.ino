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

/*
 * Command Structure:
 * Each part of command is separated by space
 * <CommandID: 1 byte> <Parameters/Payload: variable > <Checksum: 1 byte>
 * Commands supported:
 * 1. Move:
 * Command ID: 01
 * Parameters: LMSpeed(1b) RMSpeed(1b) PanAngle(1b) TiltAngle(1b)
 */
#define MAXRCVSIZE 8

#define CMD_ID_MOVE 01

#define ERR_SUCCESS            0
#define ERR_TOO_MANY_BYTES     1
#define ERR_CMD_VERIF_FAILED   2
#define ERR_CMD_INVALID        3
#define ERR_CMD_STRUCT_INVALID 4

char rcvBuffer[MAXRCVSIZE] = {0};

typedef struct cmdMove_t
{
  uint8_t cmdID;
  uint8_t pad1;
  uint8_t lspd;
  uint8_t rspd;
  uint8_t pan;
  uint8_t tilt;
  uint8_t pad5;
  uint8_t  csum;

}cmdMove_t;

void setup()
{
  Serial.begin(115200);
  pinMode(LMDIRPIN, OUTPUT);
  pinMode(LMSPDPIN, OUTPUT);
  pinMode(RMDIRPIN, OUTPUT);
  pinMode(RMSPDPIN, OUTPUT);
}

void motorChange(int motor, int speed)
{
  int direction = (speed > 0)?HIGH:LOW;
  speed = abs(speed);
  if(LM == motor)
  {
    digitalWrite(LMDIRPIN, direction);
    analogWrite(LMSPDPIN, speed);
  }
  else
  {
    digitalWrite(RMDIRPIN, direction);
    analogWrite(RMSPDPIN, speed);
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
  Serial.print(analogRead(BVMONPIN), DEC);
  Serial.print("\n");
  flagError(status);
}

int verifyCommand(int numBytes)
{
  int i = 0, csum = 0;
  int ret = ERR_SUCCESS;
  for (i = 0 ; i < numBytes - 2; ++i)
  {
    csum += rcvBuffer[i];
  }
  csum = ~csum;
  if(csum != rcvBuffer[numBytes - 1])
  {
    ret = ERR_CMD_VERIF_FAILED;
  }
  return ret;
}

int processMoveCommand(int numBytes)
{
  int ret = ERR_SUCCESS;
  if(sizeof(cmdMove_t) == numBytes)
  {
    motorChange(LM, rcvBuffer[2] - 128);
    motorChange(RM, rcvBuffer[3] - 128);
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
      break;
    }
  } while(1);

  if (MAXRCVSIZE >= numBytes)
  {
    for(i = 0; i < numBytes; ++i)
    {
      rcvBuffer[i] = Serial.read();
    }

    ret = verifyCommand(numBytes);

    if(ERR_SUCCESS == ret)
    {
      ret = processCommand(numBytes);
    }
  }
  else
  {
    ret = ERR_TOO_MANY_BYTES;
  }

  return ret;
}

void loop()
{
  sendAck(receiveCommand());
}
