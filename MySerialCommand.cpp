#include "Panner.h"

/**
 * execute serial command to move (slide/pan/tilt)
 */
static void onSerialMoveCommand(schar_t iCmd, schar_t iSpeed, unsigned uDurationSecs) {
  DEBUG_PRINT("onSerialMoveCommand iCmd=");
  DEBUG_PRINTDEC(iCmd);
  DEBUG_PRINT(" iSpeed=");
  DEBUG_PRINTDEC(iSpeed);
  DEBUG_PRINT(" uDurationSecs=");
  DEBUG_PRINTDEC(uDurationSecs);
  DEBUG_PRINTLN("");
  if(g_pCommandInterpreter->isRunning()) {
    if(g_pCommandInterpreter->isBusy(iCmd))
      g_pCommandInterpreter->stopCommand(iCmd);
    if((iSpeed != 0) && (uDurationSecs > 0))
      g_pCommandInterpreter->beginCommand(iCmd, iSpeed, uDurationSecs*1000L);
  } else {
    if((iSpeed != 0) && (uDurationSecs > 0))
      g_pCommandInterpreter->beginRun(iCmd, iSpeed, uDurationSecs*1000L);
  }
}

/**
 * get the command args and pass on to execute it
 */
static void onSerialMoveCommand(schar_t iCmd) {
  DEBUG_PRINT("onSerialMoveCommand iCmd=");
  DEBUG_PRINTDEC(iCmd);
  DEBUG_PRINTLN("");
  
  char *pArg = g_serialCommandInterpreter.next();
  if(pArg == 0)
  {
    DEBUG_PRINTLN("No argument!  Ignoring....");
    return;     // wrong command format
  }
  DEBUG_PRINT("pArg=");
  DEBUG_PRINT(pArg);
  DEBUG_PRINTLN("");

  schar_t iSpeed = (schar_t)atoi(pArg);
  DEBUG_PRINT("iSpeed=");
  DEBUG_PRINTDEC(iSpeed);
  DEBUG_PRINTLN("");

  unsigned uDurationSecs = 30;
  pArg = g_serialCommandInterpreter.next();
  if(pArg != 0)
    uDurationSecs = atoi(pArg);
  onSerialMoveCommand(iCmd, iSpeed, uDurationSecs);
}

/**
 * expected: SLI <signed speed in %> [duration in unsigned secs]
 * speed or duration 0 will result in stopping the slide
 */
static void onSerialPan()
{
  DEBUG_PRINTLN("onSerialPan()"); 
  onSerialMoveCommand(chPan);
}

/**
 * also purge serial command queue if any!
 */
static void onSerialHalt()
{
  DEBUG_PRINTLN("onSerialHalt()"); 
  if(g_pCommandInterpreter->isRunning())
    g_pCommandInterpreter->stopRun();
}
static void onSerialPause()
{
  DEBUG_PRINTLN("onSerialPause()"); 
  if(g_pCommandInterpreter->isRunning())
    g_pCommandInterpreter->pauseRun();
}
static void onSerialResume()
{
  DEBUG_PRINTLN("onSerialResume()"); 
  if(g_pCommandInterpreter->isRunning() && g_pCommandInterpreter->isPaused())
    g_pCommandInterpreter->resumeRun();
}
/*
static void onSerialWaitForCompletion()
{
  DEBUG_PRINTLN("onSerialWaitForCompletion()"); 
}
static void onSerialRest()
{
  DEBUG_PRINTLN("onSerialRest()"); 
}
static void onSerialBeginLoop()
{
  DEBUG_PRINTLN("onSerialBeginLoop()"); 
}
static void onSerialEndLoop()
{
  DEBUG_PRINTLN("onSerialEndLoop()"); 
}
*/
/**
 * set serial command handlers
 */
void MySerialCommand::begin()
{
  //addCommand("SLIDE", onSerialSlide);
  addCommand("PAN", onSerialPan); 
  //addCommand("TILT", onSerialTilt);
  //addCommand("WFC", onSerialWaitForCompletion);
  //addCommand("REST", onSerialRest);
  //addCommand("BLOOP", onSerialBeginLoop);
  //addCommand("ELOOOP", onSerialEndLoop);
  addCommand("HALT", onSerialHalt);
  addCommand("STOP", onSerialHalt);
  addCommand("PAUSE", onSerialPause);
  addCommand("RESUME", onSerialResume);
}



