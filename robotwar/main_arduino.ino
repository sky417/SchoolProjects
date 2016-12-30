/*
 *	EECE 375/474 
 *	Robot algorithm code 
 *	Written by Andrew Yoon
 */

#include <Servo.h>
#include <math.h>
#include <Wire.h>

/* I2C dev library from http://www.i2cdevlib.com/ 
 * I2Cdev.h, BMA150.h, ITG3200.h
 */
#include <I2Cdev.h>
#include <BMA150.h>
#include <ITG3200.h>

#define NUM_PWMOUTPUTS 4
#define DATA_UPDATE_INTERVAL 200
#define DEBUGMODE 0

/* weapon constants */
#define WEAPON 5
#define WEAPON_MAX 50
#define WEAPON_MIN 10
#define WEAPON_PERIOD 400 // in ms

/* wheel constants */
#define WHEEL_LEFT_FORWARD 9
#define WHEEL_LEFT_BACKWARD 11
#define WHEEL_RIGHT_FORWARD 10
#define WHEEL_RIGHT_BACKWARD 12
#define WHEEL_FAST 100
#define WHEEL_SLOW 60
#define FIND_LIMIT 1500
#define OFF_FIND_LIMIT 1200

/* reflective sensor constants */
#define BOTTOMSENSOR A1
#define BOUNDARY 800

/* proximity sensor constants */
#define PROX_LEFT_LONG A2
#define PROX_LEFT_SHORT A3
#define PROX_RIGHT_LONG A4
#define PROX_RIGHT_SHORT A5
#define PROX_LONG_MAX 430
#define PROX_LONG_MIN 60
#define PROX_LONG_THRESHOLD 100
#define PROX_SHORT_MAX 200
#define PROX_SHORT_MIN 110
#define PROX_SHORT_CONTACT 500
#define PROX_SHORT_THRESHOLD 70
#define MOVING_THRESHOLD 70
#define PROX_SAMPLE_RATE 200

/* accel-gyro sensor constants */
#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2
#define NUM_CALIB 500
#define SENSOR_STD_INTERVAL 75
#define MOVING_AVG_NUM 7

// 14.375  LSBs per deg/sec
ITG3200 gyro;
#define GYRO_SENSITIVITY 14.375

// 2g range : 246~266 LSB / g
// using 261 LSB/g since using 3.3V in 2.4~3.6V range, 246+20*(.9/1.2) = 261
// 9.8 m/s2 per 261 LSB => 1 m/s2 per 261/9.8 = 26.632653061 LSBs
// or .037547893 m/s2 per LSB
BMA150 accel;
#define ACCEL_SENSITIVITY 0.037547893
#define ACC_THRESHOLD 5

const int analogSensors[10] = {
    A0, A1, A2, A3, A4, A5, A6, A7, A8, A9};
const int PWMports[NUM_PWMOUTPUTS] = {
    WHEEL_LEFT_FORWARD, 
    WHEEL_LEFT_BACKWARD, 
    WHEEL_RIGHT_FORWARD, 
    WHEEL_RIGHT_BACKWARD};
int PWMvalues[NUM_PWMOUTPUTS] = {
    0, 0, 0, 0};

/* operation related variables */
boolean autoMode = false; /* auto mode? */
boolean escaping = false;
int currentmove = 0;
int prevmove = 0; // test
int movingLeft_long = 0;
int movingRight_long = 0;
int movingLeft_short = 0;
int movingRight_short = 0;
//boolean escapeLeft = false;
boolean backingup = false;

/* opponent tracking */
int opp_dist = 0; // 0 not found, 1 far, 2 close, 3 in-range
int opp_status_lock = 0;

/* command handling variables */
String inputString = "";
boolean stringComplete = false;

/* RPM encoder related variables 
 * Serial1: left wheel
 * Serial2: right wheel
 */
