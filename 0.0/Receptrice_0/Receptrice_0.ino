#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ESP32Servo.h>

#define VALUE_INIT 0
#define DOIGT1_PIN 14
#define DOIGT2_PIN 12
#define DOIGT3_PIN 13
#define DOIGT4_PIN 33
#define DOIGT5_PIN 32

static BLEUUID SERVICE_UUID("F000");
static BLEUUID MOTOR1_UUID("F001");
static BLEUUID MOTOR2_UUID("F002");
static BLEUUID MOTOR3_UUID("F003");
static BLEUUID MOTOR4_UUID("F004");
static BLEUUID MOTOR5_UUID("F005");

Servo Doigt1;
Servo Doigt2;
Servo Doigt3;
Servo Doigt4;
Servo Doigt5;

uint8_t MOTOR1_VALUE;
uint8_t MOTOR2_VALUE;
uint8_t MOTOR3_VALUE;
uint8_t MOTOR4_VALUE;
uint8_t MOTOR5_VALUE;

void Movement_Motor(){
  Doigt1.write(MOTOR1_VALUE);
  Doigt2.write(MOTOR2_VALUE);
  Doigt3.write(MOTOR3_VALUE);
  Doigt4.write(MOTOR4_VALUE);
  Doigt5.write(MOTOR5_VALUE);
}

uint8_t handleOnWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0) {
        uint8_t newValue = (uint8_t)value[0];  // Exemple avec un seul octet
        Serial.println("Nouvelle valeur : " + String(newValue));
        return newValue;
    }
}

class Motor1Callbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *Motor1) override {
    MOTOR1_VALUE = handleOnWrite(Motor1);
  }
};

class Motor2Callbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *Motor2) override{
    MOTOR2_VALUE = handleOnWrite(Motor2);
  }
};

class Motor3Callbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *Motor3) override {
    MOTOR3_VALUE = handleOnWrite(Motor3);
  }
};

class Motor4Callbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *Motor4) override {
    MOTOR4_VALUE = handleOnWrite(Motor4);
  }
};

class Motor5Callbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *Motor5) override {
    MOTOR5_VALUE = handleOnWrite(Motor5);
  }
};

void setup() {

  Serial.begin(115200);


  BLEDevice::init("Myo Device");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *Motor1 = pService->createCharacteristic(
    MOTOR1_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
  );

  BLECharacteristic *Motor2 = pService->createCharacteristic(
    MOTOR2_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
  );

  BLECharacteristic *Motor3 = pService->createCharacteristic(
    MOTOR3_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
  );

  BLECharacteristic *Motor4 = pService->createCharacteristic(
    MOTOR4_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
  );

  BLECharacteristic *Motor5 = pService->createCharacteristic(
    MOTOR5_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
  );

  // Définir une valeur initiale pour la caractéristique Myo
  Motor1->setValue("bonjour");
  Motor2->setValue("bonjour");
  Motor3->setValue("bonjour");
  Motor4->setValue("bonjour");
  Motor5->setValue("bonjour");

  // Activer la caractéristique BLE Myo
  Motor1 ->setCallbacks(new Motor1Callbacks());
  Motor2 ->setCallbacks(new Motor2Callbacks());
  Motor3 ->setCallbacks(new Motor3Callbacks());
  Motor4 ->setCallbacks(new Motor4Callbacks());
  Motor5 ->setCallbacks(new Motor5Callbacks());

  Doigt1.attach(DOIGT1_PIN);
  Doigt2.attach(DOIGT2_PIN);
  Doigt3.attach(DOIGT3_PIN);
  Doigt4.attach(DOIGT4_PIN);
  Doigt5.attach(DOIGT5_PIN);
  // Démarrer le service BLE
  pService->start();

  // Démarrer la publicité BLE
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->start();
}

void loop() {

  Movement_Motor();
}