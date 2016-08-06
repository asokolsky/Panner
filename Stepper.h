#ifndef STEPPER_H
#define STEPPER_H

#include <AccelStepper.h>

class Stepper : public AccelStepper
{
  const unsigned long ulDisableTimeout = 20*1000;
  
public:
  Stepper(uint8_t pinStep, uint8_t pinDirection, uint8_t pinEnable = 0xFF);

  void enable(bool bEnable = true);
  bool isEnabled() { return m_bEnabled; }

  /** I overwrote these.. */

  /// Set the target position relative to the current position
  /// \param[in] relative The desired position relative to the current position. Negative is
  /// anticlockwise from the current position.
  void move(long relative);

  /// Poll the motor and step it if a step is due, implementing
  /// accelerations and decelerations to acheive the target position. You must call this as
  /// frequently as possible, but at least once per minimum step time interval,
  /// preferably in your main loop. Note that each call to run() will make at most one step, and then only when a step is due,
  /// based on the current speed and the time since the last step.
  /// \return true if the motor is still running to the target position.
  boolean run();
  
  /// Poll the motor and step it if a step is due, implementing a constant
  /// speed as set by the most recent call to setSpeed(). You must call this as
  /// frequently as possible, but at least once per step interval,
  /// \return true if the motor was stepped.
  boolean runSpeed(unsigned long now);  

  /// Sets the desired constant speed for use with runSpeed().
  /// \param[in] speed The desired constant speed in steps per
  /// second. Positive is clockwise. Speeds of more than 1000 steps per
  /// second are unreliable. Very slow speeds may be set (eg 0.00027777 for
  /// once per hour, approximately. Speed accuracy depends on the Arduino
  /// crystal. Jitter depends on how frequently you call the runSpeed() function.
  void setSpeed(float speed);


  void DUMP();

private:
  bool m_bEnabled = false;
  unsigned long m_ulStoppedAt = 0;
};


#endif

