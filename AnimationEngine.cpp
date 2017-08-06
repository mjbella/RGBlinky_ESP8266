#include <math.h>

#include "AnimationEngine.h"

//assume led count is defined somewhere.
//assume we already have a strip from some place.

HsbColor FrameBuffer[MAX_LED_COUNT];
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
void AnimateSaturate( animation_descriptor_t* descriptor, uint32_t current_time_ms, uint32_t last_time_ms );
void AnimateShift( animation_descriptor_t* descriptor, uint32_t current_time_ms, uint32_t last_time_ms );
void AnimateDiffuse( animation_descriptor_t* descriptor, uint32_t current_time_ms, uint32_t last_time_ms );
void WriteFrameBufferToStrip(  );
void AdvanceDescriptor(uint32_t current_time );
void convolve(const double Signal[/* SignalLen */], size_t SignalLen,
              const double Kernel[/* KernelLen */], size_t KernelLen,
              double Result[/* SignalLen + KernelLen - 1 */]);
              
#define twopi 6.2831

void StartSequence( animation_descriptor_t* descriptors, int num_descriptors )
{
  DescriptorList = descriptors;
  NumDescriptors = num_descriptors;
  CurrentDescriptor = 0;
  DescriptorStartTimeMs = 0;
  NextDescriptorTimeMs = 0;
  AutoAdvanceDescriptor = false;
  for( int i = 0; i < MAX_LED_COUNT; ++i )
  {
    FrameBuffer[i] = HsbColor(0,0,0);
  }
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
    else if( current_descriptor->anim == SATURATE)
    {
      AnimateSaturate(current_descriptor, current_time_ms, last_time_ms);
    }
    else if( current_descriptor->anim == SHIFT)
    {
        AnimateShift(current_descriptor, current_time_ms, last_time_ms);
    }
    else if( current_descriptor->anim == DIFFUSE)
    {
        AnimateDiffuse(current_descriptor, current_time_ms, last_time_ms);
    }
}

void AnimateShow( animation_descriptor_t* descriptor, uint32_t current_time_ms, uint32_t last_time_ms )
{
  Serial.println("show");
    //need to populate entire buffer, or purposely leave values unchanged.
    for( int i = 0; i < MAX_LED_COUNT; ++i )
    {
        if(  (descriptor->repeat == false) && (i >= descriptor->data_len) )
        {
            return;
        }
        // 
        int lookup_index = i % descriptor->data_len;
        FrameBuffer[i] = descriptor->HsbData[lookup_index];

    }
    WriteFrameBufferToStrip();
}

void AnimateBlank( animation_descriptor_t* descriptor, uint32_t current_time_ms, uint32_t last_time_ms )
{
  Serial.println("blank");
    for( int i = 0; i < MAX_LED_COUNT; ++i )
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

    for( int i = 0; i < MAX_LED_COUNT; ++i )
    {
        if(FrameBuffer[i].B > step_size )
          FrameBuffer[i].B -= step_size;
        else
          FrameBuffer[i].B = 0;
    }
    
    WriteFrameBufferToStrip();
}

void AnimateSaturate( animation_descriptor_t* descriptor, uint32_t current_time_ms, uint32_t last_time_ms )
{
    Serial.println("saturate");
    // Increases the color saturation by n across duration ms.
    float step_size = (current_time_ms - LastFrameTimeMs) * descriptor->N / descriptor->duration;
    for( int i = 0; i < MAX_LED_COUNT; ++i )
    {
        FrameBuffer[i].S = FrameBuffer[i].S + step_size;
        if(FrameBuffer[i].S >= 1.0) FrameBuffer[i].S = 1.0;
    }
    WriteFrameBufferToStrip();
}

void AnimateShift( animation_descriptor_t* descriptor, uint32_t current_time_ms, uint32_t last_time_ms )
{
    Serial.println("shift");
    //Moves each pixel over one
    float step_size = (current_time_ms - LastFrameTimeMs) * descriptor->N / descriptor->duration;

    HsbColor tmp = FrameBuffer[MAX_LED_COUNT-1];
    
    for( int i = 1; i < MAX_LED_COUNT; ++i )
    {
        FrameBuffer[i-1] = FrameBuffer[i];
    }
    FrameBuffer[0] = tmp;
    
    WriteFrameBufferToStrip();
}

void convolve(const double Signal[/* SignalLen */], size_t SignalLen,
              const double Kernel[/* KernelLen */], size_t KernelLen,
              double Result[/* SignalLen + KernelLen - 1 */])
{
  size_t n;

  for (n = 0; n < SignalLen + KernelLen - 1; n++)
  {
    size_t kmin, kmax, k;

    Result[n] = 0;

    kmin = (n >= KernelLen - 1) ? n - (KernelLen - 1) : 0;
    kmax = (n < SignalLen - 1) ? n : SignalLen - 1;

    for (k = kmin; k <= kmax; k++)
    {
      Result[n] += Signal[k] * Kernel[n - k];
    }
  }
}


void AnimateDiffuse( animation_descriptor_t* descriptor, uint32_t current_time_ms, uint32_t last_time_ms )
{
    //Blur the pixels into their neighbors.
    float step_size = (current_time_ms - LastFrameTimeMs) * descriptor->N / descriptor->duration;
    HsbColor output[MAX_LED_COUNT];
    RgbColor rgbtmp[MAX_LED_COUNT];
    

    //Step 1 convert hsb to rgb
    for(int i=0; i < MAX_LED_COUNT; i++)
    {
      //output[i] = HsbColor(0,0,0);
      rgbtmp[i] = RgbColor(FrameBuffer[i]);
    }

    //TODO Step 2 break rgb appart and run each through the convolution seperately
    //TODO Step 3 recombine and convert back too hsv
    

    
    for( int i = 0; i < MAX_LED_COUNT; ++i )
    {
      FrameBuffer[i] = output[i];
    }
    
    WriteFrameBufferToStrip();

    /*for( int i = 0; i < MAX_LED_COUNT; ++i )
    {
        FrameBuffer[i].H = (atan2((cos(FrameBuffer[i+1].H*twopi)) + (cos(FrameBuffer[i].H*twopi)*1.5), (sin(FrameBuffer[i+1].H*twopi)) + (sin(FrameBuffer[i].H*twopi)*1.5))+3.1415)/twopi;
        FrameBuffer[i].S = (FrameBuffer[i+1].S*0.1 + FrameBuffer[i].S*0.9);
        FrameBuffer[i].B = (FrameBuffer[i+1].B*0.1 + FrameBuffer[i].B * 0.9);
    }
    */
}

void WriteFrameBufferToStrip(  )
{
    for( int i = 0; i < MAX_LED_COUNT; ++i )
    {
        //Serial.print( FrameBuffer[i].H );
        //Serial.print( " " );
        //Serial.print( FrameBuffer[i].S );
        //Serial.print( " " );
        //Serial.println( FrameBuffer[i].B );
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

