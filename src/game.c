#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define MS_PER_SEC 1000

static long long get_time_ms(void);

static void update(const Game *game);
static void tick(Game *game, long long ms);

void update(const Game *game)
{
    (void)game;
}

void tick(Game *game, long long ms)
{
    (void)game;
    (void)ms;
}

// ============================
Game *game_init(int ticks, int *err)
{
    Game *game;

    game = (Game *)calloc(1, sizeof(Game));
    if(game == NULL)
    {
        fprintf(stderr, "Game: Failed to initialize.\n");
        *err = GAME_ERR_CALLOC;
        goto exit;
    }

    game->ticks = ticks;

exit:
    return game;
}

void game_destroy(void *vgame)
{
    Game *game = (Game *)vgame;

    memset(game, 0, sizeof(Game));
    free(game);
}

void game_start(Game *game)
{
    game->running = true;
    game_loop(game);
}

void game_stop(Game *game)
{
    game->running = false;
}

void game_loop(Game *game)
{
    long long MS_PER_TICK = MS_PER_SEC / game->ticks;
    long long lastTime;

    lastTime = get_time_ms();

    while(game->running)
    {
        long long currentTime = get_time_ms();
        long long deltaTime   = currentTime - lastTime;

        update(game);

        if(deltaTime >= MS_PER_TICK)
        {
            lastTime = currentTime;

            tick(game, deltaTime);
        }
    }
}

static long long get_time_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (((long long)tv.tv_sec) * MS_PER_SEC) + (tv.tv_usec / MS_PER_SEC);
}
