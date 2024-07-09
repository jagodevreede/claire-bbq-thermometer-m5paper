#ifndef __EPDGUI_Graph_H
#define __EPDGUI_Graph_H

#include "epdgui.h"

class EPDGUI_Graph : public EPDGUI_Base {
 public:
  static const int16_t EVENT_NONE = 0;
  static const int16_t EVENT_PRESSED = 1;
  static const int16_t EVENT_RELEASED = 2;

 public:
  EPDGUI_Graph(int16_t x, int16_t y);
  ~EPDGUI_Graph();
  void Draw(m5epd_update_mode_t mode = UPDATE_MODE_DU4);
  void Draw(M5EPD_Canvas *canvas);
  void drawGraph();
  float findMaxTempInHistory(float *probeHistory);
  void update();
  void Bind(int16_t event, void (*func_cb)(epdgui_args_vector_t &));
  void setLabel(String label);
  void UpdateState(int16_t x, int16_t y);

 private:
  void (*_pressed_cb)(epdgui_args_vector_t &args) = NULL;
  epdgui_args_vector_t _pressed_cb_args;

 public:
  M5EPD_Canvas *canvas = NULL;
  unsigned long _lastUpdateTime = 0;
  int updateCounter = 0;
  int pointsToDraw = 360;  // 1 hour by default
};

#endif  //__EPDGUI_Graph_H