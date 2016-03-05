#ifndef GAME_STRUCTS
#define GAME_STRUCTS


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

    Pipe *pipe_array;
};

#endif
