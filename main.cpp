/**
 * Main controller for an electronic longboard (long skateboard) project.
 * Inteded for Arduino Nano (v3.0).
 *
 * ESC control:     PWM            pins D9 (any pwm: D3, 5, 6, 9, 10, 11)
 * Accelerometer:   I2C (library)  pins A4 (SDA), A5 (SCL)
 * Bluetooth:       Serial         pins RX0, TX1, 3V3
 */

#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>


// Bluetooth pins 0, 1, 3V3
#define Bluetooth Serial
// default
#define BluetoothSerialSpeed 9600
// serial config is default: 8, 1, n

// +- 2, +- 4, +- 8
#define AccelerationRange MMA8451_RANGE_2_G
// #define AccelerationDataRate MMA8451_DATARATE_200_HZ

// Used for blinking on error
#define StatusLed 13



// Accelerometer
Adafruit_MMA8451 mma = Adafruit_MMA8451();

void setup() {
    pinMode(StatusLed, OUTPUT);
    digitalWrite(StatusLed, HIGH); // High until setup ends
    
    // Connect i2c accelerometer
    setupAccelerometer();

    // Connect bluetooth serial
    setupBluetooth();

    digitalWrite(StatusLed, LOW);
}

// MAINLOOP
void loop() {

    // mma.read();
    // mma.x; mma.y; mma.z;
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

    if (! mma.begin()) {
        // Serial.println("Couldnt start");
        errorStop();
    }

    mma.setRange(AccelerationRange);
    //mma.setDataRate(AccelerationDataRate);
}

void setupBluetooth() {
    Bluetooth.begin(BluetoothSerialSpeed);
}

