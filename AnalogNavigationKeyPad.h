#ifndef AnalogNavigationKeypad_h
#define AnalogNavigationKeypad_h

/********************************************************************************************/
/*                                                                                          */
/* To ensure that two keys can be used simultaneously, two analog inputs are used.          */
/*                                                                                          */
/* Alex Sokolsky, 5/2015-1/2017                                                             */
/*                                                                                          */
/********************************************************************************************/

//#include <Arduino.h>

/** define some values for button (key) scan codes */
const uint8_t VK_NONE  = 0;
const uint8_t VK_RIGHT = 1;
const uint8_t VK_LEFT  = 2;
const uint8_t VK_UP    = 4;
const uint8_t VK_DOWN  = 8;
const uint8_t VK_SEL   = 16;
/** Custom context specific soft buttons */
const uint8_t VK_SOFTA = 32;
const uint8_t VK_SOFTB = 64;

class AnalogNavigationKeypad;

/** Low level class important for implementation only - ignore it but do not modify it. */
class KeypadChannel
{
  /** delay in ms to debounce */
  const int s_iDebounceDelay = 50;
  /** delay in ms to autorepeat */
  const int s_iAutoRepeatDelay = 200;
  /** delay in ms before the long key is fired */
  const int s_iLongKeyDelay = 2000;

public:
  KeypadChannel(){}

  /** Analog pin from which we are reading */
  uint8_t m_bPin = 0;
  /** when to fire long key */
  unsigned long m_ulToFireLongKey = 0;
  /** when to fire key auto repeat */
  unsigned long m_ulToFireAutoRepeat = 0;
  /** when bouncing subsides */
  unsigned long m_ulBounceSubsided = 0;
  /** 
   *  an array of scan codes to generate when one of keys is pressed 
   *  e.g. [Up, Down]
   *  e.g. [Right, Left, Sel]
   *  or [Right, Left, Sel, A] [Up, Down, None, B]
   */
  uint8_t *m_vk;
  /** size of the array pointed to by m_vk */
  uint8_t m_uKeys = 0;

  /** get one of VK_xxx */
  uint8_t getKey();

  uint8_t m_bOldKey = VK_NONE;

  bool getAndDispatchKey(unsigned long ulNow, AnalogNavigationKeypad *p, uint8_t uKeyOtherChannel);

protected:
  /** get one of VK_xxx */
  uint8_t getKey2(int16_t iReading);
  /** get one of VK_xxx */
  uint8_t getKey3(int16_t iReading);
  /** get one of VK_xxx */
  uint8_t getKey4(int16_t iReading);

};

/**
 * Main interface class for an analog keyboard as used in a common LCD & keypad Arduino shield.
 * Such a keyboard is just a voltage divider connected to a single analog inpup pin.
 * Provides for debounce which is important on fast muCPUs, e.g. Teensy.
 * Now also with provision to handle simultaneous button presses 
 * as long as those buttons sit on separate channels conencted to different analog pins.
 * 
 */
class AnalogNavigationKeypad
{
public:
  /** keypad is connected to thees analog input pins */
  AnalogNavigationKeypad(uint8_t bPin1, uint8_t bPin2);

  /** call backs.  derive a class and overwrite those */
  virtual bool onUserInActivity(unsigned long ulNow) = 0;
  virtual bool onKeyAutoRepeat(uint8_t vks) = 0;
  virtual bool onKeyDown(uint8_t vks) = 0;
  virtual bool onLongKeyDown(uint8_t vks) = 0;
  virtual bool onKeyUp(uint8_t vks) = 0;
  

  /**
   *  Call this from the main loop passing to it the result of millis();
   *  It will call
   *         onKeyDown(uint8_t vk);
   *         onKeyAutoRepeat(uint8_t vk);
   *         onLongKeyDown(uint8_t vk);
   *         onKeyUp(uint8_t vk);
   *  Returns: true if key was dispatched and processed (then screen redraw is needed!), false otherwise.
   */
  bool getAndDispatchKey(unsigned long ulNow);

  bool isUserLongInactive(unsigned long ulNow) 
  {
    return (ulNow > m_ulToFireInactivity);
  }

  /** 
   * Delay inactivity notification.  This one is called by KeypadChannel.  
   * User does NOT have to call it. 
   */
  void onUserActivity(unsigned long ulNow) 
  {
    m_ulToFireInactivity = ulNow + s_ulInactivityDelay;
  }
  
protected:
  /** get readable names of the keyes pressed */
  const char *getKeyNames(uint8_t vks);

  /** when inactivity timeout will happen */
  unsigned long m_ulToFireInactivity = 0;
  /** inactivity timeout in milliseconds */
  const unsigned long s_ulInactivityDelay = 10000;
  
  /** to ensure that multiple keys can be read at the same time... */
  KeypadChannel m_ch[2];
};

#endif

