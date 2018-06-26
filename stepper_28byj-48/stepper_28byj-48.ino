
//////////////////////////////////////////////////////////
// Fuction prototypes
//////////////////////////////////////////////////////////

void serialEvent();
bool executeCommand();
void decodeCommand(byte *inputString, byte *decodedString);
void errorSignal(int16_t nBeep);
void sendValue(byte Mark, int16_t value);
void sendValues(byte Mark, int16_t value1, int16_t value2);
void home();
void measure();
void rotate();
void motorOff();


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
    ValuesMark     = 0xFB,
    StepMark       = 0xFA,
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

//////////////////////////////////////////////////////////
// Used Pins on the Arduino
//////////////////////////////////////////////////////////
const int16_t Micro_switch = 4;

const int16_t Trigger_Pin =  5;
const int16_t Echo_Pin    =  6;

//const int16_t Step_In4  =  8;// Do not change !!
//const int16_t Step_In3  =  9;// They are accessed
//const int16_t Step_In2  = 10;// as a whole accessing
//const int16_t Step_In1  = 11;// PORTB (pins 8 to 13)

// On Nano:   LED_BUILTIN = 13; 

const int16_t maxMessageLen = 64;

// ----- micro-switch for end-stop
const bool Switch_present = false;//no micro-switch = false

// ----- serial port
const int32_t baudRate       = 115200;

//////////////////////////////////////////////////////////
// Variables
//////////////////////////////////////////////////////////
  
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

// ----- acoustic "radar" display data
int16_t Angle     = 0;
int16_t Distance  = 0;

// change to fit the steps per revolution of your motor
int16_t stepsPerRevolution = 4*32;
boolean       moveClockwise  = true;

unsigned long Delay = 2;        // give motor shaft time to move
byte Pattern;                   // Motor[] pattern


unsigned long
Speed_of_rotation = 30;         //controls beam rotation: 1 = fastest


//////////////////////////////////////////////////////////
// Functions
//////////////////////////////////////////////////////////


void 
setup() {
    Serial.begin(baudRate, SERIAL_8N1);    // initialize serial: 8 data bits, no parity, one stop bit.
    
    bSendValues = false;
    
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
      
    pinMode(Echo_Pin,    INPUT);           //make Echo pin an input
    pinMode(Trigger_Pin, OUTPUT);          //set Trig pin LOW
    digitalWrite(Trigger_Pin, LOW);

    // ----- configure stepper motor
    Pattern = DDRB;                        // get PORTB data directions
    Pattern = Pattern | B00001111;         // preserve other data direction &
    DDRB = Pattern;                        // make pins 8, 9, 10, 11 outputs
  
    // Wait until we are connected to a PC
    while(!connected) {
        serialEvent();
        if(allReceived) {
            executeCommand();
            allReceived = false;
        }
        delay(50);
        digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ^ 1);
    }
    digitalWrite(LED_BUILTIN, LOW);
    // ----- rotate beam to start-up position
    if (Switch_present) {                   // ----- configure micro-switch
        pinMode(Micro_switch, INPUT_PULLUP);//"wire-OR" normally HIGH
        delay(100);
        home();
    }
    sendValue(StepMark, stepsPerRevolution);
}


void 
loop() {
    for(Angle=0; Angle<stepsPerRevolution; Angle++) {
        serialEvent();
        if(allReceived) {
            executeCommand();
            allReceived = false;
        }
        if(!bSendValues) break;
        measure();
        sendValues(ValuesMark, Angle, Distance);
        rotate();
        delay(Speed_of_rotation);         //slows rotational speed
        /* ----- send the results to the display
        Serial.print("Angle: ");     Serial.print(Angle);
        Serial.print(" Distance: "); Serial.print(Distance);
        Serial.print(" Switch: ");   Serial.println(digitalRead(Micro_switch));
        */
    }
    moveClockwise = !moveClockwise;
    for(Angle=stepsPerRevolution; Angle>0; Angle--) {
        serialEvent();
        if(allReceived) {
            executeCommand();
            allReceived = false;
        }
        if(!bSendValues) break;
        measure();
        sendValues(ValuesMark, Angle, Distance);
        rotate();
        /* ----- send the results to the display
        Serial.print("Angle: ");     Serial.print(Angle);
        Serial.print(" Distance: "); Serial.print(Distance);
        Serial.print(" Switch: ");   Serial.println(digitalRead(Micro_switch));
        */
        delay(Speed_of_rotation);         //slows rotational speed
    }
    moveClockwise = !moveClockwise;
}


