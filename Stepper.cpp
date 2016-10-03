#include "Panner.h"
#include "Stepper.h"

Stepper::Stepper(uint8_t pinStep, uint8_t pinDirection, uint8_t pinEnable) :
  AccelStepper(AccelStepper::DRIVER, pinStep, pinDirection)
{
  _enableInverted = true;
  setEnablePin(pinEnable);
  //setMaxSpeed(uPannerMaxSpeed);
  //setAcceleration(uPannerAcceleration);
}


void Stepper::enable(bool bEnable)
{
  DEBUG_PRINT("Stepper::enable(");
  DEBUG_PRINTDEC(bEnable);
  DEBUG_PRINTLN(")");
  digitalWrite(_enablePin, bEnable ? (HIGH ^ _enableInverted) : (LOW ^ _enableInverted));
  m_bEnabled = bEnable;
}

#ifdef DEBUG
void Stepper::DUMP(const char *szText /*= 0*/) const
{
  if(szText != 0) {
    DEBUG_PRINT(szText);
  }
  DEBUG_PRINT(" Stepper@"); DEBUG_PRINTDEC((int)this); 
  DEBUG_PRINT(" m_bEnabled="); DEBUG_PRINTDEC((int)m_bEnabled);
  
  DEBUG_PRINT(" _currentPos="); DEBUG_PRINTDEC((int)_currentPos);   
  DEBUG_PRINT(" _targetPos="); DEBUG_PRINTDEC((int)_targetPos);   
  DEBUG_PRINT(" _speed="); DEBUG_PRINTDEC((int)_speed);
  DEBUG_PRINT(" _maxSpeed="); DEBUG_PRINTDEC((int)_maxSpeed);
  DEBUG_PRINT(" _acceleration="); DEBUG_PRINTDEC((int)_acceleration);
  
  DEBUG_PRINTLN("");
}
#endif




