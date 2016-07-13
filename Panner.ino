

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
#include "PannerCommandInterpreter.h"


//#define NODEBUG 1

/** 
 * Globals: simple analog keypad is connected to pin A0 
 */
//Keypad g_keyPad(A0);

/** 
 * Globals: analog ThumbStick pinX, pinY, pinButton 
 */
ThumbStick g_thumbStick(A9, A8, A7);


const uint8_t pinStep = 3;
const uint8_t pinDirection = 6;

/** 
 * Globals: stepper driver sits on these two pins.  
 */
//AccelStepper g_panner(AccelStepper::DRIVER, pinStep, pinDirection);

/**
 * Globals: Main command interpreter
 */
static PannerCommandInterpreter g_ci(pinDirection, pinStep);  // pan dir, step
CommandInterpreter *g_pCommandInterpreter = &g_ci;


/**
 * Globals: views and a pointer to a currently selected one.
 */
SimpleView g_simpleView;
View *g_pView = &g_simpleView;

/**
 * Globals: Serial Port object
 */
MySerialCommand g_serialCommandInterpreter;

void setup()
{  
  Serial.begin(9600);
  
  delay(5000);   
  while(!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }   
  DEBUG_PRINTLN("Panner test!");
  
  g_pView->setup();

  g_serialCommandInterpreter.begin();
  
  
  //g_panner.setMaxSpeed(50);  
  //g_panner.setSpeed(50);  
}

void loop()
{  
  unsigned long now = millis();
  
  if(g_ci.isRunning()) {
  if(g_ci.continueRun(now)){
    ;
  } else {
    g_ci.endRun();
  }
  }
  if(g_thumbStick.getAndDispatchThumb(now)) {
    ;
  } else if(g_serialCommandInterpreter.available()) {
    DEBUG_PRINTLN("Read a command from serial line!");   
    g_serialCommandInterpreter.readAndDispatch();
    g_ci.updateDisplay(now);    
  }
  
  //g_panner.runSpeed();
}


