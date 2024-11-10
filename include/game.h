
#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

typedef enum
{
    GAME_ERR_CALLOC
} GameErrors;

typedef struct
{
    int  ticks;
    bool running;
} Game;

Game *game_init(int ticks, int *err);
void  game_destroy(void *vgame);
void  game_start(Game *game);
void  game_stop(Game *game);
void  game_loop(Game *game);

#endif
