//#define NODEBUG 1
#include <Arduino.h>
#include "AnalogNavigationKeypad.h"
#include "Trace.h"

/** 
 * Globals: simple analog keypad is connected to pins A1 and A2 so that,e.g. up and left could be pressed simultaneously
 */

static const char *getKeyNames(uint8_t vks)
{
  static char buf[40];
  buf[0] = '\0';
  if(vks & VK_UP)
    strcat(buf, "VK_UP ");
  if(vks & VK_DOWN)
    strcat(buf, "VK_DOWN ");
  if(vks & VK_LEFT)
    strcat(buf, "VK_LEFT ");
  if(vks & VK_RIGHT)
    strcat(buf, "VK_RIGHT ");
  if(vks & VK_SEL)
    strcat(buf, "VK_SEL ");
  if(vks & VK_SOFTA)
    strcat(buf, "VK_SOFTA ");
  if(vks & VK_SOFTB)
    strcat(buf, "VK_SOFTB ");
  return buf;
}


/*#ifndef  NODEBUG
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
#endif*/

/**
 *  KeypadChannel class implementation 
 */
/**
 * get one of VK_xxx 
 * Values are 0, 512
 */
uint8_t KeypadChannel::getKey2(int16_t iReading)
{
  // 1st option for speed reasons since it will be the most likely result
  if(iReading > 760) { // 512 + 512/2
    //DEBUG_PRINTLN("Keypad::getKey2() => VK_NONE");
    return VK_NONE;
  }
  if(iReading < 256) { // 512/2
    //DEBUG_PRINTLN("Keypad::getKey2() => m_vk[0]");
    return m_vk[0];  
  }  
  //DEBUG_PRINTLN("Keypad::getKey2() => m_vk[1]");
  return m_vk[1];
}

/** 
 * get one of VK_xxx 
 * Values are 0, 341, 682
 * e.g. VK_UP, VK_DOWN, VK_SOFTB
 */
uint8_t KeypadChannel::getKey3(int16_t iReading)
{
  //DEBUG_PRINT("KeypadChannel::getKey3, iReading=0x"); DEBUG_PRINTHEX(iReading);
  // 1st option for speed reasons since it will be the most likely result
  if(iReading > 852) { // 682 + 341/2
    //DEBUG_PRINTLN(" => VK_NONE");
    return VK_NONE;
  }
  if(iReading < 170) { // 341/2
    //DEBUG_PRINTLN(" => m_vk[0]"); // 1
    return m_vk[0];  
  }  
  if(iReading < 511) { // 341 + 341/2
    //DEBUG_PRINTLN(" => m_vk[1]"); // 355
    return m_vk[1];  
  }
  //DEBUG_PRINTLN(" => m_vk[2]"); // 680
  return m_vk[2];  
}
/** 
 *  get one of VK_xxx 
 *  e.g. VK_RIGHT, VK_LEFT, VK_SEL, VK_SOFTA
 */
uint8_t KeypadChannel::getKey4(int16_t iReading)
{
  //DEBUG_PRINT("KeypadChannel::getKey4, iReading=0x"); DEBUG_PRINTHEX(iReading);
  // 1st option for speed reasons since it will be the most likely result
  if(iReading > 950) {
    //DEBUG_PRINTLN(" => VK_NONE");
    return VK_NONE;
  }
  if(iReading < 76) {
    //DEBUG_PRINTLN(" => m_vk[0]"); // 0
    return m_vk[0];  
  }  
  if(iReading < 352) { // (223+481)/2 = 352
    //DEBUG_PRINTLN(" => m_vk[1]"); // 223
    return m_vk[1];  
  }
  if(iReading < 618) { // (481+755)/2 = 618
    //DEBUG_PRINTLN(" => m_vk[2]"); // 481
    return m_vk[2];  
  }
  //DEBUG_PRINTLN(" => m_vk[3]"); // 755
  return m_vk[3];  
}

/**
 * get one of VK_xxx
 */
