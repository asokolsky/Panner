#ifndef KeypadDuo_h
#define KeypadDuo_h

/********************************************************************************************/
/* Library for an analog keyboard as used in a common LCD & keypad Arduino shield.          */
/* Such a keyboard is just a voltage divider connected to a single analog inpup pin.        */
/* Provides for debounce which is important on fast muCPUs, e.g. Teensy                     */
/*
/* To ensure that two keys can be used siultaneously, two analog inputs are used.           */
/*                                                                                          */
/* Alex Sokolsky, May 2015                                                                  */
/*                                                                                          */
/********************************************************************************************/

//#include <Arduino.h>

/** define some values for button (key) scan codes */
const uint8_t VK_NONE = 0;
const uint8_t VK_RIGHT = 1;
const uint8_t VK_LEFT = 2;
const uint8_t VK_UP = 4;
const uint8_t VK_DOWN = 8;
const uint8_t VK_SEL = 16;
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

  /** inactivity timeout in milliseconds */
  static const unsigned long s_ulInactivityDelay = 1000*90;

public:
  KeypadChannel(){}

  uint8_t m_bPin = 0;
  /** when to fire long key */
  unsigned long m_ulToFireLongKey = 0;
  /** when to fire key auto repeat */
  unsigned long m_ulToFireAutoRepeat = 0;
  /** when bouncing subsides */
  unsigned long m_ulBounceSubsided = 0;
  /** when inactivity timeout will happen */
  unsigned long m_ulToFireInactivity = 0;
  /** an array of scan codes to generate when one of keys is pressed */
  uint8_t m_vk[4];

  /** get one of VK_xxx */
  uint8_t getKey();

  uint8_t m_bOldKey = VK_NONE;

  bool getAndDispatchKey(unsigned long ulNow);
  
};

/**
 * Main interface class for an analog keyboard as used in a common LCD & keypad Arduino shield.
 * Such a keyboard is just a voltage divider connected to a single analog inpup pin.
 * Provides for debounce which is important on fast muCPUs, e.g. Teensy.
 * 
 */
class KeypadDuo
{
  KeypadChannel m_ch[2];

public:
  /** keypad is connected to this analog input pin */
  KeypadDuo(uint8_t bPin1, uint8_t bPin2);

  /**
   *  Call this from the main loop passing to it the result of millis();
   *  It will call
   *         pView->onKeyDown(uint8_t vk);
   *         pView->onKeyAutoRepeat(uint8_t vk);
   *         pView->onLongKeyDown(uint8_t vk);
   *         pView->onKeyUp(uint8_t vk);
   *  Returns: true if key wasdispatched and processed (then screen redraw is needed!), false otherwise.
   */
  bool getAndDispatchKey(unsigned long now);

};

extern KeypadDuo g_keyPad;


#endif
