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

static BLEUUID SERVICE_UUID("F000"); // Remplacez par l'UUID de votre service
static BLEUUID MOTOR1_UUID("F001");
static BLEUUID MOTOR2_UUID("F002");
static BLEUUID MOTOR3_UUID("F003");
static BLEUUID MOTOR4_UUID("F004");
static BLEUUID MOTOR5_UUID("F005");
static BLERemoteCharacteristic* motorCharacteristics[5];  // Remplacez par l'UUID de la caractéristique

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        Serial.print("BLE Advertised Device found: ");
        Serial.println(advertisedDevice.toString().c_str());

        // Nous avons trouvé un dispositif, voyons maintenant s'il contient le service que nous recherchons.
        if (advertisedDevice.haveName() && advertisedDevice.getName() == SERVICE_NAME) {
            
            Serial.println("Found Our Service");
            // Stop scanning as we found our device
            BLEDevice::getScan()->stop();
            myDevice = new BLEAdvertisedDevice(advertisedDevice);
            doConnect = true;
        } else {
            Serial.println("Device found, but not our service");
        }
    } // onResult
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

    // Réessayer la connexion un certain nombre de fois
    const int maxAttempts = 3;
    for (int attempt = 1; attempt <= maxAttempts; attempt++) {
        Serial.print("Attempt ");
        Serial.print(attempt);
        Serial.println(" of connecting to BLE Server...");

        // Connect to the BLE Server.
        if (pClient->connect(myDevice)) {
            Serial.println(" - Connected to server");

            // Obtention d'une référence au service désiré
            BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
            if (pRemoteService == nullptr) {
                Serial.print("Failed to find our service UUID: ");
                Serial.println(SERVICE_UUID.toString().c_str());
                return false;
            }
            Serial.println(" - Found our service");

            // Récupérer les caractéristiques pour chaque moteur
            motorCharacteristics[0] = pRemoteService->getCharacteristic(MOTOR1_UUID);
            motorCharacteristics[1] = pRemoteService->getCharacteristic(MOTOR2_UUID);
            motorCharacteristics[2] = pRemoteService->getCharacteristic(MOTOR3_UUID);
            motorCharacteristics[3] = pRemoteService->getCharacteristic(MOTOR4_UUID);
            motorCharacteristics[4] = pRemoteService->getCharacteristic(MOTOR5_UUID);

            // Vérification que les caractéristiques ont été trouvées
            for (int i = 0; i < 5; i++) {
                if (motorCharacteristics[i] == nullptr) {
                    Serial.print("Failed to find motor ");
                    Serial.print(i + 1);
                    Serial.println(" characteristic UUID");
                    return false;
                }
            }

            Serial.println(" - Found all motor characteristics");
            connected = true;
            return true;
        } else {
            Serial.println(" - Failed to connect, retrying...");
            delay(1000); // Délai avant de réessayer
        }
    }

    Serial.println("Failed to connect after maximum attempts");
    return false;
}


void writeMotorValue(uint8_t motorId, uint8_t value) {
  if (motorId < 1 || motorId > 5) return; // Validation de l'identifiant du moteur
  if (!connected) return; // Vérifiez si la connexion est établie

  uint8_t adjustedValue = min(max(value, (uint8_t)1), (uint8_t)180); // Limite la valeur entre 1 et 180
  motorCharacteristics[motorId - 1]->writeValue(&adjustedValue, sizeof(adjustedValue));
  Serial.println("Value written to Motor" + String(motorId) + ": " + String(adjustedValue));
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Start scanning for devices
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
    writeMotorValue(1, mapFlex(analogRead(FLEX1_PIN))); 
    writeMotorValue(2, mapFlex(analogRead(FLEX2_PIN)));
    writeMotorValue(3, mapFlex(analogRead(FLEX3_PIN)));
    writeMotorValue(4, mapFlex(analogRead(FLEX4_PIN)));
    writeMotorValue(5, mapFlex(analogRead(FLEX5_PIN)));
    delay(1000); // Delay a second between loops.
  }
}



