  
#include <Servo.h>
#include <NewPing.h> //  SR04 Ultrasonic Sensor
  
  
//////////////////////////////////////////////////////////
// Fuction prototypes
//////////////////////////////////////////////////////////

void serialEvent();
bool executeCommand();
void decodeCommand(byte *inputString, byte *decodedString);
void errorSignal(int16_t nBeep);
void sendValue(byte Mark, int16_t value);

  
// Possible commands reveived from Serial Line
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

#define TRIGGER_PIN  12
#define ECHO_PIN     11
#define SERVO_PIN     9
#define MAX_DISTANCE 400

// On Nano: LED_BUILTIN  = D13 
Servo   myservo;// create servo object to control a servo
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

bool          connected = false;// Are we connected to the PC ?
byte          command;


const int     maxMessageLen  = 64;
byte          inputString[64];
byte          outputString[64];
byte          decodedString[64];
int           decodedLength;
int           nReceivedChar;
int           commandLength;

boolean       inProgress     = false;
boolean       startFound     = false;
boolean       allReceived    = false;

const long    baudRate       = 115200;

volatile bool bSendValues    = false;

const int16_t minPWM = 600;
const int16_t maxPWM = 2400;

int16_t       pos            = 0;        // variable to store the servo position
const int16_t startA         = 0;//5;        // real motor zero
const int16_t endA           = 180;//170;      // real motor 180°
const int16_t aStep          = 5;        // Angluar Step (Sensor Aperture ~15°)
const int     nAvg           = 9;        // Number of measurements per angluar step
const int     half           = nAvg/2;
float*        dist           = new float(nAvg);
const int     servoDelay     = 10;
const int     echoDelay      = 40;
float         tmp;



void setup() {
    Serial.begin(baudRate, SERIAL_8N1); // initialize serial: 8 data bits, no parity, one stop bit.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    myservo.attach(SERVO_PIN, minPWM, maxPWM);// attaches the servo on pin 9 to the servo object
    myservo.write(startA);    // tell servo to go to zero position
    delay(1500);              // wait until servo reach the position
    // Wait until connected to a PC
    while(!connected) {
        serialEvent();
        if(allReceived) {
            executeCommand();
            allReceived = false;
        }
        sonar.ping_cm();
        delay(100);
        digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ^ 1);
    }
    digitalWrite(LED_BUILTIN, LOW);
}


void loop() {
    for(pos=startA; pos<endA; pos += aStep) { // going from startA degrees to endA degrees
        serialEvent();
        if(allReceived) {
            executeCommand();
            allReceived = false;
        }
        // in steps of 1 degree
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(servoDelay);
        sendValue(AngleMark, pos);
        //delay(echoDelay);
        //sendValue(DistMark, int16_t(sonar.ping_cm()));
        for(int i=0; i<nAvg; i++) {
            delay(echoDelay);
            *(dist+i) = sonar.ping_cm(300);
        }
        for(int i=0; i<nAvg; i++)
            for(int j=0; j<nAvg-i-1; j++)
                if(*(dist+j) > *(dist+j+1)) {
                    tmp = *(dist+j+1);
                    *(dist+j+1) = *(dist+j);
                    *(dist+j) = tmp;
                }
        sendValue(DistMark, int16_t(*(dist+half)+0.5));
        /*
        for(int i=0; i<nAvg; i++) {
            Serial.print(*(dist+i));
            Serial.print("\t");
        }
        */
    }
    for(pos=endA; pos>startA; pos -= aStep) { // goes from endA degrees to startA degrees
        serialEvent();
        if(allReceived) {
            executeCommand();
            allReceived = false;
        }
        // in steps of 1 degree
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(servoDelay);
        sendValue(AngleMark, pos);
        //delay(echoDelay);
        //sendValue(DistMark, int16_t(sonar.ping_cm()));
        for(int i=0; i<nAvg; i++) {
            delay(echoDelay);
            dist[i] = sonar.ping_cm(300);
        }
        for(int i=0; i<nAvg; i++)
            for(int j=0; j<nAvg-i-1; j++)
                if(*(dist+j) > *(dist+j+1)) {
                    tmp = *(dist+j+1);
                    *(dist+j+1) = *(dist+j);
                    *(dist+j) = tmp;
                }
        sendValue(DistMark, int16_t(*(dist+half)+0.5));
        /*
        for(int i=0; i<nAvg; i++) {
            Serial.print(dist[i]);
            Serial.print("\t");
        }
        */
    }
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
errorSignal(int iErr) {
    for(int i=0; i<iErr; i++) {
        digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ^ 1);
        delay(500);
    }
    digitalWrite(LED_BUILTIN, LOW);
}

