#include "CoolAnimations.h"

animation_descriptor_t LoadDescriptor;
animation_descriptor_t DimDescriptor;
animation_descriptor_t blinkDescriptor;

animation_descriptor_t DiscriptorSet[4];

HsbColor LoadData[4], LoadData2[4];

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

  LoadData[0] = HsbColor( 0.11, 1.0, 1.0 );
  LoadData[1] = HsbColor( 0.0, 0.0, 0.0 );
  LoadData[2] = HsbColor( 0.0, 0.0, 0.0 );
  LoadData[3] = HsbColor( 0.75, 1.0, 1.0 );

  DimDescriptor.anim = DIFFUSE;
//  DimDescriptor.anim = DIM;
  DimDescriptor.N = 10;
  DimDescriptor.duration = 3000; 
  DimDescriptor.nLoops = 1;
  //DimDescriptor.nLEDS;
  DimDescriptor.repeat = false;
  //DimDescriptor.data_len = 4;
  //DimDescriptor.HsbData = LoadData;

// ---------------------------------------
  blinkDescriptor.anim = SHOW;
  //blinkDescriptor.N = 0;
  //blinkDescriptor.duration = 1000; 
  //blinkDescriptor.nLoops = 1;
  //blink2Descriptor.nLEDS;
  blinkDescriptor.repeat = false;
  blinkDescriptor.data_len = 4;
  blinkDescriptor.HsbData = LoadData2;

  LoadData2[0] = HsbColor( 0.0, 0.0, 0.0 );
  LoadData2[1] = HsbColor( 0.45, 1.0, 1.0 );
  LoadData2[2] = HsbColor( 0.55, 1.0, 1.0 );
  LoadData2[3] = HsbColor( 0.0, 0.0, 0.0 );

  
  DiscriptorSet[0] = LoadDescriptor;
  DiscriptorSet[1] = DimDescriptor;
  DiscriptorSet[2] = blinkDescriptor;
  DiscriptorSet[3] = DimDescriptor;

  
  StartSequence( DiscriptorSet, 4 );
  
}

