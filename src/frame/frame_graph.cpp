#include "frame.h"

int Frame_Graph::run() {
  Frame_Base::run();
  _graph_ui->update();
  return 1;
}

void Frame_Graph::exit() {}

Frame_Graph::Frame_Graph(void) {
  _frame_name = "Frame_Graph";
  _frame_id = 1;
  _graph_ui = new EPDGUI_Graph(60, 0);
}

Frame_Graph::~Frame_Graph(void) {
  if (_graph_ui != NULL) delete _graph_ui;
}

int Frame_Graph::init(epdgui_args_vector_t& args) {
  _is_run = 1;
  EPDGUI_AddObject(_graph_ui);
  return 3;
}