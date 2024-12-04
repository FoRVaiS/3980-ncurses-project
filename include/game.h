
#ifndef GAME_H
#define GAME_H

#include "level.h"
#include "window.h"
#include <stdbool.h>

#define GAME_MAX_LEVELS 1

typedef enum
{
    GAME_ERR_CALLOC,
    GAME_ERR_PLAYER_CREATION_FAILED,
} GameErrors;

typedef struct
{
    int  ticks;
    bool running;

    Window *window;

    Level   levels[GAME_MAX_LEVELS];
    uint8_t nlevels;
    Level  *selected_level;
} Game;

Game *game_init(int ticks, int *err);
void  game_destroy(void *vgame);

int  game_add_level(Game *game, const Level *level);
void game_select_level(Game *game, uint8_t level_idx);

void game_start(Game *game);
void game_stop(Game *game);
void game_loop(Game *game);

#endif
