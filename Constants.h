#pragma once

//LED stuff
#define   MAX_LED_COUNT   20
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <NeoPixelBrightnessBus.h>
extern NeoPixelBrightnessBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip;

#ifdef ARDUINO_ESP8266_ESP01 
#define DEBUG_PIN1        1
#define DEBUG_PIN2        3
#define DEBUG_PIN3        4
#define DEBUG_PIN4        5


#else
#define DEBUG_PIN1        D5
#define DEBUG_PIN2        D6
#define DEBUG_PIN3        D7
#define DEBUG_PIN4        D8
#endif

extern void DebugWrite(int pin, int value );