String inputStringRW = "";
boolean stringCompleteRW = false;
String inputStringLW = "";
boolean stringCompleteLW = false;
long wheelLeftDist = 0;
long wheelLeftDistRst = 0;
long wheelRightDist = 0;
long wheelRightDistRst = 0;
boolean wheelInit = false;

/* weapon related variables */
Servo myservo;
int weaponStatus = 0; // 0 not operating, 1 going down, 2 going up
int weaponLastChanged = 0;

/* accel-gyro related variables */
int acc[3] = {
    0, 0, 0};
int acc_prev[3] = {
    0, 0, 0};
int gyr[3] = {
    0, 0, 0};
float accelDist[3] = {
    0, 0, 0};
float velocity[3] = {
    0, 0, 0};
int acc_zero[3] = {
    0, 0, 0};
int gyr_zero[3] = {
    0, 0, 0};
float angle = 0;
float gyroAngle_Z = 0;
unsigned long lastSensorUpdate = 0;
int lastLoopTime = SENSOR_STD_INTERVAL;

long lastSent = 0;
long findCounter = 0;

/* main() */
void setup() {
    /* start serial communications */
    Serial.begin(38400);
    Serial1.begin(38400);
    Serial2.begin(38400);
    
    /* set PWM output pins */
    int i;
    for (i=0; i<NUM_PWMOUTPUTS; i++) {
        pinMode(PWMports[i], OUTPUT);
    }
    
    /* initialize servo motor */
    myservo.attach(WEAPON);
    myservo.write(WEAPON_MIN); // set first position 
    
    /* initialize sensors */
    Wire.begin(); // I2Cdev library doesn't do this automatically
    Serial.begin(38400); // 38400 is chosen due to it works well at 8MHz as it does at 16MHz
    Serial.println("Initializing I2C devices...");
    accel.initialize();   
    gyro.initialize();
    Serial.println("Testing device connections..."); // verify connection
    Serial.println(accel.testConnection() ? "BMA150 connection successful" : "BMA150 connection failed");
    Serial.println(gyro.testConnection() ? "ITG3200 connection successful" : "ITG3200 connection failed");
    calibrateSensors();	
    
    /* initialize RPM encoder */
    delay(1000);
    Serial2.print("V\n"); // using distance only
    Serial1.print("V\n");  
    
    Serial.println("All initialization finished");
    Serial.print("Start loop()\n\n");
}
void loop() {
    
    if (millis() - lastSent >= DATA_UPDATE_INTERVAL) {
        if (DEBUGMODE == 0) outputstatus();
        else if (DEBUGMODE == 1) outputDebug();
        lastSent = millis();
    }
    
    /* wheel distance initialize */
    if (!wheelInit) {
        // wheel distance variable reset
        resetWheelDist();
        wheelInit = true;
    }
    
    /* Serial communications with PC*/
    if (stringComplete) {
        // rx/tx with PC
        processInputString();
        inputString = "";
        stringComplete = false;
    }
    
    /* Serial communications with RPM encoders */
    if (stringCompleteRW) {
        // rx/tx with right wheel
        if (inputStringRW.charAt(0) == 'D')
            processRWData();
        //Serial.print("wrec");
        inputStringRW = "";
        stringCompleteRW = false; 
    }  
    if (stringCompleteLW) {
        // rx/tx with left wheel
        if (inputStringLW.charAt(0) == 'D')
            processLWData();
        //Serial.print("wrec");
        inputStringLW = "";
        stringCompleteLW = false; 
    }
    
    /* auto mode operations */
    if (autoMode == true) {
        /* escaping from boundary routine */
        if (analogRead(BOTTOMSENSOR) > BOUNDARY) escaping = true;
        else {
            if (escaping == true) doneEscaping();
            escaping = false;
            op_normal();
        }
        
        if (escaping == true) {
            /* run escape routine, do nothing else */
            op_escape();
        }
        else {
            /* weapon code here */
            
            if (weaponStatus != 0) op_weapon();
            else myservo.write(WEAPON_MIN);
        }
    }
    else {
        /* manual mode, do nothing */
        
    }
    
    /* accel-gyro operations */
    int sensorInterval = millis() - lastSensorUpdate;
    if (sensorInterval > SENSOR_STD_INTERVAL) {
        lastSensorUpdate = millis();
        lastLoopTime = sensorInterval;
        
        op_gyro();
    }
    
}

