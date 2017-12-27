#include "Arduino.h"
#include "StateManager.h"
#include "MarkAnimations.h"
#include "Constants.h"
#include <math.h>

#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <NeoPixelBrightnessBus.h>
//extern NeoPixelBrightnessBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip;

typedef void (*RenderFunctionType)(uint32_t time_ms, int num_leds);

void RenderAdvanceState( uint32_t time_ms, int num_leds );
void RenderIndividualState( uint32_t time_ms, int num_leds );
void RenderMeshState( uint32_t time_ms, int num_leds );
void RenderRainbow( uint32_t time_ms, int num_leds );
void RenderStarFire( uint32_t time_ms, int num_leds );
void RenderColorSparkle( uint32_t time_ms, int num_leds );
void RenderWhiteSparkle( uint32_t time_ms, int num_leds );
void RenderSpinner( uint32_t time_ms, int num_leds );
void RenderCurrentTest( uint32_t time_ms, int num_leds );

RenderFunctionType RenderFunctions[] = { RenderRainbow, RenderRainbow, RenderStarFire, RenderColorSparkle, RenderWhiteSparkle, RenderSpinner, RenderCurrentTest };

void RenderFrame( uint32_t time_ms, int num_leds, int state )
{
  if( state < NUM_STATES )
  {
    RenderFunctions[state](time_ms, num_leds);
    
  }
  else if( state == ADVANCE_INDICATE_STATE )
  {
    RenderAdvanceState( time_ms, num_leds );
  }
  else if( state == INDIVIDUAL_INDICATE_STATE )
  {
    RenderIndividualState( time_ms, num_leds );
  }
  else if( state == MESH_INDICATE_STATE )
  {
    RenderMeshState( time_ms, num_leds );
  }

  DebugWrite(DEBUG_PIN3, HIGH);
  strip.Show();
  DebugWrite(DEBUG_PIN3, LOW);
  
  
  
}

void RenderAdvanceState( uint32_t time_ms, int num_leds )
{
  const uint32_t period_ms = 500;
  const uint32_t local_time_ms = time_ms % period_ms;
  const byte brightness = local_time_ms <= (period_ms/2) ? local_time_ms * 255 / (period_ms/2) : (period_ms - local_time_ms) * 255 / (period_ms/2);
  
  for ( uint8_t i = 0; i < num_leds; i++ ) {
    RgbColor color = RgbColor( 0, 0, 0 );
    if ( i == nextState )
    {
      color = RgbColor( brightness, brightness, brightness );
    }
    strip.SetPixelColor(i, color );
  }
}

void RenderIndividualState( uint32_t time_ms, int num_leds )
{
  const uint32_t period_ms = 500;
  const uint32_t local_time_ms = time_ms % period_ms;
  const byte brightness = local_time_ms <= (period_ms/2) ? local_time_ms * 255 / (period_ms/2) : (period_ms - local_time_ms) * 255 / (period_ms/2);
  
  for ( uint8_t i = 0; i < num_leds; i++ ) {
    RgbColor color = RgbColor( brightness, 0, 0 );
    strip.SetPixelColor(i, color );
  }
}

void RenderMeshState( uint32_t time_ms, int num_leds )
{
  const uint32_t period_ms = 500;
  const uint32_t local_time_ms = time_ms % period_ms;
  const byte brightness = local_time_ms <= (period_ms/2) ? local_time_ms * 255 / (period_ms/2) : (period_ms - local_time_ms) * 255 / (period_ms/2);
  
  for ( uint8_t i = 0; i < num_leds; i++ ) {
    HsbColor color = HsbColor( (float)i / num_leds, 1.0, (float)brightness / 255 );
    strip.SetPixelColor(i, color );
  }
}

void RenderRainbow( uint32_t time_ms, int num_leds )
{
  const uint32_t period_ms = 1000;
  const uint32_t local_time_ms = time_ms % period_ms;
  float time_fraction = (float)local_time_ms / period_ms;

  DebugWrite( DEBUG_PIN2, HIGH);
  for ( uint8_t i = 0; i < num_leds; i++ ) {
    float hue = (float)i / num_leds + time_fraction;
    hue = hue - (int)hue; //roll off to integer range.
    
    HsbColor color = HsbColor( hue, 1.0, 0.5 );
    strip.SetPixelColor( i, color );
  }
  DebugWrite( DEBUG_PIN2, LOW);
}

//helper
void RenderStarFireFlare( int location, int num_leds )
{
  //location: 0 = back of the bike.
  //flares shoot toward the ends of the strip.

  if( location >= 0 )
  {
    //front hot pixel, at the specified location.
    strip.SetPixelColor( location, RgbColor(100,100,100) );
    //and it's mirror:
    strip.SetPixelColor( num_leds-location-1, RgbColor(100,100,100) );
  }
  
  location++;
  int side_length = num_leds / 2;
  if( location > side_length )
    return;

  if( location >= 0 )
  {
    strip.SetPixelColor( location, RgbColor(175,0,0) );
    //and it's mirror:
    strip.SetPixelColor( num_leds-location-1, RgbColor(175,0,0) );
  }

  location++;
  if( location > side_length )
    return;

  if( location >= 0 )
  {
    strip.SetPixelColor( location, RgbColor(25,0,0) );
    //and it's mirror:
    strip.SetPixelColor( num_leds-location-1, RgbColor(25,0,0) );
  }
  
}

