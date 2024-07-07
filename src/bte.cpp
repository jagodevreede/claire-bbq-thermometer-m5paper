#include "bte.h"

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <arpa/inet.h>

// globals:
float probeValues[NUMBER_OF_PROBES] = {};
byte bteState = BT_STATE_NA;

static BLEAddress *pServerAddress;
BLEClient *mClient;

static String btDeviceName = "Grill BT5.0";
static BLEUUID serviceUUID;

static BLEUUID tempUUID("0000ffb2-0000-1000-8000-00805f9b34fb");

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
                advertisedDevice.getScan()->stop();
                pServerAddress = new BLEAddress(advertisedDevice.getAddress());
                serviceUUID = advertisedDevice.getServiceUUID();
                bteState = BT_STATE_CONNECTING;
                log_i("Found the thermometer with service UUID: %s", serviceUUID.toString().c_str());
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
        bteState = BT_STATE_CONNECTED;
    }

    void onDisconnect(BLEClient *pclient)
    {
        log_i("BT disconnected");
        bteState = BT_STATE_NA;
    }
};

// init the ESP32 as a BLE device and set some scan parameters
void bleInit()
{
    // Rest all probes to not connected
    for (int i = 0; i < NUMBER_OF_PROBES; i += 1)
    {
        probeValues[i] = PROBE_NOT_CONNECTED_VALUE;
    }
    log_d("Starting ble scan");
    bteState = BT_STATE_SCANNING;
    BLEDevice::init("ESP 32 Grill BT5.0");
    BLEScan *bleScan = BLEDevice::getScan();
    bleScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    bleScan->setActiveScan(true);
    bleScan->start(20);
}

struct ProbeData
{
    uint16_t headerInformation;
    uint16_t probeData[NUMBER_OF_PROBES];
};

void fixUpIndianFormat(ProbeData *data)
{
    for (int i = 0; i < NUMBER_OF_PROBES; i += 1)
    {
        data->probeData[i] = ntohs(data->probeData[i]);
    }
}

static void notifyCallback(
    BLERemoteCharacteristic *pBLERemoteCharacteristic,
    uint8_t *pData,
    size_t length, // length is always 15 there are 6 probes...
    bool isNotify)
{
    ProbeData *data = reinterpret_cast<ProbeData *>(pData);
    fixUpIndianFormat(data);

    for (int i = 0; i < NUMBER_OF_PROBES; i += 1)
    {
        if (data->probeData[i] == PROBE_NOT_CONNECTED_VALUE)
        {
            probeValues[i] = PROBE_NOT_CONNECTED_VALUE;
        }
        else
        {
            probeValues[i] = data->probeData[i] / 10.0f;
        }
    }
}

// connecting to the thermometer
bool connectToBLEServer(BLEAddress pAddress)
{
    mClient = BLEDevice::createClient();

    mClient->setClientCallbacks(new MyClientCallback());
    mClient->connect(pAddress);

    BLERemoteService *pRemoteService = mClient->getService(serviceUUID);

    if (pRemoteService == nullptr)
    {
        log_w("Disconnecting as we can't get service");
        mClient->disconnect();
        return false;
    }

    BLERemoteCharacteristic *tempatureCharacteristic = pRemoteService->getCharacteristic(tempUUID);
    if (tempatureCharacteristic == nullptr)
    {
        log_w("Disconnecting as we can't get tempature characteristic");
        mClient->disconnect();
        return false;
    }

    tempatureCharacteristic->registerForNotify(notifyCallback);
    log_i("Registerd temprature notification");

    return true;
}

void bteLoop()
{
    // doConnect is only true after the BLE scan finds a device
    // rights afterwards it is false again - also if the scan isn't successfully
    if (bteState == BT_STATE_CONNECTING)
    {
        bteState = (connectToBLEServer(*pServerAddress)) ? BT_STATE_CONNECTED : BT_STATE_NA;
    }
    else if (bteState == BT_STATE_NA)
    {
        bleInit();
    }
}

void bteMock()
{
    if (bteState == BT_STATE_NA)
    {
        bteState = BT_STATE_CONNECTED;
        for (int i = 0; i < NUMBER_OF_PROBES; i += 1)
        {
            probeValues[i] = 42.9;
        }
    }
    for (int i = 0; i < NUMBER_OF_PROBES; i += 1)
    {
        probeValues[i] += random(1, 10) / 10.0;
    }
}
