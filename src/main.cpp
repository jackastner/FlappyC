#include <ctime>
#include <cstdlib>
#include "gamelogic.h"
#include "drawlogic.h"
#include "controllerlogic.h"

#ifdef __ANDROID__
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

#define MAX_FPS 30
#define MS_TO_S 1000
#define FRAME_INTERVAL (MS_TO_S / MAX_FPS)

int main(int argc, char** argv){
    /*seed rng used by game*/
    std::srand(std::time(NULL));

    GameData data;
    DrawConfig config (&data);
    ControllerState controller (&data, &config);

    Uint32 last_frame = SDL_GetTicks();
    while(controller.try_controller()){
        Uint32 current_time = SDL_GetTicks();
        if(SDL_TICKS_PASSED(current_time, last_frame + FRAME_INTERVAL)){ 
            if(current_time - last_frame > 35){
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "slow frame time: %d\n", current_time - last_frame);
            }
            config.render_game();
            last_frame = current_time;
        } 
    }
}
