/**
 * Main controller for an electronic longboard (long skateboard) project.
 * Inteded for Arduino Nano (v3.0).
 *
 * ESC control:     PWM            pins D9 (any pwm: D3, 5, 6, 9, 10, 11)
 * Accelerometer:   I2C (library)  pins A4 (SDA), A5 (SCL), D2 (INT2; Data ready interrupt)
 * Bluetooth:       Serial         pins RX0, TX1, 3V3
 * Battery cell status: Analog IN  pins A0..A7
 */

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
#define AccelerationDataRate MMA_100hz


// Used for blinking on error
#define StatusLed 13


// GLOBALS

// Accelerometer
MMA8452 mma = MMA8452();

// tells that accelerometer data is ready
volatile bool accDataReady = false;

float lastAccValue = 0.0;
unsigned long lastAccTime = 0;
float xVelocity = 0.0;

// declarations
void setupBluetooth();
bool setupAccelerometer();
void calibrateZeroVel();
void setDataReady() {accDataReady = true;}
float trapezoidalRule(float fa, float fb, int dt);
//void sendBar(float val, float maxVal);


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

    digitalWrite(StatusLed, LOW);
}

// MAINLOOP
void loop() {
    float x, y, z;
    unsigned long newAccTime;
    
    //if (accDataReady) Bluetooth.println("DToo fast accelerometer");
    //while (!accDataReady) {};
    //accDataReady = false;
    newAccTime = micros();

    // micros overflow or not
    unsigned long dt = (newAccTime < lastAccTime) ?
        (newAccTime + 0x7FFFFFFF) - (lastAccTime & 0x7FFFFFFF):
        newAccTime - lastAccTime;

    mma.getAcceleration(&x, &y, &z);

    xVelocity += trapezoidalRule(lastAccValue, x, dt);

    // Update globals
    lastAccTime = newAccTime;
    lastAccValue = x;

    Bluetooth.print('E');
    Bluetooth.print(dt);
    Bluetooth.print(',');
    Bluetooth.print(xVelocity*1000);
    Bluetooth.print(',');
    Bluetooth.println(x*1000);
    
    //if (abs(x) > 0.05) {
    //    Serial.println(x);
        //Serial.print(",");
        //Serial.print(y);
        //Serial.print(",");
        //Serial.println(z);
    //}
    //Serial.print("A");
    //Serial.println((short)(x * 1000));
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

    mma.setHighPassFilter(true); // Defaults (?) to highest cutoff: 16Hz

    // data ready interrupt
    mma.setInterruptsEnabled(MMA_DATA_READY);
    // all defaults to INT2
    //attachInterrupt(digitalPinToInterrupt(DataReadyIntPin), setDataReady, FALLING);

    mma.setActive(true); // settings done
    Serial.println("DAccelerometer active.");
    return true;
}

void setupBluetooth() {
    Bluetooth.begin(BluetoothSerialSpeed);
    Bluetooth.println("DBluetooth module active.");
}

void calibrateZeroVel() {

}

float trapezoidalRule(float fa, float fb, int dt) {
    return (float) dt * fa * fb * 0.5;
}

//float rk4(float(*f)(float, float), float dt, float t, float y)
//{
//    float  k1 = dt * f(t, y),
//            k2 = dt * f(t + dt / 2, y + k1 / 2),
//            k3 = dt * f(t + dt / 2, y + k2 / 2),
//            k4 = dt * f(t + dt, y + k3);
//    return y + (k1 + 2 * k2 + 2 * k3 + k4) / 6;
//
//}

//sendBar(float val, float maxVal) {
//    
//    for (int i; i < )
//    Bluetooth.println();
//}

