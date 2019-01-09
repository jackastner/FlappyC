#ifndef DRAW_LOGIC_H
#define DRAW_LOGIC_H

#include "gamelogic.h"

#ifdef __ANDROID__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#define BIRD_SPRITE_NUM 4 

typedef struct DrawConfig DrawConfig;

DrawConfig *create_DrawConfig();
void destroy_DrawConfig(DrawConfig* config);


/*
 * Private Interface
 */

void render_game(GameData* data, DrawConfig* config);
void render_background(GameData* data, DrawConfig* config);
void render_all_pipes(GameData* data, DrawConfig* config);
void render_score(GameData* data, DrawConfig* config);
void render_bird(GameData* data, DrawConfig* config);
void render_gameover_message(GameData* data, DrawConfig* config);
void render_clear(DrawConfig* config);

void render_pipe(GameData* data, DrawConfig* config, Pipe* pipe);
int scale_x_to_userspace(GameData* data, DrawConfig* config, int x);
int scale_y_to_userspace(GameData* data, DrawConfig* config, int y);

#endif
