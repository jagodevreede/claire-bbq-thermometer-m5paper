#ifndef _FRAME_HOME_H_
#define _FRAME_HOME_H_

#include "frame_base.h"
#include "../epdgui/epdgui.h"

class Frame_Home : public Frame_Base
{
public:
    Frame_Home();
    ~Frame_Home();
    int init(epdgui_args_vector_t &args);
    int run();

private:
    EPDGUI_Temp *_tp_probe1;
    EPDGUI_Temp *_tp_probe2;
    EPDGUI_Temp *_tp_probe3;
    EPDGUI_Temp *_tp_probe4;
    EPDGUI_Temp *_tp_probe5;
    EPDGUI_Temp *_tp_probe6;
};

#endif //_FRAME_HOME_H_