#ifndef CONT_LOGIC_H
#define CONT_LOGIC_H

#include "drawlogic.h"
#include "gamelogic.h"

class ControllerState {
        GameData* data;
        DrawConfig* config;

        Uint32 tick_length;
        Uint32 poll_interval;
        Uint32 last_user_input;
        Uint32 last_game_tick;

        void control_game();
        void proccese_game_event();
        int control_menu();
        int proccese_menu_event();
        int handle_button_click(DrawConfig::Button button);

    public:
        ControllerState(GameData* data, DrawConfig* config);
        int try_controller();

};

#endif
