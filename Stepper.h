#ifndef STEPPER_H
#define STEPPER_H

#include <AccelStepper.h>

class Stepper : public AccelStepper
{  
public:
  Stepper(uint8_t pinStep, uint8_t pinDirection, uint8_t pinEnable = 0xFF);

  void enable(bool bEnable = true);
  bool isEnabled() { return m_bEnabled; }

  void DUMP();

private:
  bool m_bEnabled = false;
};

//extern Stepper g_panner;

#endif

