#include "controllerlogic.h"
#include "gamelogic.h"
#include "drawlogic.h"
#include <time.h>
#include <stdlib.h>

#ifdef __ANDROID__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

struct ControllerState {
    Uint32 tick_length;
    Uint32 poll_interval;
    Uint32 last_user_input;
    Uint32 last_game_tick;
};

ControllerState *create_ControllerState(){
    ControllerState* controller = malloc(sizeof (ControllerState));

    controller->tick_length = 100;
    controller->poll_interval = 50;
    controller->last_user_input = 0;
    controller->last_game_tick = 0;
    
    return controller;
}

void destroy_ControllerState(ControllerState* controller){
    free(controller);
}

int try_controller(GameData* data,ControllerState* controller, DrawConfig* config){
    /*
     * Quit events need to be handled separately from others events.
     * assume that SDL has been initialized by the view
     */
    SDL_Event event;
    SDL_PumpEvents();
    if(SDL_PeepEvents(&event,1,SDL_PEEKEVENT,SDL_QUIT,SDL_QUIT)){
        return 0;
    }

    if(!is_gameover(data)){
        control_game(data,controller);
    } else { 
        return control_menu(data, controller, config);
    }

    return  1;
}

void control_game(GameData* data,ControllerState* controller){
    /*
     * Obtain current SDL ticks 
     */
    Uint32 currentTime = SDL_GetTicks();

    /*
     *  Advance game state if enough time has passed.
     */
    if(SDL_TICKS_PASSED(currentTime,controller->last_game_tick + controller->tick_length)){
        update_state(data);
        controller->last_game_tick = currentTime;
    }

    /*
     * Modify game state based on user input if enough time has passed
     */
    if(SDL_TICKS_PASSED(currentTime,controller->last_user_input + controller->poll_interval)){ 
        proccese_game_event(data,controller);
        controller->last_user_input = currentTime;
    }
}

void proccese_game_event(GameData* data,ControllerState* controller){
    SDL_Event event;
    while(SDL_PeepEvents(&event,1,SDL_GETEVENT,SDL_FIRSTEVENT,SDL_LASTEVENT)){
        switch (event.type) {
            case SDL_KEYDOWN:
                if(event.key.keysym.sym != SDLK_SPACE){
                    break;
                }
                //fallthrough
            case SDL_FINGERDOWN:
                flap_bird(data);
                break;
        }

    }
}

int control_menu(GameData* data,ControllerState* controller, DrawConfig* config){
    /*
     * Obtain current SDL ticks 
     */
    Uint32 currentTime = SDL_GetTicks();

    /*
     * Modify menue state based on user input if enough time has passed
     */
    if(SDL_TICKS_PASSED(currentTime,controller->last_user_input + controller->poll_interval)){ 
        controller->last_user_input = currentTime;
        return proccese_menu_event(data,controller, config);
    }
    return 1;
}

int handle_button_click(GameData* data, ControllerState* controller, Button button){
    switch(button) {
        case PLAY:
            reset_state(data);
            return 1;
        case QUIT:
            return 0;
        default:
            return 1;
    }
    return 1;
}

int proccese_menu_event(GameData* data, ControllerState* controller, DrawConfig* config){
    SDL_Event event;
    while(SDL_PeepEvents(&event,1,SDL_GETEVENT,SDL_FIRSTEVENT,SDL_LASTEVENT)){
        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN: {
                SDL_Point p = {event.button.x, event.button.y};
                SDL_Rect** rects = get_button_rects(config);
                for(int i = 0; i < BUTTONS_COUNT; i++){
                    if(rects[i] != NULL && SDL_PointInRect(&p, rects[i])){
                        return handle_button_click(data, controller, i);
                    }
                }
                break;
            }
        }
    }
    return 1;
}
