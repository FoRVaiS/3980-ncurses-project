#include "game.h"
#include "entity-component.h"
#include "entity.h"
#include "level.h"
#include "scene-component.h"
#include "systems.h"
#include "window.h"
#include <stdio.h>
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
    if(game->selected_level != NULL)
    {
        Level  *level     = game->selected_level;
        Entity *entities  = level->entities;
        uint8_t nentities = level->nentities;

        input_process(entities, nentities);
    }
}

void tick(Game *game, long long ms)
{
    SceneComponent *panel;

    (void)game;
    (void)ms;

    panel = window_get_focused_panel(game->window);
    if(panel == NULL || panel->win == NULL)
    {
        return;
    }

    mvwprintw(panel->win, 1, 1, "Terminal Dimensions: %d x %d", game->window->width, game->window->height);
    window_update(game->window);

    // Run Systems
    if(game->selected_level != NULL)
    {
        Level  *level     = game->selected_level;
        Entity *entities  = level->entities;
        uint8_t nentities = level->nentities;

        Entity                   *player = &entities[0];
        EntityTransformComponent *player_transform;

        movement_system_process(entities, nentities);

        // Get player coords
        entity_find_component(player, (EntityComponent **)&player_transform, ENTITY_COMPONENT_TRANSFORM);
        mvwprintw(panel->win, 2, 1, "Player -> (%d, %d)", player_transform->x, player_transform->y);
    }
}

// ============================
Game *game_init(int ticks, int *err)
{
    Game   *game;
    Window *window;

    Level  level;
    Entity player;

    window = window_init(err);
    if(window == NULL)
    {
        game = NULL;
        fprintf(stderr, "Game: Failed to initialize Window.\n");
        goto exit;
    }

    game = (Game *)calloc(1, sizeof(Game));
    if(game == NULL)
    {
        fprintf(stderr, "Game: Failed to initialize.\n");
        *err = GAME_ERR_CALLOC;
        goto exit;
    }

    game->window = window;
    game->ticks  = ticks;

    if(entity_create_player(&player, 4, 4, '#') < 0)
    {
        fprintf(stderr, "Game: Failed to create player.\n");
        *err = GAME_ERR_PLAYER_CREATION_FAILED;
        goto exit;
    }

    level_init(&level);
    level_add_entity(&level, &player);
    game_add_level(game, &level);
    game_select_level(game, 0);    // Redundant but being explicit

exit:
    return game;
}

void game_destroy(void *vgame)
{
    Game *game = (Game *)vgame;

    window_destroy(game->window);

    memset(game, 0, sizeof(Game));
    free(game);
}

int game_add_level(Game *game, const Level *level)
{
    if(game->nlevels < GAME_MAX_LEVELS)
    {
        memcpy(&game->levels[game->nlevels], level, sizeof(Level));
        game->nlevels++;
        return 0;
    }

    return -1;
}

void game_select_level(Game *game, uint8_t level_idx)
{
    if(level_idx < game->nlevels)
    {
        game->selected_level = &game->levels[level_idx];
    }
    else
    {
        game->selected_level = NULL;
    }
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
