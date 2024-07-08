#ifndef _FRAME_GRAPH_H_
#define _FRAME_GRAPH_H_

#include "../epdgui/epdgui.h"
#include "frame_base.h"

class Frame_Graph : public Frame_Base {
 public:
  Frame_Graph();
  ~Frame_Graph();
  int init(epdgui_args_vector_t &args);
  int run();
  void drawGraph();
  float findMaxTempInHistory(float *probeHistory);
  void exit();

 private:
  M5EPD_Canvas *canvas;
  unsigned long _lastUpdateTime = 0;
  int updateCounter = 0;
};

#endif  //_FRAME_GRAPH_H_