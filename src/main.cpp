#include <Arduino.h>
#include "BluetoothSerial.h"

#define DELAY 2000
#define BLINK_SPEED 500

int incoming;
int ledPin = 2; // Built in LED on ESP32 dev board

unsigned long now;
unsigned long timestampButton;
unsigned long timestampButton2;

const int shutterPin = 23;

BluetoothSerial ESP_BT;

void setup() {
    Serial.begin(19200);
    ESP_BT.begin("RZ67 Blaa");

    pinMode(ledPin, OUTPUT);
    pinMode(shutterPin, OUTPUT);
}

void openShutter() {
    digitalWrite(shutterPin, HIGH);
}

void closeShutter() {
    digitalWrite(shutterPin, LOW);
}

void triggerShutter() {
    Serial.println("Trigger shutter");
    openShutter();
    delay(100);
    closeShutter();
}

void loop() {
    now = millis(); // Store current time
    if (ESP_BT.available()) {
        incoming = ESP_BT.read(); //Read what we receive

        // separate button ID from button value -> button ID is 10, 20, 30, etc, value is 1 or 0
        int button = floor(incoming / 10);
        int value = incoming % 10;

        switch (button) {
            case 1:
                Serial.print("Button 1:");
                Serial.println(value);
                if (value == 1) {
                    digitalWrite(ledPin, HIGH);
                    timestampButton = now;
                } else {
                    digitalWrite(ledPin, LOW);
                }
                triggerShutter();
                break;
            case 2:
                Serial.print("Button 2:");
                Serial.println(value);
                if (value == 1) {
                    digitalWrite(ledPin, HIGH);
                    timestampButton2 = now;
                } else {
                    digitalWrite(ledPin, LOW);
                }
                break;
            case 3:
                Serial.print("Button 3:");
                Serial.println(value);
                break;
        }
    }

    if (incoming == 11 and digitalRead(ledPin) and now > timestampButton + DELAY) {
        // if output port (ledPin) is active and if the delay time has passed, the button is to be switched off
        // time-out check -> check if a button is on and whether it needs to be switched off
        digitalWrite(ledPin, LOW);
        ESP_BT.write(10); // send byte to phone indicateding that Button 1 is to be set to 0 -> 10
        Serial.print("Button 1 timeout - value: 0"); // write to serial port for easy debugging
    } else if (incoming == 21 and now > timestampButton2 + BLINK_SPEED) {
        digitalWrite(ledPin, !digitalRead(ledPin));
        timestampButton2 = now;
        Serial.println("Button 2 timeout - if 2"); // write to serial port for easy debugging
    }
}


