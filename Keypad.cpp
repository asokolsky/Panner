//#define NODEBUG 1

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
    case VK_SOFTA: return "VK_SOFTA";
    case VK_SOFTB: return "VK_SOFTB";
  }
  return "VK_???";
}
#endif

Keypad::Keypad(uint8_t bPin) : m_bPin(bPin)
{
  //analogReadResolution(10);
  //pinMode(bPin, INPUT_PULLUP); - done in hardware!
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
  // Standard shield buttons when read are centered at these valies: 0, 152, 342, 525, 756
  // we add approx 50 to those values and check to see if we are close

  // My buttons are theoretically centered at these valies: 0, 146, 292, 438, 584, 730, 876
  // My buttons are in practice centered at these valies: 0, 152, 295, 446, 606, 72, 873

  // 1st option for speed reasons since it will be the most likely result
  if(adc_key_in > 950) {
    //DEBUG_PRINTLN("Keypad::getKey() => VK_NONE");
    return VK_NONE;
  }
  if(adc_key_in < 76) {
    //DEBUG_PRINTLN("Keypad::getKey() => VK_RIGHT");
    return VK_RIGHT;  
  }  
  if(adc_key_in < 224) {
    //DEBUG_PRINTLN("Keypad::getKey() => VK_UP");
    return VK_UP;
  }
  if(adc_key_in < 371) {
    //DEBUG_PRINTLN("Keypad::getKey() => VK_DOWN");
    return VK_DOWN;
  }
  if(adc_key_in < 525) {
    //DEBUG_PRINTLN("Keypad::getKey() => VK_LEFT");
    return VK_LEFT;
  }
  if(adc_key_in < 675) {
    //DEBUG_PRINTLN("Keypad::getKey() => VK_SEL");
    return VK_SEL;
  }
  if(adc_key_in < 805) {
    //DEBUG_PRINTLN("Keypad::getKey() => VK_SOFTA");
    return VK_SOFTA;
  }
  //DEBUG_PRINTLN("Keypad::getKey() => VK_SOFTB");
  return VK_SOFTB;
}

bool Keypad::getAndDispatchKey(unsigned long ulNow)
{
  // get out if we are bouncing!
  if(ulNow < m_ulBounceSubsided)
    return false;
  
  bool bRes = false;
  uint8_t vk = getKey();
  if(vk == m_bOldKey) 
  {
    if(vk == VK_NONE)
    {
      if(ulNow < m_ulToFireInactivity)
        return false;
      m_ulToFireInactivity = 0;
      return View::g_pActiveView->onKeyInactive();
    }
    // fire auto repeat logic here
    if((m_ulToFireAutoRepeat == 0) || (ulNow < m_ulToFireAutoRepeat))
    {
      ;
    }
    else
    {
      m_ulToFireAutoRepeat = ulNow + s_iAutoRepeatDelay;
      DEBUG_PRINT("onKeyAutoRepeat vk="); DEBUG_PRINT(getKeyName(vk)); DEBUG_PRINTLN("");
      bRes = View::g_pActiveView->onKeyAutoRepeat(vk);      
    }      
    // fire long key logic here
    if((m_ulToFireLongKey == 0) || (ulNow < m_ulToFireLongKey))
      return bRes;
    m_ulToFireLongKey = 0;
    DEBUG_PRINT("onLongKeyDown vk="); DEBUG_PRINT(getKeyName(vk)); DEBUG_PRINTLN("");
    return View::g_pActiveView->onLongKeyDown(vk) || bRes;
  }
  // vk != m_cOldKey
  if(m_ulBounceSubsided == 0) {
    m_ulBounceSubsided = ulNow + s_iDebounceDelay;
    return false;
  }
  if(m_bOldKey == VK_NONE) 
  {
    m_ulToFireLongKey = ulNow + s_iLongKeyDelay;
    m_ulToFireAutoRepeat = ulNow + s_iAutoRepeatDelay;
    m_ulBounceSubsided = 0;
    m_ulToFireInactivity = ulNow + s_ulInactivityDelay;
    DEBUG_PRINT("onKeyDown vk="); DEBUG_PRINT(getKeyName(vk)); DEBUG_PRINT(" m_bOldKey="); DEBUG_PRINT(getKeyName(m_bOldKey)); DEBUG_PRINTLN("");
    bRes = View::g_pActiveView->onKeyDown(vk);
  }
  else if(vk != VK_NONE)
  {
    // ignore transients!
    return false;
  }
  else
  {
    m_ulToFireAutoRepeat = m_ulToFireLongKey = m_ulBounceSubsided = 0;
    m_ulToFireInactivity = ulNow + s_ulInactivityDelay;
    DEBUG_PRINT("onKeyUp vk="); DEBUG_PRINT(getKeyName(vk)); DEBUG_PRINT(" m_bOldKey="); DEBUG_PRINT(getKeyName(m_bOldKey)); DEBUG_PRINTLN("");
    bRes = View::g_pActiveView->onKeyUp(m_bOldKey);
  }
  m_bOldKey = vk;
  return bRes;
}


