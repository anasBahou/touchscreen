#ifndef BUTTON_h
#define BUTTON_h

#include "DmTftBase.h"

typedef void (*cbFunc)(uint32_t);

class Button {
public:
  Button(const char* caption, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
  void setAction(cbFunc func, uint32_t arg) { _func = func; _funcArg = arg; }
  void setCaption(const char* caption) { _caption = caption; }
  void setColors(uint16_t bg, uint16_t fg, uint16_t bgPressed, uint16_t fgPressed);
  bool handle(uint16_t x, uint16_t y, bool pressed);
  void draw(DmTftBase* tft);

private:
  const char* _caption;
  uint16_t _x0, _y0, _x1, _y1;
  uint16_t _bgCol, _fgCol, _bgColPressed, _fgColPressed;
  bool _enabled, _pressed;
  cbFunc _func;
  uint32_t _funcArg;
};

#endif /* BUTTON_h */