/* robot operations */
void op_normal() {
    int left_long, right_long;
    int left_short, right_short;
    int mode;
    
    /* set sampling rate to PROX_SAMPLE_RATE */
    if (millis() - opp_status_lock < PROX_SAMPLE_RATE) return;
    else opp_status_lock = millis();
    
    /* read the long range values first */
    left_long = analogRead(PROX_LEFT_LONG);
    right_long = analogRead(PROX_RIGHT_LONG);
    left_short = analogRead(PROX_LEFT_SHORT);
    right_short = analogRead(PROX_RIGHT_SHORT);
    
    /* find the opponent */
    if (left_long < PROX_LONG_MIN && right_long < PROX_LONG_MIN) {
        
        /* opponent not found */
        if (weaponStatus > 0) weaponStatus = 0; // turn off weapon
        
        op_find();
        opp_dist = 0;
    }
    else if (left_long < PROX_LONG_MAX && right_long < PROX_LONG_MAX) {
        
        if (left_short > PROX_SHORT_CONTACT || right_short > PROX_SHORT_CONTACT) {
            
            /* error area: both sensors reading.. opponent is actually at in-range */
            if (weaponStatus == 0) weaponStatus = 1; // turn on the weapon
            
            if (left_short < PROX_SHORT_MAX) mode = 2; // right
            else if (right_short < PROX_SHORT_MAX) mode = 1; // left
            else mode = 0; // FRONT
            
            op_inrange(mode);
            opp_dist = 3;
        }
        else {
            /* opponent is at far range */
            if (((left_long + right_long) > 350) && (weaponStatus == 0)) weaponStatus = 1;
            else if (((left_long + right_long) < 350) && (weaponStatus > 0)) weaponStatus = 0;
            
            if (abs(left_long - right_long) < PROX_LONG_THRESHOLD) mode = 0; // front
            else {
                if (left_long > right_long) mode = 1; // left
                else if (left_long < right_long) mode = 2; // right
            }
            op_far(mode);
            opp_dist = 1;
        }
    }
    else if (left_short < PROX_SHORT_MAX && right_short < PROX_SHORT_MAX) {
        
        /* opponent is at close range */
        if (weaponStatus == 0) weaponStatus = 1; // turn on the weapon
        
        if (abs(left_short - right_short) < PROX_SHORT_THRESHOLD) mode = 0; // front
        else {
            if (left_short > right_short) mode = 1; // left
            else if (left_short < right_short) mode = 2; // right
        }
        op_close(mode);
        opp_dist = 2;
    }
    else if (left_short > PROX_SHORT_MAX || right_short > PROX_SHORT_MAX) {
        
        /* opponent is at in-range */
        if (weaponStatus == 0) weaponStatus = 1; // turn on the weapon
        
        if (left_short < PROX_SHORT_MAX) mode = 2; // right
        else if (right_short < PROX_SHORT_MAX) mode = 1; // left
        else mode = 0; // FRONT
        
        op_inrange(mode);
        opp_dist = 3;
    }
}
void op_find() {
	backingup = false;
    if (findCounter < FIND_LIMIT) {
        findCounter++;
        // rotate left until discovery 
        moveRobot(0, WHEEL_SLOW, WHEEL_SLOW, 0);
    }
    
    else if (findCounter < (FIND_LIMIT + OFF_FIND_LIMIT)) {
        findCounter++;
        // go straight
        moveRobot(WHEEL_FAST, 0, WHEEL_FAST, 0);
    }
    
    else findCounter = 0;
    
    currentmove = 0;
}
void op_far(int mode) {
	backingup = false;
    if (mode == 1) {
        /* opponent is on the left, turn left gradually */
        moveRobot(WHEEL_SLOW, 0, WHEEL_FAST, 0);
        currentmove = 1;
    }
    else if (mode == 2) {
        /* opponent is on the right, turn right gradually */
        moveRobot(WHEEL_FAST, 0, WHEEL_SLOW, 0);
        currentmove = 2;
    }
    else if (mode == 0) {
        /* opponent is straight ahead */
        moveRobot(WHEEL_FAST, 0, WHEEL_FAST, 0);
        currentmove = 3;
    }
}
void op_close(int mode) {
	backingup = false;
    if (mode == 1) {
        /* opponent is on the left, rotate left */
        moveRobot(0, WHEEL_SLOW, WHEEL_SLOW, 0);
        currentmove = 4;
    }
    else if (mode == 2) {
        /* opponent is on the right, rotate right */
        moveRobot(WHEEL_SLOW, 0, 0, WHEEL_SLOW);
        currentmove = 5;
    }
    else if (mode == 0) {
        /* opponent is straight ahead, stop */
        moveRobot(0, 0, 0, 0);
        currentmove = 6;
    }
}
void op_inrange(int mode) {
	backingup = true;
    if (mode == 1) {
        /* opponent is very close on left, retreat & turn left */
        moveRobot(0, WHEEL_FAST, 0, WHEEL_SLOW);
        currentmove = 7;
    }
    else if (mode == 2) {
        /* opponent is very close on right, retreat & turn right */
        moveRobot(0, WHEEL_SLOW, 0, WHEEL_FAST);
        currentmove = 8;
    }
    else if (mode == 0) {
        /* opponent is within the range of weapon, move straight backward */
        moveRobot(0, WHEEL_FAST, 0, WHEEL_FAST);
        currentmove = 9;
    }
}
void op_escape() {
    /* 
     * rotate to left until A0 < BOUNDARY
     * left wheel backward, right wheel forward 
     */
    currentmove = 10;
    if (backingup) {
        // robot backed into the boundary, go straight until the sensor is out of boundary
        moveRobot(WHEEL_FAST, 0, WHEEL_FAST, 0);
    }
    else {
        // normal boundary escape
        moveRobot(0, WHEEL_FAST, WHEEL_FAST, 0);
	}
}
void doneEscaping() {
    /* stop the robot, and resume the normal operation */
    moveRobot(0, 0, 0, 0);
}
void op_weapon() {
    /* Normal operation */
    int currentTime = millis();
    if ((currentTime - weaponLastChanged) > WEAPON_PERIOD) {
        if (weaponStatus == 1) {
            weaponStatus = 2;
            myservo.write(WEAPON_MAX);
        }
        else if (weaponStatus == 2) {
            weaponStatus = 1;
            myservo.write(WEAPON_MIN);
        }		
        weaponLastChanged = currentTime;
    }
}
void op_gyro() {
    
    updateSensors();
    float gyroRate_Z = getGyroRate(Z_AXIS);
    int sensorInterval = millis() - lastSensorUpdate;
    
    gyroAngle_Z = gyroAngle_Z + (getGyroAngle(gyroRate_Z));
    
    // keep the value within 0~360 degree
    if (gyroAngle_Z > 360) gyroAngle_Z -= 360;
    if (gyroAngle_Z < 0) gyroAngle_Z += 360;
    
    angle = gyroAngle_Z;
}

