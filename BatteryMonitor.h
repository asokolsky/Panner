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
public: 
  /** customize this depending on your battery and divider */
  static const uint16_t uReadingBatteryEmpty = 236; // 2.9V per cell - 11.6V total
  /** customize this depending on your battery and divider */
  static const uint16_t uReadingBatteryFull = 330;  // 4.2V per cell - 16.8V total
  
  /**  update period 10sec */
  static const unsigned long ulUpdatePeriod = 1*1000;

protected:  
  uint8_t m_pin;
  /** % of the battery full */
  uint8_t m_gauge;
  unsigned long m_ulNextUpdate;

public:  
  BatteryMonitor(uint8_t pin);

  /** 
   *  Updates the battery state  respecting uUpdatePeriod.
   *    Returns true if the state changed
   *    Returns false if the state did not change
   */
  bool update(unsigned long now);
  /** 
   *  Updates the battery state unconditionally
   *    Returns true if the state changed
   *    Returns false if the state did not change
   */
  bool updateMaybe(unsigned long now)
  {
    if(now < m_ulNextUpdate)
    {
      //DEBUG_PRINTLN("BatteryMonitor::update - too early!");
      return false;
    }
    bool res = update(now);
    m_ulNextUpdate = now + ulUpdatePeriod;
    return res;    
  }

  /** get % of the battery charge */
  uint8_t getGauge() {
    return m_gauge;
  }
  
};

extern BatteryMonitor g_batteryMonitor;

#endif


