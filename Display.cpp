#include "Panner.h"

const uint8_t pinCS = 10;
const uint8_t pinDC = 9;

/**
 *  Globals
 */
Display m_lcd;

/**
 * Class Implementation
 */
Display::Display() : ILI9341_t3(pinCS, pinDC /* uint8_t _RST = 255, uint8_t _MOSI=11, uint8_t _SCLK=13, uint8_t _MISO=12 */)
{
  
}

/**
 * Called once to set things up.
 */
void Display::setup()
{
  begin();
  fillScreen(ILI9341_BLACK);
  setRotation(1);
  setTextWrap(false);
}

#ifdef DEBUG
void Display::DUMP(const char *szText /* = 0*/)
{
  if(szText != 0) {
    DEBUG_PRINT(szText);
  }
  DEBUG_PRINT(" Display@"); DEBUG_PRINTDEC((int)this); 
  DEBUG_PRINT(" _width="); DEBUG_PRINTDEC(_width); DEBUG_PRINT(" _height="); DEBUG_PRINTDEC(_height); 
  
  //int16_t  cursor_x, cursor_y;
  RECT r;
  r.left = _clipx1;
  r.right = _clipx2;
  r.top = _clipy1;
  r.top = _clipy2;
  r.DUMP(" clipRect: ");

  //int16_t  origin_x, origin_y;
  
  r.left = rel_clipx1;
  r.right = rel_clipx2;
  r.top = rel_clipy1;
  r.top = rel_clipy2;
  r.DUMP(" rel_clipRect: ");
  
  //uint16_t textcolor, textbgcolor;
  //uint8_t textsize, rotation;  
}
#endif

