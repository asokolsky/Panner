/**
 * DEBUG utils
 */
#ifndef TRACE_h
#define TRACE_h

#ifndef NODEBUG
#define DEBUG 1
#endif

#ifdef DEBUG
  #define DEBUG_PRINT(x)    Serial.print(x)
  #define DEBUG_PRINTDEC(x) Serial.print(x, DEC)
  #define DEBUG_PRINTHEX(x) Serial.print(x, HEX)
  #define DEBUG_PRINTLN(x)  Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTDEC(x)
  #define DEBUG_PRINTHEX(x)
  #define DEBUG_PRINTLN(x)
#endif 

#endif //TRACE_h

