#pragma once

//LED stuff
#define   NPLEN           20
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <NeoPixelBrightnessBus.h>
extern NeoPixelBrightnessBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip;
