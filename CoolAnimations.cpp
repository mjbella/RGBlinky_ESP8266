#include "CoolAnimations.h"

animation_descriptor_t LoadDescriptor;
animation_descriptor_t DimDescriptor;

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
  LoadData[0] = HsbColor( 0.0, 0.0, 0.0 );
  LoadData[1] = HsbColor( 0.8, 0.9, 0.75 );
  LoadData[2] = HsbColor( 0.0, 0.0, 0.0 );
  LoadData[3] = HsbColor( 0.0, 1.0, 0.5 );

  DimDescriptor.anim = DIFFUSE;
//  DimDescriptor.anim = DIM;
  DimDescriptor.N = 0.25;
  DimDescriptor.duration = 4000; 
  DimDescriptor.nLoops = 1;
  //DimDescriptor.nLEDS;
  DimDescriptor.repeat = false;
  //DimDescriptor.data_len = 4;
  //DimDescriptor.HsbData = LoadData;

  DiscriptorSet[0] = LoadDescriptor;
  DiscriptorSet[1] = DimDescriptor;
  StartSequence( DiscriptorSet, 2 );
  
}

