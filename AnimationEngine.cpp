#include <math.h>

#include "AnimationEngine.h"



//assume led count is defined somewhere.
//assume we already have a strip from some place.

HsbColor FrameBuffer[NPLEN];
uint32_t LastFrameTimeMs;
animation_descriptor_t* DescriptorList;
int NumDescriptors;
int CurrentDescriptor;
uint32_t DescriptorStartTimeMs;
uint32_t NextDescriptorTimeMs;
bool AutoAdvanceDescriptor = false;

void AnimateShow( animation_descriptor_t* descriptor, uint32_t current_time_ms, uint32_t last_time_ms );
void AnimateBlank( animation_descriptor_t* descriptor, uint32_t current_time_ms, uint32_t last_time_ms );
void AnimateDim( animation_descriptor_t* descriptor, uint32_t current_time_ms, uint32_t last_time_ms );
void WriteFrameBufferToStrip(  );
void AdvanceDescriptor(uint32_t current_time );

void StartSequence( animation_descriptor_t* descriptors, int num_descriptors )
{
  DescriptorList = descriptors;
  NumDescriptors = num_descriptors;
  CurrentDescriptor = 0;
  DescriptorStartTimeMs = 0;
  NextDescriptorTimeMs = 0;
  AutoAdvanceDescriptor = false;
}

void Animate( uint32_t current_time_ms, uint32_t last_time_ms )
{
  if( DescriptorStartTimeMs == 0 )
  {
    DescriptorStartTimeMs = current_time_ms;
  }

  if( (AutoAdvanceDescriptor == true) && (current_time_ms >= NextDescriptorTimeMs) )
  {
    AdvanceDescriptor(current_time_ms);
  }

  
  animation_descriptor_t* current_descriptor = DescriptorList + CurrentDescriptor;

  
  
    LastFrameTimeMs = last_time_ms;

    if( current_descriptor->anim == SHOW )
    {
        AnimateShow(current_descriptor, current_time_ms, last_time_ms);
        //auto advance!
        AdvanceDescriptor(current_time_ms);
    }
    else if( current_descriptor->anim == BLANK )
    {
        AnimateBlank(current_descriptor, current_time_ms, last_time_ms);
        AdvanceDescriptor(current_time_ms);
    }
    else if( current_descriptor->anim == DIM )
    {
      AnimateDim(current_descriptor, current_time_ms, last_time_ms);
    }

}

void AnimateShow( animation_descriptor_t* descriptor, uint32_t current_time_ms, uint32_t last_time_ms )
{
  Serial.println("show");
    //need to populate entire buffer, or purposely leave values unchanged.
    for( int i = 0; i < NPLEN; ++i )
    {
        if(  (descriptor->repeat == false) && (i >= descriptor->data_len) )
        {
            return;
        }

        int lookup_index = i % descriptor->data_len;
        FrameBuffer[i] = descriptor->HsbData[lookup_index];

    }
    WriteFrameBufferToStrip();
}

void AnimateBlank( animation_descriptor_t* descriptor, uint32_t current_time_ms, uint32_t last_time_ms )
{
  Serial.println("blank");
    for( int i = 0; i < NPLEN; ++i )
    {
        FrameBuffer[i].H = 0;
        FrameBuffer[i].S = 0;
        FrameBuffer[i].B = 0;
    }

    WriteFrameBufferToStrip();
}

void AnimateDim( animation_descriptor_t* descriptor, uint32_t current_time_ms, uint32_t last_time_ms )
{
  Serial.println("dim");
    //only affects brightness.
    float step_size = (current_time_ms - LastFrameTimeMs) * descriptor->N / descriptor->duration;

    for( int i = 0; i < NPLEN; ++i )
    {
        FrameBuffer[i].B = _max( 0, FrameBuffer[i].B - step_size );
    }
    
    WriteFrameBufferToStrip();
}

void WriteFrameBufferToStrip(  )
{
    for( int i = 0; i < NPLEN; ++i )
    {
        Serial.print( FrameBuffer[i].H );
        Serial.print( " " );
        Serial.print( FrameBuffer[i].S );
        Serial.print( " " );
        Serial.println( FrameBuffer[i].B );
        strip.SetPixelColor( i, FrameBuffer[i] );
    }

    strip.Show();
}

void AdvanceDescriptor( uint32_t current_time )
{
  int next_descriptor_index = CurrentDescriptor + 1;
  if( next_descriptor_index >= NumDescriptors )
  {
    next_descriptor_index = 0;
  }

  animation_descriptor_t* next_descriptor = DescriptorList + next_descriptor_index;

  if( (next_descriptor->anim == SHOW) || (next_descriptor->anim == BLANK) )
  {
    NextDescriptorTimeMs = 0;
    AutoAdvanceDescriptor = false;
  }
  else
  {
    NextDescriptorTimeMs = current_time + (next_descriptor->duration * next_descriptor->nLoops);
    AutoAdvanceDescriptor = true;
  }
  Serial.println(current_time);
  Serial.println( NextDescriptorTimeMs );
  Serial.print("advancing descriptor to ");
  Serial.println(next_descriptor_index);
  DescriptorStartTimeMs = current_time;
  CurrentDescriptor = next_descriptor_index;
}

