#include "controllerlogic.h"
#include "gamelogic.h"
#include "drawlogic.h"
#include <SDL2/SDL.h>

#include <stdio.h>

struct ControllerState {
    int tick_length;
    int refresh_rate;
};

ControllerState *create_ControllerState(){
    ControllerState* controller = malloc(sizeof (ControllerState));

    controller->tick_length = 100;
    controller->refresh_rate = 1000;

    return controller;
}

void destroy_ControllerState(ControllerState* controller){
    free(controller);
}

int run_controller(GameData* data,DrawConfig* config, ControllerState* controller){
  /*
   * These function pointer give me warnings out my bunghole.
   *
   */
    Uint32 tick_callback(Uint32 interval, void *param){
        printf("Tick\n");

        SDL_Event event;
        SDL_UserEvent userevent;
        
        userevent.type = SDL_USEREVENT;
        userevent.code = 0;
        userevent.data1 = &update_state;
        userevent.data2 = data;

        event.type = SDL_USEREVENT;
        event.user = userevent;

        SDL_PushEvent(&event);
        return(interval);
    }

    SDL_TimerID tick_timer = SDL_AddTimer(controller->tick_length,&tick_callback,NULL);

    // Uint32 refresh_callback(Uint32 interval, void *param){
    //     SDL_Event event;
    //     SDL_UserEvent userevent;
        
    //     userevent.type = SDL_USEREVENT;
    //     userevent.code = 0;
    //     userevent.data1 = &update_state; 
    //     userevent.data2 = ;

    //     event.type = SDL_USEREVENT;
    //     event.user = userevent;

    //     SDL_PushEvent(&event);
    //     return(interval);
    // }
    // SDL_TimerID refresh_timer = SDL_AddTimer(controller->refresh_rate,&refresh_callback,NULL);

    SDL_Event event;
    while (!is_gameover(data)){

        if(SDL_PollEvent (&event)){
            if(event.type == SDL_QUIT){
                break;
            }

            switch(event.type) {
                case SDL_USEREVENT: {
                    printf("User event\n");
                    void (*p) (void *, ...) = event.user.data1;
                    p(event.user.data2);
                    break;
                }
                case SDL_KEYDOWN: {
                    if(event.key.keysym.sym == SDLK_SPACE){
                        printf("Flap\n");
                        flap_bird(data);
                    }
                    break;
                }
            }

            //non-sdl-timer hack to make the game render
            render_game(data,config);
        }
    }

    SDL_RemoveTimer(tick_timer);
    //SDL_RemoveTimer(refresh_timer);
}