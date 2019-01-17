#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <vector>

struct Pipe {
    Pipe(int x, int top_end, int bottom_start);
    int x;
    int top_end;
    int bottom_start;
};

class GameData {
        int stage_width;
        int stage_height;

        int bird_width;
        int bird_height;

        int pipe_width;
        int pipe_interval;
        int pipe_v;

        int score;

        int bird_v;
        int bird_x;
        int bird_y;

        int bird_a;
        int flap_v;

        std::vector<Pipe> pipes;

        int pipe_collision();

    public:
        GameData();

        void reset_state();
        void flap_bird();
        void update_state();

        int get_flap_v(); int get_bird_v();
        int get_bird_y();
        int get_bird_x();
        int get_bird_height();
        int get_bird_width();

        int get_stage_width();
        int get_stage_height();

        int get_score();

        int is_gameover();

        int get_num_pipes();
        Pipe get_pipe(int index);
        int get_pipe_width();
};
#endif
