#include <Arduino.h>
#include "BluetoothSerial.h"

// the delay between button on and button off
#define DELAY 1000

int led_pin_1 = 4; // TTGO-Display led

int incoming;
unsigned long now;
unsigned long timestampButton1;

const int shutterPin = 23;

BluetoothSerial ESP_BT;

void setup() {
    Serial.begin(19200);
    ESP_BT.begin("RZ67 Blaa");

    // Digital buttons from app
    pinMode(led_pin_1, OUTPUT);

    // Physical buttons on breadboard / device
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

void handleDigitalTriggers() {
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
                digitalWrite(led_pin_1, value); // Indicate in led
                if(value == 1) // check if the button is switched on
                    timestampButton1 = now; // if button is switched on, write the current time to the timestamp
                triggerShutter();
                break;
            case 2:
                Serial.print("Button 2:");
                Serial.println(value);
                break;
            case 3:
                Serial.print("Button 3:");
                Serial.println(value);
                break;
        }
    }

    // time-out check -> check if a button is on and whether it needs to be switched off
    if(digitalRead(led_pin_1) and now > timestampButton1 + DELAY) { // if output port (led_pin_1) is active and if the delay time has passed, the button is to be switched off
        digitalWrite(led_pin_1, 0); // set output port to 0
        ESP_BT.write(10); // send byte to phone indicateding that Button 1 is to be set to 0 -> 10
        Serial.print("Button 1 timeout - value: 0"); // write to serial port for easy debugging
    }
}

void loop() {
    handleDigitalTriggers();
}

