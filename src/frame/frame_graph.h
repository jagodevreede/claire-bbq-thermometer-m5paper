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
  void exit();

 private:
  EPDGUI_Graph *_graph_ui;
};

#endif  //_FRAME_GRAPH_H_