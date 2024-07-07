#include "systeminit.h"
#include "epdgui/epdgui.h"
#include "frame/frame.h"
#include "Free_Fonts.h"
#include "resources/binaryttf.h"
#include <WiFi.h>

M5EPD_Canvas _initcanvas(&M5.EPD);

QueueHandle_t xQueue_Info = xQueueCreate(20, sizeof(uint32_t));

void SysInit_Start(void)
{
    bool ret = false;
    M5.begin(true, false, true, true, false);
    M5.EPD.Clear(true);
    M5.EPD.SetRotation(M5EPD_Driver::ROTATE_90);
    M5.TP.SetRotation(GT911::ROTATE_90);

    M5.BatteryADCBegin();

    _initcanvas.loadFont(binaryttf, sizeof(binaryttf));

    taskYIELD();

    _initcanvas.createRender(26, 128);

    // Frame_Main *frame_main = new Frame_Main();
    // EPDGUI_PushFrame(frame_main);

    Frame_Home *frame_home = new Frame_Home();
    EPDGUI_PushFrame(frame_home);
}