#include "controllerlogic.h"
#include "gamelogic.h"
#include "drawlogic.h"
#include <ctime>
#include <cstdlib>

#ifdef __ANDROID__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

ControllerState::ControllerState(GameData *data, DrawConfig* config) :
    data(data),
    config(config),
    tick_length(100),
    poll_interval(50),
    last_user_input(0),
    last_game_tick(0)
{}

int ControllerState::try_controller(){
    /*
     * Quit events need to be handled separately from others events.
     * assume that SDL has been initialized by the view
     */
    SDL_Event event;
    SDL_PumpEvents();
    if(SDL_PeepEvents(&event,1,SDL_PEEKEVENT,SDL_QUIT,SDL_QUIT)){
        return 0;
    }

    if(!data->is_gameover()){
        control_game();
    } else { 
        return control_menu();
    }

    return  1;
}

void ControllerState::control_game(){
    /*
     * Obtain current SDL ticks 
     */
    Uint32 currentTime = SDL_GetTicks();

    /*
     *  Advance game state if enough time has passed.
     */
    if(SDL_TICKS_PASSED(currentTime,last_game_tick + tick_length)){
        data->update_state();
        last_game_tick = currentTime;
    }

    /*
     * Modify game state based on user input if enough time has passed
     */
    if(SDL_TICKS_PASSED(currentTime,last_user_input + poll_interval)){ 
        proccese_game_event();
        last_user_input = currentTime;
    }
}

void ControllerState::proccese_game_event(){
    SDL_Event event;
    while(SDL_PeepEvents(&event,1,SDL_GETEVENT,SDL_FIRSTEVENT,SDL_LASTEVENT)){
        switch (event.type) {
            case SDL_KEYDOWN:
                if(event.key.keysym.sym != SDLK_SPACE){
                    break;
                }
                //fallthrough
            case SDL_FINGERDOWN:
                data->flap_bird();
                break;
        }

    }
}

int ControllerState::control_menu(){
    /*
     * Obtain current SDL ticks 
     */
    Uint32 currentTime = SDL_GetTicks();

    /*
     * Modify menue state based on user input if enough time has passed
     */
    if(SDL_TICKS_PASSED(currentTime,last_user_input + poll_interval)){ 
        last_user_input = currentTime;
        return proccese_menu_event();
    }
    return 1;
}

int ControllerState::handle_button_click(DrawConfig::Button button){
    switch(button) {
        case DrawConfig::Button::PLAY:
            data->reset_state();
            return 1;
        case DrawConfig::Button::QUIT:
            return 0;
        default:
            return 1;
    }
    return 1;
}

int ControllerState::proccese_menu_event(){
    SDL_Event event;
    while(SDL_PeepEvents(&event,1,SDL_GETEVENT,SDL_FIRSTEVENT,SDL_LASTEVENT)){
        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN: {
                SDL_Point p = {event.button.x, event.button.y};
                std::vector<SDL_Rect> rects = config->get_button_rects();
                for(int i = 0; i < DrawConfig::Button::BUTTONS_COUNT; i++){
                    if(SDL_PointInRect(&p, &rects[i])){
                        return handle_button_click((DrawConfig::Button) i);
                    }
                }
                break;
            }
        }
    }
    return 1;
}
