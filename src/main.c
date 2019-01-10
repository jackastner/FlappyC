#include <string.h>
#include <time.h>
#include "gamelogic.h"
#include "drawlogic.h"
#include "controllerlogic.h"

#ifdef __ANDROID__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#define MAX_FPS 30
#define MS_TO_S 1000
#define FRAME_INTERVAL (MS_TO_S / MAX_FPS)

int main(int argc, char** argv){
    DrawConfig* config = create_DrawConfig();
    GameData* data = create_GameData();
    ControllerState* controller = create_ControllerState();

    Uint32 last_frame = SDL_GetTicks();
    while(try_controller(data, controller, config)){
        Uint32 current_time = SDL_GetTicks();
        if(SDL_TICKS_PASSED(current_time, last_frame + FRAME_INTERVAL)){ 
            if(current_time - last_frame > 35){
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "slow frame time: %d\n", current_time - last_frame);
            }
            render_game(data,config);
            last_frame = current_time;
        } 
    }

    destroy_DrawConfig(config);
    destroy_GameData(data);
    destroy_ControllerState(controller);
}
