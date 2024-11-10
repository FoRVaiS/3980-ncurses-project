#include "scene.h"
#include "scene-component.h"
#include "vector.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#define A 100

Scene *scene_init(int *err)
{
    Scene *scene;

    Vector *components = vector_init(A, err);
    if(components == NULL)
    {
        scene = NULL;
        fprintf(stderr, "Scene: Failed to initialize components vector.\n");
        goto exit;
    }

    scene = (Scene *)calloc(1, sizeof(Scene));
    if(scene == NULL)
    {
        fprintf(stderr, "Scene: Failed to calloc space for scene instance.\n");
        goto exit;
    }

    scene->components           = components;
    scene->focused_component_id = -1;

exit:
    return scene;
}

void scene_destroy(void *vscene)
{
    int err;

    Scene *scene;

    scene = (Scene *)vscene;

    vector_destroy(scene->components, scene_component_destroy, &err);
    memset(scene, 0, sizeof(Scene));
    free(scene);
}

void scene_update(const Scene *scene)
{
    int err;

    for(size_t component_id = 0; component_id < vector_size(scene->components); component_id++)
    {
        const SceneComponent *component;

        err       = 0;
        component = (SceneComponent *)vector_get(scene->components, component_id, &err);
        scene_component_update(component);
    }
}

void scene_add_component(Scene *scene, SceneComponent *component)
{
    int err;

    err = 0;
    vector_add(scene->components, component, &err);
}

void scene_remove_component(Scene *scene, size_t component_id)
{
    SceneComponent *component;

    component = (SceneComponent *)vector_remove(scene->components, component_id);

    if(component != NULL)
    {
        scene_component_destroy(component);
    }
}

void scene_focus_component(Scene *scene, size_t component_id)
{
    scene->focused_component_id = (ssize_t)component_id;
}

SceneComponent *scene_get_component(const Scene *scene, size_t component_id)
{
    int err;

    SceneComponent *component;

    err       = 0;
    component = (SceneComponent *)vector_get(scene->components, component_id, &err);
    if(err != 0)
    {
        return NULL;
    }

    return component;
}

SceneComponent *scene_get_focused_component(Scene *scene)
{
    size_t ncomponents;

    ncomponents = vector_size(scene->components);

    if(ncomponents == 0)
    {
        return NULL;
    }

    if(scene->focused_component_id == -1)
    {
        // There are components but none were focused
        // Automatically select the last component
        scene->focused_component_id = (ssize_t)(ncomponents - 1);
    }

    return scene_get_component(scene, (size_t)scene->focused_component_id);
}
