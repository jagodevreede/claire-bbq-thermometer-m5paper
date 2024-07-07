#ifndef _FRAME_HOME_H_
#define _FRAME_HOME_H_

#include "frame_base.h"
#include "../epdgui/epdgui.h"
#include "../bte.h"

class Frame_Home : public Frame_Base
{
public:
    Frame_Home();
    void createFontSizes();
    ~Frame_Home();
    int init(epdgui_args_vector_t &args);
    int run();

private:
    EPDGUI_Temp *_tp_probe[NUMBER_OF_PROBES];
    EPDGUI_Temp *_infoBox;

    float lastKnownprobeValues[NUMBER_OF_PROBES] = {};
    int updateCounter[NUMBER_OF_PROBES] = {};
};

#endif //_FRAME_HOME_H_