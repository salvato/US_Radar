
//////////////////////////////////////////////////////////
// Fuction prototypes
//////////////////////////////////////////////////////////

void serialEvent();
bool executeCommand();
void decodeCommand(byte *inputString, byte *decodedString);
void errorSignal(int16_t nBeep);
void sendValue(byte Mark, int16_t value);
void home();
void measure();
void rotate();


//////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////
  
// Possible commands passing to Serial Line
enum commands {
    AreYouThere    = 0xAA,
    StartSending   = 0x81,
    StopSending    = 0x82,
    StartMarker    = 0xFF,
    EndMarker      = 0xFE,
    AngleMark      = 0xFD,
    DistMark       = 0xFC,
    SpecialByte    = 0x80
};

// ----- motor pattern
const byte Motor[8] = //half-stepping
{ B00001000,
  B00001100,
  B00000100,
  B00000110,
  B00000010,
  B00000011,
  B00000001,
  B00001001
};

const int16_t TRIGGER_PIN =  5;
const int16_t ECHO_PIN    =  6;
const int16_t STEP_IN4    =  8;
const int16_t STEP_IN3    =  9;
const int16_t STEP_IN2    = 10;
const int16_t STEP_IN1    = 11;
// On Nano:   LED_BUILTIN = 13; 

const int16_t MAX_DISTANCE =   2;
const int16_t maxMessageLen = 64;

// change the following to fit the number of steps per revolution 
// for your motor
const int16_t stepsPerRevolution = 16*32;

const int32_t baudRate       = 115200;

const int16_t startA         = 0;        // real motor zero
const int16_t endA           = 360;      // real motor 180°
const int16_t aStep          = 1;        // Angluar Step (Sensor Aperture ~15°)
const int16_t nAvg           = 1;       // Number of measurements per angluar step
const int16_t half           = nAvg/2;

const int16_t servoDelay     = 100;
const int16_t echoDelay      = 50;


//////////////////////////////////////////////////////////
// Variables
//////////////////////////////////////////////////////////
  
// initialize the stepper library on pins 8 through 11:
CheapStepper myStepper (STEP_IN1, STEP_IN2, STEP_IN3, STEP_IN4);
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

bool          connected = false;// Are we connected to the PC ?
byte          command;
byte          inputString[64];
byte          outputString[64];
byte          decodedString[64];
int16_t       decodedLength;
int16_t       nReceivedChar;
int16_t       commandLength;

boolean       inProgress     = false;
boolean       startFound     = false;
boolean       allReceived    = false;

volatile bool bSendValues    = false;

int16_t       pos            = 0;        // variable to store the servo position
int16_t       dist;
boolean       moveClockwise = true;


int Index = 0;                  //Motor[] array index
int Step_counter = 0;           //360 degrees requires 1024 steps
unsigned long Delay = 2;        //give motor shaft time to move
byte Pattern;                   //Motor[] pattern

// ----- acoustic "radar" display data
int Azimuth   = 0;                //Azimuth (PI/64 radians) measured CCW from reference
int Distance  = 0;
int Direction = 0;              //counter-clockwise=0, clockwise=1

unsigned long
Speed_of_rotation = 30;         //controls beam rotation: 1 = fastest

//////////////////////////////////////////////////////////
// Functions
//////////////////////////////////////////////////////////

void 
setup() {
    Serial.begin(baudRate, SERIAL_8N1); // initialize serial: 8 data bits, no parity, one stop bit.
    bSendValues = false;
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    myStepper.setTotalSteps(stepsPerRevolution);
    // <6 rpm blocked in code, may overheat
    // 23-24rpm may skip steps
    myStepper.setRpm(22);
    // Wait until connected to a PC
    /*
    while(!connected) {
        serialEvent();
        if(allReceived) {
            executeCommand();
            allReceived = false;
        }
        sonar.ping_cm();
        delay(echoDelay);
        digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ^ 1);
    }
    digitalWrite(LED_BUILTIN, LOW);
    */
}


