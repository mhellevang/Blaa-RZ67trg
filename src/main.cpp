#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>

#define SERVICE_UUID        "c9239c9e-6fc9-4168-b3aa-53105eb990b0"
#define CHARACTERISTIC_UUID "458d4dc9-349f-401d-b092-a2b1c55f5319"

#define DELAY 2000
#define BLINK_SPEED 500

const int ledPin = 2; // Built in LED on ESP32 dev board
const int shutterPin = 23; // Pin for trigger

int incoming;
unsigned long now;
unsigned long timestampButton;
unsigned long timestampButton2;

bool deviceConnected = false;

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

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer *pServer) {
        deviceConnected = false;
    };

};

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        now = millis(); // Store current time
        incoming = atoi(pCharacteristic->getValue().c_str());

        int button = floor(incoming / 10);
        int value = incoming % 10;

        switch (button) {
            case 1:
                Serial.print("Button 1, value = ");
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
                Serial.print("Button 2, value = ");
                Serial.println(value);
                if (value == 1) {
                    digitalWrite(ledPin, HIGH);
                    timestampButton2 = now;
                } else {
                    digitalWrite(ledPin, LOW);
                }
                break;
        }
    }
};

void setup() {
    Serial.begin(19200);
    Serial.println("Hello from RZ67 Blaa!");

    BLEDevice::init("RZ67 Blaa");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );
    pCharacteristic->setCallbacks(new MyCallbacks());

    pCharacteristic->setValue("Hello World says Neil");
    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it in your phone!");

    pinMode(ledPin, OUTPUT);
    pinMode(shutterPin, OUTPUT);
}


void loop() {
    now = millis(); // Store current time

    if (incoming == 11 and digitalRead(ledPin) and now > timestampButton + DELAY) {
        // Shutter has fired, disable LED
        digitalWrite(ledPin, LOW);
        Serial.println("Button 1 timeout reached");
    } else if (incoming == 21 and now > timestampButton2 + BLINK_SPEED) {
        // Blink LED to indicate countdown
        digitalWrite(ledPin, !digitalRead(ledPin));
        timestampButton2 = now;
        Serial.println("Button 2: Blink LED");
    }
}

