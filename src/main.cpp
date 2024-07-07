#include <M5EPD.h>
#include <ESPmDNS.h>

#include "bte.h"
#include "systeminit.h"
#include "epdgui/epdgui.h"

M5EPD_Canvas canvas(&M5.EPD);

void setup()
{
    SysInit_Start();
}

void shutdown()
{
    log_w("Now the system is shutting down.");
    M5.EPD.Clear();
    M5.EPD.WritePartGram4bpp(92, 182, 356, 300, ImageResource_logo_356x300);
    M5.EPD.UpdateFull(UPDATE_MODE_GC16);
    M5.EPD.UpdateFull(UPDATE_MODE_GC16);
    M5.disableEPDPower();
    M5.disableEXTPower();
    M5.disableMainPower();
    esp_deep_sleep_start();
}

void loop()
{
    EPDGUI_MainLoop();
    bteLoop();
    // bteMock();

    M5.update();

    if (M5.BtnP.isPressed())
    {
        shutdown();
    }
    delay(2000);
}
