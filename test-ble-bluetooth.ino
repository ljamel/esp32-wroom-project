#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// télécharger une appliction capable de dtecter le bluthoot ble

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "abcdefab-1234-1234-1234-abcdefabcdef"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

void setup() {
  Serial.begin(115200);

  // Initialiser BLE
  BLEDevice::init("ESP32_BLE");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Créer un service BLE
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Créer une caractéristique BLE
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pCharacteristic->setValue("Hello ESP32");
  pService->start();

  // Démarrer la publicité
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("BLE device is ready to connect!");
}

void loop() {
  if (deviceConnected) {
    String value = pCharacteristic->getValue(); // <- corrigé ici
    if (value.length() > 0) {
      Serial.print("Received: ");
      Serial.println(value);
      pCharacteristic->setValue("Message received");
    }
  }
  delay(1000);
}