/* read functions */
void readData() {
    char type = inputString.charAt(1);
    int port = stoi(inputString.substring(2,4));
    if (type == 'a' || type == 'A') {
        // Analog
        readAnalog(port);
    }
    else if (type == 'd' || type == 'D') {
        // Digital
        readDigital(port);
    }
    else if (type == 'g' || type == 'G') {
        // gyroscope
        readGyro();
    }
    else if (type == 'c' || type == 'C') {
        // accelerometer
        // not used
    }
    else if (type == 'w' || type == 'W') {
        // wheel value
        readWheel(); 
    }
    else if (type == 'r' || type == 'R') {
        // weapon
        readWeapon();
    }
    else if (type == 'm' || type == 'M') {
        // current mode
        readAuto();
    }
    else if (type == 's' || type == 'S') {
        // stop the robot
        moveRobot(0, 0, 0, 0); 
    }
    else {
        // wrong command
        sendError(2, 0);
        return;
    }
}
void readAnalog(int port) {
    int value = analogRead(analogSensors[port]);
    
    sendResult('A', port, value);
}
void readDigital(int port) {
    int portIndex = -1;
    portIndex = findPWMPortIndex(port);
    if (portIndex == -1) {
        // error finding port index
        sendError(3, 0);
    }
    else {
        sendResult('D', port, PWMvalues[portIndex]);
    }
}
void readGyro() { 
    sendResult('G', 0, (int)angle);
}
void readAccel() {
    
}
void readWheel() {
    if (inputString.charAt(2) == 'r' || inputString.charAt(2) == 'R') {
        Serial.println("Resetting distance...");
        resetWheelDist();
    }
    else {
        sendResult('W', 1, wheelRightDist - wheelRightDistRst);
        sendResult('W', 0, wheelLeftDist - wheelLeftDistRst);
    }
    
    resetWheelDist();
}
void readWeapon() {
    // weaponStatus 0 not moving, 1 going up, 2 going down
    sendResult('R', 0, weaponStatus);
}
void readAuto() {
    if (autoMode == true) sendResult('M', 0, 1);
    else sendResult('M', 0, 0);
}

