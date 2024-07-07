#ifndef __BTE_H
#define __BRE_H

#include <Arduino.h>

#define NUMBER_OF_PROBES 6
#define PROBE_NOT_CONNECTED_VALUE 65535

static float probeValues[NUMBER_OF_PROBES] = {};
static boolean bteScanning = false;
static boolean bteConnecting = false;
static boolean bteConnected = false;

void bteLoop();

#endif //__BRE_H