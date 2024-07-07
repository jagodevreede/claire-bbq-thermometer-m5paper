#ifndef __BTE_H
#define __BTE_H

#include <Arduino.h>

#define NUMBER_OF_PROBES 6
#define HISTORY_LENGTH 600
#define PROBE_NOT_CONNECTED_VALUE 65535

#define BT_STATE_NA 0
#define BT_STATE_SCANNING 1
#define BT_STATE_CONNECTING 2
#define BT_STATE_CONNECTED 3


extern float probeValues[NUMBER_OF_PROBES];
extern float probeHistory[NUMBER_OF_PROBES][HISTORY_LENGTH];
extern byte bteState;

void bteLoop();
void bteMock();

#endif  //__BTE_H