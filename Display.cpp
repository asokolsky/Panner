#include "Panner.h"

const uint8_t pinCS = 10;
const uint8_t pinDC = 9;

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


