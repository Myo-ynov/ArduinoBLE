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

String Motors[100];
int StringCount = 0;

void Movement_Motor(){
  Doigt1.write(Motors[0].toInt());
  Doigt2.write(Motors[1].toInt());
  Doigt3.write(Motors[2].toInt());
  Doigt4.write(Motors[3].toInt());
  Doigt5.write(Motors[4].toInt());
  Serial.println("fait");
}

void handleOnWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    Serial.print("Valeur du message de base : ");
    String valueStr = String(value.c_str());
    Serial.println(valueStr);

  while (valueStr.length() > 0) 
  {
    int index = valueStr.indexOf(' ');
    if (index == -1) // No space found
    {
      Serial.println(valueStr);
      Motors[StringCount++] = valueStr;
      break;
    }
    else
    {
      Motors[StringCount++] = valueStr.substring(0, index);
      valueStr = valueStr.substring(index+1);
    }
  }

  StringCount = 0;
  Serial.println(Motors[0]);
  Serial.println(Motors[1]);
  Serial.println(Motors[2]);
  Serial.println(Motors[3]);
  Serial.println(Motors[4]);
  Movement_Motor();
}

class Motor1Callbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *Motor1) override {
     handleOnWrite(Motor1); // MOTOR1_VALUE =
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

  // Définir une valeur initiale pour la caractéristique Myo
  Motor1 -> setValue("bonjour");

  // Activer la caractéristique BLE Myo
  Motor1 -> setCallbacks(new Motor1Callbacks());

  Doigt1.attach(DOIGT1_PIN);
  Doigt2.attach(DOIGT2_PIN);
  Doigt3.attach(DOIGT3_PIN);
  Doigt4.attach(DOIGT4_PIN);
  Doigt5.attach(DOIGT5_PIN);

  Movement_Motor();
  // Démarrer le service BLE
  pService->start();

  // Démarrer la publicité BLE
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);

  BLEDevice::startAdvertising();
}

void loop() {
}
