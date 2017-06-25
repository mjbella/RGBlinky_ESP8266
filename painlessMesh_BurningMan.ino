//************************************************************
// this is a simple example that uses the painlessMesh library
//
// 1. sends a silly message to every node on the mesh at a random time betweew 1 and 5 seconds
// 2. prints anything it recieves to Serial.print
// 
//
//************************************************************
#include "painlessMesh.h"

#include <NeoPixelAnimator.h>
#include <NeoPixelBrightnessBus.h>
#include <NeoPixelBus.h>
//#include <math.h>

#define   MESH_PREFIX     "RGB_LED"
#define   MESH_PASSWORD   "BurningMan"
#define   MESH_PORT       5555

#define   LEDPIN          4
#define   NPLEN           4

extern int testData[200];

painlessMesh  mesh;

uint32_t sendMessageTime = 0;
uint32_t sent = 0;

// Goertzel library
int sensorPin = A0;
const int N = 150;
const float SAMPLING_FREQUENCY = 8000; 

//NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip(NPLEN, LEDPIN);
NeoPixelBus<NeoRgbFeature, NeoEsp8266Uart800KbpsMethod> strip(NPLEN, LEDPIN);

// ADC Data tmp location
short sound_data[N];


void setup() {
  Serial.begin(115200);

  strip.Begin();
  led_pattern();

//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
}

void loop() {
  mesh.update();

 // get next random time to send a message
  if ( sendMessageTime == 0 ) {
    sendMessageTime = mesh.getNodeTime() + random( 1000000, 5000000 );
  }

  // if the time is ripe, send everyone a message!
  if ( sendMessageTime != 0 && sendMessageTime < mesh.getNodeTime() ){
    String msg = "Hello from node ";
    msg += mesh.getNodeId();
    mesh.sendBroadcast( msg );
    sendMessageTime = 0;
  }

  strip.Show();
}

void led_pattern() {
  for ( uint8_t i = 0; i < NPLEN; i++ ) {
    strip.SetPixelColor(i, RgbColor( 0, 0, 0 ) );
  }
  strip.Show();
}

void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
    Serial.printf("Changed connections %s\n",mesh.subConnectionJson().c_str());
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}