void RenderStarFire( uint32_t time_ms, int num_leds )
{
  const uint32_t period_ms = 800;
  const uint32_t local_time_ms = time_ms % period_ms;

  int side_length = num_leds / 2;
 
  if( num_leds % 2 == 1 )
    strip.SetPixelColor( num_leds / 2 + 1, RgbColor(0,0,0) );

  for ( uint8_t i = 0; i < num_leds; i++ ) {
    strip.SetPixelColor( i, RgbColor(0,0,0) );
  }

  

  //index starts at num_leds / 2, and goes to 0, based on local_time_ms.
  uint32_t location = side_length * local_time_ms / period_ms;
  
  int flare_spacing = 5;
  int flare_count = side_length / flare_spacing;
  if( side_length % flare_spacing != 0 )
    flare_count++;
  for( int i = 0; i < flare_count; ++i )
  {
    int32_t location = side_length * local_time_ms / period_ms;
    location += i * flare_spacing;
    location = location % side_length;
    RenderStarFireFlare(side_length - location, num_leds);
  }
  
}

void RenderSparkle( uint32_t time_ms, int num_leds, bool use_color )
{
  //TODO: lets get way more sparkles, but make more of them dim.
  
  //each pixel has a random change of sparkling.
  //lets say that we want about one third of the strip to sparkle each second on average.
  const int odds =  30 / 2;
  
  
  for ( uint8_t i = 0; i < num_leds; i++ ) {
    if( random(odds) == 0 )
    {
      //sparkle!  
      float hue = random(100) / 100.0;
      float brightness = random(20, 100) / 100.0; //all are at least 50% brightness.
      brightness = brightness * brightness * brightness * brightness * brightness * brightness;
      float saturation = random(70, 100) / 100.0; //some color?
      if( use_color == false ) 
      {
          saturation = 0.0;
          hue = 0.0;
      }
      HsbColor color = HsbColor(  hue, saturation, brightness );
      strip.SetPixelColor( i, color );
      if( brightness > 0.65 )
      {
        //lets glow the neighbors.
        color = HsbColor(  hue, saturation, brightness / 4.0 );
        if( i >= 1 )
        {
          strip.SetPixelColor( i-1, color );
        }
        if( i < (num_leds-1) )
        {
          strip.SetPixelColor( i+1, color );
        }
      }
    }
    else
    {
      //decay.
      RgbColor color = strip.GetPixelColor( i );
      HsbColor hsb = HsbColor( color );
      if( hsb.B > 0.0 )
      {
        hsb.B = std::max( 0.0, hsb.B - (1.0 / 40) );
      }
      strip.SetPixelColor( i, hsb );
    }
    
  }

}

void RenderColorSparkle( uint32_t time_ms, int num_leds )
{
  RenderSparkle( time_ms, num_leds, true );
}

void RenderWhiteSparkle( uint32_t time_ms, int num_leds )
{
  RenderSparkle( time_ms, num_leds, false );
}

float intPower( float value, int times )
{
  float original = value;
  for( int i = 0; i < times-1; ++i )
    value = value * original;
  return value;
}

void RenderSpinner( uint32_t time_ms, int num_leds )
{
    //loop every 1 second.
    float position = (time_ms % 1000) / 1000.0 * (float)num_leds;

    float size = 10.0;
    float half_size = size / 2.0;
 
    //hue cycles every 10 seconds.
    float hue = ( time_ms % 10000 ) / 10000.0;

    for( int i = 0; i < num_leds; ++i )
    {
      float delta1 = abs(position - (float)i);
      float delta2 = abs((position+num_leds) - (float)i);
      float delta3 = abs(position - (float)(i+num_leds));
      float delta = std::min( delta3, std::min(delta1, delta2) );

      float brightness = std::max( ( half_size - delta ), (float)0.0 ) / half_size;
        
      strip.SetPixelColor( i, HsbColor( hue, 1.0, intPower(brightness, 3 ) ) );
    }
}

void RenderCurrentTest( uint32_t time_ms, int num_leds )
{
  //lets do 5 steps for 3 seconds seach of 4 modes:
  //white, red, green, blue.
  //20 states, x3 = 60 seconds.
  const uint32_t state_period = 4;
  const uint32_t num_levels = 5;
  const uint32_t period = 4 * num_levels * state_period;
  uint32_t local_time_s = (time_ms / 1000) % period;

  //Serial.print("local time: ");
  //Serial.println(local_time_s);
  //level should cycle 0 to 4, 4 times per cycle.
  int level = (local_time_s / state_period) % num_levels;
  int color_state = local_time_s / (num_levels * state_period);
  //Serial.print("color: ");
  //Serial.println(color_state);
  //Serial.print("level: ");
  //Serial.println(level);
  int brightness = level * 255 / 4;
  int r = 0;
  int g = 0;
  int b = 0;
  if (color_state == 0 || color_state == 1 )
    r = brightness;
  if (color_state == 0 || color_state == 2 )
    g = brightness;
  if (color_state == 0 || color_state == 3 )
    b = brightness;
  

  RgbColor color = RgbColor(r, g, b);

  for( int i = 0; i < num_leds; ++i )
    strip.SetPixelColor(i, color);
  
  
}

