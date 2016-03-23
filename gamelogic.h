#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

/*
 * Public interface
 */

typedef struct Pipe Pipe;
struct Pipe {
    int x;
    int top_end;
    int bottom_start;
};

typedef struct GameData GameData;
struct GameData {
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

    int seed;

    Pipe *pipe_array;
};

GameData* create_GameData();
void destroy_GameData(GameData* data);



/*
 * setter methods for use by the controller
 */

void reset_state(GameData* data);
void flap_bird(GameData* data);
void update_state(GameData* data);

/*
 * access method for use by the view
 */
int get_bird_y(GameData* data);
int get_bird_x(GameData* data);

int get_stage_width(GameData* data);
int get_stage_height(GameData* data);

int get_score(GameData* data);

int is_gameover(GameData* data);
int _pipe_collision(GameData* data);

int get_num_pipes(GameData* data);
Pipe *get_pipe(GameData* data, int index);

#endif
