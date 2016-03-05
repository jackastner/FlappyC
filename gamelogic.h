#ifndef GAMELOGIC_H
#define GAMELOGIC_H

/* type definitions required by both c and haskell source */
#include "gamestructs.h"

/* Functions exported by haskell source */
#include "gamelogic_stub.h"

/* Accessor function implemented in c */

int get_bird_y(GameData* data);
int get_bird_x(GameData* data);

int get_stage_width(GameData* data);
int get_stage_height(GameData* data);

int get_score(GameData* data);

Pipe *get_pipe(GameData* data, int index);
#endif
