#include "epdgui_temp.h"
#include "Free_Fonts.h"

EPDGUI_Temp::EPDGUI_Temp(String label, int16_t x, int16_t y, int16_t w,
                         int16_t h) : EPDGUI_Base(x, y, w, h)
{
    this->_label = label;

    this->_CanvasNormal = new M5EPD_Canvas(&M5.EPD);
    this->_CanvasNormal->createCanvas(_w, _h);
    this->_CanvasNormal->fillCanvas(0);
    this->_CanvasNormal->setTextColor(15);

    this->_CanvasNormal->drawRect(0, 0, _w, _h, 15);

    this->_CanvasEmpty = new M5EPD_Canvas(&M5.EPD);
    this->_CanvasEmpty->createCanvas(_w, _h);
    this->_CanvasEmpty->fillCanvas(0);
}

EPDGUI_Temp::~EPDGUI_Temp()
{
    delete this->_CanvasNormal;
}

void EPDGUI_Temp::Draw(m5epd_update_mode_t mode)
{
    if (_ishide)
    {
        this->_CanvasEmpty->pushCanvas(_x, _y, mode);
    }
    else
    {
        this->_CanvasNormal->pushCanvas(_x, _y, mode);
    }
}

void EPDGUI_Temp::Draw(M5EPD_Canvas *canvas)
{
    if (_ishide)
    {
        this->_CanvasEmpty->pushToCanvas(_x, _y, canvas);
    }
    else
    {
        _CanvasNormal->pushToCanvas(_x, _y, canvas);
    }
}

void EPDGUI_Temp::Bind(int16_t event,
                       void (*func_cb)(epdgui_args_vector_t &))
{
    // Nothing to bind
}

void EPDGUI_Temp::setLabel(String label)
{
    _label = label;

    this->_CanvasNormal->fillCanvas(0);
    this->_CanvasNormal->drawRect(0, 0, _w, _h, 15);
    this->_CanvasNormal->setTextSize(26);
    this->_CanvasNormal->setTextDatum(TC_DATUM);
    this->_CanvasNormal->drawString(_label, _w / 2, _h / 2 + 3);
}

void EPDGUI_Temp::AddArgs(int16_t event, uint16_t n, void *arg)
{
}

void EPDGUI_Temp::UpdateState(int16_t x, int16_t y)
{
}
