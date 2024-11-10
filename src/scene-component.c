#include "scene-component.h"
#include <ncurses.h>
#include <stdlib.h>

SceneComponent *scene_component_create(int x, int y, int width, int height)
{
    WINDOW         *win;
    SceneComponent *component;

    win = newwin(height, width, y, x);

    component = (SceneComponent *)calloc(1, sizeof(SceneComponent));
    if(component == NULL)
    {
        goto exit;
    }

    component->win    = win;
    component->x      = x;
    component->y      = y;
    component->width  = width;
    component->height = height;

    scene_component_clear(component);
    scene_component_refresh(component);

exit:
    return component;
}

void scene_component_destroy(void *vcomponent)
{
    SceneComponent *component = (SceneComponent *)vcomponent;

    delwin(component->win);
    free(component);
}

void scene_component_update(const SceneComponent *component)
{
    scene_component_refresh(component);
}

void scene_component_clear(const SceneComponent *component)
{
    wclear(component->win);
    box(component->win, 0, 0);
}

void scene_component_refresh(const SceneComponent *component)
{
    wrefresh(component->win);
}
