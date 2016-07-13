#include <U8glib.h>
#include "Panner.h"

//#include "Keypad.h"
//#include "Trace.h"
//#include "Views.h"

/**
 *  +------------------------------
 *  |CurPos:   -12345
 *  |CurSpeed: 123
 *  |MaxSpeed: 1234
 *  |
 *  |MaxLeft:  -123456*  
 *  |MaxRight: 1234
 *  |MaxSpeed: 123
 *  |
 *  +------------------------------
 */


SimpleView::SimpleView()
{
}

SimpleView::~SimpleView()
{
}
 

void SimpleView::setup()
{
  
}
/*
void SimpleView::onKeyDown(uint8_t vk)
{
  DEBUG_PRINT("SimpleView::onKeyDown ");
  DEBUG_PRINTDEC(vk);
  DEBUG_PRINTLN("");
}

void SimpleView::onKeyUp(uint8_t vk)
{
  DEBUG_PRINT("SimpleView::onKeyUp ");
  DEBUG_PRINTDEC(vk);
  DEBUG_PRINTLN("");
}

void SimpleView::onLongKeyDown(uint8_t vk)
{
  DEBUG_PRINT("SimpleView::onLongKeyDown ");
  DEBUG_PRINTDEC(vk);
  DEBUG_PRINTLN("");
}
*/

/** ThumbStick APIs where vk is one of VK_xxx */
void SimpleView::onThumbDown()
{
  DEBUG_PRINTLN("SimpleView::onThumbDown");
}

void SimpleView::onThumbUp()
{
  DEBUG_PRINTLN("SimpleView::onThumbUp");
}

void SimpleView::onLongThumbDown()
{
  DEBUG_PRINTLN("SimpleView::onLongThumbDown");
}

void SimpleView::onThumbStickX(int16_t x)
{
  DEBUG_PRINT("SimpleView::onThumbStickX ");
  DEBUG_PRINTDEC(x);
  DEBUG_PRINTLN("");
}

void SimpleView::onThumbStickY(int16_t y)
{
  DEBUG_PRINT("SimpleView::onThumbStickY ");
  DEBUG_PRINTDEC(y);
  DEBUG_PRINTLN("");
}


