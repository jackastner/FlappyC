#include <string.h>
#include <time.h>
#include "gamelogic.h"
#include "drawlogic.h"
#include "controllerlogic.h"

#define MAX_FPS 30
#define MS_TO_S 1000
#define FRAME_INTERVAL (MS_TO_S / MAX_FPS)

int main(int argc, char** argv){
    DrawConfig* config = create_DrawConfig();
    GameData* data = create_GameData();
    ControllerState* controller = create_ControllerState();

    Uint32 last_frame = SDL_GetTicks();
    while(try_controller(data,controller)){
        Uint32 current_time = SDL_GetTicks();
        if(SDL_TICKS_PASSED(current_time, last_frame + FRAME_INTERVAL)){ 
            render_game(data,config);
            last_frame = current_time;
        } 
    }

    destroy_DrawConfig(config);
    destroy_GameData(data);
    destroy_ControllerState(controller);

}
