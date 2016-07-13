#include <Arduino.h>
#include "DeBouncedButton.h"

#define DEBOUNCED_STATE 0
#define UNSTABLE_STATE  1
#define STATE_CHANGED   3

/** Just a public constructor */
DeBouncedButton::DeBouncedButton(uint8_t pin) : m_pin(pin)
{
  // Setup the button with an internal pull-up :
  pinMode(m_pin, INPUT_PULLUP);
  // attach
  m_state = 0;
  if(digitalRead(m_pin)) {
    m_state = _BV(DEBOUNCED_STATE) | _BV(UNSTABLE_STATE);
  }
#ifdef BOUNCE_LOCK_OUT
  m_previous_millis = 0;
#else
  m_previous_millis = millis();
#endif
}


bool DeBouncedButton::update()
{
#ifdef BOUNCE_LOCK_OUT
  m_state &= ~_BV(STATE_CHANGED);
  // Ignore everything if we are locked out
  if(millis() - m_previous_millis >= interval_millis) 
  {
    bool currentState = digitalRead(m_pin);
    if((bool)(m_state & _BV(DEBOUNCED_STATE)) != currentState) {
        m_previous_millis = millis();
        m_state ^= _BV(DEBOUNCED_STATE);
        m_state |= _BV(STATE_CHANGED);
    }
  }
  return m_state & _BV(STATE_CHANGED);

#elif defined BOUNCE_WITH_PROMPT_DETECTION

  // Read the state of the switch port into a temporary variable.
  bool readState = digitalRead(m_pin); 
  // Clear Changed State Flag - will be reset if we confirm a button state change.
  m_state &= ~_BV(STATE_CHANGED);  
  if(readState != (bool)(m_state & _BV(DEBOUNCED_STATE))) 
  {
    // We have seen a change from the current button state.  
    if( millis() - m_previous_millis >= interval_millis)
    {
      // We have passed the time threshold, so a new change of state is allowed.
      // set the STATE_CHANGED flag and the new DEBOUNCED_STATE.
      // This will be prompt as long as there has been greater than interval_misllis ms since last change of input.
      // Otherwise debounced state will not change again until bouncing is stable for the timeout period.
      m_state ^= _BV(DEBOUNCED_STATE);
      m_state |= _BV(STATE_CHANGED);
    }
  }

  // If the readState is different from previous readState, 
  // reset the debounce timer - as input is still unstable
  // and we want to prevent new button state changes 
  // until the previous one has remained stable for the timeout.
  if(readState != (bool)(m_state & _BV(UNSTABLE_STATE))) 
  {
    // Update Unstable Bit to macth readState
    m_state ^= _BV(UNSTABLE_STATE);
    m_previous_millis = millis();
  }
  // return just the sate changed bit
  return m_state & _BV(STATE_CHANGED);
    
#else
    // Read the state of the switch in a temporary variable.
    bool currentState = digitalRead(m_pin);
    m_state &= ~_BV(STATE_CHANGED);
    // If the reading is different from last reading, reset the debounce counter
    if(currentState != (bool)(m_state & _BV(UNSTABLE_STATE))) 
    {
      m_previous_millis = millis();
      m_state ^= _BV(UNSTABLE_STATE);
    }
    else if(millis() - m_previous_millis >= interval_millis) 
    {
      // We have passed the threshold time, so the input is now stable
      // If it is different from last state, set the STATE_CHANGED flag
      if((bool)(m_state & _BV(DEBOUNCED_STATE)) != currentState) {
          m_previous_millis = millis();
          m_state ^= _BV(DEBOUNCED_STATE);
          m_state |= _BV(STATE_CHANGED);
      }
    }
    return m_state & _BV(STATE_CHANGED);
#endif 
}

bool DeBouncedButton::isPressed()
{
  return ((m_state & _BV(DEBOUNCED_STATE)) == LOW);
}

/**
 * 
 */
bool DeBouncedButton::justPressed()
{
  return !(m_state & _BV(DEBOUNCED_STATE)) && (m_state & _BV(STATE_CHANGED)); 
}

/**
 * 
 */
bool DeBouncedButton::justReleased()
{
  return (m_state & _BV(DEBOUNCED_STATE)) && (m_state & _BV(STATE_CHANGED));
}
/*

bool Bounce::read()
{
    return state & _BV(DEBOUNCED_STATE);
}

bool Bounce::rose()
{
    return ( state & _BV(DEBOUNCED_STATE) ) && ( state & _BV(STATE_CHANGED));
}

bool Bounce::fell()
{
    return !( state & _BV(DEBOUNCED_STATE) ) && ( state & _BV(STATE_CHANGED));
}

*/


