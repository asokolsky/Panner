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
 *  Necessary for STL to link
 */
extern "C"{
  int _getpid(){ return -1;}
  int _kill(int pid, int sig){ return -1; }
}


/**
 * Globals: reading battery voltage on A0 (through divider offcourse)
 */
BatteryMonitor g_batteryMonitor(pinBatteryMonitor);


/**
 * Globals: Main command interpreter
 */

CommandInterpreter g_ci(pinPanStep, pinPanDirection, pinSteppersEnable);  // pan pinStep, pinDirection, pinEnable

/**
 * Globals: Serial Port object
 */
//MySerialCommand g_serialCommandInterpreter;

/**
 * Globals: My Navigation Keypad
 */
class MyNavKeyPad: public AnalogNavigationKeypad
{
public:  
  /** this test if for a keyboard connected to A0 and A1 */
  MyNavKeyPad() : AnalogNavigationKeypad(A1, A2) 
  {
    
  }
  bool onUserInActivity(unsigned long ulNow);
  bool onKeyAutoRepeat(uint8_t vks);
  bool onKeyDown(uint8_t vks);
  bool onLongKeyDown(uint8_t vks);
  bool onKeyUp(uint8_t vks);
};

bool MyNavKeyPad::onUserInActivity(unsigned long ulNow)
{
  DEBUG_PRINT("MyNavKeyPad::onUserInActivity ulNow="); DEBUG_PRINTDEC(ulNow); DEBUG_PRINTLN("");
  return View::g_pActiveView->onKeysInactive(); 
}

bool MyNavKeyPad::onKeyAutoRepeat(uint8_t vks)
{
  DEBUG_PRINT("MyNavKeyPad::onKeyAutoRepeat vks="); DEBUG_PRINTLN(getKeyNames(vks));
  return View::g_pActiveView->onKeyAutoRepeat(vks);  
}

bool MyNavKeyPad::onKeyDown(uint8_t vks)
{
  DEBUG_PRINT("MyNavKeyPad::onKeyDown vks="); DEBUG_PRINTLN(getKeyNames(vks));
  return View::g_pActiveView->onKeyDown(vks); 
}

bool MyNavKeyPad::onLongKeyDown(uint8_t vks)
{
  DEBUG_PRINT("MyNavKeyPad::onLongKeyDown vks="); DEBUG_PRINTLN(getKeyNames(vks));
  return View::g_pActiveView->onLongKeyDown(vks); 
}

bool MyNavKeyPad::onKeyUp(uint8_t vks)
{
  DEBUG_PRINT("MyNavKeyPad::onKeyUp vks="); DEBUG_PRINTLN(getKeyNames(vks));
  return View::g_pActiveView->onKeyUp(vks); 
}



/** kb interface */
MyNavKeyPad g_keyPad;


/**
 * Entry Point
 */
void setup()
{  
  Serial.begin(9600);
  
  delay(1000);   
  //while(!Serial)  ; // wait for serial port to connect. Needed for Leonardo only
  DEBUG_PRINTLN("Panner test!");

  g_batteryMonitor.update(millis());

  g_settings.restore();

  View::setup();
  View::activate(&g_controlView);

  //g_serialCommandInterpreter.begin();
}

void loop()
{  
  unsigned long now = millis();

  bool bUpdateDisplay = false;
  if(View::g_pActiveView != 0)
    bUpdateDisplay = View::g_pActiveView->loop(now);

  if(g_keyPad.getAndDispatchKey(now)) 
  {
    bUpdateDisplay = true;
  } 
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
  if(bUpdateDisplay && View::g_pActiveView != 0)
    View::g_pActiveView->update(now);
}


