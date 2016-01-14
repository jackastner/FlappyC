#ifndef CONT_LOGIC_H
#define CONT_LOGIC_H

#include "drawlogic.h"

typedef struct ControllerState ControllerState;

ControllerState *create_ControllerState();
void destroy_ControllerState(ControllerState* controller);

int try_controller(GameData* data,ControllerState* controller);

void control_game(GameData* data,ControllerState* controller,SDL_Event event);
void proccese_game_event(GameData* data,ControllerState* controller, SDL_Event event);

void control_menu(GameData* data,ControllerState* controller,SDL_Event event);
void proccese_menu_event(GameData* data,ControllerState* controller, SDL_Event event);


#endif