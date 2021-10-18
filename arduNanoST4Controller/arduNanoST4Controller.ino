#include <TimerOne.h>

float SIDEREAL_DAY = 86164.0905;    // Sidereal day in seconds
float MOUNT_WORM = 144;             // Number of teeth for Vixen GP2 worm
float GEAR_RATIO = 2.5;             // Gear ratio, depends on the pulleys 
float MICROSTEPPING = 32;           // Microstepping defined for the motor controlles
float STEPS = 200;                  // Number of steps of the motors 

//Sidereal rate in microseconds
const float siderealRate = (1000000 * SIDEREAL_DAY) / (MOUNT_WORM * GEAR_RATIO * MICROSTEPPING * STEPS);

//Minimun reaction time of the motor (in microsenconds, is 1.9, but we have to put more)
int minMotorTime = 4;

//Define input pins
#define dirRigth 5
#define dirDown 4 
#define dirLeft 6
#define dirUp 3
#define speedx1Pin 8
#define speedx16Pin 9
#define turboPin 7

//Define output pins
#define stepPinAR A5
#define dirPinAR A4
#define stepPinDEC A2
#define dirPinDEC A3
#define microStep0 1 
#define microStep1 0
#define microStep2 2

//Define direction of the motors
#define dirFwd HIGH
#define dirBck LOW

//Status variables
float objectiveSpeed = 1;
bool isSlewingAR = false;
bool isSlewingDec = false;
bool isTracking = false;
bool isGOTOMovingAR = false;
bool isGOTOMovingDEC = false;
float intermediateStep = 0;
int selectedSpeed;

//Input variables
bool inSpeed1x;
bool inSpeed16x;
bool inTurbo;
bool inDirLeft;
bool inDirRight;
bool inDirUp;
bool inDirDown;

void setup() {
  //First of all configure serial port (57.6k, 8 data bits, no parity, one stop bit)
  Serial.begin(57600, SERIAL_8N1);
  Serial.println("INITIALIZED#");

  //Microstepping Setup
  pinMode (microStep0, OUTPUT);
  pinMode (microStep1, OUTPUT);
  pinMode (microStep2, OUTPUT);

  //AR and DEC Output Setup
  pinMode (dirPinAR, OUTPUT);
  pinMode (stepPinAR, OUTPUT);
  pinMode (dirPinDEC, OUTPUT);
  pinMode (stepPinDEC, OUTPUT);

  //Input pins configuration
  pinMode(dirUp, INPUT_PULLUP);
  pinMode(dirLeft, INPUT_PULLUP);
  pinMode(dirRigth, INPUT_PULLUP);
  pinMode(dirDown, INPUT_PULLUP);
  pinMode(speedx1Pin, INPUT_PULLUP);  
  pinMode(speedx16Pin, INPUT_PULLUP);  
  pinMode(turboPin, INPUT);  
  digitalWrite(turboPin, HIGH);

  //Set direction and init motors  
  digitalWrite(dirPinAR, dirFwd);
  digitalWrite(dirPinDEC, dirFwd);
  Timer1.initialize(siderealRate);
}

void loop() {
  //Set microstepping to 1/32
  digitalWrite(microStep0, HIGH);
  digitalWrite(microStep1, HIGH);
  digitalWrite(microStep2, HIGH);

  //Get inputs
  inSpeed1x = digitalRead(speedx1Pin) == LOW ? true : false;
  inSpeed16x = digitalRead(speedx16Pin) == LOW ? true : false;
  inTurbo = digitalRead(turboPin) == LOW ? true : false;
  inDirLeft = digitalRead(dirLeft) == LOW ? true : false;
  inDirRight = digitalRead(dirRigth) == LOW ? true : false;
  inDirUp = digitalRead(dirUp) == LOW ? true : false;
  inDirDown = digitalRead(dirDown) == LOW ? true : false;

  //Call GOTO function
  GOTOMountControl();

  //Check speed buttons
  if (inSpeed1x)
    selectedSpeed = 1;
  else if (inSpeed16x)
    selectedSpeed = 16;
  else
    selectedSpeed = 8;

  if (inTurbo)
    selectedSpeed = selectedSpeed * 50;

  //Check status depending of the buttons pushed   
  if (inDirUp)
  {
    isTracking = true;  
    isSlewingAR = false; 
    isSlewingDec = true;
    objectiveSpeed = selectedSpeed;
    digitalWrite(dirPinDEC, dirFwd);      
  }  
  else if (inDirDown)
  {
    isTracking = true;  
    isSlewingAR = false; 
    isSlewingDec = true;
    objectiveSpeed = selectedSpeed;
    digitalWrite(dirPinDEC, dirBck);   
  }
  else if (inDirRight)
  { 
    isTracking = false;
    isSlewingAR = true; 
    isSlewingDec = false;
    objectiveSpeed = selectedSpeed;
    digitalWrite(dirPinAR, dirFwd);
  }
  else if (inDirLeft)
  { 
    isTracking = false;
    isSlewingAR = true; 
    isSlewingDec = false;
    objectiveSpeed = selectedSpeed;
    digitalWrite(dirPinAR, dirBck);
  }
  else
  {  
    if (!isGOTOMovingAR && !isGOTOMovingDEC)
    {
      //We have not pushed any button, and not in a GOTO
      isTracking = true; 
      isSlewingAR = false; 
      isSlewingDec = false;
      objectiveSpeed = 1; 
      digitalWrite(dirPinAR, dirFwd);
    }
    else
    {
      //We are in a GOTO
      isTracking = !isGOTOMovingAR; 
      isSlewingAR = isGOTOMovingAR; 
      isSlewingDec = isGOTOMovingDEC;
      objectiveSpeed = selectedSpeed;
    }
  }

  //This section controls the movement of the motor (we treat x1 speed in RA as "special")
  if (selectedSpeed == 1 && inDirRight)
  {    
    objectiveSpeed = 2;
    Timer1.setPeriod(siderealRate/objectiveSpeed);
  } 
  else if (selectedSpeed == 1 && inDirLeft)
  {
    objectiveSpeed = 0;
    Timer1.detachInterrupt();
  } 
  else
  {
    Timer1.setPeriod(siderealRate/objectiveSpeed);           
    Timer1.attachInterrupt(move_tracking);             
  }     
}

void move_tracking()
{
  //Move AR
  if (isSlewingAR || (isTracking && !isSlewingDec) || (isSlewingDec && intermediateStep >= objectiveSpeed))
  {
    digitalWrite(stepPinAR, HIGH);  
    delayMicroseconds(minMotorTime);
    digitalWrite(stepPinAR, LOW);  
    intermediateStep = 0;
  }
  
  //Mode DEC
  if (isSlewingDec)
  {
    digitalWrite(stepPinDEC, HIGH);  
    delayMicroseconds(minMotorTime);
    digitalWrite(stepPinDEC, LOW);  

    intermediateStep = intermediateStep + 1;
  }
}