/* write functions */
void writeData() {
    // writeData is for digital only
    int port = stoi(inputString.substring(1,3));
    int data = stoi(inputString.substring(3,6));
    
    if (port == 99) {
        /* auto/manual mode setting */
        // w99000 = manual mode
        // w99001 = auto mode
        writeAuto(data);
    }
    else if (port == 5) {
        /* weapon command */
        writeWeapon(data);
    }
    else if (port >= 9 && port <= 12) {
        /* wheel command */
        writePWMData(port, data);
        readDigital(port);
    }
    else {
        // wrong command
        sendError(2, 0);
        return;
    }
}
void writePWMData(int port, int data) {
    
    int outputValue = 0;
    int portIndex = -1;
    
    portIndex = findPWMPortIndex(port);
    
    if (portIndex == -1) {
        // wrong port
        sendError(3, 0);
        return;
    }
    else if (data < 0 || data > 100) {
        // data is corrupted
        // do nothing
        sendError(4, 0);
        return;
    }
    
    else {
        PWMvalues[portIndex] = data;
        outputValue = map(data, 0, 100, 0, 255);
        analogWrite(port, outputValue);
    }
}
void writeWeapon(int deg) {
    // manually move the weapon
    myservo.write(deg);
    readWeapon();
}
void writeAuto(int mode) {
    if (mode == 1) {
        // change to auto mode, reset the distance from RPM encoders
        autoMode = true;
        if (!wheelInit) {
            resetWheelDist();
            wheelInit = true;
        }
    }
    else if (mode == 0) {
        autoMode = false;
        wheelInit = false;
        moveRobot(0, 0, 0, 0); // stop the robot
    }
    else {
        /* wrong mode received */
        sendError(99, 0);
    }
    readAuto();
}

/* serial event handlers */
void serialEvent() {
    while (Serial.available()) {
        char inChar = (char)Serial.read();
        inputString += inChar;
        
        if (inChar == '\n') {
            stringComplete = true;
        }
    } 
}
void serialEvent1() {
    while (Serial1.available()) {
        char inChar1 = (char)Serial1.read();
        inputStringRW += inChar1;
        
        if (inChar1 == '\n') {
            stringCompleteRW = true;
        }
    } 
}
void serialEvent2() {
    while (Serial2.available()) {
        char inChar2 = (char)Serial2.read();
        inputStringLW += inChar2;
        
        if (inChar2 == '\n') {
            stringCompleteLW = true;
        }
    } 
}

