#include "Panner.h"

/**
 * Generic CommandInterpreterChannel implementation 
 */
CommandInterpreterChannel::CommandInterpreterChannel(uint8_t pinStep, uint8_t pinDirection, uint8_t pinEnable) : 
  m_motor(pinStep, pinDirection, pinEnable) 
{
}

boolean CommandInterpreterChannel::beginCommand(const Command *p, unsigned long now) 
{
  p->DUMP("CommandInterpreterChannel::beginCommand");
  boolean bBlocking = false;
  switch(p->m_command)
  {
    case cmdGo:
      m_motor.move(p->m_lPosition);
      bBlocking = true;
      break;
    case cmdGoTo:
      m_motor.moveTo(p->m_lPosition);
      bBlocking = true;
      break;
    case cmdGoToWaypoint: {
      char szKey [] = " ";
      szKey[0] = (char)p->m_lPosition;
      m_motor.moveToWayPoint(szKey);
      bBlocking = true;
      break;
    }
    case cmdSetMaxSpeed:
      m_motor.setMaxSpeed((float)p->m_uValue);
      break;    
    case cmdSetAcceleration:
      m_motor.setAcceleration((float)p->m_uValue);
      break;
    
    //default:
      // melting core now!
  }
  return !bBlocking;
}

boolean CommandInterpreterChannel::endCommand() 
{
  DEBUG_PRINTLN("CommandInterpreterChannel::endCommand()");
  return true;
}

/** 
 * may communicate with hardware 
 */
void CommandInterpreterChannel::pauseCommand() 
{
  DEBUG_PRINTLN("CommandInterpreterChannel::pauseCommand()");
  if(isBusy())
    m_motor.stop();
}

/** 
 * may communicate with hardware 
 */
void CommandInterpreterChannel::resumeCommand(unsigned long ulPauseDuration) 
{
  DEBUG_PRINTLN("CommandInterpreterChannel::resumeCommand()");
  if(!isBusy())
    return;
  //if(m_ulNext != 0) m_ulNext += ulPauseDuration;
}
