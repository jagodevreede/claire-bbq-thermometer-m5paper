#ifndef __EPDGUI_H
#define __EPDGUI_H

#include "../frame/frame_base.h"
#include "epdgui_base.h"
#include "epdgui_button.h"
#include "epdgui_graph.h"
#include "epdgui_mutexswitch.h"
#include "epdgui_switch.h"
#include "epdgui_temp.h"
#include "epdgui_textbox.h"

void EPDGUI_AddObject(EPDGUI_Base* object);
void EPDGUI_Draw(m5epd_update_mode_t mode = UPDATE_MODE_GC16);
void EPDGUI_Process(void);
void EPDGUI_Process(int16_t x, int16_t y);
void EPDGUI_Clear(void);
void EPDGUI_Run(Frame_Base* frame);
void EPDGUI_MainLoop(void);
void EPDGUI_PushFrame(Frame_Base* frame);
void EPDGUI_PopFrame(bool isDelete = false);
void EPDGUI_OverwriteFrame(Frame_Base* frame);
void EPDGUI_AddFrame(String name, Frame_Base* frame);
void EPDGUI_AddFrameArg(String name, int n, void* arg);
Frame_Base* EPDGUI_GetFrame(String name);
void EPDGUI_SetAutoUpdate(bool isAuto);

extern uint32_t g_last_active_time_millis;

void EPDGUI_UpdateGlobalLastActiveTime();

#endif  //__EPDGUI_H