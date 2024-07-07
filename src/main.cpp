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

void loop()
{
    EPDGUI_MainLoop();
    bteLoop();
    // bteMock();

    delay(2000);
}
