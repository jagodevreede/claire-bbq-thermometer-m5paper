#include "frame_base.h"

#include "../epdgui/epdgui.h"
#include "bte.h"

Frame_Base::Frame_Base() {
  _bar = new M5EPD_Canvas(&M5.EPD);
  _bar->createCanvas(540, 44);
  _bar->setTextSize(26);

  _time = 0;
  forceStatusBarUpdate();
  StatusBar(UPDATE_MODE_NONE);

  EPDGUI_UpdateGlobalLastActiveTime();
}

void Frame_Base::forceStatusBarUpdate() { _next_update_time = 0; }

Frame_Base::~Frame_Base() {
  if (_key_exit != NULL) delete _key_exit;
}

void Frame_Base::StatusBar(m5epd_update_mode_t mode) {
  if ((millis() - _time) < _next_update_time && lastKnownBtState == bteState) {
    return;
  }
  lastKnownBtState = bteState;
  char buf[20];
  _bar->fillCanvas(0);
  _bar->drawFastHLine(0, 43, 540, 15);
  _bar->setTextDatum(CL_DATUM);
  _bar->drawString("Grill 5.0BT", 10, 27);

  // Battery
  _bar->setTextDatum(CR_DATUM);
  _bar->pushImage(498, 8, 32, 32, ImageResource_status_bar_battery_32x32);
  uint32_t vol = M5.getBatteryVoltage();

  if (vol < 3300) {
    vol = 3300;
  } else if (vol > 4350) {
    vol = 4350;
  }
  float battery = (float)(vol - 3300) / (float)(4350 - 3300);
  if (battery <= 0.01) {
    battery = 0.01;
  }
  if (battery > 1) {
    battery = 1;
  }
  uint8_t px = battery * 25;
  sprintf(buf, "%d%%", (int)(battery * 100));
  _bar->fillRect(498 + 3, 8 + 10, px, 13, 15);

  // bt connection status
  switch (bteState) {
    case BT_STATE_CONNECTING:
      sprintf(buf, "Connecting...");
      break;
    case BT_STATE_SCANNING:
      sprintf(buf, "Scanning...");
      break;
    case BT_STATE_CONNECTED:
      sprintf(buf, "Connected");
      break;
    default:
      sprintf(buf, "Not connected");
  }

  _bar->setTextDatum(CC_DATUM);
  _bar->drawString(buf, 270, 27);
  _bar->pushCanvas(0, 0, mode);

  // Time
  rtc_time_t time_struct;
  M5.RTC.getTime(&time_struct);

  _time = millis();
  _next_update_time = (60 - time_struct.sec) * 1000;
}

void Frame_Base::exitbtn(String title, uint16_t width) {
  _key_exit = new EPDGUI_Button(8, 12, width, 48);
  _key_exit->CanvasNormal()->fillCanvas(0);
  _key_exit->CanvasNormal()->setTextSize(26);
  _key_exit->CanvasNormal()->setTextDatum(CL_DATUM);
  _key_exit->CanvasNormal()->setTextColor(15);
  _key_exit->CanvasNormal()->drawString(title, 47 + 13, 28);
  _key_exit->CanvasNormal()->pushImage(15, 8, 32, 32,
                                       ImageResource_item_icon_arrow_l_32x32);
  *(_key_exit->CanvasPressed()) = *(_key_exit->CanvasNormal());
  _key_exit->CanvasPressed()->ReverseColor();
}

int Frame_Base::run(void) {
  StatusBar(UPDATE_MODE_GL16);
  return _is_run;
}

void Frame_Base::exit(void) {}

void Frame_Base::exit_cb(epdgui_args_vector_t &args) {
  EPDGUI_PopFrame();
  *((int *)(args[0])) = 0;
}
