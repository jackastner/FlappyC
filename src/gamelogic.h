#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#define FLAP_V -5

/*
 * Public interface
 */

typedef struct GameData GameData;

typedef struct Pipe Pipe;
struct Pipe {
    int x;
    int top_end;
    int bottom_start;
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
int get_bird_v(GameData* data);
int get_bird_y(GameData* data);
int get_bird_x(GameData* data);
int get_bird_height(GameData* data);
int get_bird_width(GameData* data);

int get_stage_width(GameData* data);
int get_stage_height(GameData* data);

int get_score(GameData* data);

int is_gameover(GameData* data);
int _pipe_collision(GameData* data);

int get_num_pipes(GameData* data);
Pipe get_pipe(GameData* data, int index);
int get_pipe_width(GameData* data);

#endif