/* serial data process */
void processRWData() {
    wheelRightDist = (-1 * htoi(inputStringRW.substring(1,9))) / 6; 
}
void processLWData() {
    wheelLeftDist = (htoi(inputStringLW.substring(1,9))) / 6; 
}
void processInputString() {
    /* READ command is 5 bytes, WRITE command is 7 bytes */
    char command = inputString.charAt(0);
    
    if (command == 'r' || command == 'R') {
        if (inputString.length() != 5) {
            /* wrong command, ignore and terminate */
            //Serial.println("incorrect command, ignored");
            sendError(0, 0);
            return;
        }
        
        // Read command
        readData();
    }
    
    else if (command == 'w' || command == 'W') {
        if (inputString.length() != 7) {
            /* wrong command, ignore and terminate */
            //Serial.println("incorrect command, ignored");
            sendError(0, 0);
            return;
        }
        
        writeData();
    }
    else {
        // wrong command
        sendError(1, 0);
        return;
    }
}

/* accel-gyro handle functions */
void calibrateSensors() {                 
    // Set zero sensor values
    long sum[6] = {
        0, 0, 0, 0, 0, 0        };
    
    Serial.println("Start accel-gyro sensors init");
    
    /* get rid of first 50 values */
    for (int i=0; i<50; i++) {
        gyro.getRotation(&gyr[0], &gyr[1], &gyr[2]);
    }
    
    gyr[0] = 0;
    gyr[1] = 0;
    gyr[2] = 0;
    
    for (int i=0; i<NUM_CALIB; i++) {
        // average of first NUM_CALIB values
        gyro.getRotation(&gyr[0], &gyr[1], &gyr[2]);
        
        for (int j=0; j<3; j++) {
            sum[j+3] += gyr[j];
        }
    }
    
    for (int i=0; i<3; i++) {
        gyr_zero[i] = sum[i+3] / NUM_CALIB;
    }
    Serial.println("accel-gyro sensors init finished");
    
    // used sum[6] to use accel/gyro together, but not actually used
}
void updateSensors() {
    long gyr_raw[3] = { 
        0, 0, 0   };
    long gyr_raw_max[3] = { 
        0, 0, 0   };
    long gyr_raw_min[3] = { 
        600, 600, 600   };
    
    
    int tmp[6];
    for (int i=0; i<MOVING_AVG_NUM; i++) {
        //    accel.getAcceleration(&tmp[0], &tmp[1], &tmp[2]);
        gyro.getRotation(&tmp[3], &tmp[4], &tmp[5]);
        for (int j=0; j<3; j++) {
            //      acc_raw[j] += tmp[j];
            gyr_raw[j] += tmp[j+3];
            if (tmp[j+3] > gyr_raw_max[j]) gyr_raw_max[j] = tmp[j+3];
            else if (tmp[j+3] < gyr_raw_min[j]) gyr_raw_min[j] = tmp[j+3];
        }
    }
    
    for (int i=0; i<3; i++) {
        /* get rid of max, min value */
        gyr_raw[i] -= gyr_raw_max[i];
        gyr_raw[i] -= gyr_raw_min[i];
    }
    
    for (int i=0; i<3; i++) {
        //    acc[i] = acc_raw[i]/MOVING_AVG_NUM - acc_zero[i];
        gyr[i] = gyr_raw[i]/(MOVING_AVG_NUM - 2) - gyr_zero[i];
    }
}
float getGyroRate(int axis) {
    return (float)gyr[axis] / GYRO_SENSITIVITY;
}
float getGyroAngle(float rate) {
    
    return rate * lastLoopTime / 1000.0;
}

