#define NODEBUG 1

#include "Panner.h"
//#include "Trace.h"
//#include "Views.h"

#ifndef  NODEBUG
/** debug/trace helper */
static const char *getKeyName(uint8_t vk) {
  switch(vk) {
    case VK_NONE: return "VK_NONE";
    case VK_RIGHT: return "VK_RIGHT";
    case VK_UP: return "VK_UP";
    case VK_DOWN: return "VK_DOWN";
    case VK_LEFT: return "VK_LEFT";
    case VK_SEL: return "VK_SEL";  
  }
  return "VK_???";
}
#endif

Keypad::Keypad(uint8_t bPin) : m_bPin(bPin)
{
  analogReadResolution(10);
  pinMode(bPin, INPUT_PULLUP);
}

/**
 * get one of VK_xxx
 */
uint8_t Keypad::getKey()
{
  int adc_key_in = analogRead(m_bPin);      // read the value from the sensor 
/*
  DEBUG_PRINT("analogRead(m_bPin=");
  DEBUG_PRINTDEC(m_bPin);
  DEBUG_PRINT(") =>");
  DEBUG_PRINTDEC(adc_key_in);
  DEBUG_PRINTLN("");
*/
  // buttons when read are centered at these valies: 0, 152, 342, 525, 756
  // we add approx 50 to those values and check to see if we are close

  // 1st option for speed reasons since it will be the most likely result
  if (adc_key_in > 900) {
    //DEBUG_PRINTLN("Keypad::getKey() => VK_NONE");
    return VK_NONE;
  }
  if (adc_key_in < 80) {
    //DEBUG_PRINTLN("Keypad::getKey() => VK_RIGHT");
    return VK_RIGHT;  
  }
  if (adc_key_in < 250) {
    //DEBUG_PRINTLN("Keypad::getKey() => VK_DOWN");
    return VK_DOWN;
  }
  if (adc_key_in < 450) {
    //DEBUG_PRINTLN("Keypad::getKey() => VK_UP");
    return VK_UP;
  }
  if (adc_key_in < 650) {
    //DEBUG_PRINTLN("Keypad::getKey() => VK_LEFT");
    return VK_LEFT;
  }
  //DEBUG_PRINTLN("Keypad::getKey() => VK_SEL");
  return VK_SEL;
}

bool Keypad::getAndDispatchKey(unsigned long ulNow)
{
  // get out if we are bouncing!
  if(ulNow < m_ulBounceSubsided)
    return false;
  
  uint8_t vk = getKey();
  if(vk == m_bOldKey) { 
    if(vk == VK_NONE)
      return false;
    // fire long key logic here
    if((m_ulToFireLongKey == 0) || (ulNow < m_ulToFireLongKey))
      return false;
    m_ulToFireLongKey = 0;
    DEBUG_PRINT("onLongKeyDown vk=");
    DEBUG_PRINT(getKeyName(vk));
    DEBUG_PRINTLN("");
    g_pView->onLongKeyDown(vk);
    return true;
  }
  // vk != m_cOldKey
  if(m_ulBounceSubsided == 0) {
    m_ulBounceSubsided = ulNow + s_iDebounceDelay;
    return false;
  }
  if(m_bOldKey == VK_NONE) 
  {
    m_ulToFireLongKey = ulNow + s_iLongKeyDelay;
    m_ulBounceSubsided = 0;
    DEBUG_PRINT("onKeyDown vk=");
    DEBUG_PRINT(getKeyName(vk));
    DEBUG_PRINT(" m_bOldKey=");
    DEBUG_PRINT(getKeyName(m_bOldKey));
    DEBUG_PRINTLN("");
    g_pView->onKeyDown(vk);
  }
  else if(vk != VK_NONE)
  {
    // ignore transients!
    return false;
  }
  else
  {
    m_ulToFireLongKey = 0;
    m_ulBounceSubsided = 0;
    DEBUG_PRINT("onKeyUp vk=");
    DEBUG_PRINT(getKeyName(vk));
    DEBUG_PRINT(" m_bOldKey=");
    DEBUG_PRINT(getKeyName(m_bOldKey));
    DEBUG_PRINTLN("");
    g_pView->onKeyUp(m_bOldKey);
  }
  m_bOldKey = vk;
  return true;
}




