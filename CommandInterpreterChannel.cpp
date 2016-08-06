#include "Panner.h"

//extern volatile byte g_byteSliderEndSwitch;

/**
 * Generic CommandInterpreterChannel implementation 
 */
CommandInterpreterChannel::CommandInterpreterChannel(uint8_t pinStep, uint8_t pinDirection, uint8_t pinEnable) : 
  m_motor(pinStep, pinDirection, pinEnable) 
{
}

void CommandInterpreterChannel::beginCommands() {
  m_cSpeed = 0;
  m_ulNext = 0;
}

void CommandInterpreterChannel::beginCommand(Command *p, unsigned long now) {
  schar_t channel = p->m_channel; 
  schar_t command = p->m_command;
  m_cSpeed = p->m_speed;
  unsigned long ulDuration = p->m_uDuration; 
  
  DEBUG_PRINT("CommandInterpreterChannel::beginCommand channel=");
  DEBUG_PRINTDEC(channel);
  DEBUG_PRINT(" command=");
  DEBUG_PRINTDEC(command);
  DEBUG_PRINT(" m_cSpeed=");
  DEBUG_PRINTDEC(m_cSpeed);
  DEBUG_PRINTLN("");

  m_ulNext = now + ulDuration;

  switch(command)
  {
    case cmdGo:
      m_motor.move(p->m_lPosition);
      break;
    case cmdGoTo:
      m_motor.moveTo(p->m_lPosition);
      break;
    //default:
      // melting core now!
  }
}

boolean CommandInterpreterChannel::endCommand() {
  //DEBUG_PRINTLN("CommandInterpreterChannel::endCommand()");

  // work with hardware here
  //m_motor.stop();

  m_cSpeed = 0;
  m_ulNext = 0;
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

}


boolean CommandInterpreterChannel::isBusy() {
  //return (m_ulNext > 0);
  return m_motor.run(); // (m_motor.speed() == 0) && (m_motor.distanceToGo() == 0);
}

/**
 * by default we are concerned about command expiration only, 
 * no end-switches are in the picture
 */
boolean CommandInterpreterChannel::isReadyToEndCommand(unsigned long now) 
{
  //return (m_ulNext != 0) && (now >= m_ulNext);
  
  boolean bRes = !m_motor.run(); // (m_motor.speed() == 0) && (m_motor.distanceToGo() == 0);
/*
  DEBUG_PRINT("CommandInterpreterChannel::isReadyToEndCommand(now=");
  DEBUG_PRINTDEC(now);
  DEBUG_PRINT(") =>");
  DEBUG_PRINTDEC(bRes);
  DEBUG_PRINTLN("");
*/
  return bRes;
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
  //doSpeedStep(now);
}


void CommandInterpreterChannel::tick(unsigned long now) {
  if(m_ulNext == 0)
    return;
  //doSpeedStep(now);  
  m_motor.run();
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



