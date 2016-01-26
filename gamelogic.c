#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "gamelogic.h"

struct GameData {
    int stage_width;
    int stage_height;

    int bird_width;
    int bird_height;

    int pipe_width;
    int pipe_gap_height;
    int pipe_interval;
    int pipe_v;

    int score;

    int bird_v;
    int bird_x;
    int bird_y;

    Pipe *pipe_array;
};

GameData *create_GameData(){
    GameData* data = malloc(sizeof (GameData));

    /*
     * Set stage attributes
     */

    data->stage_width = 100;
    data->stage_height = 100;
    data->bird_width = 5;
    data->bird_height = 4;
    data->pipe_width = 8;
    data->pipe_v = 2;

    data->pipe_interval = (data->stage_width + 2*data->pipe_width) / 2;

    /*
     * allocate memory to store pipes
     */
    data->pipe_array = malloc((sizeof (Pipe))*get_num_pipes(data));

    /*
     * Set nominaly mutable game properties to initial values.
     */
    reset_state(data);


    /*
     * Seed random for use in random pipe placement
     */
    srand(time(NULL));

    return data;
}

void destroy_GameData(GameData* data){
    free(data->pipe_array);
    free(data);
}

void reset_state(GameData* data){
    /*
     * Set initial bird attributes
     */
    data->bird_x = 50;
    data->bird_y = 50;
    data->bird_v = -5;

    data->score = 0;

    /*
     * Fill pipe array with initial pipes
     */

    int num_pipes = get_num_pipes(data);
    for(int i = 0; i < num_pipes; i++){
        data->pipe_array[i].x = data->pipe_interval*(i+1) - 20;  
        data->pipe_array[i].top_end = data->stage_height/2 - 15;
        data->pipe_array[i].bottom_start = data->stage_height/2 + 15;
    }
}

void update_state(GameData* data){
    /*
     * Increase score if applicable
     */
    for(int i = 0; i < get_num_pipes(data); i++){
        Pipe pipe = data->pipe_array[i];
        int in_x_range = data->bird_x > (pipe.x - data->pipe_v) && 
                         data->bird_x <= pipe.x;
        if(in_x_range){
            data->score++;
        }
    }

    /*
     * Move bird and update velocity
     */
    data->bird_y+=data->bird_v;
    data->bird_v++;

    /*
     * Move pipes forward
     */
    for(int i = 0; i < get_num_pipes(data); i++){
        data->pipe_array[i].x-=data->pipe_v;
        if(data->pipe_array[i].x < -data->pipe_width){
            data->pipe_array[i].x = data->stage_width + data->pipe_width;

            data->pipe_array[i].top_end = rand()%(data->stage_height-30);
            data->pipe_array[i].bottom_start = data->pipe_array[i].top_end + 30;
        }
    }
}

void flap_bird(GameData* data){
    data->bird_v = -5;
}

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

int is_gameover(GameData* data){
    return data->bird_y < 0 ||
           data->bird_y >= data->stage_height || 
           _pipe_collision(data);
}

int _pipe_collision(GameData* data){
    for(int i = 0; i < get_num_pipes(data); i++){
        Pipe pipe = data->pipe_array[i];
        int in_x_range = data->bird_x > (pipe.x - data->pipe_width/2 - data->bird_width/2) && 
                         data->bird_x < (pipe.x + data->pipe_width/2 + data->bird_width/2);
        int in_y_range = data->bird_y > (pipe.bottom_start - data->bird_height/2) ||
                         data->bird_y < pipe.top_end + data->bird_height/2;
        if(in_x_range && in_y_range){
            return 1;
        }
    }
    return 0;
}

Pipe get_pipe(GameData* data, int index){
    return data->pipe_array[index];
}

int get_num_pipes(GameData* data){
    return (data->stage_width + 2*data->pipe_width)/data->pipe_interval;
}
