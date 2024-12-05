// cppcheck-suppress-file unusedStructMember

#ifndef WINDOW_H
#define WINDOW_H

#include "scene.h"
#include "vector.h"
#include <ncurses.h>

typedef struct
{
    // Scene **scenes;
    // size_t  nscenes;
    Vector *scenes;
    ssize_t focused_scene_id;

    int width;
    int height;
} Window;

typedef enum
{
    WINDOW_ERR_CALLOC = -1,
} WindowErrors;

Window *window_init(int *err);
void    window_destroy(void *vwindow);

void            window_update(Window *window);
void            window_add_scene(Window *window, Scene *scene);
void            window_remove_scene(Window *window, size_t scene_id);
void            window_focus_scene(Window *window, size_t scene_id);
Scene          *window_get_scene(const Window *window, size_t scene_id);
Scene          *window_get_focused_scene(Window *window);
SceneComponent *window_get_focused_panel(Window *window);

#endif
