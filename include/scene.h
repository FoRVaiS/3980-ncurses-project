#ifndef SCENE_H
#define SCENE_H

#include "scene-component.h"
#include "vector.h"
#include <stdlib.h>

typedef struct
{
    Vector *components;
    ssize_t focused_component_id;
} Scene;

Scene          *scene_init(int *err);
void            scene_destroy(void *vscene);
void            scene_update(const Scene *scene);
void            scene_add_component(Scene *scene, SceneComponent *component);
void            scene_remove_component(Scene *scene, size_t component_id);
void            scene_focus_component(Scene *scene, size_t component_id);
SceneComponent *scene_get_component(const Scene *scene, size_t component_id);
SceneComponent *scene_get_focused_component(Scene *scene);

#endif