uint8_t KeypadChannel::getKey()
{
  //analogRead(m_bPin);                     // switch the channel to m_bPin and hold for 104µs
  int adc_key_in = analogRead(m_bPin);    // read the value from the sensor 
  // 1st option for speed reasons since it will be the most likely result
  if(adc_key_in > 950) {
    //DEBUG_PRINTLN("Keypad::getKey() => VK_NONE");
    return VK_NONE;
  }    
  //DEBUG_PRINT("analogRead(m_bPin="); DEBUG_PRINTDEC(m_bPin); DEBUG_PRINT(") =>"); DEBUG_PRINTDEC(adc_key_in); DEBUG_PRINTLN("");  
  switch(m_uKeys)
  {
    case 2:
      return getKey2(adc_key_in);
    case 3:
      return getKey3(adc_key_in);
    case 4:
      return getKey4(adc_key_in);
    default:
      DEBUG_PRINTLN("KeypadChannel::getKey() wrong m_uKeys");
  }
  return VK_NONE;
}

bool KeypadChannel::getAndDispatchKey(unsigned long ulNow, AnalogNavigationKeypad *p, uint8_t uKeyOtherChannel)
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
      if(!p->isUserLongInactive(ulNow))
        return false;
      bRes = p->onUserInActivity(ulNow);
      p->onUserActivity(ulNow);
      return bRes;
      
    }
    // fire auto repeat logic here
    if((m_ulToFireAutoRepeat == 0) || (ulNow < m_ulToFireAutoRepeat))
    {
      ;
    }
    else
    {
      m_ulToFireAutoRepeat = ulNow + s_iAutoRepeatDelay;
      //DEBUG_PRINT("onKeyAutoRepeat vk="); DEBUG_PRINT(getKeyName(vk)); DEBUG_PRINTLN("");
      bRes = p->onKeyAutoRepeat(vk|uKeyOtherChannel);      
    }      
    // fire long key logic here
    if((m_ulToFireLongKey == 0) || (ulNow < m_ulToFireLongKey))
      return bRes;
    m_ulToFireLongKey = 0;
    DEBUG_PRINT("onLongKeyDown vk="); DEBUG_PRINT(getKeyNames(vk)); DEBUG_PRINTLN("");
    return p->onLongKeyDown(vk|uKeyOtherChannel) || bRes;
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
    DEBUG_PRINT("onKeyDown vk="); DEBUG_PRINT(getKeyNames(vk)); DEBUG_PRINT(" m_bOldKey="); DEBUG_PRINT(getKeyNames(m_bOldKey)); DEBUG_PRINTLN("");
    bRes = p->onKeyDown(vk|uKeyOtherChannel);
    p->onUserActivity(ulNow);
  }
  else if(vk != VK_NONE)
  {
    // ignore transients!
    return false;
  }
  else
  {
    m_ulToFireAutoRepeat = m_ulToFireLongKey = m_ulBounceSubsided = 0;
    DEBUG_PRINT("onKeyUp vk="); DEBUG_PRINT(getKeyNames(vk)); DEBUG_PRINT(" m_bOldKey="); DEBUG_PRINT(getKeyNames(m_bOldKey)); DEBUG_PRINTLN("");
    bRes = p->onKeyUp(m_bOldKey|uKeyOtherChannel);
    p->onUserActivity(ulNow);
  }
  m_bOldKey = vk;
  return bRes;
}

/**
 *  AnalogNavigationKeypad class implementation 
 */
AnalogNavigationKeypad::AnalogNavigationKeypad(uint8_t bPin1, uint8_t bPin2) :
  m_ulToFireInactivity(s_ulInactivityDelay)
{
  static uint8_t Keys1[] = {VK_RIGHT, VK_LEFT, VK_SEL, VK_SOFTA}; // this reflects PCB connections
  static uint8_t Keys2[] = {VK_UP, VK_DOWN, VK_SOFTB};            // this reflects PCB connections

  m_ch[0].m_bPin = bPin1;
  m_ch[0].m_vk = Keys1;
  m_ch[0].m_uKeys = sizeof(Keys1)/sizeof(Keys1[0]);
  m_ch[1].m_bPin = bPin2;
  m_ch[1].m_vk = Keys2;
  m_ch[1].m_uKeys = sizeof(Keys2)/sizeof(Keys2[0]);
}

bool AnalogNavigationKeypad::getAndDispatchKey(unsigned long now)
{
  static uint16_t i = 0;
  if(i >= sizeof(m_ch)/sizeof(m_ch[0]))
    i = 0;
  uint8_t uKeyOtherChannel = (i == 0) ? m_ch[1].m_bOldKey : m_ch[0].m_bOldKey;
  return m_ch[i++].getAndDispatchKey(now, this, uKeyOtherChannel);
}

/**
 *  For visualization only
 */
const char *AnalogNavigationKeypad::getKeyNames(uint8_t vks)
{
  return ::getKeyNames(vks);
}

