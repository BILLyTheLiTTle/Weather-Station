#pragma once

#include <Arduino.h>

#define DEBUG_ON 1

#if DEBUG_ON
  #define DBG_BEGIN(baud) { Serial.begin(baud); }

  #define DBG(msg) { Serial.print(msg); }
  #define DBG_LN(msg) { Serial.println(msg); }

  #define DBG_STAT(label, value) { Serial.print(label); Serial.print(F(": ")); Serial.println(value); }

  #define DBG_SEP() { Serial.println(F("--------------------")); }

  #define DBG_FLUSH() { Serial.flush(); }
  #else
    // NO-OP
    #define DBG_BEGIN(baud)         ((void)0)

    #define DBG(msg)                ((void)0)
    #define DBG_LN(msg)             ((void)0)

    #define DBG_STAT(label, value)  ((void)0)

    #define DBG_SEP()               ((void)0)

    #define DBG_FLUSH()             ((void)0)
#endif