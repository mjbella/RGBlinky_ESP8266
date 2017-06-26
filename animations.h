// Definitions for the animation engine go here!

/* Animation Types
 * Show     - Just update leds based on led data.
 * Blank    - Set all off
 * Dim      - Decrease each chanel of each led by n
 * Saturate - Increase each LED's color intensity by n
 * Shift    - Move the pattern towards the end of the string
 * bShift   - Move the pattern towards the front of the string
 * Diffuse  - Blur the string (bleed each pixel on to its neighbors)
 * Scramb   - Swap pixels randomly every n milliseconds
 * Blink    - Flash between the existing pattern and black! n (ms) period
 * (ADD MORE NEW ANIMATION TYPES HERE!)
 */
typedef enum {
  SHOW,
  BLANK,
  DIM,
  SATURATE,
  SHIFT,
  bSHIFT,
  DIFFUSE,
  SCRAMB,
  BLINK
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
  animation_effect anim,
  long N,
  long duration,
  long nLoops,
  int nLEDS,
  bool repeat,
  int data_len,
  
}



