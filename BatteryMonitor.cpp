#include <Arduino.h>
#include "BatteryMonitor.h"

BatteryMonitor::BatteryMonitor(uint8_t pin) : m_pin(pin), m_gauge(0), m_ulUpdated(0)
{
  // Setup the input
  pinMode(m_pin, INPUT);
  
}

/** 
 *  Updates the battery state.
 *    Returns true if the state changed
 *    Returns false if the state did not change
 */
bool BatteryMonitor::update(unsigned long now)
{
  if(m_ulUpdated + uUpdatePeriod < now)
    return false;
  m_ulUpdated = now;
  
  uint16_t uReading = 0; // accumulate samples here
  for(int i=0; i < 4; i++)
  {
    uint16_t intmp = analogRead(m_pin);
    uReading = uReading + intmp;
    //Serial.println(intmp);
    //delay(1);
  }
  uReading = uReading >> 2; // averaged over 4 samples

  if(uReading < uReadingBatteryEmpty)
    uReading = uReadingBatteryEmpty;
  else if(uReading > uReadingBatteryFull)
    uReading = uReadingBatteryFull;
  uint8_t gauge = map(uReading, uReadingBatteryEmpty, uReadingBatteryFull, 0, 100);
  if(abs(gauge - m_gauge) > 4) { // ignore +-4%
    m_gauge = gauge;
    return true;
  }
  return false;
}