void 
loop() {
  for(int j=0; j<10; j++) {
    for(pos=0; pos<stepsPerRevolution; pos += 1) { // going from startA degrees to endA degrees
/*        serialEvent();
        if(allReceived) {
            executeCommand();
            allReceived = false;
        }
        if(!bSendValues) break;
*/        
        myStepper.moveDegrees(moveClockwise, 1);
//        delay(servoDelay);
//        dist = sonar.ping_cm();
//        sendValue(AngleMark, pos);
//        sendValue(DistMark, dist);
//Serial.print("Pos: ");    Serial.print(pos);
//Serial.print("\tDist: "); Serial.println(dist);
        /*
        for(int i=0; i<nAvg; i++) {
            Serial.print(*(dist+i));
            Serial.print("\t");
        }
        */
    }
  }
  delay (5000);
    moveClockwise = !moveClockwise;
}


// SerialEvent occurs whenever a new data comes in the hardware serial RX.
// This routine is run between each time loop() runs, so using delay inside loop 
// can delay response. Multiple bytes of data may be available.
void 
serialEvent() {
    while(Serial.available()) {
        byte inChar = (byte)Serial.read();
        if(inChar == StartMarker) {
            nReceivedChar = 0;
            inProgress = true;
        }
        if(inProgress) {
            inputString[nReceivedChar] = inChar;
            nReceivedChar++;
        }
        if(nReceivedChar == maxMessageLen) {
            errorSignal(2);
        }
        if(inChar == EndMarker) {
            allReceived = true;
            commandLength = nReceivedChar;
            inProgress = false;
            return;
        }
    }
}


void 
decodeCommand(byte *decoded) {
    decodedLength = 0;
    for(int i=1; i<commandLength-1; i++) {
        if(inputString[i] == SpecialByte) {
            i++;
            decoded[decodedLength] = byte(SpecialByte + inputString[i]); 
        }
        else {
            decoded[decodedLength] = byte(inputString[i]);
        }
        decodedLength++;
    }
}


bool
executeCommand() {
    decodeCommand(decodedString);
    command = decodedString[1];
    
    if(command == byte(AreYouThere)) {// Are you there ? 
        outputString[0] = StartMarker;
        outputString[1] = byte(4);// Total Command length
        outputString[2] = byte(AreYouThere);// The command
        outputString[3] = EndMarker;
        for(int i=0; i<outputString[1]; i++) {
            Serial.write(byte(outputString[i]));
        }
        connected = true;
        return true;
    }
    else if(command == byte(StopSending)) {
        bSendValues = false;
        return true;
    }
    else if(command == byte(StartSending)) {
        bSendValues = true;
//>>>>>>>>>>>>        myservo.write(startA);    // tell servo to go to zero position
        delay(1500);              // wait until servo reach the position
        return true;
    }
    else {
        errorSignal(1);
    }
    return false;
}


void
sendValue(byte Mark, int16_t value) {
    byte currentByte;
    // Prepare header and command
    outputString[0] = StartMarker;
    outputString[2] = Mark;
    
    int nBytes = 3;
    for(int i=0; i<sizeof(value); i++) {
        currentByte = byte(value & 0xFF);
        if(currentByte >= SpecialByte) {
            outputString[nBytes++] = SpecialByte;
            outputString[nBytes++] = byte(currentByte-SpecialByte);
        }
        else {
            outputString[nBytes++] = currentByte;
        }
        value >>= 8;
    }
    outputString[nBytes++] = EndMarker;
    outputString[1] = nBytes;
    for(int i=0; i<outputString[1]; i++) {
        Serial.write(byte(outputString[i]));
    }
}


void
errorSignal(int iErr) {
    for(int i=0; i<iErr; i++) {
        digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ^ 1);
        delay(500);
    }
    digitalWrite(LED_BUILTIN, LOW);
}


