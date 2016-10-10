#include "Panner.h"

/**
 * Command Debug Support
 */
#ifdef DEBUG
void Command::DUMP(const char *szText) const
{
  if(szText != 0) {
    DEBUG_PRINT(szText);
  }
  DEBUG_PRINT(" Command@"); DEBUG_PRINTDEC((int)this); 
  DEBUG_PRINT(" m_channel="); DEBUG_PRINTDEC(m_channel); 
  DEBUG_PRINT(" m_command="); DEBUG_PRINTDEC(m_command); 
  DEBUG_PRINT(" m_lPosition="); DEBUG_PRINTDEC(m_lPosition); 
  DEBUG_PRINTLN("");
}
#endif


/**
 * CommandInterpreter implementation
 */
PannerCommandInterpreter::PannerCommandInterpreter(uint8_t pinStep, uint8_t pinDirection, uint8_t pinEnable)
{
  m_channels[chControl] = 0;
  m_channels[chPan] = new /*Pan*/ CommandInterpreterChannel(pinStep, pinDirection, pinEnable);
}

/** 
 * external APIs 
 */
void PannerCommandInterpreter::begin() 
{
  // begin for all channels.....
  for(unsigned char i = 0; i < sizeof(m_channels)/sizeof(m_channels[0]); i++)
    if(m_channels[i] != 0)
      m_channels[i]->begin();
}

//
// buffer for a sequence of one command
//
static Command cmds[] = {
  {chControl, cmdControlNone, 0},
  {chControl, cmdControlNone, 0},
};

void PannerCommandInterpreter::beginRun(schar_t cmd, unsigned long ulDuration)
{
  DEBUG_PRINTLN("PannerCommandInterpreter::beginRun");
  cmds[0].m_command = cmd;
  cmds[0].m_uValue = ulDuration;
  beginRun(cmds);
}

void PannerCommandInterpreter::beginRun(Command *p) {
  p->DUMP("PannerCommandInterpreter::beginRun");
  // beginCommands for all channels.....
  for(unsigned char i = 0; i < sizeof(m_channels)/sizeof(m_channels[0]); i++)
    if(m_channels[i] != 0)
      m_channels[i]->beginCommands();

  m_ulCompletionExpiration = 0;
  beginCommand(p, millis());
}

void PannerCommandInterpreter::endRun() {
  DEBUG_PRINTLN("PannerCommandInterpreter::endRun");
  m_ulCompletionExpiration = 0;
  m_pCommand = 0;
  m_ulNext = 0;

  for(unsigned char i = 0; i < sizeof(m_channels)/sizeof(m_channels[0]); i++)
    if(m_channels[i] != 0)
      m_channels[i]->endCommands();
}

/** force stop processing commands */
void PannerCommandInterpreter::stopRun() {
  DEBUG_PRINTLN("PannerCommandInterpreter::stopRun");
  m_ulNext = m_ulPaused = m_ulCompletionExpiration = 0;
  m_pCommand = 0;
    
  for(unsigned char i = 0; i < sizeof(m_channels)/sizeof(m_channels[0]); i++)
    if((m_channels[i] != 0) && m_channels[i]->isBusy())
      m_channels[i]->endCommand();
}


/** suspend the run, can resume */
void PannerCommandInterpreter::pauseRun() {
  DEBUG_PRINTLN("PannerCommandInterpreter::pauseRun");
  if(m_ulPaused != 0)
    return;
  m_ulPaused = millis();
  for(unsigned char i = 0; i < sizeof(m_channels)/sizeof(m_channels[0]); i++)
    if((m_channels[i] != 0) && m_channels[i]->isBusy())
      m_channels[i]->pauseCommand();
  //updateDisplay(m_ulPaused);
}

