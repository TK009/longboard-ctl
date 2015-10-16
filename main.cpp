/**
 * Main controller for an electronic longboard (long skateboard) project.
 * Inteded for arduino nano.
 *
 * ESC control: PWM (analog write?)
 * Accelerometer: I2C (library)
 * Bluetooth: Serial?
 */

#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>


// Bluetooth pins 0, 1, 3V3
#define Bluetooth Serial
// default
#define BluetoothSerialSpeed 9600
// serial config is default: 8, 1, n

// I2C: A4 (SDA), A5 (SCL)


// Used for blinking on error
#define StatusLed 13



void setup() {
    pinMode(StatusLed, OUTPUT);
    
    // Connect i2c accelerometer
    setupAccelerometer();

    // Connect bluetooth serial
    setupBluetooth();
}

// MAINLOOP
void loop() {

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

}

void setupBluetooth() {
    Bluetooth.begin(BluetoothSerialSpeed);
}

