#ifndef CommandInterpreter_h
#define CommandInterpreter_h

/**
 * Interpreter for commands to slide/pan/tilt/zoom/activateShutter for photo/video camera
 * 
 */
 
/** 
 * constants for commands 
 * these are ALSO OFFSETS!
 */
//const schar_t cmdSlide = 0;
const schar_t cmdPan = 0;
//const schar_t cmdTilt = 2;
//const char cmdZoom  = 3;
//const char cmdShutter = 4;

const schar_t cmdFirst = cmdPan;
const schar_t cmdLast = cmdPan;
const schar_t cmdMax = cmdLast + 1; // the # of commands!  This is array size!
/** 
 * not really a command but the absense of commands 
 * delay consideration of the next command for this many millis
 */
const schar_t cmdRest = 10;
/** 
 * not really a command but a synchronization primitive
 * wait for all the channels to complete execution
 * either by time expiration or by exception such as endswitch
 */
const schar_t cmdWaitForCompletion = 11;

/**
 * Loop commands
 */
const schar_t cmdBeginLoop = 21;
const schar_t cmdEndLoop = 22;
 
/** not really a command but the marker of the end of commands */
const schar_t cmdNone = -1;

/**
 * Main Interface Class
 */
class CommandInterpreter
{
public:
  virtual void begin();
  virtual void beginRun(schar_t cmd, schar_t iSpeed, unsigned long ulDuration);

  /** external API of this class */
  virtual void beginCommand(schar_t cmd, schar_t cSpeed, unsigned long ulDuration);
  /**
  * iCmd is actually a channel #
  * to be called from interrupt handler or in response to kb
  */
  virtual void stopCommand(schar_t cCmd);
  /** adjust speed */
  virtual void adjustCommandSpeed(schar_t cmd, schar_t cSpeedAdjustment);
  /** Duration adjustment in seconds */
  virtual void adjustCommandDuration(schar_t cmd, int iDurationAdjustment);

  virtual void stopRun();

  virtual boolean isRunning();
  virtual boolean isPaused();
  /** is this channel busy? */
  virtual boolean isBusy(schar_t cChannel);

  /** suspend the run, can resume */
  virtual void pauseRun();
  /** resume the run */
  virtual void resumeRun();

  virtual void updateDisplay(unsigned long now);
};

extern CommandInterpreter *g_pCommandInterpreter;

#endif

