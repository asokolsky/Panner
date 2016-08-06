#include "Panner.h"
//#include "Views.h"
//#include "ThumbStick.h"


/** Just a public constructor */
ThumbStick::ThumbStick(uint8_t pinX, uint8_t pinY, uint8_t pinButton) : 
  DeBouncedButton(pinButton), m_pinX(pinX), m_pinY(pinY)
{
  // Setup the X/Y inputs with an internal pull-up :
  pinMode(m_pinX, INPUT);
  pinMode(m_pinY, INPUT);

  m_x = analogRead(m_pinX);
  m_y = analogRead(m_pinY);
}


bool ThumbStick::update()
{
  // careful here!
  // the order is important because of the side effects
  uint16_t x = analogRead(m_pinX);
  uint16_t y = analogRead(m_pinY);
  bool res = DeBouncedButton::update() || (x != m_x) || (y != m_y);
  m_x = x;
  m_y = y;
  return res;
}

/**
 *  call this (alternative to update) from the main loop passing to it the result of millis();
 *  It will call
 *         pView->onThumbDown();
 *         pView->onThumbUp();
 *         //pView->onLongThumbDown();
 *         pView->onThumbStickX(uint16_t x);
 *         pView->onThumbStickY(uint16_t y);
 */
bool ThumbStick::getAndDispatchThumb(unsigned long now)
{
  // careful here!
  // the order is important because of the side effects
  uint16_t x = analogRead(m_pinX);
  uint16_t y = analogRead(m_pinY);
  if(DeBouncedButton::update())
  {
    if(justPressed())
      View::g_pActiveView->onThumbDown();
    else
      View::g_pActiveView->onThumbUp();  
    return true;
  }  
  if(abs(y - m_y) > 10) { // ignore +-10
    m_y = y;
    View::g_pActiveView->onThumbStickY(getYmapped());
    return true;
  }
  if(abs(x - m_x) > 10) { // ignore +-10
    m_x = x;
    View::g_pActiveView->onThumbStickX(getXmapped());
    return true;
  }
  return false;
}

