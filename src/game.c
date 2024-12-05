#include "game.h"
#include "entity-component.h"
#include "entity.h"
#include "input.h"
#include "level.h"
#include "networking.h"
#include "packet.h"
#include "scene-component.h"
#include "scene.h"
#include "systems.h"
#include "utils.h"
#include "window.h"
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define MS_PER_SEC 1000

#define P1X 10
#define P1Y 10
#define P2X 20
#define P2Y 20

static void send_position_packets(Game *game, const EntityTransformComponent *component);
static int  setup_level(Game *game, int *err);
static void on_packet(Game *game, const uint8_t *packet);
static void update(Game *game);
static void tick(Game *game, long long ms);

void update(Game *game)
{
    int err;

    uint8_t *packet;

    if(game->selected_level != NULL)
    {
        Level  *level     = game->selected_level;
        Entity *entities  = level->entities;
        uint8_t nentities = level->nentities;

        input_process(entities, nentities);
    }

    err = 0;
    if(client_read_packet(game->client, &packet, &err) == 1)
    {
        on_packet(game, packet);
    }
}

void tick(Game *game, long long ms)
{
    const Scene *scene;

    const SceneComponent *main;
    SceneComponent       *header;

    (void)game;
    (void)ms;

    main   = window_get_focused_panel(game->window);
    scene  = window_get_scene(game->window, 0);
    header = scene_get_component(scene, 0);

    if(main == NULL || main->win == NULL || header == NULL || header->win == NULL)
    {
        return;
    }

    mvwprintw(header->win, 1, 1, "Window Dimensions: %d x %d", main->width, main->height);
    window_update(game->window);

    // Run Systems
    if(game->selected_level != NULL)
    {
        Level  *level     = game->selected_level;
        Entity *entities  = level->entities;
        uint8_t nentities = level->nentities;

        Entity                   *player = &entities[0];
        EntityTransformComponent *player_transform;

        Entity                   *other_player = &entities[1];
        EntityTransformComponent *other_player_transform;

        movement_system_process(entities, nentities, (uint8_t)(main->width - 2), (uint8_t)(main->height - 2));
        render_process(main, entities, nentities);

        // Get player coords
        entity_find_component(player, (EntityComponent **)&player_transform, ENTITY_COMPONENT_TRANSFORM);
        entity_find_component(other_player, (EntityComponent **)&other_player_transform, ENTITY_COMPONENT_TRANSFORM);
        mvwprintw(header->win, 2, 1, "Player -> (%d, %d)", player_transform->x, player_transform->y);
        mvwprintw(header->win, 3, 1, "Other Player -> (%d, %d)", other_player_transform->x, other_player_transform->y);

        send_position_packets(game, player_transform);
    }
}

static void send_position_packets(Game *game, const EntityTransformComponent *component)
{
    int err;

    uint8_t *bytes_transform;

    PacketHeader                   header;
    EntityComponentPacketHeader    component_header;
    EntityTransformComponentPacket transform_packet;

    packet_create_entity_transform_component(&transform_packet, &header, &component_header, game->client->conn.packet_id, component);
    serialize_entity_transform_component(&bytes_transform, &transform_packet);
    connection_update_packet_id(&game->client->conn, &header);

    err = 0;
    client_send_packet(game->client, bytes_transform, &err);

    free(bytes_transform);
}

static void on_packet(Game *game, const uint8_t *packet)
{
    PacketHeader header;
    size_t       offset;

    offset = 0;
    deserialize_header(&header, packet, &offset);

    if(header.payload_type == PAYLOAD_COMPONENT)
    {
        EntityComponentPacketHeader component_header;
        deserialize_entity_component_header(&component_header, packet, &offset);

        if(component_header.component_type == ENTITY_COMPONENT_TRANSFORM)
        {
            Entity                   *other_player;
            EntityTransformComponent *other_player_transform;

            EntityTransformComponentPacket transform_packet;
            deserialize_entity_transform_component(&transform_packet, packet);

            other_player = &game->selected_level->entities[1];
            entity_find_component(other_player, (EntityComponent **)&other_player_transform, ENTITY_COMPONENT_TRANSFORM);

            other_player_transform->x = transform_packet.component.x;
            other_player_transform->y = transform_packet.component.y;
        }
    }
}

// ============================
Game *game_init(Client *client, int ticks, int *err)
{
    Game   *game;
    Window *window;

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
    game->client = client;
    game->ticks  = ticks;

    if(setup_level(game, err) < 0)
    {
        fprintf(stderr, "Game: Failed to create level.\n");
        free(game);
        game = NULL;
        goto exit;
    }

exit:
    return game;
}

static int setup_level(Game *game, int *err)
{
    int retval;

    Level  level;
    Entity player;
    Entity otherPlayer;

    if(entity_create_player(&player, P1X, P1Y, 'i', get_input_device()) < 0)
    {
        fprintf(stderr, "Game: Failed to create player.\n");
        *err   = GAME_ERR_PLAYER_CREATION_FAILED;
        retval = -1;
        goto exit;
    }

    if(entity_create_player(&otherPlayer, P2X, P2Y, ';', NULL) < 0)
    {
        fprintf(stderr, "Game: Failed to create player.\n");
        *err   = GAME_ERR_PLAYER_CREATION_FAILED;
        retval = -2;
        goto exit;
    }

    level_init(&level);
    level_add_entity(&level, &player);
    level_add_entity(&level, &otherPlayer);
    game_add_level(game, &level);
    game_select_level(game, 0);    // Redundant but being explicit

    retval = 0;

exit:
    return retval;
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
