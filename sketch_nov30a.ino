#include <string.h>
#include <Arduino.h>
#include <SPI.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#if SOFTWARE_SERIAL_AVAILABLE
  include <SoftwareSerial.h>
#endif


#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"
/*=========================================================================*/


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

// function prototypes over in packetparser.cpp
float parsefloat(uint8_t *buffer);
void printHex(const uint8_t * data, const uint32_t numBytes);

// the packet buffer
extern uint8_t packetbuffer[];


/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void)
{
  Serial.println("Home");
  pinMode(led, OUTPUT);                                                                                                    // pinMode

  while (!Serial);  // required for Flora & Micro
  delay(500);



// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();

    if (value.length() > 0) {
      Serial.println("*********");
      Serial.print("New value: ");
      for (int i = 0; i < value.length(); i++) {
        Serial.print(value[i]);
      }

      Serial.println();
      Serial.println("*********");
    }
  }
};

void setup() {
  Serial.println("Home");
  pinMode(led, OUTPUT);                                                                                                    // pinMode

  while (!Serial);  // required for Flora & Micro
  delay(500);
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("AutoFeelment");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic =
    pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("Hello World says Ilesh");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop(void)
{
  /* Wait for new data to arrive */
  // uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);
  // if (len == 0) return;

  // /* Got a packet! */
  // // printHex(packetbuffer, len);


  // // Buttons
  // if (packetbuffer[1] == 'B') {
  //   uint8_t buttnum = packetbuffer[2] - '0';
  //   boolean pressed = packetbuffer[3] - '0';
  //   //Serial.print ("Button "); Serial.print(buttnum);
  //   if (pressed) {
  //     if (buttnum == 1) {
  //     Serial.println(" Running");
      
      digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(5000);                // wait for a half second
      digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
      delay(5000);                // wait for a half second

     
      // while(true);
      
      //}
   //}
    
  //}
}