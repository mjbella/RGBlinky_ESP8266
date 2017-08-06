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
void sigma_kern(double sigma, double kern[]);

#define twopi 6.2831f

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
    else if( current_descriptor->anim == RANDADD)
    {
      
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

// =========================== for diffuse! =============================
// https://stackoverflow.com/questions/8424170/1d-linear-convolution-in-ansi-c-code
void convolve(double Signal[/* SignalLen */], size_t SignalLen,
              double Kernel[/* KernelLen */], size_t KernelLen,
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

void sigma_kern(double sigma, double kern[]){
  float j = 0, sum;
  for( int i = 0; i < 3; i++) {
    j = (i - 1) / 10.0f;
    kern[i] = ( 1 / sqrt(twopi * sigma)) * exp( -0.5f * ( pow(j,2) / pow(sigma,2) ) );
    //Serial.println(kern[i], 6);
  }
  
  // Normalize the kernel!!! (or else!!)
  for( int i = 0; i < 3; i++) sum += kern[i];
  for( int i = 0; i < 3; i++) kern[i] /= sum;

  
  //Serial.println();
}
// =========================== for diffuse! =============================
void AnimateDiffuse( animation_descriptor_t* descriptor, uint32_t current_time_ms, uint32_t last_time_ms )
{
    //Blur the pixels into their neighbors.
    float step_size = (current_time_ms - LastFrameTimeMs) * descriptor->N / descriptor->duration;
    
    HsbColor output[MAX_LED_COUNT];
    RgbColor tmp;
    double R[MAX_LED_COUNT], G[MAX_LED_COUNT], B[MAX_LED_COUNT];
    double Rc[MAX_LED_COUNT + 2], Gc[MAX_LED_COUNT + 2], Bc[MAX_LED_COUNT + 2];
    
    //double kernel[] = {0.27901, 0.44198, 0.27901}; // sigma=1
    //double kernel[] = {0.157731, 0.684538, 0.157731}; // sigma=0.5
    //double kernel[] = {0.04779, 0.90442, 0.04779}; // sigma=0.3
    double kernel[3];

    // Calculate our incremental kernel
    Serial.println(step_size, 6);
    sigma_kern(step_size, kernel);

    //Step 1 convert hsb to rgb
    for(int i=0; i < MAX_LED_COUNT; i++)
    {
      //output[i] = HsbColor(0,0,0);
       tmp = RgbColor(FrameBuffer[i]);
       // Step 2 split the rgb values into their own arrays
       R[i] = tmp.R;
       G[i] = tmp.G;
       B[i] = tmp.B;
    }

    //TODO Step 3 run R G and B through a convolution seperately
    convolve(R, MAX_LED_COUNT, kernel, 3, Rc);
    convolve(G, MAX_LED_COUNT, kernel, 3, Gc);
    convolve(B, MAX_LED_COUNT, kernel, 3, Bc);
    
    //TODO Step 4 recombine and convert back too hsv
    for( int i = 0; i < MAX_LED_COUNT; ++i )
    {
      FrameBuffer[i] = HsbColor(RgbColor(Rc[i+1], Gc[i+1], Bc[i+1]));
    }
    
    WriteFrameBufferToStrip();
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

