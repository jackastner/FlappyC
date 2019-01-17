#include <ctime>
#include <cstdlib>
#include <cstdio>

#include "gamelogic.h"

Pipe::Pipe(int x, int top_end, int bottom_start) :
    x(x),
    top_end(top_end),
    bottom_start(bottom_start)
{}

GameData::GameData() {

    /*
     * Set stage attributes
     */
    stage_width = 100;
    stage_height = 100;
    bird_width = 9;
    bird_height = 7;
    pipe_width = 13;
    pipe_v = 3;

    flap_v = -6;
    bird_a = 2;

    pipe_interval = (stage_width + 2*pipe_width) / 2;

    /*
     * Set nominaly mutable game properties to initial values.
     */
    reset_state();
}

void GameData::reset_state(){
    /*
     * Set initial bird attributes
     */
    bird_x = 50;
    bird_y = 50;
    bird_v = flap_v;

    score = 0;

    /*
     * Fill pipe array with initial pipes
     */

    pipes.clear();
    for(int i = 0; i < get_num_pipes(); i++){
       Pipe pipe (pipe_interval*(i+1) - 20, stage_height/2 - 15, stage_height/2 + 15);
       pipes.push_back(pipe);
    }
}

void GameData::update_state(){
    /*
     * Increase score if applicable
     */
    for(int i = 0; i < get_num_pipes(); i++){
        Pipe pipe = pipes[i];
        int in_x_range = bird_x > (pipe.x - pipe_v) && 
                         bird_x <= pipe.x;
        if(in_x_range){
            score++;
        }
    }

    /*
     * Move bird and update velocity
     */
    bird_y += bird_v;
    bird_v += bird_a;

    /*
     * Move pipes forward
     */
    for(int i = 0; i < get_num_pipes(); i++){
        pipes[i].x-=pipe_v;
        if(pipes[i].x < -pipe_width){
            pipes[i].x = stage_width + pipe_width;
            pipes[i].top_end = std::rand()%(stage_height-30);
            pipes[i].bottom_start = pipes[i].top_end + 30;
        }
    }
}

void GameData::flap_bird(){
    bird_v = flap_v;
}

int GameData::get_flap_v(){
    return flap_v;
}

int GameData::get_bird_v(){
    return bird_v;
}

int GameData::get_bird_y(){
    return bird_y;
}

int GameData::get_bird_x(){
    return bird_x;
}

int GameData::get_bird_height(){
    return bird_height;
}

int GameData::get_bird_width(){
    return bird_width;
}

int GameData::get_stage_width(){
    return stage_width;
}

int GameData::get_stage_height(){
    return stage_height;
}

int GameData::get_score(){
    return score;
}

int GameData::is_gameover(){
    return bird_y < 0 ||
           bird_y >= stage_height || 
           pipe_collision();
}

int GameData::pipe_collision(){
    for(int i = 0; i < get_num_pipes(); i++){
        Pipe pipe = pipes[i];
        int in_x_range = bird_x > (pipe.x - pipe_width/2 - bird_width/2) && 
                         bird_x < (pipe.x + pipe_width/2 + bird_width/2);
        int in_y_range = bird_y > (pipe.bottom_start - bird_height/2) ||
                         bird_y < pipe.top_end + bird_height/2;
        if(in_x_range && in_y_range){
            return 1;
        }
    }
    return 0;
}

Pipe GameData::get_pipe(int index){
    return pipes[index];
}

int GameData::get_num_pipes(){
    return (stage_width + 2*pipe_width)/pipe_interval;
}

int GameData::get_pipe_width(){
    return pipe_width;
}
