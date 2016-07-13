#ifndef BatteryMonitor_h
#define BatteryMonitor_h

/**
 * Class to monitor battery.  
 * Relies on measuring battery voltage alone.
 * Connect battery lead to a voltage divider to analog pin
 */
#include <inttypes.h>

class BatteryMonitor
{
  uint8_t m_pin;
  /** % of the battery full */
  uint8_t m_gauge;
  unsigned long m_ulUpdated;
  
public:
  /** customize this depending on your battery and divider */
  static const uint16_t uReadingBatteryEmpty = 100;
  /** customize this depending on your battery and divider */
  static const uint16_t uReadingBatteryFull = 356;
  /**  update period 10sec */
  static const unsigned int uUpdatePeriod = 10*1000;
  
  BatteryMonitor(uint8_t pin);

  /** 
   *  Updates the battery state.
   *    Returns true if the state changed
   *    Returns false if the state did not change
   */
  bool update(unsigned long now);

  /** get % of the battery charge */
  uint8_t getGauge() {
    return m_gauge;
  }
  
};

#endif


