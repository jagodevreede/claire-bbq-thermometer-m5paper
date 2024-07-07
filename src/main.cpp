#include <M5EPD.h>
#include <ESPmDNS.h>

#include "bte.h"

M5EPD_Canvas canvas(&M5.EPD);

void setup()
{
    M5.begin(true, false, true, true, false);
    M5.EPD.SetRotation(90);
    M5.TP.SetRotation(90);
    M5.EPD.Clear(true);
    canvas.createCanvas(540, 960);
    canvas.setTextSize(5);
    // canvas.drawString("Touch The Screen!", 20, 400);
    canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
}

void loop()
{
    bteLoop();
}
