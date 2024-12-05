// cppcheck-suppress-file unusedStructMember

#ifndef SCENE_COMPONENT_H
#define SCENE_COMPONENT_H

#include <ncurses.h>

typedef struct
{
    // ncurses
    WINDOW *win;

    // Layout Properties
    int x;
    int y;
    int width;
    int height;
} SceneComponent;

SceneComponent *scene_component_create(int x, int y, int width, int height);
void            scene_component_destroy(void *vcomponent);
void            scene_component_redraw(const SceneComponent *component);
void            scene_component_update(const SceneComponent *component);
void            scene_component_clear(const SceneComponent *component);
void            scene_component_refresh(const SceneComponent *component);
void            scene_component_render(const SceneComponent *component, int x, int y, chtype ch);

#endif
