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
 * Globals: Main command interpreter
 */
PannerCommandInterpreter g_ci(pinPanStep, pinPanDirection, pinPanEnable);  // pan pinStep, pinDirection, pinEnable
CommandInterpreter *g_pCommandInterpreter = &g_ci;

/**
 * Globals: Serial Port object
 */
//MySerialCommand g_serialCommandInterpreter;

void setup()
{  
  Serial.begin(9600);
  
  delay(1000);   
  //while(!Serial)  ; // wait for serial port to connect. Needed for Leonardo only
  DEBUG_PRINTLN("Panner test!");

  g_batteryMonitor.update(millis());
  
  View::setup();
  View::activate(&g_controlView);

  //g_serialCommandInterpreter.begin();
    
  g_ci.begin();
}

void loop()
{  
  unsigned long now = millis();

  bool bUpdateDisplay = View::g_pActiveView->loop(now);

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
  else if(g_batteryMonitor.updateMaybe(now))
  {    
    bUpdateDisplay = true;
  }
  //if(bUpdateDisplay) g_ci.updateDisplay(now);   
  if(bUpdateDisplay)
    View::g_pActiveView->update(now);
}


