#pragma once

#define   NPLEN           4
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <NeoPixelBrightnessBus.h>
extern NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip;
