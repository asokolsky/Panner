#ifndef STEPPER_H
#define STEPPER_H

//#include <AccelStepper.h>

class Stepper : public AccelStepper
{  
public:
  Stepper(uint8_t pinStep, uint8_t pinDirection, uint8_t pinEnable = 0xFF);

  void enable(bool bEnable = true);
  
  bool isEnabled() const { 
    return m_bEnabled; 
  }

#ifdef DEBUG
  void DUMP(const char *szText = 0) const;
#else
  void DUMP(const char *szText = 0) {}
#endif

  //
  std::map<std::string, long> m_wayPoints;

  /** 
   *  Move to one of the defined waypoints
   *  Ignore if such waypoint does not exist.
   */
  void moveToWayPoint(const char wp[]);
  
  /** simple accessor */
  float getMaxSpeed() const {
    return _maxSpeed;
  }
  /** simple accessor */
  float getAcceleration() const {
    return _acceleration;
  }

private:
  bool m_bEnabled = false;
};


#endif

