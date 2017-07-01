#include "Arduino.h"
#include "StateManager.h"
#include "MarkAnimations.h"


#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <NeoPixelBrightnessBus.h>
extern NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip;

typedef void (*RenderFunctionType)(uint32_t time_ms, int num_leds);

void RenderAdvanceState( uint32_t time_ms, int num_leds );
void RenderIndividualState( uint32_t time_ms, int num_leds );
void RenderMeshState( uint32_t time_ms, int num_leds );
void RenderRainbow( uint32_t time_ms, int num_leds );
void RenderStarFire( uint32_t time_ms, int num_leds );

RenderFunctionType RenderFunctions[] = { RenderRainbow, RenderRainbow, RenderStarFire };

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

  strip.Show();
  
  
  
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
  
  for ( uint8_t i = 0; i < num_leds; i++ ) {
    float hue = (float)i / num_leds + time_fraction;
    hue = hue - (int)hue; //roll off to integer range.
    
    HsbColor color = HsbColor( hue, 1.0, 0.5 );
    strip.SetPixelColor( i, color );
  }
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

