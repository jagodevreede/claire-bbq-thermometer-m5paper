#include "EPDGUI_Graph.h"

#include "../bte.h"
#include "../frame/frame.h"
#include "Free_Fonts.h"

#define MAX_REDRAWS 200

EPDGUI_Graph::EPDGUI_Graph(int16_t x, int16_t y) : EPDGUI_Base(x, y, 900, 540) {
  this->canvas = new M5EPD_Canvas(&M5.EPD);
  this->canvas->createCanvas(900, 540);
  this->canvas->fillCanvas(0);
  this->canvas->setTextSize(26);
  this->canvas->setTextColor(15);
}

EPDGUI_Graph::~EPDGUI_Graph() { delete this->canvas; }

void EPDGUI_Graph::drawGraph() {
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
  if (pointsToDraw == 360) {
    this->canvas->drawString("1h", 0, 510);
  } else if (pointsToDraw == 60) {
    this->canvas->drawString("10m", 0, 510);
  } else {
    this->canvas->drawString("2h", 0, 510);
  }

  this->canvas->drawLine(100, 0, 100, 500, 15);
  this->canvas->drawLine(820, 500, 100, 500, 15);

  float *probeHistory = getProbeHistory(selectedProbe);
  float maxValue = findMaxTempInHistory(probeHistory);

  for (int i = 1; i < 10; i += 1) {
    this->canvas->drawLine(820, i * 50, 90, i * 50, 15);
    this->canvas->drawNumber(maxValue - ((maxValue / 10) * i), 40,
                             (i * 50) - 10);
  }

  int numberOfLines = pointsToDraw / 6;
  // draw time lines
  for (int i = 0; i < numberOfLines; i += 1) {
    this->canvas->drawLine(100 + (720 / numberOfLines) * i, 500,
                           100 + (720 / numberOfLines) * i, 510, 15);
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
    this->canvas->pushCanvas(_x, _y, UPDATE_MODE_GC16);
    updateCounter = 0;
  } else {
    this->canvas->pushCanvas(_x, _y, UPDATE_MODE_DU);
  }
  M5.EPD.SetRotation(M5EPD_Driver::ROTATE_90);
  M5.TP.SetRotation(GT911::ROTATE_90);
}

void EPDGUI_Graph::Draw(m5epd_update_mode_t mode) { this->drawGraph(); }

void EPDGUI_Graph::Draw(M5EPD_Canvas *canvas) { this->drawGraph(); }

void EPDGUI_Graph::update() {
  if ((millis() - this->_lastUpdateTime) > 10000) {
    this->_lastUpdateTime = millis();
    this->drawGraph();
  }
}

float EPDGUI_Graph::findMaxTempInHistory(float *probeHistory) {
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

void EPDGUI_Graph::Bind(int16_t event,
                        void (*func_cb)(epdgui_args_vector_t &)) {
  // nothing to bind
}

void EPDGUI_Graph::UpdateState(int16_t x, int16_t y) {
  if (_ishide) {
    return;
  }

  bool is_in_area = isInBox(x, y);

  if (is_in_area) {
    if (pointsToDraw == 360) {  // 1h
      pointsToDraw = 60;        // 10m
    } else if (pointsToDraw == 60) {
      pointsToDraw = 720;  // 2h
    } else {
      pointsToDraw = 360;
    }

    drawGraph();
    if (_pressed_cb != NULL) {
      _pressed_cb(_pressed_cb_args);
    }
  }
}
