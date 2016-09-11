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

void Display::drawButton(RECT rButton, const ILI9341_t3_font_t *pFont, const char *szLabel, bool bEraseBkgnd)
{ 
  drawRoundRect(rButton.left, rButton.top, rButton.width(), rButton.height(), iButtonCornerRadius, uButtonBorderColor);
  rButton.deflate(iButtonCornerRadius);
  if(bEraseBkgnd || (szLabel == 0) || (szLabel[0] == '\0'))
    fillRect(rButton, uButtonFaceColor); // clear the entire button face
  if((szLabel == 0) || (szLabel[0] == '\0'))
    return;
  // draw the text!
  setTextColor(uButtonLabelColor, uButtonFaceColor);
  setFont(*pFont);
  setClipRect(rButton);

    int16_t gw = (rButton.width() - m_lcd.measureTextWidth(szLabel))/2;
    RECT rFill = rButton;
    rFill.right = rFill.left + gw;
    if(!bEraseBkgnd)
      fillRect(rFill, uButtonFaceColor); // clear the entire button face
    setCursor(rFill.right, rButton.top + ((rButton.height() - m_lcd.measureTextHeight(szLabel))/2));
    print(szLabel);
    rFill.right = rButton.right;
    rFill.left = rFill.right - gw;
    if(!bEraseBkgnd)
      fillRect(rFill, uButtonFaceColor); // clear the entire button face

  ILI9341_t3::setClipRect();
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

