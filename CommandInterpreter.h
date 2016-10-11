#ifndef CommandInterpreter_h
#define CommandInterpreter_h

/**
 * Interpreter for commands to slide/pan/tilt/zoom/activateShutter for photo/video camera
 * 
 */

struct Command
{
  /** control/slide/pan/tilt/zoom/focus/shutter... */
  schar_t m_channel;
  /** channel-specific command */         
  schar_t m_command;
  union 
  {
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


/**
* Channel for slide, pan or tilt or zoom,  uses a motor,
* may implement trapezoidal speed profile on the DC motor
*/
class CommandInterpreterChannel
{
protected:
  /** 
   * Object to interact with the physical motor 
   */
  Stepper m_motor;

public:
  CommandInterpreterChannel(uint8_t pinStep, uint8_t pinDirection, uint8_t pinEnable = 0xFF);


  float getMotorSpeed() {
    return m_motor.speed();
  }
  long getMotorPosition() {
    return m_motor.currentPosition();
  }

  /**
   * may communicate with hardware
   * return true if the command is non-blocking
   */
  boolean beginCommand(const Command *p, unsigned long now);
  /**
   * may communicate with hardware
   */
  boolean endCommand();

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
  /*void stopCommand(unsigned long now) {
    //if (m_ulNext > 0) m_ulNext = now;
  }*/

  /**
   * Did we work long enough or did we hit an endswitch?
   */
  boolean isReadyToEndCommand(unsigned long now) {
    return !m_motor.run();
  }
  /**
   * command is being executed by hardware
   */
  boolean isBusy() {
    return m_motor.run();
  }
 
  /**
   * allows for trapezoidal velocity profile implementation
   */
  void tick(unsigned long now) {
    m_motor.run();
  }

  Stepper *getMotor() {
    return &m_motor;
  }
  
};


/**
* Main Interface Class for Controller
*/
class CommandInterpreter
{
public:
  /**
  * Call this according to your connections.
  */
  CommandInterpreter(uint8_t pinStep, uint8_t pinDirection, uint8_t pinEnable = 0xFF);
  ~CommandInterpreter() {}


  /** start executing an array of commands terminated with cmdNone */
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


  /** is this command interpreter still interpreting commands? */
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

  /**
  * iCmd is actually a channel #
  * to be called from interrupt handler or in response to kb
  */
  //void stopCommand(schar_t cCmd);

  /** When the next command will be executed? */
  unsigned long getNext() {
    return m_ulRestExpiration;
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
    return (m_ulRestExpiration > 0);
  }
  /** Are we waiting for completion? */
  boolean isWaitingForCompletion() {
    return (m_ulCompletionExpiration > 0);
  }

  Stepper *getPanner() {
    return m_channels[chPan]->getMotor();
  }


private:
  void beginCommand(const Command *p, unsigned long now);

  boolean isReadyToEndRest(unsigned long now) {
    return (m_ulRestExpiration > 0) && (now >= m_ulRestExpiration);
  }
  /** get the total # of busy channels */
  char getBusyChannels();

  /** command currently being executed */
  const Command *m_pCommand = 0;
  /** Loop command to jump back to when we encounter EndLoop */
  const Command *m_pBeginLoopCommand = 0;
  /** array of interpreter channels such as slide/pan/tilt/zoom */
  CommandInterpreterChannel *m_channels[chMax];

  /** when to execute next command */
  unsigned long m_ulRestExpiration = 0;
  /** when we were paused */
  unsigned long m_ulPaused = 0;
  /** we are waiting for (channel) command(s) to be completed. */
  unsigned long m_ulCompletionExpiration = 0;
};

extern CommandInterpreter g_ci;

#endif

