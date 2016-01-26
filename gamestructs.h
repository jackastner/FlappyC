#ifndef GAME_STRUCTS
#define GAME_STRUCTS
typedef struct GameData GameData;

typedef struct Pipe Pipe;
struct Pipe {
    int x;
    int top_end;
    int bottom_start;
};
#endif
