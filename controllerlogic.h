#ifndef CONT_LOGIC_H
#define CONT_LOGIC_H

#include "drawlogic.h"
#include "gamelogic.h"
#include <SDL2/SDL.h>

// typedef void (*EVENT_CALLBACK)(void);

typedef struct ControllerState ControllerState;

ControllerState *create_ControllerState();
void destroy_ControllerState(ControllerState* controller);

int run_controller(GameData* data,DrawConfig* config, ControllerState* controller);

//SDL_TimerCallback _create_timer_callback(void (*callback)(void))

#endif