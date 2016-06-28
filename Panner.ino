

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

//#define NODEBUG 1

Keypad g_keyPad(A0);

const uint8_t pinStep = 3;
const uint8_t pinDirection = 6;
AccelStepper g_panner(AccelStepper::DRIVER, pinStep, pinDirection);

SimpleView g_simpleView;
View *g_pView = &g_simpleView;

void setup()
{  
   Serial.begin(9600);
   
   delay(5000);   
   DEBUG_PRINTLN("Panner test!");
  
   g_pView->setup();
   
   g_panner.setMaxSpeed(50);  
   g_panner.setSpeed(50);  
}

void loop()
{  
   unsigned long now = millis();

   if(g_keyPad.getAndDispatchKey(now)) {
     ;
   }

   g_panner.runSpeed();
}


