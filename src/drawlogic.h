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

typedef enum {PLAY, QUIT, BUTTONS_COUNT} Button;

typedef enum Text Text;

DrawConfig *create_DrawConfig();
void destroy_DrawConfig(DrawConfig* config);

SDL_Rect** get_button_rects(DrawConfig* config);

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
void render_string(DrawConfig* config, char* str, int center_x, int center_y);
void render_known_string(DrawConfig* config, Text str_id, int center_x, int center_y);
void render_button(DrawConfig* config, Text str_id, Button button_id, int center_x, int center_y);
void texture_of_string(DrawConfig* config, SDL_Color color, char* str, SDL_Texture** texture);

void render_pipe(GameData* data, DrawConfig* config, Pipe* pipe);
int scale_x_to_userspace(GameData* data, DrawConfig* config, int x);
int scale_y_to_userspace(GameData* data, DrawConfig* config, int y);

#endif
