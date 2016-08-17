/********************************************************\
 *                                                      *
 * Simple automated video panner                        *
 *                                                      *
 * Alex Sokolsky                                        *
 *                                                      *
 *   v0.1  6/23/2016                                    *
 *                                                      *
\********************************************************/

#include <ILI9341_t3.h>
#include <Wire.h>
#include <AccelStepper.h>

/** CHAR is signed on AVR and unsigned on ARM!  WTF! */
typedef signed char schar_t;

#include "Trace.h"
//#include "ThumbStick.h"
#include "Keypad.h"
#include "Stepper.h"
#include "Views.h"
#include "CommandInterpreter.h"
//#include "SerialCommand.h"
#include "Stepper.h"



