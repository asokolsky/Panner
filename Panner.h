/********************************************************\
 *                                                      *
 * Simple automated video panner                        *
 *                                                      *
 * Alex Sokolsky                                        *
 *                                                      *
 *   v0.1  6/23/2016                                    *
 *                                                      *
\********************************************************/
#include <Arduino.h>

#include <vector>
#include <string>
#include <map>
#include <climits>

inline int16_t min(int16_t a, int16_t b) {
  return (a < b) ? a : b;
}
inline int16_t max(int16_t a, int16_t b) {
  return (a > b) ? a : b;
}

#include <ILI9341_t3.h>
#include <AccelStepper.h>
#include <Wire.h>

/** CHAR is signed on AVR and unsigned on ARM!  WTF! */
typedef signed char schar_t;

//#define NODEBUG 1
#include "Trace.h"
//#include "ThumbStick.h"
#include "AnalogNavigationKeyPad.h"
#include "Stepper.h"
#include "CommandInterpreter.h"
#include "PersistentSettings.h"
#include "Views.h"
#include "Views2.h"
#include "PCB.h"
//#include "SerialCommand.h"



