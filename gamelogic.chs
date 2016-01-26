{-# LANGUAGE ForeignFunctionInterface #-}
module GameLogic where

#include "gamestructs.h"

#c
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
#endc
