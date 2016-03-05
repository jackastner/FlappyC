#include "gamelogic.h"

int get_bird_y(GameData* data){
        return data->bird_y;
}

int get_bird_x(GameData* data){
        return data->bird_x;
}

int get_stage_width(GameData* data){
        return data->stage_width;
}

int get_stage_height(GameData* data){
        return data->stage_height;
}

int get_score(GameData* data){
        return data->score;
}

Pipe *get_pipe(GameData* data, int index){
        return (data->pipe_array) + index;
}

