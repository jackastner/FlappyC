#include <string.h>
#include <time.h>
#include "gamelogic.h"
#include "drawlogic.h"
#include "controllerlogic.h"

int main(int argc, char** argv){
    DrawConfig* config = create_DrawConfig();
    GameData* data = create_GameData();
    ControllerState* controller = create_ControllerState();

    while(try_controller(data,controller)){
        render_game(data,config);
    }

    destroy_DrawConfig(config);
    destroy_GameData(data);
    destroy_ControllerState(controller);

}
