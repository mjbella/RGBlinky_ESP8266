#include "Arduino.h"

#include "StateManager.h"


//state variables
int currentState = 0;
unsigned long timeOfNextStateChange = 0;
int nextState = 0;
application_mode_t AppMode = MODE_MESH;
int meshState = 0;
MeshStateChangeCallbackType meshCallback = NULL;

//button variables
unsigned long lastButtonPressTime;
int CurrentButtonState = BUTTON_IDLE; //active low.

void HandleShortButtonPress();
void HandleLongButtonPress();

void InitStateManager(MeshStateChangeCallbackType callback)
{
  //setup button.
  pinMode(BUTTON_PIN, INPUT);
  meshCallback = callback;
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

void SetState( int next_state, bool block_transmit )
{
  currentState = next_state;
  if( (AppMode == MODE_MESH) && (next_state < NUM_STATES) && (block_transmit == false) ) //filter out status states, only tx real states.
  {
    meshState = next_state;
    meshCallback( next_state );
  }
}

void MeshStateChanged( int mesh_state )
{
  meshState = mesh_state;
  if( AppMode == MODE_MESH )
  {
    timeOfNextStateChange = 0; //abort any automatic state changes
    SetState( mesh_state, true );
  }
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
    nextState = meshState;
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
