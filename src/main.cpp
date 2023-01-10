#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "c9239c9e-6fc9-4168-b3aa-53105eb990b0"
#define CHARACTERISTIC_UUID "458d4dc9-349f-401d-b092-a2b1c55f5319"

// the delay between button on and button off
#define DELAY 1000

const int led_pin_1 = 4; // TTGO-Display led
const int shutterPin = 23; // Pin for trigger

int incoming;
unsigned long now;
unsigned long timestampButton1;

// BLE
bool deviceConnected = false;

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
        std::string value = pCharacteristic->getValue();

        Serial.print("Value: ");
        Serial.println(value.c_str());
        try {
            Serial.println("Trying to convert");
            char c = value.c_str()[0];
            if (c == '1') {
                // TODO: Not completely sure if this is the right way to do it
                digitalWrite(shutterPin, HIGH);
                delay(100);
                digitalWrite(shutterPin, LOW);
            }
        } catch (const std::exception &e) {
            Serial.println("Error");
        }


        unsigned long newTime = millis();
        unsigned long diff = newTime - now;
        Serial.print("Time to process: ");
        Serial.print(diff);
        Serial.println("ms");

    }
};

void setup() {
    Serial.begin(19200);
    // Serial.begin(115200);
    Serial.println("Starting BLE work!");

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

    // Digital buttons from app
    pinMode(led_pin_1, OUTPUT);

    // Physical buttons on breadboard / device
    pinMode(shutterPin, OUTPUT);
}


void loop() {
    // put your main code here, to run repeatedly:
    if (deviceConnected) {
        digitalWrite(led_pin_1, HIGH);
    } else {
        digitalWrite(led_pin_1, LOW);
    }
}

