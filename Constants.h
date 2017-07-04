#pragma once

//LED stuff
#define   MAX_LED_COUNT   120
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <NeoPixelBrightnessBus.h>
extern NeoPixelBrightnessBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip;
