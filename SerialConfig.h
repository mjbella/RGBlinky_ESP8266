#pragma once

typedef void (*SerialConfigChangedCallbackType)(uint8_t value);

void InitSerialConfig(SerialConfigChangedCallbackType brightness_changed, SerialConfigChangedCallbackType led_count_changed);

void ProcessSerial();

uint8_t GetConfigBrightness();
uint8_t GetConfigLedCount();