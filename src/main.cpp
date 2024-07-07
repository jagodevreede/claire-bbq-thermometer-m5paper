#include <M5EPD.h>
#include <ESPmDNS.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

M5EPD_Canvas canvas(&M5.EPD);

int point[2][2];
static BLEAddress *pServerAddress;
BLEClient *pClient;
BLEScan *pBLEScan;

static String btDeviceName = "Grill BT5.0";
static boolean doConnect = false;
static boolean connected = false;
static BLEUUID serviceUUID;

static BLEUUID tempUUID("0000ffb1-0000-1000-8000-00805f9b34fb");
static BLEUUID notUUID("0000ffb2-0000-1000-8000-00805f9b34fb");

static BLERemoteCharacteristic *pTempCharacteristic;
static BLERemoteCharacteristic *pNotCharacteristic;

// BLE scan results
// as we don't connect directly to the mac address of the thermometer
// we have to scan for the service UUID and connect to the first device which equals the inkbird UUID
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{

    void onResult(BLEAdvertisedDevice advertisedDevice)
    {

        if (advertisedDevice.haveName())
        {
            log_i("%s", advertisedDevice.getName().c_str());
            if (btDeviceName.equals(advertisedDevice.getName().c_str()))
            {
                log_i("Found the thermometer");
                advertisedDevice.getScan()->stop();
                pServerAddress = new BLEAddress(advertisedDevice.getAddress());
                serviceUUID = advertisedDevice.getServiceUUID();
                doConnect = true;
                log_i("Service UUID: %s", serviceUUID.toString().c_str());
            }
        }
    }
};

// generic BLE callbacks. just used to re-/set the connected state
class MyClientCallback : public BLEClientCallbacks
{

    void onConnect(BLEClient *pclient)
    {
        log_i("BT connected");
        connected = true;
    }

    void onDisconnect(BLEClient *pclient)
    {
        log_i("BT disconnected");
        connected = false;
    }
};

// init the ESP32 as a BLE device and set some scan parameters
void bleInit()
{
    log_d("Starting ble scan");
    BLEDevice::init("ESP 32 Grill BT5.0");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->start(20);
}

struct ProbeData
{
    uint16_t headerInformation;
    uint16_t probeData[6];
};

void printBinaryValue(uint8_t *pData)
{
    if (pData)
    {                           // Check if the pointer is not null
        uint8_t value = *pData; // Dereference the pointer to get the value
        for (int i = 7; i >= 0; --i)
        {                                   // Loop through each bit
            Serial.print((value >> i) & 1); // Print each bit
        }
        Serial.println(); // New line after printing the binary value
    }
    else
    {
        Serial.println("Pointer is null.");
    }
}

static void notifyCallback(
    BLERemoteCharacteristic *pBLERemoteCharacteristic,
    uint8_t *pData,
    size_t length,
    bool isNotify)
{

    ProbeData *data = reinterpret_cast<ProbeData *>(pData);
    for (int i = 0; i < 6; i += 1)
    {
        data->probeData[i] = ntohs(data->probeData[i]);
    }

    for (int i = 0; i < 6; i += 1)
    {
        log_i("ntohs %d %d", i, ntohs(data->probeData[i]));
    }
}

// connecting to the thermometer
bool connectToBLEServer(BLEAddress pAddress)
{

    pClient = BLEDevice::createClient();

    pClient->setClientCallbacks(new MyClientCallback());
    pClient->connect(pAddress);

    BLERemoteService *pRemoteService = pClient->getService(serviceUUID);

    if (pRemoteService == nullptr)
    {
        return false;
    }

    // std::map<std::string, BLERemoteCharacteristic*>* characteristics = pRemoteService->getCharacteristics();
    // for (const auto& pair : *characteristics) {
    //     std::string key = pair.first;
    //     BLERemoteCharacteristic* value = pair.second;
    //     // Do something with key and value
    //     log_i("%s %d", key.c_str(), value->getHandle());
    // }

    // std::map<std::string, BLERemoteCharacteristic *>::iterator it;
    // for (it = pRemoteService->getCharacteristics()->begin(); it != pRemoteService->getCharacteristics()->end(); it++)
    // {
    //     log_i("Characteristic %s: %s", it->first.c_str(), it->second->toString().c_str());
    // }

    // pTempCharacteristic = pRemoteService->getCharacteristic(tempUUID);
    // if (pTempCharacteristic == nullptr) {
    //     return false;
    // }

    // float value = pTempCharacteristic->readFloat();
    // log_i("Got: %f", value);

    pNotCharacteristic = pRemoteService->getCharacteristic(notUUID);
    if (pNotCharacteristic == nullptr)
    {
        return false;
    }

    pNotCharacteristic->registerForNotify(notifyCallback);
    log_i("Registerd notify");

    //[  6416][I][main.cpp:86] connectToBLEServer(): 0000ffb1-0000-1000-8000-00805f9b34fb
    //[  6421][I][main.cpp:86] connectToBLEServer(): 0000ffb2-0000-1000-8000-00805f9b34fb

    //    pAuthCharacteristic = pRemoteService->getCharacteristic(characteristicAuthUUID);

    // if (pAuthCharacteristic == nullptr) {
    //     return false;
    // }

    // pAuthCharacteristic->writeValue((uint8_t *)enableAccess, sizeof(enableAccess), true);

    // pRemoteCharacteristic = pRemoteService->getCharacteristic(characteristicRealTimeDataUUID);

    // if (pRemoteCharacteristic == nullptr) {
    //     return false;
    // }

    // pSettingsCharacteristic = pRemoteService->getCharacteristic(characteristicSettingsUUID);

    // if (pSettingsCharacteristic == nullptr) {
    //     return false;
    // }

    // pSettingsCharacteristic->writeValue((uint8_t *)enableRealTimeData, sizeof(enableRealTimeData), true);
    // pSettingsCharacteristic->writeValue((uint8_t *)enableUnitCelsius, sizeof(enableUnitCelsius), true);

    // pRemoteCharacteristic->registerForNotify(notifyCallback);

    // pSettingsResultsCharacteristic = pRemoteService->getCharacteristic(characteristicSettingsResultsUUID);

    // if (pSettingsResultsCharacteristic == nullptr) {
    //     return false;
    // }

    // pSettingsResultsCharacteristic->registerForNotify(notifyResultsCallback);

    // // get the battery data on connection once
    // // in the loop() we will request it just every ~60 seconds
    // getBatteryState();

    return true;
}

void setup()
{
    M5.begin(true, false, true, true, false);
    // M5.EPD.SetRotation(90);
    // M5.TP.SetRotation(90);
    // M5.EPD.Clear(true);
    // canvas.createCanvas(540, 960);
    // canvas.setTextSize(5);
    // // canvas.drawString("Touch The Screen!", 20, 400);
    // canvas.pushCanvas(0,0,UPDATE_MODE_DU4);

    bleInit();
}

void loop()
{
    // doConnect is only true after the BLE scan finds a device
    // rights afterwards it is false again - also if the scan isn't successfully
    if (doConnect == true)
    {

        // true if the ESP is connected
        connected = (connectToBLEServer(*pServerAddress)) ? true : false;
        doConnect = false;
    }

    delay(10000); // Delay a second between loops.
}
