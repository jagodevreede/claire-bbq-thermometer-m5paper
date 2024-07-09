#include "../bte.h"
#include "frame.h"

#define MAX_REDRAWS 200

void key_button1_pressed(epdgui_args_vector_t &args) {
  selectedProbe = 1;
  log_d("Opening probe %d graph", selectedProbe);
  Frame_Base *frame = EPDGUI_GetFrame("Frame_Graph");
  EPDGUI_PushFrame(frame);
}

void key_button2_pressed(epdgui_args_vector_t &args) {
  selectedProbe = 2;
  log_d("Opening probe %d graph", selectedProbe);
  Frame_Base *frame = EPDGUI_GetFrame("Frame_Graph");
  EPDGUI_PushFrame(frame);
}

void key_button3_pressed(epdgui_args_vector_t &args) {
  selectedProbe = 3;
  log_d("Opening probe %d graph", selectedProbe);
  Frame_Base *frame = EPDGUI_GetFrame("Frame_Graph");
  EPDGUI_PushFrame(frame);
}
void key_button4_pressed(epdgui_args_vector_t &args) {
  selectedProbe = 4;
  log_d("Opening probe %d graph", selectedProbe);
  Frame_Base *frame = EPDGUI_GetFrame("Frame_Graph");
  EPDGUI_PushFrame(frame);
}
void key_button5_pressed(epdgui_args_vector_t &args) {
  selectedProbe = 5;
  log_d("Opening probe %d graph", selectedProbe);
  Frame_Base *frame = EPDGUI_GetFrame("Frame_Graph");
  EPDGUI_PushFrame(frame);
}
void key_button0_pressed(epdgui_args_vector_t &args) {
  selectedProbe = 0;
  log_d("Opening probe %d graph", selectedProbe);
  Frame_Base *frame = EPDGUI_GetFrame("Frame_Graph");
  EPDGUI_PushFrame(frame);
}
Frame_Home::Frame_Home(void) : Frame_Base() {
  _frame_name = "Frame_Home";
  _frame_id = 0;

  createFontSizes();

  _tp_probe[0] = new EPDGUI_Temp("", 20, 44 + 72, 228, 228);
  _tp_probe[1] = new EPDGUI_Temp("", 288, 44 + 72, 228, 228);
  _tp_probe[2] = new EPDGUI_Temp("", 20, 324 + 72, 228, 228);
  _tp_probe[3] = new EPDGUI_Temp("", 288, 324 + 72, 228, 228);
  _tp_probe[4] = new EPDGUI_Temp("", 20, 604 + 72, 228, 228);
  _tp_probe[5] = new EPDGUI_Temp("", 288, 604 + 72, 228, 228);

  // Hide all probes by default
  for (int i = 0; i < NUMBER_OF_PROBES; i += 1) {
    _tp_probe[i]->SetHide(true);
  }
  _tp_probe[0]->Bind(EPDGUI_Button::EVENT_RELEASED, key_button0_pressed);
  _tp_probe[1]->Bind(EPDGUI_Button::EVENT_RELEASED, key_button1_pressed);
  _tp_probe[2]->Bind(EPDGUI_Button::EVENT_RELEASED, key_button2_pressed);
  _tp_probe[3]->Bind(EPDGUI_Button::EVENT_RELEASED, key_button3_pressed);
  _tp_probe[4]->Bind(EPDGUI_Button::EVENT_RELEASED, key_button4_pressed);
  _tp_probe[5]->Bind(EPDGUI_Button::EVENT_RELEASED, key_button5_pressed);
}

void Frame_Home::createFontSizes() {
  M5EPD_Canvas canvas_temp(&M5.EPD);
  canvas_temp.createRender(72);
}

Frame_Home::~Frame_Home(void) {
  delete _tp_probe[0];
  delete _tp_probe[1];
  delete _tp_probe[2];
  delete _tp_probe[3];
  delete _tp_probe[4];
  delete _tp_probe[5];
}

int Frame_Home::init(epdgui_args_vector_t &args) {
  _is_run = 1;
  _frame_id = 0;
  log_d("Init home screen");
  M5.EPD.Clear();
  EPDGUI_AddObject(_tp_probe[0]);
  EPDGUI_AddObject(_tp_probe[1]);
  EPDGUI_AddObject(_tp_probe[2]);
  EPDGUI_AddObject(_tp_probe[3]);
  EPDGUI_AddObject(_tp_probe[4]);
  EPDGUI_AddObject(_tp_probe[5]);
  forceStatusBarUpdate();
  for (int i = 0; i < NUMBER_OF_PROBES; i += 1) {
    lastKnownprobeValues[i] = PROBE_NOT_CONNECTED_VALUE;
  }
  return 3;
}

int Frame_Home::run() {
  Frame_Base::run();
  for (int i = 0; i < NUMBER_OF_PROBES; i += 1) {
    if (lastKnownprobeValues[i] != probeValues[i]) {
      if (probeValues[i] == PROBE_NOT_CONNECTED_VALUE) {
        log_d("Probe %d not connected removing from display", i);
        _tp_probe[i]->SetHide(true);
        _tp_probe[i]->Draw(UPDATE_MODE_GC16);
      } else {
        log_v("Probe %d updated to %f from %f", i, probeValues[i],
              lastKnownprobeValues[i]);
        _tp_probe[i]->SetHide(false);
        _tp_probe[i]->setLabel(String(probeValues[i], 1));
        updateCounter[i]++;
        if (updateCounter[i] > MAX_REDRAWS) {
          // Force redraw
          _tp_probe[i]->Draw(UPDATE_MODE_GC16);
          updateCounter[i] = 0;
        } else {
          _tp_probe[i]->Draw(UPDATE_MODE_DU);
        }
      }
      lastKnownprobeValues[i] = probeValues[i];
    } else {
      log_v("Probe %d same as last update cycle, %f %f", i,
            lastKnownprobeValues[i], probeValues[i]);
    }
  }

  return 1;
}