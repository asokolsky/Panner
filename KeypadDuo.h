#ifndef KeypadDuo_h
#define KeypadDuo_h

/********************************************************************************************/
/* Library for an analog keyboard as used in a common LCD & keypad Arduino shield.          */
/* Such a keyboard is just a voltage divider connected to a single analog inpup pin.        */
/* Provides for debounce which is important on fast muCPUs, e.g. Teensy                     */
/*                                                                                          */
/* To ensure that two keys can be used simultaneously, two analog inputs are used.          */
/*                                                                                          */
/* Alex Sokolsky, May 2015                                                                  */
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


class KeypadChannel
{
  /** delay in ms to debounce */
  static const int s_iDebounceDelay = 50;
  /** delay in ms to autorepeat */
  static const int s_iAutoRepeatDelay = 200;
  /** delay in ms before the long key is fired */
  static const int s_iLongKeyDelay = 2000;

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
  uint8_t m_uKeys = 0;

  /** get one of VK_xxx */
  uint8_t getKey();

  uint8_t m_bOldKey = VK_NONE;

  bool getAndDispatchKey(unsigned long ulNow);

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
 * 
 */
class KeypadDuo
{
  /** inactivity timeout in milliseconds */
  static const unsigned long s_ulInactivityDelay = 10000;
  
  /** to ensure that multiple keys can be read at the same time... */
  KeypadChannel m_ch[2];

public:
  /** when inactivity timeout will happen */
  unsigned long m_ulToFireInactivity = 0;

  
  /** keypad is connected to this analog input pin */
  KeypadDuo(uint8_t bPin1, uint8_t Keys1[], uint8_t uKeys1,
            uint8_t bPin2, uint8_t Keys2[], uint8_t uKeys2);

  /**
   *  Call this from the main loop passing to it the result of millis();
   *  It will call
   *         pView->onKeyDown(uint8_t vk);
   *         pView->onKeyAutoRepeat(uint8_t vk);
   *         pView->onLongKeyDown(uint8_t vk);
   *         pView->onKeyUp(uint8_t vk);
   *  Returns: true if key wasdispatched and processed (then screen redraw is needed!), false otherwise.
   */
  bool getAndDispatchKey(unsigned long ulNow);

  /** Delay inactivity notification */
  void onUserActivity(unsigned long ulNow) 
  {
    m_ulToFireInactivity = ulNow + s_ulInactivityDelay;
  }
  /** act on user inactivity, reset counters */
  bool onUserInActivity(unsigned long now);

};

extern KeypadDuo g_keyPad;


#endif
