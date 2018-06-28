

//////////////////////////////////////////////////////////
// Fuction prototypes
//////////////////////////////////////////////////////////

void errorSignal(int16_t nBeep);
void rotate();
void motorOff();


//////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////
  
const bool    Disable            = HIGH;
const bool    Enable             = LOW;
const int32_t Motor_Delay        = 10;// To give motor shaft time to move
const int16_t stepsPerRevolution = 48;// change to fit your motor
const int32_t baudRate           = 115200;// ----- serial port


//////////////////////////////////////////////////////////
// Used Pins on the Arduino
//////////////////////////////////////////////////////////

const int16_t Motor_Step         =  8;// To execut a Step
const int16_t Motor_Direction    =  9;// The step direction
const int16_t Motor_Enable       = 10;// To Enable/Disable motor

// On Nano:   LED_BUILTIN        = 13;// The built in Led

//////////////////////////////////////////////////////////
// Variables
//////////////////////////////////////////////////////////
  
byte          command;
byte          inputString[64];
byte          outputString[64];
byte          decodedString[64];
int16_t       decodedLength;
int16_t       nReceivedChar;
int16_t       commandLength;
bool          connected;
bool          inProgress;
bool          startFound;
bool          allReceived;
volatile bool bSendValues;


int16_t       Angle;   // Acoustic "Radar" display data
int16_t       Distance;//     "
bool          moveClockwise;


//////////////////////////////////////////////////////////
// Functions
//////////////////////////////////////////////////////////


void 
setup() {
    Serial.begin(baudRate, SERIAL_8N1);    // initialize serial: 8 data bits, no parity, one stop bit.
    
    connected      = false;
    bSendValues    = false;
    inProgress     = false;
    startFound     = false;
    allReceived    = false;

    Angle          = 0;
    Distance       = 0;

    moveClockwise  = true;
    
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
      
    // ----- configure stepper motor controller Pins
    pinMode(Motor_Enable, OUTPUT);
    digitalWrite(Motor_Enable, Disable);
    
    pinMode(Motor_Step, OUTPUT);
    digitalWrite(Motor_Step, LOW);
    pinMode(Motor_Direction, OUTPUT);
    digitalWrite(Motor_Direction, moveClockwise ? HIGH : LOW);
    digitalWrite(Motor_Enable, Enable);
}


void 
loop() {
    for(Angle=0; Angle<stepsPerRevolution; Angle++) {
        rotate();
        delay(Motor_Delay);         //slows rotational speed
    }
    moveClockwise = !moveClockwise;
    digitalWrite(Motor_Direction, moveClockwise ? HIGH : LOW);
    digitalWrite(Motor_Enable, Disable);
    delay(1000);
    digitalWrite(Motor_Enable, Enable);
    for(Angle=stepsPerRevolution; Angle>0; Angle--) {
        rotate();
        delay(Motor_Delay);         //slows rotational speed
    }
    moveClockwise = !moveClockwise;
    digitalWrite(Motor_Direction, moveClockwise ? HIGH : LOW);
    digitalWrite(Motor_Enable, Disable);
    delay(1000);
    digitalWrite(Motor_Enable, Enable);
}


void
rotate() {
    digitalWrite(Motor_Step, HIGH);
    delay(Motor_Delay);
    digitalWrite(Motor_Step, LOW);
}


void
motorOff() {
    digitalWrite(Motor_Enable, Disable);
}


void
errorSignal(int iErr) {
    for(int i=0; i<iErr; i++) {
        digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ^ 1);
        delay(500);
    }
    digitalWrite(LED_BUILTIN, LOW);
}


