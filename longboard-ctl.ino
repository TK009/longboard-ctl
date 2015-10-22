/**
 * Main controller for an electronic longboard (long skateboard) project.
 * Inteded for Arduino Nano (v3.0).
 *
 * ESC control:     PWM            pins D9 (any pwm: D3, 5, 6, 9, 10, 11)
 * Accelerometer:   I2C (library)  pins A4 (SDA), A5 (SCL)
 * Bluetooth:       Serial         pins RX0, TX1, 3V3
 */

#include <Wire.h>
#include <MMA8452.h>


// Bluetooth pins 0, 1, 3V3
#define Bluetooth Serial
// default
#define BluetoothSerialSpeed 9600
// serial config is default: 8, 1, n

#define AccelerometerAddr 0x1C
// +- 2, +- 4, +- 8
#define AccelerationRange MMA_RANGE_2G
// #define AccelerationDataRate MMA_800hz

// Used for blinking on error
#define StatusLed 13



// Accelerometer
MMA8452 mma = MMA8452();

// declarations
void setupBluetooth();
bool setupAccelerometer();


void setup() {
    pinMode(StatusLed, OUTPUT);
    digitalWrite(StatusLed, HIGH); // High until setup ends


    // Connect bluetooth serial
    setupBluetooth();
    
    // Connect i2c accelerometer
    setupAccelerometer();

    digitalWrite(StatusLed, LOW);
}

// MAINLOOP
void loop() {
    uint16_t x, y, z;
    mma.getRawData(&x, &y, &z);
    Serial.print(x);
    Serial.print(",");
    Serial.print(y);
    Serial.print(",");
    Serial.println(z);
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

    if (! mma.init()) {
        Serial.println("Couldnt start accelerometer!");
        errorStop();
    }

    mma.setRange(AccelerationRange);
    //mma.setDataRate(AccelerationDataRate);
    return true;
}

void setupBluetooth() {
    Bluetooth.begin(BluetoothSerialSpeed);
    Bluetooth.println("Bluetooth test");
}

