#include "Panner.h"
#include "Stepper.h"

Stepper::Stepper(uint8_t pinStep, uint8_t pinDirection, uint8_t pinEnable) :
  AccelStepper(AccelStepper::DRIVER, pinStep, pinDirection)
{
  //_enableInverted = true;
  setEnablePin(pinEnable);

  setMaxSpeed(50);
  setAcceleration(10);
}


void Stepper::enable(bool bEnable)
{
  DEBUG_PRINT("Stepper::enable(");
  DEBUG_PRINTDEC(bEnable);
  DEBUG_PRINTLN(")");
  //digitalWrite(_enablePin, bEnable ? (HIGH ^ _enableInverted) : (LOW ^ _enableInverted));
  m_bEnabled = bEnable;
}

/// Set the target position relative to the current position
/// \param[in] relative The desired position relative to the current position. Negative is
/// anticlockwise from the current position.
void Stepper::move(long relative)
{
  DEBUG_PRINT("Stepper::move(");
  DEBUG_PRINTDEC(relative);
  DEBUG_PRINTLN(")");
  DUMP();
  AccelStepper::move(relative);
}

/// Poll the motor and step it if a step is due, implementing
/// accelerations and decelerations to acheive the target position. You must call this as
/// frequently as possible, but at least once per minimum step time interval,
/// preferably in your main loop. Note that each call to run() will make at most one step, and then only when a step is due,
/// based on the current speed and the time since the last step.
/// \return true if the motor is still running to the target position.
boolean Stepper::run()
{
  return AccelStepper::run();
}

/// Poll the motor and step it if a step is due, implementing a constant
/// speed as set by the most recent call to setSpeed(). You must call this as
/// frequently as possible, but at least once per step interval,
/// \return true if the motor was stepped.
boolean Stepper::runSpeed(unsigned long now)
{
  if((m_ulStoppedAt != 0) && isEnabled() && (m_ulStoppedAt + ulDisableTimeout < now))
  {
    enable(false);
    m_ulStoppedAt = 0;
    DUMP();
  }
  return AccelStepper::runSpeed();
}

/// Sets the desired constant speed for use with runSpeed().
/// \param[in] speed The desired constant speed in steps per
/// second. Positive is clockwise. Speeds of more than 1000 steps per
/// second are unreliable. Very slow speeds may be set (eg 0.00027777 for
/// once per hour, approximately. Speed accuracy depends on the Arduino
/// crystal. Jitter depends on how frequently you call the runSpeed() function.
void Stepper::setSpeed(float spd)
{
  DEBUG_PRINT("Stepper::setSpeed(");
  DEBUG_PRINTDEC((int)spd);
  DEBUG_PRINTLN(")");
  if(spd == speed())
    return;
  if(spd == 0.0) 
  {
    m_ulStoppedAt = millis();
    DUMP();
  }
  else 
  {
    m_ulStoppedAt = 0;
    if(!isEnabled()) 
    {
      enable();
      DUMP();
    }
  }
  AccelStepper::setSpeed(spd);
}


void Stepper::DUMP()
{
  DEBUG_PRINT("Stepper@"); DEBUG_PRINTDEC((int)this); 
  DEBUG_PRINT(" m_bEnabled="); DEBUG_PRINTDEC((int)m_bEnabled);
  DEBUG_PRINT(" m_ulStoppedAt="); DEBUG_PRINTDEC((int)m_ulStoppedAt);
  /*
  DEBUG_PRINT(" _currentPos="); DEBUG_PRINTDEC((int)_currentPos);   
  DEBUG_PRINT(" _targetPos="); DEBUG_PRINTDEC((int)_targetPos);   
  DEBUG_PRINT(" _speed="); DEBUG_PRINTDEC((int)_speed);
  DEBUG_PRINT(" _maxSpeed="); DEBUG_PRINTDEC((int)_maxSpeed);
  DEBUG_PRINT(" _acceleration="); DEBUG_PRINTDEC((int)_acceleration);
  */
  DEBUG_PRINTLN("");
}



