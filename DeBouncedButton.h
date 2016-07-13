#ifndef DeBouncedButton_h
#define DeBouncedButton_h

/**
 *  Bounce2 with API which I find more reasonable
 * 
 */
#include <inttypes.h>


class DeBouncedButton
{
protected:
  /** Sets the debounce interval */
  static const uint16_t interval_millis = 5;

  unsigned long int m_previous_millis = 0;
  uint8_t m_state = 0;
  uint8_t m_pin;

public:
  /** Just a public constructor */
  DeBouncedButton(uint8_t pin);
  
  /** 
   * attach to hardware 
   */
  //void setup(); // now it is done in the constructor
  
  /** 
   *  Updates the pin state, does de-bounce
   *    Returns true if the state changed
   *    Returns false if the state did not change
   */
  bool update();
  /**
   * 
   */
  bool isPressed();
  /**
   * 
   */
  bool justPressed();
  /**
   * 
   */
  bool justReleased();
};

#endif


