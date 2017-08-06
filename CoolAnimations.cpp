#include "CoolAnimations.h"

animation_descriptor_t LoadDescriptor;
animation_descriptor_t DimDescriptor;
animation_descriptor_t Dim2Descriptor;

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
  LoadData[1] = HsbColor( 0.5, 1.0, 1.0 );
  LoadData[2] = HsbColor( 0.0, 0.0, 0.0 );
  LoadData[3] = HsbColor( 0.0, 1.0, 0.5 );

  DimDescriptor.anim = DIFFUSE;
//  DimDescriptor.anim = DIM;
  DimDescriptor.N = 0.25;
  DimDescriptor.duration = 2000; 
  DimDescriptor.nLoops = 1;
  //DimDescriptor.nLEDS;
  DimDescriptor.repeat = false;
  //DimDescriptor.data_len = 4;
  //DimDescriptor.HsbData = LoadData;

// ---------------------------------------
  Dim2Descriptor.anim = DIM;
  Dim2Descriptor.N = 0.5;
  Dim2Descriptor.duration = 5000; 
  Dim2Descriptor.nLoops = 1;
  //Dim2Descriptor.nLEDS;
  Dim2Descriptor.repeat = false;
  //Dim2Descriptor.data_len = 4;
  //Dim2Descriptor.HsbData = LoadData;

  DiscriptorSet[0] = LoadDescriptor;
  DiscriptorSet[1] = DimDescriptor;
  DiscriptorSet[2] = Dim2Descriptor;
  
  
  StartSequence( DiscriptorSet, 2 );
  
}