// ===============================
// measure distances
// ===============================
void 
measure() {
    // ----- locals
    unsigned long start_time;           //microseconds
    unsigned long finish_time;          //microseconds
    unsigned long time_taken;           //microseconds
    unsigned long timeout;              //microseconds
    unsigned long pause;                //microseconds
    boolean flag;
  
    // ----- generate 10uS start pulse
    digitalWrite(Trigger_Pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(Trigger_Pin, LOW);
    // ----- wait for pulse(s) to be sent
    while(!digitalRead(Echo_Pin));             //wait for high
    
    start_time = micros();
    // ----- set timeout radius
    timeout = start_time + 18000;//15000;       //set timeout radius to 2.5 meters
    // ----- measure first object distance
    flag = false;
    while(!flag) {
        if(!digitalRead(Echo_Pin)) flag = true;  //exit loop if object detected
        if(timeout < micros())     flag = true;  //exit loop if timeout exceeded
    }
    finish_time = micros();
    if(finish_time >= timeout) {
        Distance = -1;    
    }
    else {
        // ----- calculate object distance(cm)
        time_taken = finish_time - start_time;
        Distance = ((float)time_taken) / 59;
        // ----- wait for first object echo to finish
        pause = finish_time + 1000;                //1000uS means 17cm closest object spacing
        while(pause > micros());                   //wait 1000uS
    }
}


void
rotate() {
    if(!moveClockwise) {// ----- counter-clockwise scan
        for(int i=7; i>=0; i--) {
            // ----- rotate motor to next step
            Pattern = PORTB;                //get current motor pattern
            Pattern = Pattern & B11110000;  //preserve MSN
            Pattern = Pattern | Motor[i];   //create new motor pattern
            PORTB = Pattern;                //send new pattern to motor
            delay(Delay);                   //controls motor speed (fastest=1)
        }
    }
    else {// ----- clockwise scan
        for(int i=0; i<8; i++) {
            // ----- rotate motor to next step
            Pattern = PORTB;                //get current motor pattern
            Pattern = Pattern & B11110000;  //preserve MSN
            Pattern = Pattern | Motor[i];   //create new motor pattern
            PORTB = Pattern;                //send new pattern to motor
            delay(Delay);                   //controls motor speed (fastest=1)
        }
    }
}


void
motorOff() {
    PORTB = PORTB & B11110000;
}


void
home() {// find zero position for beam
    if(digitalRead(Micro_switch)) {// Keep off from limit
        moveClockwise = false;
        while(digitalRead(Micro_switch)) {
            rotate();
            delay(Delay);
        }
        for(int i=0; i<stepsPerRevolution/16; i++)
        rotate();
        delay(Delay);
    }
    
    delay(5000);
    // Now we are for sure out of the limit then
    // rotate clockwise until limit switch operates
    moveClockwise = true;
    int Step_counter = 4 * stepsPerRevolution;
    while(!digitalRead(Micro_switch)) {
        rotate();
        delay(Delay);
    }
    int nSteps = 0;
    for(nSteps = 0; nSteps<stepsPerRevolution/16; nSteps++) {
        rotate();
        delay(Delay);
    }
    while(!digitalRead(Micro_switch)) {
        rotate();
        delay(Delay);
        nSteps++;
    }
    stepsPerRevolution = nSteps;
    //Serial.print("True Step Number: "); 
    //Serial.println(stepsPerRevolution);
    delay(5000);
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
sendValues(byte Mark, int16_t value1, int16_t value2) {
    byte currentByte;
    // Prepare header and command
    outputString[0] = StartMarker;
    outputString[2] = Mark;
    
    int nBytes = 3;
    for(int i=0; i<sizeof(value1); i++) {
        currentByte = byte(value1 & 0xFF);
        if(currentByte >= SpecialByte) {
            outputString[nBytes++] = SpecialByte;
            outputString[nBytes++] = byte(currentByte-SpecialByte);
        }
        else {
            outputString[nBytes++] = currentByte;
        }
        value1 >>= 8;
    }
    for(int i=0; i<sizeof(value2); i++) {
        currentByte = byte(value2 & 0xFF);
        if(currentByte >= SpecialByte) {
            outputString[nBytes++] = SpecialByte;
            outputString[nBytes++] = byte(currentByte-SpecialByte);
        }
        else {
            outputString[nBytes++] = currentByte;
        }
        value2 >>= 8;
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


