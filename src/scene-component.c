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

    nodelay(component->win, TRUE);
    scene_component_redraw(component);
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

void scene_component_redraw(const SceneComponent *component)
{
    WINDOW *win      = component->win;
    int     currentW = getmaxx(component->win);
    int     currentH = getmaxy(component->win);

    // current = the dimensions of the window currently (may be smaller due to terminal resize)
    // component-> = the desired dimensions of the window

    mvwaddch(win, 0, 0, ACS_ULCORNER);

    if(currentW < component->width)
    {
        mvwhline(win, 0, 1, 0, currentW - 1);
        mvwhline(win, component->height - 1, 1, 0, currentW - 1);
    }

    if(currentH < component->height)
    {
        mvwvline(win, 1, 0, 0, currentH - 1);
        mvwvline(win, 1, component->width - 1, 0, currentH - 1);
    }

    if(currentH >= component->height)
    {
        mvwvline(win, 1, 0, 0, component->height - 1);
        mvwvline(win, 1, component->width - 1, 0, component->height - 1);
        mvwaddch(win, component->height - 1, 0, ACS_LLCORNER);
    }

    if(currentW >= component->width)
    {
        mvwhline(win, 0, 1, 0, component->width - 1);
        mvwhline(win, component->height - 1, 1, 0, component->width - 1);
        mvwaddch(win, 0, component->width - 1, ACS_URCORNER);
    }

    if(currentH >= component->height && currentW >= component->width)
    {
        mvwaddch(win, component->height - 1, component->width - 1, ACS_LRCORNER);
    }
}

void scene_component_update(const SceneComponent *component)
{
    scene_component_refresh(component);
    scene_component_redraw(component);
}

void scene_component_clear(const SceneComponent *component)
{
    for(int i = 1; i < component->height - 1; i++)
    {
        mvwhline(component->win, i, 1, ' ', component->width - 1 - 1);
    }
}

void scene_component_refresh(const SceneComponent *component)
{
    wrefresh(component->win);
}

void scene_component_render(const SceneComponent *component, int x, int y, chtype ch)
{
    mvwaddch(component->win, y, x, ch);
}
