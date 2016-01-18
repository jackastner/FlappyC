#include <string.h>
#include "gamelogic.h"
#include "drawlogic.h"
#include "controllerlogic.h"

int main(int argc, char** argv){
    DrawConfig* config = create_DrawConfig();
    GameData* data = create_GameData();
    ControllerState* controller = create_ControllerState();

    printf("Start\n");
    run_controller(data, config, controller);
    printf("End\n");

    destroy_DrawConfig(config);
    destroy_GameData(data);
    destroy_ControllerState(controller);

}