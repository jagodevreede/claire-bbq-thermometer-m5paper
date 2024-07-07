#include "frame_home.h"
#include "frame_base.h"

Frame_Home::Frame_Home(void) : Frame_Base()
{
    _frame_name = "Frame_Home";
    _frame_id = 0;

    _tp_probe1 = new EPDGUI_Temp("_tp_probe1", 20, 44 + 72, 228, 228);
    _tp_probe2 = new EPDGUI_Temp("_tp_probe2", 288, 44 + 72, 228, 228);
    _tp_probe3 = new EPDGUI_Temp("_tp_probe3", 20, 324 + 72, 228, 228);
    _tp_probe4 = new EPDGUI_Temp("_tp_probe4", 288, 324 + 72, 228, 228);
    _tp_probe5 = new EPDGUI_Temp("_tp_probe5", 20, 604 + 72, 228, 228);
    _tp_probe6 = new EPDGUI_Temp("_tp_probe6", 288, 604 + 72, 228, 228);
    _tp_probe1->SetHide(true);
}

Frame_Home::~Frame_Home(void)
{
    delete _tp_probe1;
    delete _tp_probe2;
    delete _tp_probe3;
    delete _tp_probe4;
    delete _tp_probe5;
    delete _tp_probe6;
}

int Frame_Home::init(epdgui_args_vector_t &args)
{
    _is_run = 1;
    M5.EPD.Clear();
    EPDGUI_AddObject(_tp_probe1);
    EPDGUI_AddObject(_tp_probe2);
    EPDGUI_AddObject(_tp_probe3);
    EPDGUI_AddObject(_tp_probe4);
    EPDGUI_AddObject(_tp_probe5);
    EPDGUI_AddObject(_tp_probe6);
    forceStatusBarUpdate();
    return 3;
}

int Frame_Home::run()
{
    Frame_Base::run();
    return 1;
}