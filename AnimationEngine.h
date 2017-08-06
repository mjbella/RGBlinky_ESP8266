#pragma once

// Definitions for the animation engine go here!

#include "Constants.h"
#include <NeoPixelBus.h>

/* Animation Types
 * Show     - Just update leds based on led data. (overwrites)
 * Blank    - Set all off (overwrites)
 * Dim      - Decrease each chanel of each led by n (modifies)
 * Bright   - Increase the brightness of each HSB pixel value. (modifies)
 * Saturate - Increase each LED's color intensity by n (modifies)
 * Shift    - Move the pattern towards the end of the string (modifies)
 * bShift   - Move the pattern towards the front of the string (modifies)
 * Diffuse  - Blur the string (bleed each pixel on to its neighbors) (modifies)
 * Scramb   - Swap pixels randomly every n milliseconds (modifies)
 * Blink    - Flash between the existing pattern and black! n (ms) period (Combinable!)
 *            [Run blink after another time based effect in order to combine the blinking with the prior effect.]
 * Delay    - Hold the existing pattern on the display for n milliseconds.
 * HueShift - Rotate the hue once around the color wheel 
 * Twinkle  - Flash your led data color palette randomly on the strip with a decay effect between flashes.
 * Spazz    - Cycle through your led data color pallette on each LED quickly (out of phase) for a dazling effect) for n ms
 * add      - Add to all LEDs from led data (modifies)
 * randadd  - Add random color to a random location (modifies)
 * (ADD MORE NEW ANIMATION TYPES HERE!)
 */
typedef enum {
  SHOW,
  BLANK,
  DIM,
  BRIGHT,
  SATURATE,
  SHIFT,
  bSHIFT,
  DIFFUSE,
  SCRAMB,
  BLINK,
  DELAY,
  HUESHIFT,
  TWINKLE,
  SPAZZ,
  ADD,
  RANDADD
} animation_effect;



/* Animation Sequence Format (Struct)
 * Effect Type
 * N (long)
 * Duration
 * nLoops
 * nLEDS
 * Repeat Pattern? (if true repeat the pattern to the end of the string)
 * Data Length
 * RGB LED Data Array (NeoPixelBus Array)
 */
typedef struct {
  animation_effect anim;
  float N;
  long duration; //ms
  long nLoops;
  int nLEDS;
  bool repeat;
  int data_len;
  HsbColor* HsbData;
  
} animation_descriptor_t;


void StartSequence( animation_descriptor_t* descriptors, int num_descriptors );

void Animate( uint32_t current_time_ms, uint32_t last_time_ms  );
