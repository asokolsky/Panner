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

  void moveToWayPoint(const std::string &s) {
    long int lPos = m_wayPoints[s];
    moveTo(lPos);
  }
  float getAcceleration() {
    return _acceleration;
  }

private:
  bool m_bEnabled = false;
};

//extern Stepper g_panner;

const uint16_t uPannerMaxSpeed = 50;
const uint16_t uPannerAcceleration = 15;

#endif

