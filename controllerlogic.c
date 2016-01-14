#include "controllerlogic.h"
#include "gamelogic.h"
#include <SDL2/SDL.h>
#include <time.h>


struct ControllerState {
    int tick_length;
    int poll_interval;
    struct timespec last_user_input;
    struct timespec last_game_tick;
};

ControllerState *create_ControllerState(){
    ControllerState* controller = malloc(sizeof (ControllerState));

    controller->tick_length = 100000000;
    controller->poll_interval = 10000;
    clock_gettime(CLOCK_MONOTONIC, &(controller->last_user_input));
    clock_gettime(CLOCK_MONOTONIC, &(controller->last_game_tick));

    return controller;
}

void destroy_ControllerState(ControllerState* controller){
    free(controller);
}

int try_controller(GameData* data,ControllerState* controller){
    /*
     * Quit events need to be handled separately from others events.
     * assume that SDL has been initialized by the view
     */
    SDL_Event event ;
    SDL_PollEvent(&event);
    if(event.type == SDL_QUIT){
        return 0;
    }

    if(!is_gameover(data)){
        control_game(data,controller,event);
    } else { 
        control_menu(data,controller,event);
    }

    return  1;
}

void control_game(GameData* data,ControllerState* controller,SDL_Event event){
    /*
     * Obtain current system time
     */
    struct timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);

    /*
     *  Advance game state if enough time has passed.
     */
    int update_ellapsed = 1000000000 * (currentTime.tv_sec - controller->last_game_tick.tv_sec) + 
                          (currentTime.tv_nsec - controller->last_game_tick.tv_nsec);
    if(update_ellapsed > controller->tick_length){
        update_state(data);
        clock_gettime(CLOCK_MONOTONIC, &(controller->last_game_tick));
    }

    /*
     * Modify game state based on user input if enough time has passed
     */
    int poll_ellapsed = 1000000000 * (currentTime.tv_sec - controller->last_user_input.tv_sec) + 
                        (currentTime.tv_nsec - controller->last_user_input.tv_nsec);
    if(poll_ellapsed > controller->poll_interval){
        proccese_game_event(data,controller,event);
        clock_gettime(CLOCK_MONOTONIC, &(controller->last_user_input));
    }
}

void proccese_game_event(GameData* data,ControllerState* controller, SDL_Event event){
    if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE){
        flap_bird(data);
    }
}


void control_menu(GameData* data,ControllerState* controller, SDL_Event event){
    /*
     * Obtain current system time
     */
    struct timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);

    /*
     * Modify game state based on user input if enough time has passed
     */
    int poll_ellapsed = 1000000000 * (currentTime.tv_sec - controller->last_user_input.tv_sec) + 
                        (currentTime.tv_nsec - controller->last_user_input.tv_nsec);
    if(poll_ellapsed > controller->poll_interval){
        proccese_menu_event(data,controller,event);
        clock_gettime(CLOCK_MONOTONIC, &(controller->last_user_input));
    }
}

void proccese_menu_event(GameData* data,ControllerState* controller, SDL_Event event){
    if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE){
        reset_state(data);
    }
}