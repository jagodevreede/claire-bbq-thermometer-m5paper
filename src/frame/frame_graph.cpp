#include "../bte.h"
#include "frame.h"

#define MAX_REDRAWS 100
static int pointsToDraw = 360;  // 1 hour

int Frame_Graph::run() {
  Frame_Base::run();
  if ((millis() - this->_lastUpdateTime) > 10000) {
    this->_lastUpdateTime = millis();
    this->drawGraph();
  }

  return 1;
}

void Frame_Graph::exit() {}

Frame_Graph::Frame_Graph(void) {
  _frame_name = "Frame_Graph";
  _frame_id = 1;

  this->canvas = new M5EPD_Canvas(&M5.EPD);
  this->canvas->createCanvas(900, 540);
  this->canvas->fillCanvas(0);
  this->canvas->setTextSize(26);
  this->canvas->setTextColor(15);
}

Frame_Graph::~Frame_Graph(void) {
  if (canvas != NULL) delete canvas;
}

void Frame_Graph::drawGraph() {
  // draw the graph with a rotation of 0, later we will set it back
  M5.EPD.SetRotation(M5EPD_Driver::ROTATE_0);
  M5.TP.SetRotation(GT911::ROTATE_0);
  this->canvas->clear();
  this->canvas->setTextSize(72);
  if (probeValues[selectedProbe] == PROBE_NOT_CONNECTED_VALUE) {
    this->canvas->drawString("N/A", 350, 10);
  } else {
    this->canvas->drawString(String(probeValues[selectedProbe], 1), 350, 10);
  }
  this->canvas->setTextSize(26);
  this->canvas->drawLine(100, 0, 100, 500, 15);
  this->canvas->drawLine(820, 500, 100, 500, 15);

  float* probeHistory = getProbeHistory(selectedProbe);
  float maxValue = findMaxTempInHistory(probeHistory);

  for (int i = 1; i < 10; i += 1) {
    this->canvas->drawLine(820, i * 50, 90, i * 50, 15);
    this->canvas->drawNumber(maxValue - ((maxValue / 10) * i), 40,
                             (i * 50) - 10);
  }

  // draw time lines
  for (int i = 0; i < 60; i += 1) {
    this->canvas->drawLine(100 + (720 / 60) * i, 500, 100 + (720 / 60) * i, 510,
                           15);
  }

  // X = time (100 = 1h ago 820 is now)
  // Y = temp scal (500 = 0; 0 = max value)
  int32_t lastPointX = 100;
  int32_t lastPointY = 500;
  for (int i = 0; i < pointsToDraw; i += 1) {
    float temp = probeHistory[i + HISTORY_LENGTH - pointsToDraw];
    int32_t newPointX = 100 + (720 / pointsToDraw) * i;

    if (temp == PROBE_NOT_CONNECTED_VALUE) {
      // skip value but move time
      lastPointX = newPointX;
      continue;
    }

    int32_t newPointY = 500 - (temp / maxValue * 500.0f);
    this->canvas->drawLine(lastPointX, lastPointY, newPointX, newPointY, 15);
    lastPointX = newPointX;
    lastPointY = newPointY;
  }
  updateCounter++;
  if (updateCounter > MAX_REDRAWS) {
    // Force redraw
    this->canvas->pushCanvas(60, 0, UPDATE_MODE_GC16);
    updateCounter = 0;
  } else {
    this->canvas->pushCanvas(60, 0, UPDATE_MODE_DU);
  }
  M5.EPD.SetRotation(M5EPD_Driver::ROTATE_90);
  M5.TP.SetRotation(GT911::ROTATE_90);
}

float Frame_Graph::findMaxTempInHistory(float* probeHistory) {
  float maxValue = 100;
  for (int i = 0; i < pointsToDraw; i += 1) {
    float temp = probeHistory[i + HISTORY_LENGTH - pointsToDraw];
    if (temp == PROBE_NOT_CONNECTED_VALUE) {
      continue;
    }
    if (temp > maxValue) {
      if (temp < 100) {
        maxValue = 100;
      } else if (temp < 200) {
        maxValue = 100;
      }
      return 300;
    }
  }
  return maxValue;
}

int Frame_Graph::init(epdgui_args_vector_t& args) {
  _is_run = 1;
  this->drawGraph();
  return 3;
}