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

#include "Constants.h"
#include "AnimationEngine.h" //contains Michael's animation engine.
#include "CoolAnimations.h" //configures Michael's animations.
#include "MarkAnimations.h" //contains Mark's animation engine.
#include "StateManager.h" //handles buttons and state changes.
#include "SerialConfig.h" //handles saving and loading settings, as well as getting new settings from the PC.

//WIFI stuff
#define   MESH_PREFIX     "RGB_LED"
#define   MESH_PASSWORD   "BurningMan"
#define   MESH_PORT       5555
painlessMesh  mesh;
uint32_t sendMessageTime = 0;
uint32_t sent = 0;
volatile bool broadcastNeeded = false;


// LED stuff
#ifdef ARDUINO_ESP8266_ESP01 //detect which board we're using. the little one uses pin 2.
//note, for the little guy, use DOUT, not DIO for programming.
#define   LEDPIN          2
#else
#define   LEDPIN          D4
#endif



NeoPixelBrightnessBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip(MAX_LED_COUNT, LEDPIN);
//NeoPixelBus<NeoRgbFeature, NeoEsp8266Uart800KbpsMethod> strip(MAX_LED_COUNT, LEDPIN);



//timer
os_timer_t frameTimer;
boolean on = false;
uint32_t lastFrameTimeMs = 0;
#define FRAME_RATE 30


//this function passes off to Michael's animation engine.
void passToAnimationEngine()
{
  uint32_t node_time_us = mesh.getNodeTime();

  uint32_t local_time_ms = node_time_us / 1000ul;

  //just in case the node time ever goes backwards after a sync, lets make sure that lastFrameTimeMs never gets ahead of the real time.
  if ( lastFrameTimeMs >= local_time_ms )
  {
    Serial.println("lastFrameTimeMs got ahead of the clock.");
    lastFrameTimeMs = 0;
  }

  //this hack simulates the last frame time during the first render. used by animations that need to compute time since last draw.
  if ( lastFrameTimeMs == 0 )
  {
    Serial.println("init lastFrameTimeMs");
    if ( local_time_ms < 31 )
      lastFrameTimeMs = 1;
    else
      lastFrameTimeMs = local_time_ms - 30;
  }

  Animate( local_time_ms, lastFrameTimeMs );

  lastFrameTimeMs = local_time_ms;
}

void timerCallback(void *pArg)
{
  digitalWrite(DEBUG_PIN1, HIGH);

  //state 1 redirects to Michael's animation engine.
  if ( currentState == 1 )
  {
    passToAnimationEngine();
    digitalWrite(DEBUG_PIN1, LOW);
    return;
  }

  //all other states are routed to Mark's animation engine.
  uint32_t node_time_us = mesh.getNodeTime();
  uint32_t node_time_ms = node_time_us / 1000ul;
  RenderFrame( node_time_ms, GetConfigLedCount(), currentState );
  digitalWrite(DEBUG_PIN1, LOW);
}


void InitStrip()
{
  strip.Begin();
  strip.SetBrightness( GetConfigBrightness() );
  //turn off the strip during init.
  strip.ClearTo(RgbColor( 0, 0, 0 ) );
  strip.Show();
}

void InitMesh()
{
  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
}

void InitTimer()
{
  //setup timer
  os_timer_setfn(&frameTimer, timerCallback, NULL);
  os_timer_arm(&frameTimer, 1000 / FRAME_RATE, true); //1000ms, repeat = true;

}

void InitDebug()
{
  pinMode(DEBUG_PIN1, OUTPUT);
  pinMode(DEBUG_PIN2, OUTPUT);
  pinMode(DEBUG_PIN3, OUTPUT);
  pinMode(DEBUG_PIN4, OUTPUT);
  digitalWrite(DEBUG_PIN1, LOW);
  digitalWrite(DEBUG_PIN2, LOW);
  digitalWrite(DEBUG_PIN3, LOW);
  digitalWrite(DEBUG_PIN4, LOW);
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nTOP OF SETUP FUNCTION");
  InitDebug();
  Serial.println("about to init serial...");
  InitSerialConfig( OnBrightnessChanged, OnLedCountChanged );
  Serial.println("about to init strip...");
  InitStrip();
  Serial.println("about to init mesh...");
  InitMesh();
  Serial.println("about to init state manager...");
  InitStateManager(BroadcastState);
  //init Michael's animations
  Serial.println("about to init animations...");
  InitAnimations();
  Serial.println("about to init timer...");
  InitTimer();
  Serial.println("BOTTOM OF SETUP FUNCTION");
  
}

void loop() {
  //Serial.println("TOP OF LOOP");
  digitalWrite(DEBUG_PIN4, HIGH);
  mesh.update();

  //calls into state management code
  ProcessButton();

  //calls into state management code.
  ProcessAutomaticStateChange();

  //handles sending the new state to other nodes when the button is pressed.
  ProcessBroadcastFlag();

  //handles changing the LED count and brightness over the serial port:
  ProcessSerial();
  digitalWrite(DEBUG_PIN4, LOW);
  //Serial.println("BOTTOM OF LOOP");
}

void ProcessBroadcastFlag()
{
  if ( !broadcastNeeded )
    return;

  uint32_t node_id = mesh.getNodeId();
  SimpleList<uint32_t> node_list = mesh.getNodeList();

  uint32_t highest_node = node_id;
  for (SimpleList<uint32_t>::iterator itr = node_list.begin(); itr != node_list.end(); ++itr)
  {
    if ( *itr > highest_node )
      highest_node = *itr;
  }

  if ( highest_node == node_id )
  {
    Serial.println("this is the master node - transmitting state.");
    BroadcastState(nextState); //beleive it or not, next state always points to a valid animation state. currentState could be status information.
  }
  else
  {
    Serial.println("this is a slave node - transmitting state.");
  }

  Serial.print("this nodeId: ");
  Serial.println( node_id );
  Serial.print("master nodeId: ");
  Serial.println( highest_node );

  broadcastNeeded = false;
}

void BroadcastState(int state)
{
  String msg = "SET_STATE:";
  msg += state;
  bool success = mesh.sendBroadcast( msg );
  if ( success )
  {
    Serial.print("sent broadcast message: ");
    Serial.println(msg);
  }
  else
  {
    Serial.println("failed to send broadcast message");
    Serial.print("failed broadcast message: ");
    Serial.println(msg);
  }
}

void ParseCommand( String message )
{
  int start_index = message.indexOf(":");
  if ( start_index < 0 )
    return;
  message.remove(0, start_index + 1);
  int new_state = message.toInt();
  Serial.print("received new state from the mesh: ");
  Serial.println(new_state );
  MeshStateChanged( new_state );
}

void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  //todo: we should change our state here.

  ParseCommand( msg );
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);

}

void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());
  //TODO: if we are the highest node index, we should transmit our state to the network.
  broadcastNeeded = true;
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
  //nothing to do here, all animations handle themselves.
}

void OnBrightnessChanged( uint8_t brightness )
{
  Serial.println("brightness changed");
  strip.SetBrightness( brightness );
  //strip.Show();
  
}

void OnLedCountChanged( uint8_t led_count )
{
   Serial.println("led count changed");
   for( int i = led_count; i < MAX_LED_COUNT; ++i )
    strip.SetPixelColor( i, RgbColor( 0, 0, 0 ) );

}
