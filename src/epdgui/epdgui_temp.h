#ifndef __EPDGUI_TEMP_H
#define __EPDGUI_TEMP_H

#include "epdgui_base.h"

class EPDGUI_Temp : public EPDGUI_Base {
 public:
  static const int16_t EVENT_NONE = 0;
  static const int16_t EVENT_PRESSED = 1;
  static const int16_t EVENT_RELEASED = 2;

 public:
  EPDGUI_Temp(String label, int16_t x, int16_t y, int16_t w, int16_t h);
  ~EPDGUI_Temp();
  void Draw(m5epd_update_mode_t mode = UPDATE_MODE_DU4);
  void Draw(M5EPD_Canvas *canvas);
  void Bind(int16_t event, void (*func_cb)(epdgui_args_vector_t &));
  void setLabel(String label);
  void UpdateState(int16_t x, int16_t y);
  String getLabel(void) { return _label; }

 private:
  void (*_pressed_cb)(epdgui_args_vector_t &args) = NULL;
  epdgui_args_vector_t _pressed_cb_args;
  int16_t _state = EVENT_NONE;
  String _label;
  int8_t _probeIndex;

 public:
  M5EPD_Canvas *_CanvasNormal = NULL;
  M5EPD_Canvas *_CanvasEmpty = NULL;
};

#endif  //__EPDGUI_TEMP_H