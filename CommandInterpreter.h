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
  schar_t m_speed;           // also carries information on direction and speed in %: -100%..100%
  union {
    unsigned long m_uDuration; // in milli seconds
    long m_lPosition;               // signed position for stepper
  };
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
/** Nore really a Command but the end marker */
const schar_t cmdControlNone = 0;
const schar_t cmdNone = 0;
 
/** 
 * not really a command but the absense of commands 
 * delay consideration of the next command for this many millis in m_uDuration
 */
const schar_t cmdControlRest = 1;
/** 
 * not really a command but a synchronization primitive
 * wait for all the channels to complete execution
 * either by time expiration m_uDuration or by exception such as endswitch
 */
const schar_t cmdControlWaitForCompletion = 2;

/**
 * Loop commands
 */
const schar_t cmdControlBeginLoop = 3;
const schar_t cmdControlEndLoop = 4;
 

/** Command to move to relative position m_lPosition */
const schar_t cmdGo = 1;
/** Command to move to absolute position m_lPosition */
const schar_t cmdGoTo = 2;

/** */

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
  virtual void adjustCommandSpeed(schar_t ch, schar_t cSpeedAdjustment);
  /** Duration adjustment in seconds */
  virtual void adjustCommandDuration(schar_t ch, schar_t cmd, int iDurationAdjustment);

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
  schar_t m_cSpeed = 0;

  Stepper m_motor;

public:
  CommandInterpreterChannel(uint8_t pinStep, uint8_t pinDirection, uint8_t pinEnable = 0xFF);


  unsigned long getNext() {
    return m_ulNext;
  }
  schar_t getSpeed() {
    return m_cSpeed;
  }
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
  void adjustCommandSpeed(schar_t iSpeedAdjustment);
  /**
  * Handle a GUI request
  */
  void adjustCommandDuration(schar_t iChangeSecs);

  Stepper *getMotor() {
    return &m_motor;
  }
  
};


/**
* Main Interface Class for Controller
*/
class PannerCommandInterpreter : public CommandInterpreter
{
public:
  /**
  * Call this according to your connections.
  */
  PannerCommandInterpreter(uint8_t pinStep, uint8_t pinDirection, uint8_t pinEnable = 0xFF);
  ~PannerCommandInterpreter() {}


  /** external APIs of this class */
  void begin();
  void beginRun(schar_t cmd, schar_t iSpeed, unsigned long ulDuration);
  void beginRun(Command *p);
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
  void beginCommand(schar_t cmd, schar_t cSpeed, unsigned long ulDuration);
  /** adjust speed */
  void adjustCommandSpeed(schar_t ch, schar_t cSpeedAdjustment);
  /** Duration adjustment in seconds */
  void adjustCommandDuration(schar_t ch, schar_t cmd, int iDurationAdjustment);

  //void updateDisplay(unsigned long now);

  /**
  * iCmd is actually a channel #
  * to be called from interrupt handler or in response to kb
  */
  void stopCommand(schar_t cCmd);

  unsigned long getNext() {
    return m_ulNext;
  }
  
  unsigned getBusySeconds(unsigned long now);

  boolean isResting() {
    return (m_ulNext > 0);
  }

  boolean isWaitingForCompletion() {
    return m_bWaitingForCompletion;
  }

  Stepper *getPanner() {
    return m_channels[chPan]->getMotor();
  }


private:
  void beginCommand(Command *p, unsigned long now);

  /** cmdWaitForCompletion command handler */
  void beginWaitForCompletion();
  void endWaitForCompletion();

  /** cmdRest command handler */
  void beginRest(unsigned long ulDuration, unsigned long now);
  void endRest();

  /** cmdBeginLoop command handler */
  void beginLoop(Command *p);
  /** cmdEndLoop command handler */
  Command *endLoop();

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
  boolean m_bWaitingForCompletion = false;
};

extern PannerCommandInterpreter g_ci;

#endif

