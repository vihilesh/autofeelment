/*
 /*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
   Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE" 
   Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with  "NOTIFY"

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   In this example rxValue is the data received (only accessible inside that function).
   And txValue is the data to be sent, in this example just a byte incremented every second. 
*/
#include <string.h>
#include <Arduino.h>
#include <SPI.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


#include <PololuMaestro.h>

  





BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
float txValue = 0;
const int readPin = 32; // Use GPIO number. See ESP32 board pinouts
const int LED = 2; // Could be different depending on the dev board. I used the DOIT ESP32 dev board.

//std::string rxValue; // Could also make this a global var to access it in loop()

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

// Create Maestro controller instance using Serial1 
// (typically on pins 18/19 for Mega-based Arduinos)
//MaestroServoController2 maestro();

MiniMaestro maestro(Serial1);


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
      void onWrite(BLECharacteristic *pCharacteristic) {
     
        std::string rxValue = pCharacteristic->getValue().c_str();

        if (rxValue.length() > 0) {
          Serial.println("*********");
          Serial.print("Received Value: ");
          Serial.println(rxValue.c_str());

          boolean pressed = false;
          uint8_t buttnum = 0;
          for (int i = 0; i < rxValue.length(); i++) {
            if (rxValue[1] == 'B') {
              buttnum = rxValue[2] - '0';
              boolean pressed = rxValue[3] - '0';
            }
          }

          //Serial.println();

          // Do stuff based on the command received from the app
          Serial.print("Buttnum: "); 
          Serial.println(buttnum);
          Serial.print("Pressed: ");
          Serial.println(pressed);
       

          if ((buttnum == 1) ) {
            Serial.println("Turning 1");
            digitalWrite(LED, LOW);

             maestro.setSpeed(0, 0);
             maestro.setAcceleration(0, 0);
            for (unsigned short pos = 0; pos <= 1000; pos += 500) {
              maestro.setTarget(0, pos);
              delay(200);
            }
            delay(2000);
            // maestro.setTarget(0,8000);
            // delay(2000);
            //while(true);
            Serial.println("Loop done");
          
            // maestro.setSpeed(1, 0);
            // maestro.setAcceleration(1, 0);
            // maestro.setTarget(1,4000);
            // delay(2000);
            // maestro.setTarget(1,8000);
            // delay(2000);
            // //while(true);
            // Serial.println("Loop done");
          
            // maestro.setSpeed(2, 0);
            // maestro.setAcceleration(2, 0);
            // maestro.setTarget(2,4000);
            // delay(2000);
            // maestro.setTarget(2,8000);
            // delay(2000);
            // //while(true);
            // Serial.println("Loop done");
          
            // maestro.setSpeed(3, 0);
            // maestro.setAcceleration(3, 0);
            // maestro.setTarget(3,4000);
            // delay(2000);
            // maestro.setTarget(3,8000);
            // delay(2000);

          } else if( buttnum == 2) {
            Serial.println("Turning 2");
            digitalWrite(LED, LOW);
          } else if( buttnum == 3) {
            Serial.println("Turning 3");
            digitalWrite(LED, LOW);
          } else if( buttnum == 4) {
            Serial.println("Turning 4");
            digitalWrite(LED, LOW);
          }

          Serial.println();
          Serial.println("*********");
          pressed = false;
        }
       
      }
};

void setup() {
  
  Serial1.begin(115200);
  Serial.begin(9600);
  Serial.println("Pololu Maestro Servo Controller Initialized");
  maestro.goHome();
  //Serial.println("Maestro at Home");
  //maestro.setTarget(0, 5000);
  delay(1000);

  pinMode(LED, OUTPUT);

  // Create the BLE Device
  BLEDevice::init("ESP32 UART Test"); // Give it a name

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
                      
  pCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  if (deviceConnected) {
    maestro.setTarget(0, 5000);

    // Fabricate some arbitrary junk for now...
    txValue = analogRead(readPin) / 3.456; // This could be an actual sensor reading!

    // Let's convert the value to a char array:
    char txString[8]; // make sure this is big enuffz
    dtostrf(txValue, 1, 2, txString); // float_val, min_width, digits_after_decimal, char_buffer
    
//    pCharacteristic->setValue(&txValue, 1); // To send the integer value
//    pCharacteristic->setValue("Hello!"); // Sending a test message
    pCharacteristic->setValue(txString);
    
   // pCharacteristic->notify(); // Send the value to the app!
    // Serial.print("*** Sent Value: ");
    // Serial.print(txString);
    // Serial.println(" ***");

    // You can add the rxValue checks down here instead
    // if you set "rxValue" as a global var at the top!
    // Note you will have to delete "std::string" declaration
    // of "rxValue" in the callback function.
//    if (rxValue.find("A") != -1) { 
//      Serial.println("Turning ON!");
//      digitalWrite(LED, HIGH);
//    }
//    else if (rxValue.find("B") != -1) {
//      Serial.println("Turning OFF!");
//      digitalWrite(LED, LOW);
//    }
  }
  delay(1000);
}