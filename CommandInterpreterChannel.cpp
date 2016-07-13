#include "Panner.h"
#include "PannerCommandInterpreter.h"

//extern volatile byte g_byteSliderEndSwitch;

/**
 * Generic CommandInterpreterChannel implementation 
 */
CommandInterpreterChannel::CommandInterpreterChannel(byte pinDirection, byte pinStep) : 
  m_motor(AccelStepper::DRIVER, pinStep, pinDirection) 
{
}

void CommandInterpreterChannel::beginCommands() {
  m_cSpeed = m_cCurrentSpeed = 0;
  m_ulNext = 0;
}

void CommandInterpreterChannel::beginCommand(schar_t cSpeed, unsigned long ulDuration, unsigned long now) {
  DEBUG_PRINT("CommandInterpreterChannel::beginCommand cSpeed=");
  DEBUG_PRINTDEC(cSpeed);
  DEBUG_PRINT(", ulDuration=");
  DEBUG_PRINTDEC(ulDuration);
  DEBUG_PRINTLN("");

  m_ulNext = now + ulDuration;
  m_cSpeed = cSpeed;
  m_cCurrentSpeed = 0;
  doSpeedStep(now);
}

boolean CommandInterpreterChannel::endCommand() {
  DEBUG_PRINTLN("CommandInterpreterChannel::endCommand()");

  // work with hardware here
  //m_motor.stop();

  m_cSpeed = m_cCurrentSpeed = 0;
  m_ulNext = m_ulNextSpeedUpdate = 0;
  return true;
}

/** 
 * may communicate with hardware 
 */
void CommandInterpreterChannel::pauseCommand() {
  if(!isBusy())
    return;
  //m_motor.stop();
}

/** 
 * may communicate with hardware 
 */
void CommandInterpreterChannel::resumeCommand(unsigned long ulPauseDuration) {
  if(!isBusy())
    return;
  if(m_ulNext != 0)
    m_ulNext += ulPauseDuration;
  if(m_ulNextSpeedUpdate != 0)
    m_ulNextSpeedUpdate += ulPauseDuration;

  //m_motor.setSpeed((m_cCurrentSpeed > 0), abs(m_cCurrentSpeed));
  //m_motor.go();
}


/**
 * by default we are concerned about command expiration only, 
 * no end-switches are in the picture
 */
boolean CommandInterpreterChannel::isReadyToEndCommand(unsigned long now) {
  return (m_ulNext != 0) && (now >= m_ulNext);
}

void CommandInterpreterChannel::adjustCommandDuration(schar_t iSecs) {
  DEBUG_PRINT("CommandInterpreterChannel::adjustCommandDuration iSecs=");
  DEBUG_PRINTDEC(iSecs);
  DEBUG_PRINT(", m_ulNext=");
  DEBUG_PRINTDEC(m_ulNext);
  DEBUG_PRINTLN("");

  if(!isBusy())
    return;
  if(iSecs > 0) {
    m_ulNext = m_ulNext + 1000l * (unsigned long)iSecs;
  } else {
    unsigned long now = millis();
    unsigned long ulNext = m_ulNext - 1000l * (unsigned long)(-iSecs);
    m_ulNext = (now >= ulNext) ? now : ulNext;
  }
  DEBUG_PRINT("m_ulNext=");
  DEBUG_PRINTDEC(m_ulNext);
  DEBUG_PRINTLN("");
}

static char sanitizeSpeed(schar_t cSpeed) {
  return constrain(cSpeed, -100, 100);
}

void CommandInterpreterChannel::adjustCommandSpeed(schar_t cSpeedAdjustment) {
  DEBUG_PRINT("CommandInterpreterChannel::adjustCommandSpeed cSpeedAdjustment=");
  DEBUG_PRINTDEC(cSpeedAdjustment);
  DEBUG_PRINTLN("");

  if(!isBusy())
    return;
  unsigned long now = millis();
  char cSpeed = sanitizeSpeed(m_cSpeed + cSpeedAdjustment);
  if(cSpeed == 0) {
    stopCommand(now);
    return;
  }
  if(cSpeed == m_cSpeed)
    return;
  unsigned long ulNext = m_ulNext;
  if(now >= ulNext)
    return;
       
  m_cSpeed = cSpeed;
  doSpeedStep(now);
}


void CommandInterpreterChannel::tick(unsigned long now) {
  if((m_ulNext == 0) || (m_ulNextSpeedUpdate == 0) || (now < m_ulNextSpeedUpdate))
    return;
  doSpeedStep(now);  
}

/** assume uCurrentSpeed > uSpeed */
static unsigned slowDown(unsigned uCurrentSpeed, unsigned uSpeed, unsigned uSpeedUpdateStep) {
  return (uCurrentSpeed > (uSpeedUpdateStep + uSpeed))
   ? (uCurrentSpeed - uSpeedUpdateStep)
   : uSpeed;
}

/** assume uCurrentSpeed < uSpeed */
static unsigned speedUp(unsigned uCurrentSpeed, unsigned uSpeed, unsigned uSpeedUpdateStep) {
  return ((uCurrentSpeed + uSpeedUpdateStep) < uSpeed)
   ? (uCurrentSpeed + uSpeedUpdateStep)
   : uSpeed;
}


/**
 * implement a trapezoidal velocity profile
 * communicate with hardware
 *
 * we are now at m_cCurrentSpeed
 * we need to be at m_cSpeed
 * adjust speed by cSpeedUpdateStep
 * given that in m_ulNext-now we need to be at 0
 */
void CommandInterpreterChannel::doSpeedStep(unsigned long now) { 
  /*DEBUG_PRINT("doSpeedStep  m_cCurrentSpeed=");
  DEBUG_PRINTDEC(m_cCurrentSpeed);
  DEBUG_PRINT("  m_cSpeed=");
  DEBUG_PRINTDEC(m_cSpeed);
  DEBUG_PRINTLN("");*/

  // say when next time
  m_ulNextSpeedUpdate = now + ulSpeedUpdateTick;
  
  boolean bCW = (m_cCurrentSpeed == 0) 
    ? (m_cSpeed > 0) 
    : (m_cCurrentSpeed > 0);
  
  unsigned uCurrentSpeed = abs(m_cCurrentSpeed);
  unsigned uSpeed = abs(m_cSpeed);
  if(uSpeed < uCurrentSpeed) 
  {
    // slow down!
    uCurrentSpeed = slowDown(uCurrentSpeed, uSpeed, bSpeedUpdateStep);
  } 
  else if (uCurrentSpeed >= (bSpeedUpdateStep * ((m_ulNext-now)/ulSpeedUpdateTick)))
  {
    // faget about it!
    // start slowing down! the end is near!
    uSpeed = m_cSpeed = 0;
    uCurrentSpeed = slowDown(uCurrentSpeed, uSpeed, bSpeedUpdateStep);
  } 
  else if(uSpeed == uCurrentSpeed)
  {
    return;
  }
  else
  {
    // speed up!
    uCurrentSpeed = speedUp(uCurrentSpeed, uSpeed, bSpeedUpdateStep);
  }
  m_cCurrentSpeed = bCW ? uCurrentSpeed : -uCurrentSpeed;


  // work with the hardware here!
  // this will start the motor spinning!
  /*DEBUG_PRINT("m_motor.setSpeed ");
  DEBUG_PRINTDEC(m_cCurrentSpeed);
  DEBUG_PRINT(" now=");
  DEBUG_PRINTDEC(now);
  DEBUG_PRINTLN("");*/

  //m_motor.setSpeed(bCW, uCurrentSpeed);
}


