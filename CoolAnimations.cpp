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
  //LoadDescriptor.repeat = true;
  LoadDescriptor.data_len = 4;
  LoadDescriptor.HsbData = LoadData;

  for( int i = 0; i < 4; ++i )
  {
    LoadData[i] = HsbColor( (float)i / 4.0,  1.0, 0.5 );
    Serial.print( LoadData[i].H );
    Serial.print( " " );
    Serial.print( LoadData[i].S );
    Serial.print( " " );
    Serial.println( LoadData[i].B );
  }


  DimDescriptor.anim = DIM;
  DimDescriptor.N = 0.5;
  DimDescriptor.duration = 4000; //1 s
  DimDescriptor.nLoops = 1;
  //DimDescriptor.nLEDS;
  //DimDescriptor.repeat = true;
  //DimDescriptor.data_len = 4;
  //DimDescriptor.HsbData = LoadData;

  DiscriptorSet[0] = LoadDescriptor;
  DiscriptorSet[1] = DimDescriptor;
  StartSequence( DiscriptorSet, 2 );
  
}

