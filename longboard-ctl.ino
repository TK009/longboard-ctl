/**
 * Main controller for an electronic longboard (long skateboard) project.
 * Inteded for Arduino Nano (v3.0).
 *
 * ESC control:     PWM            pins D9 (any pwm: D3, 5, 6, 9, 10, 11)
 * Accelerometer:   I2C (library)  pins A4 (SDA), A5 (SCL), D2 (INT2; Data ready interrupt)
 * Bluetooth:       Serial         pins RX0, TX1, 3V3
 * Battery cell status: Analog IN  pins A0..A7
 */

// Used for blinking on error
#define StatusLed 13
#define Throttle 9
#define DataReadyIntPin 2

// Bluetooth pins 0, 1, 3V3
#define Bluetooth Serial
// default
#define BluetoothSerialSpeed 9600
// serial config is default: 8, 1, n


// Pin that defines the address of accelerometer
#define SA0 1

#include <Wire.h>
#include <MMA8452.h>
//#include <MMA8452Reg.h>

// accuracy: +- 2, +- 4, +- 8
#define AccelerationRange MMA_RANGE_2G
#define AccelerationDataRate MMA_12_5hz
#define AccelerometerNewDataInterrupt 1
// Filter for 
#define AccelerometerSoftLPFilter 1



#if AccelerometerSoftLPFilter
#include <FilterOnePole.h>
#define LowpassFreq 0.005
auto lpFilter = FilterOnePole( LOWPASS, LowpassFreq );
#endif

// GLOBALS

// Accelerometer
auto mma = MMA8452();

// Zero velocity handling
float zeroOffset = 0;
unsigned int zeroResetCounter = 0;
#define ZeroResetCounterEps 0.001
const float ZeroResetDamping = 0.01;
const unsigned int MaxZeroCounter = 1.0 / ZeroResetDamping;

// Zero offset handling
const unsigned int CalibrationSamples = 100;

unsigned long newAccTime;
unsigned int dt;

#if AccelerometerNewDataInterrupt
// tells that accelerometer data is ready (with interrupt)
volatile bool accDataReady = false;
void setDataReady() {accDataReady = true;}
#endif

float lastAccValue = 0.0;
unsigned long lastAccTime = 0;
float xVelocity = 0.0;

// declarations
void setupBluetooth();
bool setupAccelerometer();
inline float getUnfilteredXAcc();
inline float getXAccSample();
void calibrateZeroVel();
float trapezoidalRule(float fa, float fb, int dt);



void setup() {
    pinMode(StatusLed, OUTPUT);
    digitalWrite(StatusLed, HIGH); // High until setup ends


    // Connect bluetooth serial
    setupBluetooth();
    
    // Connect i2c accelerometer
    setupAccelerometer();

    // get initial values
    lastAccTime = micros();
    float x, y, z;
    mma.getAcceleration(&x, &y, &z);
    lastAccValue = x;
#if AccelerometerSoftLPFilter
    lpFilter.input(x);
#endif

    delay(1000);
    calibrateZeroVel();

    digitalWrite(StatusLed, LOW);
}

// MAINLOOP
void loop() {
    float x = getXAccSample();

    xVelocity += trapezoidalRule(lastAccValue, x, dt);
    // damping factor
    xVelocity += -xVelocity * zeroResetCounter * ZeroResetDamping;

    // Update globals
    lastAccTime = newAccTime;
    lastAccValue = x;

    Bluetooth.println(xVelocity*1000);
    
}
inline float getUnfilteredXAcc() {
    float x, y, z;
    
#if AccelerometerNewDataInterrupt
    if (accDataReady) Bluetooth.println("DToo fast accelerometer");
    while (!accDataReady) {};
    accDataReady = false;
#endif
    newAccTime = micros();

    // micros overflow or not
    dt = (newAccTime < lastAccTime) ?
        (newAccTime + 0x7FFFFFFFL) - (lastAccTime & 0x7FFFFFFFL):
        newAccTime - lastAccTime;

    mma.getAcceleration(&x, &y, &z);
    x += zeroOffset;
    return x;
}

inline float getXAccSample() {
    float x = getUnfilteredXAcc();

    if (abs(x) < ZeroResetCounterEps)
        zeroResetCounter < MaxZeroCounter ? ++zeroResetCounter : 0 ;
    else
        zeroResetCounter = (float) zeroResetCounter * 0.4;

    Bluetooth.print('E');
    Bluetooth.print(x*1000);
    Bluetooth.print(',');
#if AccelerometerSoftLPFilter
    lpFilter.input(x);
    x = lpFilter.output();
#endif
    Bluetooth.print(x*1000);
    Bluetooth.print(',');
    return x;
}


// Stop and blink when error occurs
void errorStop() {
    while (1) {
        digitalWrite(StatusLed, LOW);
        delay(1000);
        digitalWrite(StatusLed, HIGH);
        delay(1000);
    }
}

bool setupAccelerometer() {
    Wire.begin();

    Serial.println("DTrying to connect accelerometer...");
    if (! mma.init()) {
        Serial.println("DCouldnt connect accelerometer!");
        errorStop();
    }
    Serial.println("DAccelerometer connected.");
    mma.setActive(false); // changing settings might need

    mma.setRange(AccelerationRange);
    mma.setDataRate(AccelerationDataRate);

    mma.setLowNoiseMode(true);
    mma.setPowerMode(MMA_HIGH_RESOLUTION);

    mma.setHighPassFilter(true, MMA_HP3); // Defaults (?) to highest cutoff: 16Hz

#if AccelerometerNewDataInterrupt
    // data ready interrupt
    mma.setInterruptsEnabled(MMA_DATA_READY);
    // all defaults to INT2
    attachInterrupt(digitalPinToInterrupt(DataReadyIntPin), setDataReady, FALLING);
#endif

    mma.setActive(true); // settings done
    Serial.println("DAccelerometer active.");
    return true;
}

void setupBluetooth() {
    Bluetooth.begin(BluetoothSerialSpeed);
    Bluetooth.println("DBluetooth module active.");
}

void calibrateZeroVel() {
    Bluetooth.println("DCalibrating zero-speed");
    
    // avarage
    float sum = 0;
    for (unsigned i = 0; i < CalibrationSamples; ++i) {
        sum += getUnfilteredXAcc();
    }
    zeroOffset = -sum / (float) CalibrationSamples;
    Bluetooth.print("DCalibrated: ");
    Bluetooth.println(zeroOffset*1000);
}

float trapezoidalRule(float fa, float fb, int dt) {
    return ((float) dt) * (fa + fb) * 0.5;
}


