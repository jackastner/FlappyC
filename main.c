#include "drawlogic.h"
#include "controllerlogic.h"
#include "gamelogic.h"
#include <stdio.h>
#include <HsFFI.h>

int main(int argc, char** argv){
    hs_init(&argc,&argv);

    DrawConfig* config = create_DrawConfig();
    GameData* data = create_GameData();
    ControllerState* controller = create_ControllerState();

    while(try_controller(data,controller)){
        render_game(data,config);
    }

    destroy_DrawConfig(config);
    destroy_GameData(data);
    destroy_ControllerState(controller);

    hs_exit();
}
