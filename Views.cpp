#include <U8glib.h>
#include "Panner.h"

//#include "Keypad.h"
//#include "Trace.h"
//#include "Views.h"



SimpleView::SimpleView()
{
}

SimpleView::~SimpleView()
{
}
 

void SimpleView::setup()
{
  
}

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


