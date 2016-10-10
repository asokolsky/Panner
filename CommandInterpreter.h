#ifndef CommandInterpreter_h
#define CommandInterpreter_h

/**
 * Interpreter for commands to slide/pan/tilt/zoom/activateShutter for photo/video camera
 * 
 */

struct Command
{
  schar_t m_channel;         // control/slide/pan/tilt/zoom/focus/shutter...
  schar_t m_command;         // channel-specific command
  //schar_t m_speed;           // also carries information on direction and speed in %: -100%..100%
  union {
    /** unsigned duration (in milli seconds) or speed (in steps/sec or %) */
    unsigned long m_uValue;
    /** signed position for stepper */ 
    long m_lPosition;
    /** e.g. name of the waypoint */
    char m_szParam[4];
  };

#ifdef DEBUG
  void DUMP(const char *szText = 0) const ;
#else
  void DUMP(const char *szText = 0) {}
#endif
  
};


 
/** 
 * constants for channels - these are ALSO OFFSETS!
 */
const schar_t chControl = 0;
const schar_t chPan = 1;
//const schar_t chTilt = 2;
//const schar_t chZoom  = 3;
//const schar_t chSlide = 4;
//const schar_t chFocus = 5;
//const schar_t chShutter = 6;

const schar_t chMax = 2; // the # of channels!  This is array size!

/**
 *  Control channel commands
 */
/** Not really a Command but the end marker */
const schar_t cmdControlNone = 0;
const schar_t cmdNone = 0;
 
/** 
 * not really a command but the absense of commands 
 * delay consideration of the next command for this many millis in m_uDuration
 */
const schar_t cmdControlRest = 100;
/** 
 * not really a command but a synchronization primitive
 * wait for all the channels to complete execution
 * either by time expiration m_uDuration or by exception such as endswitch
 */
const schar_t cmdControlWaitForCompletion = 101;

/**
 * Loop commands
 */
const schar_t cmdControlBeginLoop = 102;
const schar_t cmdControlEndLoop = 103;
 
/**
 *  Pan/Tilt/Slide channel Commands
 */

/** Command to move to relative position m_lPosition */
const schar_t cmdGo = 1;
/** Command to move to absolute position m_lPosition */
const schar_t cmdGoTo = 2;
/** Command to move to a user-defined waypoint */
const schar_t cmdGoToWaypoint = 3;
/** 
 *  Command to set max speed for the channel (if appropriate) 
 *  In steps/sec for steppers
 *  of % for DC motors
 */
const schar_t cmdSetMaxSpeed = 10;
/** 
 *  Command to set acceleration (if appropriate)
 *  In steps/sec2 for steppers
 */
const schar_t cmdSetAcceleration = 11;

/** */


/**
 * Main Interface Class
 */
#if 0 
class CommandInterpreter
{
public:
  virtual void begin();
  virtual void beginRun(schar_t cmd, /*schar_t iSpeed,*/ unsigned long ulDuration);

  /** external API of this class */
  virtual void beginCommand(schar_t cmd, /*schar_t cSpeed,*/ unsigned long ulDuration);
  /**
  * iCmd is actually a channel #
  * to be called from interrupt handler or in response to kb
  */
  virtual void stopCommand(schar_t cCmd);
  /** adjust speed */
  //virtual void adjustCommandSpeed(schar_t ch, schar_t cSpeedAdjustment);
  /** Duration adjustment in seconds */
  //virtual void adjustCommandDuration(schar_t ch, schar_t cmd, int iDurationAdjustment);

  virtual void stopRun();

  virtual boolean isRunning();
  virtual boolean isPaused();
  /** is this channel busy? */
  virtual boolean isBusy(schar_t cChannel);

  /** suspend the run, can resume */
  virtual void pauseRun();
  /** resume the run */
  virtual void resumeRun();

  //virtual void updateDisplay(unsigned long now);
};

extern CommandInterpreter *g_pCommandInterpreter;
#endif

/**
* (abstract) channel for slide, pan or tilt or zoom
*  uses a motor
* may implement trapezoidal speed profile on the DC motor
*/
class CommandInterpreterChannel
{
protected:
  /**
  * If a command is being executed on the channel this is when it will end in ms
  * otherwise - 0
  */
  unsigned long m_ulNext;
  /** setpoint - where we want for the ultimate motor speed to be, in signed % */
  //schar_t m_cSpeed = 0;

  Stepper m_motor;

public:
  CommandInterpreterChannel(uint8_t pinStep, uint8_t pinDirection, uint8_t pinEnable = 0xFF);


  unsigned long getNext() {
    return m_ulNext;
  }
  /*schar_t getSpeed() {
    return m_cSpeed;
  }*/
  float getMotorSpeed() {
    return m_motor.speed();
  }
  long getMotorPosition() {
    return m_motor.currentPosition();
  }