/* send result or error to PC via serial */
void sendResult(char type, int port, int value) {
    Serial.print(type);
    if (port < 10) Serial.print("0");
    Serial.print(port);  
    if (value < 1000) Serial.print("0");
    if (value < 100) Serial.print("0");
    if (value < 10) Serial.print("0");
    Serial.print(value);
    Serial.println(",");
}
void sendError(int code, int value) {
    Serial.print("E");
    if (code < 10) Serial.print("0");
    Serial.print(code); 
    if (value < 1000) Serial.print("0");
    if (value < 100) Serial.print("0");
    if (value < 10) Serial.print("0");
    Serial.println(value); 
}

/* helper functions */
int stoi(String text) {
	// convert String into integer
    char temp[5];
    text.toCharArray(temp, 4);
    int x = atoi(temp);
    return x;
}  
long htoi(String text) {
	// convert hexadecimal String into integer
    int length = text.length();
    int i;
    long result = 0;
    for (i=0; i<length; i++) {
        result *= 16;
        char c = text.charAt(i);
        if (c >= '0' && c <= '9') {
            result += c - '0';
        } 
        else if (c >= 'a' && c <= 'f') {
            result += c - 'a' + 10;
        } 
        else if (c >= 'A' && c <= 'F') {
            result += c - 'A' + 10;
        }  
    }
    
    return result;
}
int findPWMPortIndex(int port) {
	// find the index of the specified port from the list of PWM pins
    int i;
    for (i=0; i<NUM_PWMOUTPUTS; i++) {
        if (PWMports[i] == port) return i; 
    }
    
    // error finding port
    return -1;
}
void moveRobot(int leftFor, int leftBack, int rightFor, int rightBack) {
	// move the robot
    writePWMData(WHEEL_LEFT_FORWARD, leftFor);
    writePWMData(WHEEL_LEFT_BACKWARD, leftBack);
    writePWMData(WHEEL_RIGHT_FORWARD, rightFor);
    writePWMData(WHEEL_RIGHT_BACKWARD, rightBack);
    
}
void resetWheelDist() {
    wheelRightDistRst = wheelRightDist; 
    wheelLeftDistRst = wheelLeftDist;
}

/* output functions to serial */
void outputstatus() {
    readAnalog(1);
    readAnalog(2);
    readAnalog(3);
    readAnalog(4);
    readAnalog(5);
    readDigital(9);
    readDigital(10);
    readDigital(11);
    readDigital(12);
    readGyro();
    readWeapon();
    readWheel();
}
void outputDebug() {
	// used in debug mode only
    String outStr;
    switch (currentmove) {
        case 0:
            outStr = "NF";
            break;
        case 1:
            outStr = "FL";
            break;
        case 2:
            outStr = "FR";
            break;
        case 3:
            outStr = "FF";
            break;
        case 4:
            outStr = "CL";
            break;
        case 5:
            outStr = "CR";
            break;
        case 6:
            outStr = "CF";
            break;
        case 7:
            outStr = "IL";
            break;
        case 8:
            outStr = "IR";
            break;
        case 9:
            outStr = "IF";
            break;
        case 10:
            outStr = "BD";
            break;
    }
    
    Serial.print(outStr);
    Serial.print("\tgy:");
    Serial.print(angle);
    Serial.print("\tPWM:\t");
    Serial.print(PWMvalues[0]);
    Serial.print("\t");
    Serial.print(PWMvalues[1]);
    Serial.print("\t");
    Serial.print(PWMvalues[2]);
    Serial.print("\t");
    Serial.print(PWMvalues[3]);
    Serial.print("\tAnalog:\t");
    Serial.print(analogRead(A2));
    Serial.print("\t");
    Serial.print(analogRead(A3));
    Serial.print("\t");
    Serial.print(analogRead(A4));
    Serial.print("\t");
    Serial.println(analogRead(A5));
    
    //prevmove = currentmove;
}