/** resume the run */
void PannerCommandInterpreter::resumeRun() {
  DEBUG_PRINTLN("PannerCommandInterpreter::resumeRun");
  if(m_ulPaused == 0)
    return;
  unsigned long now = millis();
  unsigned long ulPauseDuration = now - m_ulPaused;
  for(unsigned char i = 0; i < sizeof(m_channels)/sizeof(m_channels[0]); i++)
    if((m_channels[i] != 0) && m_channels[i]->isBusy())
      m_channels[i]->resumeCommand(ulPauseDuration);
  if(m_ulNext != 0)
    m_ulNext += ulPauseDuration;
  if(m_ulCompletionExpiration != 0)
    m_ulCompletionExpiration += ulPauseDuration;
  m_ulPaused = 0;
  //updateDisplay(now);
}


/** 
 * Called from loop()
 * Times tick, update interpreter status
 * return true to continue running
 * return false to end the run
 */
bool PannerCommandInterpreter::continueRun(unsigned long now)
{
  //DEBUG_PRINT("PannerCommandInterpreter::continueRun now="); DEBUG_PRINTDEC(now); DEBUG_PRINTLN("");
  if(m_pCommand == 0) {
    DEBUG_PRINTLN("PannerCommandInterpreter::continueRun m_pCommand==0");
    return false;
  }
  if(isPaused())
    return true;
  // update the display at least once a sec
  bool bUpdateDisplay = false; // (now > m_ulToUpdateDisplay);
  //
  // for all channels...
  // tick thus giving an opportunity to do trapezoidal velocity profile...
  // end command if it is time to do so
  //
  for(unsigned char i = 0; i < sizeof(m_channels)/sizeof(m_channels[0]); i++) {
    if(m_channels[i] == 0)
      continue;
    if(m_channels[i]->isReadyToEndCommand(now)) {
      m_channels[i]->endCommand();
      bUpdateDisplay = true;
    } else {
      m_channels[i]->tick(now);
    }
  }
  if(bUpdateDisplay) {
    //updateDisplay(now);
    ;
  }
  if(m_pCommand->m_command == cmdControlNone) {
    // wait till all the commands are completed
    return (getBusyChannels() > 0);
  } else if(isResting()) {
    DEBUG_PRINTLN("PannerCommandInterpreter::continueRun - isResting!");
    if(!isReadyToEndRest(now))
      return true;
    //endRest();
    m_ulNext = 0;
  } else if(isWaitingForCompletion()) {
    DEBUG_PRINTLN("PannerCommandInterpreter::continueRun - isWaitingForCompletion!");
    if(getBusyChannels() > 0)
      return true;
    //endWaitForCompletion();
    m_ulCompletionExpiration = 0;
  } else {
    //DEBUG_PRINTLN("PannerCommandInterpreter::continueRun - not resting!");
    // is the channell needed for the next command already busy?
    Command *p = m_pCommand + 1;
    if(isBusy(p->m_channel))
        return true;
  } 
  // execute next command pointed to by m_pCommand
  beginCommand(m_pCommand+1, now);
  return true;
}

boolean PannerCommandInterpreter::isBusy(schar_t cChannel) {
  return isRunning()
    && (0 <= cChannel) 
    && (cChannel < chMax) 
    && (m_channels[cChannel] != 0) 
    && m_channels[cChannel]->isBusy();
}

/** get the # of channels running command */
char PannerCommandInterpreter::getBusyChannels() {
  char iRes = 0;
  for(unsigned char i = 0; i < sizeof(m_channels)/sizeof(m_channels[0]); i++)
    if((m_channels[i] != 0) && (m_channels[i]->isBusy()))
      iRes++;
  return iRes;
}


/** 
 * execute the command pointed to by p
 * side effect - may change m_pCommand 
 */
