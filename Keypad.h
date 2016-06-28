#ifndef Keypad_h
#define Keypad_h

/********************************************************************************************/
/* Library for an analog keyboard as used in a common LCD & keypad Arduino shield.          */
/* Such a keyboard is just a voltage divider connected to a single analog inpup pin.        */
/* Provides for debounce which is important on fast muCPUs, e.g. Teensy                     */
/*                                                                                          */
/* Alex Sokolsky, May 2015                                                                  */
/*                                                                                          */
/********************************************************************************************/

//#include <Arduino.h>

/** define some values for button (key) scan codes */
const uint8_t VK_NONE = 0;
const uint8_t VK_RIGHT = 1;
const uint8_t VK_UP = 2;
const uint8_t VK_DOWN = 3;
const uint8_t VK_LEFT = 4;
const uint8_t VK_SEL = 5;

/**
 * Main interface class for an analog keyboard as used in a common LCD & keypad Arduino shield.
 * Such a keyboard is just a voltage divider connected to a single analog inpup pin.
 * Provides for debounce which is important on fast muCPUs, e.g. Teensy.
 * 
 */
class Keypad 
{
  /** delay in ms before the long key is fired */
  static const int s_iLongKeyDelay = 2000;
  /** delay in ms to debounce */
  static const int s_iDebounceDelay = 100;

public:
  /** keypad is connected to this analog input pin */
  Keypad(uint8_t bPin);

  /**
   *  call this from the main loop passing to it result of millis();
   *  It will call
   *         pView->ononKeyDown(uint8_t vk);
   *         pView->ononKeyUp(uint8_t vk);
   *         pView->ononLongKeyDown(uint8_t vk);
   */
  bool getAndDispatchKey(unsigned long now);

private:
  uint8_t m_bPin;
  /** when to fire long key */
  unsigned long m_ulToFireLongKey = 0;
  /** when bouncing subsides */
  unsigned long m_ulBounceSubsided = 0;
  /** get one of VK_xxx */
  uint8_t getKey();

  uint8_t m_bOldKey = VK_NONE;
};

extern Keypad g_keyPad;


#endif
