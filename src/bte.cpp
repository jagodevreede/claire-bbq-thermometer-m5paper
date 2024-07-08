#include "bte.h"

#include <Arduino.h>
#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <RingBufCPP.h>
#include <arpa/inet.h>

// globals:
float probeValues[NUMBER_OF_PROBES] = {};
byte bteState = BT_STATE_NA;

static RingBufCPP<float, HISTORY_LENGTH> probeHistory[NUMBER_OF_PROBES];
static bool probeHistoryLoaded = 0;

unsigned long lastHistoryUpdate = 0;
static BLEAddress *pServerAddress;
BLEClient *mClient;

static String btDeviceName = "Grill BT5.0";
static BLEUUID serviceUUID;

static BLEUUID tempUUID("0000ffb2-0000-1000-8000-00805f9b34fb");

// BLE scan results
// as we don't connect directly to the mac address of the thermometer
// we have to scan for the service UUID and connect to the first device which
// equals the inkbird UUID
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveName()) {
      log_i("%s", advertisedDevice.getName().c_str());
      if (btDeviceName.equals(advertisedDevice.getName().c_str())) {
        advertisedDevice.getScan()->stop();
        pServerAddress = new BLEAddress(advertisedDevice.getAddress());
        serviceUUID = advertisedDevice.getServiceUUID();
        bteState = BT_STATE_CONNECTING;
        log_i("Found the thermometer with service UUID: %s",
              serviceUUID.toString().c_str());
      }
    }
  }
};

// generic BLE callbacks. just used to re-/set the connected state
class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient *pclient) {
    log_i("BT connected");
    bteState = BT_STATE_CONNECTED;
  }

  void onDisconnect(BLEClient *pclient) {
    log_i("BT disconnected");
    bteState = BT_STATE_NA;
  }
};

void historyKeeperLoop() {
  unsigned long seconds = millis() / 1000;
  if ((lastHistoryUpdate + 10) <= seconds) {
    log_v("Update history");
    lastHistoryUpdate = seconds;
    for (int i = 0; i < NUMBER_OF_PROBES; i += 1) {
      probeHistory[i].add(probeValues[i], true);
    }
  }
}

float *getProbeHistory(int probeNumber) {
  float *result = new float[HISTORY_LENGTH];
  for (int i = 0; i < HISTORY_LENGTH; i += 1) {
    float value = *probeHistory[probeNumber].peek(i);
    result[i] = value;
  }
  return result;
}

// init the ESP32 as a BLE device and set some scan parameters
void bleInit() {
  // Reset all probes to not connected
  for (int i = 0; i < NUMBER_OF_PROBES; i += 1) {
    probeValues[i] = PROBE_NOT_CONNECTED_VALUE;
  }
  if (!probeHistoryLoaded) {
    log_i("Pre filling history data");
    // loadup full history so we always have a sliding window, this makes it
    // easy to get data later
    for (int i = 0; i < NUMBER_OF_PROBES; i += 1) {
      for (int j = 0; j < HISTORY_LENGTH; j += 1) {
        probeHistory[i].add(PROBE_NOT_CONNECTED_VALUE);
      }
    }
    probeHistoryLoaded = true;
  }
  log_d("Starting ble scan");
  bteState = BT_STATE_SCANNING;
  BLEDevice::init("ESP32 Grill BT5.0");
  BLEScan *bleScan = BLEDevice::getScan();
  bleScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  bleScan->setActiveScan(true);
  bleScan->start(20);
}

struct ProbeData {
  uint16_t headerInformation;
  uint16_t probeData[NUMBER_OF_PROBES];
};

void fixUpIndianFormat(ProbeData *data) {
  for (int i = 0; i < NUMBER_OF_PROBES; i += 1) {
    data->probeData[i] = ntohs(data->probeData[i]);
  }
}

static void notifyCallback(
    BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData,
    size_t length,  // length is always 15 there are 6 probes...
    bool isNotify) {
  ProbeData *data = reinterpret_cast<ProbeData *>(pData);
  fixUpIndianFormat(data);

  for (int i = 0; i < NUMBER_OF_PROBES; i += 1) {
    if (data->probeData[i] == PROBE_NOT_CONNECTED_VALUE) {
      probeValues[i] = PROBE_NOT_CONNECTED_VALUE;
    } else {
      probeValues[i] = data->probeData[i] / 10.0f;
      // Filter bad values
      if (probeValues[i] > 600) {  // 572 is max probes can handle
        probeValues[i] = PROBE_NOT_CONNECTED_VALUE;
      } else if (probeValues[i] < 1) {  // 1 is min probes can handle
        probeValues[i] = PROBE_NOT_CONNECTED_VALUE;
      }
    }
  }
}

// connecting to the thermometer
bool connectToBLEServer(BLEAddress pAddress) {
  mClient = BLEDevice::createClient();

  mClient->setClientCallbacks(new MyClientCallback());
  mClient->connect(pAddress);

  BLERemoteService *pRemoteService = mClient->getService(serviceUUID);

  if (pRemoteService == nullptr) {
    log_w("Disconnecting as we can't get service");
    mClient->disconnect();
    return false;
  }

  BLERemoteCharacteristic *tempatureCharacteristic =
      pRemoteService->getCharacteristic(tempUUID);
  if (tempatureCharacteristic == nullptr) {
    log_w("Disconnecting as we can't get tempature characteristic");
    mClient->disconnect();
    return false;
  }

  tempatureCharacteristic->registerForNotify(notifyCallback);
  log_i("Registerd temprature notification");

  return true;
}

void bteLoop() {
  // doConnect is only true after the BLE scan finds a device
  // rights afterwards it is false again - also if the scan isn't successfully
  if (bteState == BT_STATE_CONNECTING) {
    bteState = (connectToBLEServer(*pServerAddress)) ? BT_STATE_CONNECTED
                                                     : BT_STATE_NA;
  } else if (bteState == BT_STATE_NA) {
    bleInit();
  }
  historyKeeperLoop();
}

void bteMock() {
  if (bteState == BT_STATE_NA) {
    float maxValue = 80.0f * 2;
    for (int i = 0; i < NUMBER_OF_PROBES; i += 1) {
      for (int j = 0; j < HISTORY_LENGTH; j += 1) {
        float x = (float)j / HISTORY_LENGTH * 2.0 * PI;

        probeHistory[i].add((sin(x) + 1.0) / 2.0 * maxValue);
        // probeHistory[i].add((80.0f / HISTORY_LENGTH) * j);
      }
    }
    bteState = BT_STATE_CONNECTED;
    for (int i = 0; i < NUMBER_OF_PROBES; i += 1) {
      probeValues[i] = 12.9;
    }
  }
  for (int i = 0; i < NUMBER_OF_PROBES; i += 1) {
    probeValues[i] += random(1, 10 + (i * 10)) / 10.0;
    if (probeValues[i] >= 300) {
      probeValues[i] = 1;
    }
  }
  historyKeeperLoop();
}
