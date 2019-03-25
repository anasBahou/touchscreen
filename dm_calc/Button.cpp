#include "Button.h"

Button::Button(const char* caption, uint16_t x, uint16_t y, uint16_t width, uint16_t height) :
  _caption(caption), _x0(x), _y0(y), _x1(x+width), _y1(y+height), _bgCol(BLACK), 
  _fgCol(WHITE), _bgColPressed(CYAN), _fgColPressed(BLACK)
{
  _enabled = true;
  _pressed = false;
  _func = NULL;
}

void Button::setColors(uint16_t bg, uint16_t fg, uint16_t bgPressed, uint16_t fgPressed)
{
  _bgCol = bg;
  _fgCol = fg;
  _bgColPressed = bgPressed;
  _fgColPressed = fgPressed;
}

bool Button::handle(uint16_t x, uint16_t y, bool pressed)
{
  bool needsRepaint = false;
  if (_enabled) {
    if (!pressed && _pressed) {
      // user released => click
      needsRepaint = true;
      _pressed = false;
      if (_func != NULL) {
        _func(_funcArg);
      }       
    }
    else if ((x >= _x0) && (y >= _y0) && (x <= _x1) && (y <= _y1)) {
      if (pressed && !_pressed) {
        // user pressing inside area
        needsRepaint = true;
        _pressed = true;
      } 
    }
  }
  return needsRepaint;
}

void Button::draw(DmTftBase* tft)
{
  if (_pressed) {
    tft->fillRectangle(_x0+1, _y0+1, _x1-1, _y1-1, _bgColPressed);
    tft->drawRectangle(_x0, _y0, _x1, _y1, _fgColPressed);
    tft->setTextColor(_bgColPressed, _fgColPressed);
    tft->drawStringCentered(_x0, _y0, _x1-_x0, _y1-_y0, _caption);
  } else {
    tft->fillRectangle(_x0+1, _y0+1, _x1-1, _y1-1, _bgCol);
    tft->drawRectangle(_x0, _y0, _x1, _y1, _fgCol);
    tft->setTextColor(_bgCol, _fgCol);
    tft->drawStringCentered(_x0, _y0, _x1-_x0, _y1-_y0, _caption);
  }
}
