#pragma once

//state defines
#define NUM_STATES  6
#define ADVANCE_INDICATE_STATE (NUM_STATES+1)
#define INDIVIDUAL_INDICATE_STATE (NUM_STATES+2)
#define MESH_INDICATE_STATE (NUM_STATES+3)


//button defines.
//button is active low. (idle high)
# define BUTTON_PIN     0 //GPIO 0, D3, flash button
#define BUTTON_ACTIVE LOW
#define BUTTON_IDLE HIGH
#define SHORT_PRESS_MIN   100
#define LONG_PRESS_MIN  3000


//state typedefs
typedef enum { MODE_MESH, MODE_INDIVIDUAL } application_mode_t;
typedef void (*MeshStateChangeCallbackType)(int state);


//state variables
extern int currentState; //use to get current state.
extern int nextState; //use to look ahead, only useful for displaying meta data about next state.
extern application_mode_t AppMode;

void InitStateManager(MeshStateChangeCallbackType callback);
void ProcessButton();
void ProcessAutomaticStateChange();
void SetState( int next_state, bool block_transmit = false );
void MeshStateChanged( int mesh_state );
