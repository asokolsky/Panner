#ifndef STEPPER_H
#define STEPPER_H

#include <AccelStepper.h>

class Stepper : public AccelStepper
{  
public:
  Stepper(uint8_t pinStep, uint8_t pinDirection, uint8_t pinEnable = 0xFF);

  void enable(bool bEnable = true);
  bool isEnabled() { return m_bEnabled; }

#ifdef DEBUG
  void DUMP(const char *szText = 0);
#else
  void DUMP(const char *szText = 0) {}
#endif

  //
  std::map<std::string, long> m_wayPoints;

  void moveToWayPoint(const char wp[]) {
    long int lPos = m_wayPoints[wp];
    moveTo(lPos);
  }
  float getAcceleration() {
    return _acceleration;
  }

private:
  bool m_bEnabled = false;
};


#endif

