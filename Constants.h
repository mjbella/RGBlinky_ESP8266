#pragma once

//LED stuff
#define   MAX_LED_COUNT   20
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <NeoPixelBrightnessBus.h>
extern NeoPixelBrightnessBus<NeoRgbFeature, NeoEsp8266Uart800KbpsMethod> strip;
