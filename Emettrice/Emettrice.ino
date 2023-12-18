#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEClient.h>
#include <BLE2902.h>

#define FLEX1_PIN 36
#define FLEX2_PIN 12
#define FLEX3_PIN 14
#define FLEX4_PIN 13
#define FLEX5_PIN 32
#define SERVICE_NAME "Myo Device"

static BLEUUID SERVICE_UUID("F000"); 
static BLEUUID MOTOR1_UUID("F001");
static BLERemoteCharacteristic* motorCharacteristics[5];  

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    if (advertisedDevice.haveName() && advertisedDevice.getName() == SERVICE_NAME) {
        
        Serial.println("Found Our Service");
        BLEDevice::getScan()->stop();
        myDevice = new BLEAdvertisedDevice(advertisedDevice);
        doConnect = true;
    } else {
        Serial.println("Device found, but not our service");
    }
  } 
}; 

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("Disconnected");
  }
};

bool connectToServer() {
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient* pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback());

  const int maxAttempts = 3;

  for (int attempt = 1; attempt <= maxAttempts; attempt++) {
    Serial.print("Attempt ");
    Serial.print(attempt);
    Serial.println(" of connecting to BLE Server...");

    if (pClient->connect(myDevice)) {
        Serial.println(" - Connected to server");

        BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
        if (pRemoteService == nullptr) {
            Serial.print("Failed to find our service UUID: ");
            Serial.println(SERVICE_UUID.toString().c_str());
            return false;
        }
        Serial.println(" - Found our service");

        motorCharacteristics[0] = pRemoteService->getCharacteristic(MOTOR1_UUID);

        Serial.println(" - Found all motor characteristics");
        connected = true;
        return true;
    } 
    else {
      Serial.println(" - Failed to connect, retrying...");
      delay(1000); // Délai avant de réessayer
    }
  }

  Serial.println("Failed to connect after maximum attempts");
  return false;
}

void writeMotorValue(String value) {
    if (!connected) {
        Serial.println("Not connected, cannot write value.");
        return; // Vérifiez si la connexion est établie
    }

    int valueLength = value.length();
    char valueArray[valueLength + 1]; // +1 pour le caractère nul
    value.toCharArray(valueArray, valueLength + 1);

    motorCharacteristics[0]->writeValue((uint8_t*)valueArray, valueLength);
    Serial.println("Value written to Motor 1 : " + value);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  BLEScan* pBLEScan = BLEDevice::getScan(); 
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

int mapFlex(int ValueFlex){

  int angle;

  if (ValueFlex > 1000)
  {
    angle = map(ValueFlex, 1000, 4095, 120, 180);
  }
  else if (ValueFlex > 600)
  {
    angle = map(ValueFlex, 600, 1000, 60, 120);
  }
  else if (ValueFlex > 0)
  {
    angle = map(ValueFlex, 0, 600, 0, 60);
  } 

  return angle;
}

String creaMess(){

  int Flex_1 = mapFlex(analogRead(FLEX1_PIN));
  int Flex_2 = mapFlex(analogRead(FLEX2_PIN));
  int Flex_3 = mapFlex(analogRead(FLEX3_PIN));
  int Flex_4 = mapFlex(analogRead(FLEX4_PIN));
  int Flex_5 = mapFlex(analogRead(FLEX5_PIN));

  String valueMess = String(Flex_1) + "/" + String(Flex_2) + "/" + String(Flex_3) + "/" + String(Flex_4) + "/" + String(Flex_5);
  Serial.println(valueMess);

  return valueMess;
}

void loop() {

  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  if (connected) {
    String Mess = creaMess() ;
    writeMotorValue(Mess); 
    delay(1000);
  }
}



