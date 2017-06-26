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



//WIFI stuff
#define   MESH_PREFIX     "RGB_LED"
#define   MESH_PASSWORD   "BurningMan"
#define   MESH_PORT       5555
painlessMesh  mesh;
uint32_t sendMessageTime = 0;
uint32_t sent = 0;
uint32_t timeOfStateBeginningUs = 0;

// LED stuff
#define   LEDPIN          4
#define   NPLEN           4
NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip(NPLEN, LEDPIN);
//NeoPixelBus<NeoRgbFeature, NeoEsp8266Uart800KbpsMethod> strip(NPLEN, LEDPIN);


//Button stuff.
//button is active low. (idle high)
# define BUTTON_PIN     0 //GPIO 0, D3, flash button
#define BUTTON_ACTIVE LOW
#define BUTTON_IDLE HIGH
#define SHORT_PRESS_MIN   100
#define LONG_PRESS_MIN  3000
unsigned long lastButtonPressTime;
int CurrentButtonState = BUTTON_IDLE; //active low.


//timer
os_timer_t frameTimer;
boolean on = false;


//state
typedef enum { MODE_MESH, MODE_INDIVIDUAL } application_mode_t;
int currentState = 0;
unsigned long timeOfNextStateChange = 0;
int nextState = 0;
application_mode_t AppMode = MODE_MESH;

#define NUM_STATES  3
#define ADVANCE_INDICATE_STATE (NUM_STATES+1)
#define INDIVIDUAL_INDICATE_STATE (NUM_STATES+2)
#define MESH_INDICATE_STATE (NUM_STATES+3)

void timerCallback(void *pArg)
{
  uint32_t node_time_us = mesh.getNodeTime();
  if ( node_time_us < timeOfStateBeginningUs )
  {
    //BIG PROBLEMS!
    timeOfStateBeginningUs = node_time_us; //for now, lets fudge it.
    return;
  }

  uint32_t local_time_ms = (node_time_us - timeOfStateBeginningUs) / 1000ul;

  //lets say all states cycle every 1 second for now.
  uint32_t ms_per_cycle = 1000;
  uint32_t looped_time_ms = local_time_ms % ms_per_cycle;

  //local_ms hopefully should be smooth gradient over 5second
  byte brightness = looped_time_ms * 50ul / ms_per_cycle;



  RgbColor color;
  if ( currentState == 0 )
  {
    color = RgbColor( brightness, 0, 0 );
    for ( uint8_t i = 0; i < NPLEN; i++ ) {
      strip.SetPixelColor(i, color );
    }
  }
  else if ( currentState == 1 )
  {
    color = RgbColor( 0, brightness, 0 );
    for ( uint8_t i = 0; i < NPLEN; i++ ) {
      strip.SetPixelColor(i, color );
    }
  }
  else if ( currentState == 2 )
  {
    color = RgbColor( 0, 0, brightness );
    for ( uint8_t i = 0; i < NPLEN; i++ ) {
      strip.SetPixelColor(i, color );
    }
  }
  else if ( currentState == ADVANCE_INDICATE_STATE )
  {
    for ( uint8_t i = 0; i < NPLEN; i++ ) {
      color = RgbColor( 0, 0, 0 );
      if ( i == nextState )
      {
        color = RgbColor( 50, 50, 50 );
      }
      strip.SetPixelColor(i, color );
    }
  }
  else if ( currentState == INDIVIDUAL_INDICATE_STATE )
  {
    color = RgbColor( 0, 0, 50 );
    for ( uint8_t i = 0; i < NPLEN; i++ ) {
      strip.SetPixelColor(i, color );
    }
  }
  else if ( currentState == MESH_INDICATE_STATE )
  {
    color = RgbColor( 50, 0, 0 );
    for ( uint8_t i = 0; i < NPLEN; i++ ) {
      strip.SetPixelColor(i, color );
    }
  }
  else
  {
    color = RgbColor( 10, 10, 10 );
  }



  strip.Show();

}

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

  //setup timer
  os_timer_setfn(&frameTimer, timerCallback, NULL);
  os_timer_arm(&frameTimer, 33, true); //1000ms, repeat = true;

  //setup button.
  pinMode(BUTTON_PIN, INPUT);
}

void loop() {
  mesh.update();

  ProcessButton();

  ProcessAutomaticStateChange();

  return;

  // get next random time to send a message
  if ( sendMessageTime == 0 ) {
    sendMessageTime = mesh.getNodeTime() + random( 1000000, 5000000 );
  }

  // if the time is ripe, send everyone a message!
  if ( sendMessageTime != 0 && sendMessageTime < mesh.getNodeTime() ) {
    String msg = "Hello from node ";
    msg += mesh.getNodeId();
    mesh.sendBroadcast( msg );
    sendMessageTime = 0;
  }

  //strip.Show();
}


void ProcessAutomaticStateChange()
{
  if ( timeOfNextStateChange == 0 )
  {
    return;
  }

  if ( millis() < timeOfNextStateChange )
  {
    return;
  }

  //handle automatic state change!
  timeOfNextStateChange = 0;
  SetState( nextState );
}

void SetState( int next_state )
{
  timeOfStateBeginningUs = mesh.getNodeTime();
  currentState = next_state;
}

void SetMode( application_mode_t new_mode )
{

  if ( new_mode == AppMode )
  {
    return;
  }
  
  nextState = currentState;
  timeOfNextStateChange = millis() + 1000;
  if ( new_mode == MODE_MESH )
  {
    SetState( MESH_INDICATE_STATE );
  }
  else
  {
    SetState( INDIVIDUAL_INDICATE_STATE );
  }


  AppMode = new_mode;
}

void ProcessButton()
{
  int last_button_state = CurrentButtonState;
  CurrentButtonState = digitalRead(BUTTON_PIN);

  if ( last_button_state == BUTTON_IDLE && CurrentButtonState == BUTTON_ACTIVE )
  {
    //active edge.
    lastButtonPressTime = millis();
  }
  else if ( last_button_state == BUTTON_ACTIVE && CurrentButtonState == BUTTON_IDLE )
  {
    //inactive edge.
    unsigned long durration = millis() - lastButtonPressTime;
    if ( durration > SHORT_PRESS_MIN && durration < LONG_PRESS_MIN )
    {
      HandleShortButtonPress();
    }
    if ( durration >= LONG_PRESS_MIN )
    {
      HandleLongButtonPress();
    }
  }

}

void HandleShortButtonPress()
{
  if ( timeOfNextStateChange > 0 )
  {
    //lets not do anything if a temporary animation is playing.
    return;
  }

  //advance to next state!
  int next_state = currentState + 1;
  if ( next_state >= NUM_STATES )
  {
    next_state = 0;
  }

  //play an animation for 3 seconds!
  SetState( ADVANCE_INDICATE_STATE );
  timeOfNextStateChange = millis() + 1000;
  nextState = next_state;

}

void HandleLongButtonPress()
{
  if ( timeOfNextStateChange > 0 )
  {
    //lets not do anything if a temporary animation is playing.
    return;
  }

  if ( AppMode == MODE_MESH )
  {
    SetMode( MODE_INDIVIDUAL );


  }
  else
  {
    SetMode( MODE_MESH );
  }

}



void led_pattern() {
  for ( uint8_t i = 0; i < NPLEN; i++ ) {
    strip.SetPixelColor(i, RgbColor( 50, i * 50, 0 ) );
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
  Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
  SetState( currentState ); //this triggers the state to restart!
}