void PannerCommandInterpreter::beginCommand(Command *p, unsigned long now) {
  DEBUG_PRINTLN("PannerCommandInterpreter::beginCommand");
  for(bool bContinue = true; bContinue;) 
  {
    m_pCommand = p;
    m_pCommand->DUMP("m_pCommand=");
    schar_t ch = p->m_channel;
    switch(ch)
    {
      case chControl:
        switch(p->m_command)
        {
          case cmdControlRest:
            //beginRest(p->m_uValue, now);
            m_ulNext = now + p->m_uValue;
            bContinue = false;
            break;
          case cmdControlWaitForCompletion:
            //beginWaitForCompletion(p->m_uValue, now);
            m_ulCompletionExpiration = now + p->m_uValue;
            bContinue = false;
            break;
          case cmdControlBeginLoop:
            //beginLoop(p);
            m_pBeginLoopCommand = p;
            p++;
            break;
          case cmdControlEndLoop:
            //p = endLoop();
            p = m_pBeginLoopCommand;
            m_pBeginLoopCommand = 0;            
            break;
          //case cmdNone:
          default:
            // melt the core here
            DEBUG_PRINTLN("PannerCommandInterpreter::beginCommand - UNKNOWN control command!");  
            return;
        }
        break;

      case chPan:
      //case chSlide:
      //case chTilt:
      //case chZoom:
        if(m_channels[ch] == 0) {
          // we are not equiped to handle this command!  Ignore it!
          DEBUG_PRINTLN("PannerCommandInterpreter::beginCommand on 0 channel!  Ignoring..");
          p++;
        } else {
          m_channels[ch]->beginCommand(p, now);
          bContinue = false;
        }
        break;
        
      default:
        // melt the core here
        return;
      
    }
  }
  //updateDisplay(now);
}

/** 
 * External API 
 */
/*void PannerCommandInterpreter::beginCommand(schar_t cmd, unsigned long ulDuration) 
{
  DEBUG_PRINTLN("PannerCommandInterpreter::beginCommand");
  cmds[0].m_command = cmd;
  cmds[0].m_uValue = ulDuration;
  beginCommand(cmds, millis());
}*/

/*void PannerCommandInterpreter::adjustCommandSpeed(schar_t ch, schar_t iSpeed) 
{
  DEBUG_PRINTLN("PannerCommandInterpreter::adjustCommandSpeed");
  switch(ch) {
    //case chSlide:
    case chPan:
    //case chTilt:
      if(m_channels[ch] != 0)
        m_channels[ch]->adjustCommandSpeed(iSpeed);
      break;
  }
}*/

/*void PannerCommandInterpreter::adjustCommandDuration(schar_t ch, schar_t iCmd, int iSecs) {
  DEBUG_PRINTLN("PannerCommandInterpreter::adjustCommandDuration");
  switch(ch) {
    case chControl:
      switch(iCmd)
      {
        case cmdControlRest:
        case cmdControlWaitForCompletion:
          stopRun();
          break;
      }
      break;
    //case chSlide:
    case chPan:
    //case chTilt:
      if(m_channels[ch] != 0)
        m_channels[ch]->adjustCommandDuration(iSecs);
      break;
  }
}*/

/** 
 * iCmd is actually a channel # 
 * to be called from interrupt handler or in response to kb
 */
void PannerCommandInterpreter::stopCommand(schar_t iCmd) {
  if((iCmd < 0) || ((unsigned)iCmd) > sizeof(m_channels)/sizeof(m_channels[0]))
    return;
  if(m_channels[iCmd] == 0)
   return;
  m_channels[iCmd]->stopCommand(millis());
}

/** 
 * find out for how long the first busy channel will be busy
 */
word PannerCommandInterpreter::getBusySeconds(unsigned long now) {
  word wSecsRes = 0;
  for(unsigned char i = 0; i < sizeof(m_channels)/sizeof(m_channels[0]); i++)
    if((m_channels[i] != 0) && m_channels[i]->isBusy()) {
      unsigned long ulNext = m_channels[i]->getNext();
      if(now < ulNext) {
        word wSecs = (ulNext - now) / 1000L;
        if((wSecsRes == 0) || ((wSecs != 0) && (wSecs < wSecsRes)))
          wSecsRes = wSecs;
      }
    }
  return wSecsRes;
}

/** 
 * how much more wait for completion will last
 */
unsigned PannerCommandInterpreter::getWaitSeconds(unsigned long now) 
{
  return (m_ulCompletionExpiration > now) ?
         (unsigned)((m_ulCompletionExpiration - now) / 1000) :
         0;
}

