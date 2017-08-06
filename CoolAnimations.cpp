#include "CoolAnimations.h"

animation_descriptor_t LoadDescriptor;
animation_descriptor_t DimDescriptor;
animation_descriptor_t blinkDescriptor;

animation_descriptor_t DiscriptorSet[2];

HsbColor LoadData[4];

void InitAnimations()
{
  LoadDescriptor.anim = SHOW;
  //LoadDescriptor.N;
  //LoadDescriptor.duration;
  //LoadDescriptor.nLoops;
  //LoadDescriptor.nLEDS;
  LoadDescriptor.repeat = false;
  LoadDescriptor.data_len = 4;
  LoadDescriptor.HsbData = LoadData;

/*  for( int i = 0; i < 4; ++i )
  {
    LoadData[i] = HsbColor( (float)i / 4.0,  1.0, 0.5 );
    Serial.print( LoadData[i].H );
    Serial.print( " " );
    Serial.print( LoadData[i].S );
    Serial.print( " " );
    Serial.println( LoadData[i].B );
  }
*/
  LoadData[0] = HsbColor( 0.11, 1.0, 1.0 );
  LoadData[1] = HsbColor( 0.0, 0.0, 0.0 );
  LoadData[2] = HsbColor( 0.0, 0.0, 0.0 );
  LoadData[3] = HsbColor( 0.75, 1.0, 1.0 );

  DimDescriptor.anim = DIFFUSE;
//  DimDescriptor.anim = DIM;
  DimDescriptor.N = 3;
  DimDescriptor.duration = 2000; 
  DimDescriptor.nLoops = 1;
  //DimDescriptor.nLEDS;
  DimDescriptor.repeat = false;
  //DimDescriptor.data_len = 4;
  //DimDescriptor.HsbData = LoadData;

// ---------------------------------------
  blinkDescriptor.anim = SATURATE;
  blinkDescriptor.N = -2;
  blinkDescriptor.duration = 1000; 
  blinkDescriptor.nLoops = 1;
  //blink2Descriptor.nLEDS;
  blinkDescriptor.repeat = false;
  //blinkDescriptor.data_len = 4;
  //blinkDescriptor.HsbData = LoadData;

  DiscriptorSet[0] = LoadDescriptor;
  DiscriptorSet[1] = DimDescriptor;
  DiscriptorSet[2] = blinkDescriptor;
  
  
  StartSequence( DiscriptorSet, 3 );
  
}

