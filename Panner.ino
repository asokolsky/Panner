/********************************************************\
 *                                                      *
 * Simple automated video panner                        *
 *                                                      *
 * Alex Sokolsky                                        *
 *                                                      *
 *   v0.1  6/23/2016                                    *
 *                                                      *
\********************************************************/

#include "Panner.h"
#include "BatteryMonitor.h"


//#define NODEBUG 1

/**
 * Globals: reading battery voltage on A0 (through divider offcourse)
 */
BatteryMonitor g_batteryMonitor(A0);
/** 
 * Globals: simple analog keypad is connected to pin A1
 */
Keypad g_keyPad(A1);

/** 
 * Globals: analog ThumbStick pinX, pinY, pinButton 
 */
//ThumbStick g_thumbStick(A9, A8, A7);


const uint8_t pinPanStep = 3;
const uint8_t pinPanEnable = 4;
const uint8_t pinPanDirection = 6;

/** 
 * Globals: stepper driver sits on these two pins.  
 */
//Stepper g_panner(pinPanStep, pinPanDirection, pinPanEnable);

/**
 * Globals: Main command interpreter
 */
static PannerCommandInterpreter g_ci(pinPanStep, pinPanDirection, pinPanEnable);  // pan pinStep, pinDirection, pinEnable
CommandInterpreter *g_pCommandInterpreter = &g_ci;

/**
 * Globals: commands to run at startup
 */
static Command cmds[] = {
  {chControl, cmdControlBeginLoop, 0, 0},
    {chControl, cmdControlRest,  0, 10000},  // rest for 10 sec
    {chPan,     cmdGoTo, 0, -400},                // go left
    {chControl, cmdControlWaitForCompletion,  0, 50000},  // wait for the movement to be completed for 50 sec
    {chControl, cmdControlRest,  0, 10000},  // rest for 10 sec
    {chPan,     cmdGoTo, 0, 400},                 // go right
    {chControl, cmdControlWaitForCompletion,  0, 50000},  // wait for the movement to be completed for 50 sec
  {chControl, cmdControlEndLoop, 0, 0},
  {chControl, cmdControlNone,    0, 0}
};

/**
 * Globals: views
 */
ControlView g_controlView;
EditView g_editView;
RunView g_runView;
PausedRunView g_pausedRunView;

/**
 * Globals: Serial Port object
 */
//MySerialCommand g_serialCommandInterpreter;

void setup()
{  
  Serial.begin(9600);
  
  delay(1000);   
  while(!Serial)  ; // wait for serial port to connect. Needed for Leonardo only
  DEBUG_PRINTLN("Panner test!");

  g_batteryMonitor.update(millis());
  
  View::setup();
  View::activate(&g_controlView);

  //g_serialCommandInterpreter.begin();
    
  g_ci.begin();
  //g_ci.beginRun(cmds);
}

void loop()
{  

  unsigned long now = millis();

  bool bUpdateDisplay = false;

  if(g_ci.isRunning()) {
    if(g_ci.continueRun(now)){
      ;
    } else {
      g_ci.endRun();
    }
  }
  
  //g_panner.runSpeed(now);

  if(g_keyPad.getAndDispatchKey(now)) 
  {
    bUpdateDisplay = true;
  } 
  /*else if(g_thumbStick.getAndDispatchThumb(now)) 
  {
    bUpdateDisplay = true;
  }*/
  /*else if(g_serialCommandInterpreter.available()) 
  {
    DEBUG_PRINTLN("Read a command from serial line!");   
    g_serialCommandInterpreter.readAndDispatch();
    bUpdateDisplay = true;
  } */
  else 
  {
    bUpdateDisplay = g_batteryMonitor.updateMaybe(now);
  }
  if(bUpdateDisplay)
    g_ci.updateDisplay(now);    
  
}


