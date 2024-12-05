#include "window.h"
#include "scene-component.h"
#include "scene.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GAME_SCENE_HEADER_X 0
#define GAME_SCENE_HEADER_Y 0
#define GAME_SCENE_HEADER_WIDTH 80
#define GAME_SCENE_HEADER_HEIGHT 5

#define GAME_SCENE_MAIN_X 0
#define GAME_SCENE_MAIN_Y 5
#define GAME_SCENE_MAIN_WIDTH 80
#define GAME_SCENE_MAIN_HEIGHT (30 - 5)

static int setup_scenes(Window *window, int *err);

Window *window_init(int *err)
{
    Window *window;
    Vector *scenes;

    initscr();
    curs_set(0);

    scenes = vector_init(1, err);
    if(scenes == NULL)
    {
        fprintf(stderr, "Window: Failed to initialize scenes vector.\n");
        window = NULL;
        goto exit;
    }

    window = (Window *)calloc(1, sizeof(Window));
    if(window == NULL)
    {
        *err = WINDOW_ERR_CALLOC;
        goto exit;
    }

    window->scenes           = scenes;
    window->focused_scene_id = -1;

    if(setup_scenes(window, err) < 0)
    {
        fprintf(stderr, "Window: Failed to setup scenes.\n");
        window_destroy(window);
        window = NULL;
    }

exit:
    return window;
}

static int setup_scenes(Window *window, int *err)
{
    int retval = 0;

    Scene          *game_scene;
    SceneComponent *game_scene_header;
    SceneComponent *game_scene_main;

    *err       = 0;
    game_scene = scene_init(err);
    if(game_scene == NULL)
    {
        retval = -1;
        goto exit;
    }

    // Game Scene
    game_scene_header = scene_component_create(GAME_SCENE_HEADER_X, GAME_SCENE_HEADER_Y, GAME_SCENE_HEADER_WIDTH, GAME_SCENE_HEADER_HEIGHT);
    game_scene_main   = scene_component_create(GAME_SCENE_MAIN_X, GAME_SCENE_MAIN_Y, GAME_SCENE_MAIN_WIDTH, GAME_SCENE_MAIN_HEIGHT);
    scene_add_component(game_scene, game_scene_header);
    scene_add_component(game_scene, game_scene_main);
    scene_focus_component(game_scene, 1);

    window_add_scene(window, game_scene);

exit:
    return retval;
}

void window_destroy(void *vwindow)
{
    int err;

    Window *window = (Window *)vwindow;

    vector_destroy(window->scenes, scene_destroy, &err);
    memset(window, 0, sizeof(Window));
    free(window);

    // Clean up ncurses resources
    endwin();
}

void window_update(Window *window)
{
    const Scene *scene = window_get_focused_scene(window);

    if(scene != NULL)
    {
        scene_update(scene);
    }
}

void window_add_scene(Window *window, Scene *scene)
{
    int err;

    err = 0;
    vector_add(window->scenes, scene, &err);
}

void window_remove_scene(Window *window, size_t scene_id)
{
    vector_remove(window->scenes, scene_id);
}

void window_focus_scene(Window *window, size_t scene_id)
{
    window->focused_scene_id = (ssize_t)scene_id;
}

Scene *window_get_scene(const Window *window, size_t scene_id)
{
    int err;

    Scene *scene;

    err   = 0;
    scene = (Scene *)vector_get(window->scenes, scene_id, &err);
    if(err != 0)
    {
        return NULL;
    }

    return scene;
}

Scene *window_get_focused_scene(Window *window)
{
    size_t nscenes;

    nscenes = vector_size(window->scenes);

    if(nscenes == 0)
    {
        return NULL;
    }

    if(window->focused_scene_id == -1)
    {
        window->focused_scene_id = (ssize_t)(nscenes - 1);
    }

    return window_get_scene(window, (size_t)window->focused_scene_id);
}

SceneComponent *window_get_focused_panel(Window *window)
{
    Scene *scene = window_get_focused_scene(window);

    if(scene == NULL)
    {
        return NULL;
    }

    return scene_get_focused_component(scene);
}
