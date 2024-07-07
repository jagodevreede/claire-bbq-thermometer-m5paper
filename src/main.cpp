#include <M5EPD.h>
#include <ESPmDNS.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define NUMBER_OF_PROBES 6
#define PROBE_NOT_CONNECTED_VALUE 65535

static float probeValues[NUMBER_OF_PROBES] = {};

M5EPD_Canvas canvas(&M5.EPD);

static BLEAddress *pServerAddress;
BLEClient *mClient;
BLEScan *pBLEScan;

static String btDeviceName = "Grill BT5.0";
static boolean doConnect = false;
static boolean connected = false;
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
                doConnect = true;
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
        return false;
    }

    BLERemoteCharacteristic *tempatureCharacteristic = pRemoteService->getCharacteristic(tempUUID);
    if (tempatureCharacteristic == nullptr)
    {
        return false;
    }

    tempatureCharacteristic->registerForNotify(notifyCallback);
    log_i("Registerd temprature notification");

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
        connected = (connectToBLEServer(*pServerAddress)) ? true : false;
        doConnect = false;
    }

    delay(10000); // Delay a second between loops.
}