  void begin() {
    //m_motor.begin();
  }
  void beginCommands();
  void endCommands() {}

  /**
  * may communicate with hardware
  */
  virtual void beginCommand(Command *p, unsigned long now);
  /**
  * may communicate with hardware
  */
  virtual boolean endCommand();

  /**
  * may communicate with hardware
  */
  void pauseCommand();
  /**
  * may communicate with hardware
  */
  void resumeCommand(unsigned long ulPauseDuration);
  /**
  * mark command as ready to be completed
  * actual hw communication is done in endCommand();
  */
  void stopCommand(unsigned long now) {
    if (m_ulNext > 0)
      m_ulNext = now;
  }

  /**
  * Did we work long enough or did we hit an endswitch?
  */
  virtual boolean isReadyToEndCommand(unsigned long now);
  /**
  * command is being executed by hardware
  */
  boolean isBusy();
 
  /**
  * allows for trapezoidal velocity profile implementation
  */
  void tick(unsigned long now);

  /**
    * Handle a GUI request
    */
  //void adjustCommandSpeed(schar_t iSpeedAdjustment);
  /**
  * Handle a GUI request
  */
  //void adjustCommandDuration(schar_t iChangeSecs);

  Stepper *getMotor() {
    return &m_motor;
  }
  
};


/**
* Main Interface Class for Controller
*/
class PannerCommandInterpreter //: public CommandInterpreter
{
public:
  /**
  * Call this according to your connections.
  */
  PannerCommandInterpreter(uint8_t pinStep, uint8_t pinDirection, uint8_t pinEnable = 0xFF);
  ~PannerCommandInterpreter() {}


  /** external APIs of this class */
  void begin();
  void beginRun(schar_t cmd, /*schar_t iSpeed,*/ unsigned long ulDuration);
  void beginRun(Command *p);
  /** 
   * Called from loop()
   * Times tick, update interpreter status
   * return true to continue running
   * return false to end the run
   */
  bool continueRun(unsigned long now);
  void endRun();

  /** stop processing commands */
  void stopRun();

  /** suspend the run, can resume */
  void pauseRun();
  /** resume the run */
  void resumeRun();


  /** is this command interpreter interpreting commands? */
  boolean isRunning() {
    return (m_pCommand != 0);
  }
  /** is this command interpreter paused? */
  boolean isPaused() {
    return (m_ulPaused != 0);
  }
  /** is this channel busy? */
  boolean isBusy(schar_t cChannel);


  /** external API of this class */
  //void beginCommand(schar_t cmd, /*schar_t cSpeed,*/ unsigned long ulDuration);
  /** adjust speed */
  //void adjustCommandSpeed(schar_t ch, schar_t cSpeedAdjustment);
  /** Duration adjustment in seconds */
  //void adjustCommandDuration(schar_t ch, schar_t cmd, int iDurationAdjustment);

  //void updateDisplay(unsigned long now);

  /**
  * iCmd is actually a channel #
  * to be called from interrupt handler or in response to kb
  */
  void stopCommand(schar_t cCmd);

  unsigned long getNext() {
    return m_ulNext;
  }
  /** 
   * find out for how long the first busy channel will be busy
   */  
  unsigned getBusySeconds(unsigned long now);
  /** 
   * how much more wait for completion will last
   */
  unsigned getWaitSeconds(unsigned long now);

  /** Are we resting? */
  boolean isResting() {
    return (m_ulNext > 0);
  }
  /** Are we waiting for completion? */
  boolean isWaitingForCompletion() {
    return (m_ulCompletionExpiration > 0);
  }

  Stepper *getPanner() {
    return m_channels[chPan]->getMotor();
  }


private:
  void beginCommand(Command *p, unsigned long now);

  boolean isReadyToEndRest(unsigned long now) {
    return (m_ulNext > 0) && (now >= m_ulNext);
  }
  /** get the total # of busy channels */
  char getBusyChannels();

  /** command currently being executed */
  Command *m_pCommand = 0;
  /** Loop command to jump back to when we encounter EndLoop */
  Command *m_pBeginLoopCommand = 0;
  /** array of interpreter channels such as slide/pan/tilt/zoom */
  CommandInterpreterChannel *m_channels[chMax];
  /** when to execute next command */
  unsigned long m_ulNext = 0;
  /** Display update interval */
  //const unsigned long ulDisplayUpdateInterval = 900;
  /** when the display will be updated */
  //unsigned long m_ulToUpdateDisplay = 0;
  /** when we were paused */
  unsigned long m_ulPaused = 0;
  /** we are waiting for command(s) to be completed. */
  unsigned long m_ulCompletionExpiration = 0;
};

extern PannerCommandInterpreter g_ci;

#endif

