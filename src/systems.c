#include "systems.h"
#include "entity-component.h"
#include "entity.h"
#include "input.h"
#include "scene-component.h"

void movement_system_process(Entity *entities, uint8_t nentities)
{
    for(uint8_t idx = 0; idx < nentities; idx++)
    {
        EntityTransformComponent        *transform;
        EntityPlayerControllerComponent *controller;
        Entity                          *entity = &entities[idx];

        if(entity_find_component(entity, (EntityComponent **)&transform, ENTITY_COMPONENT_TRANSFORM) < 0)
        {
            continue;    // No component found, go to next entity
        }

        if(entity_find_component(entity, (EntityComponent **)&controller, ENTITY_COMPONENT_PLAYER_CONTROLLER) < 0)
        {
            continue;    // No component found, go to next entity
        }

        if(controller->inputs.RIGHT)
        {
            transform->x++;
        }

        if(controller->inputs.LEFT)
        {
            transform->x--;
        }

        if(controller->inputs.UP)
        {
            transform->y++;
        }

        if(controller->inputs.DOWN)
        {
            transform->y--;
        }
    }
}

void input_process(Entity *entities, uint8_t nentities)
{
    for(uint8_t idx = 0; idx < nentities; idx++)
    {
        EntityPlayerControllerComponent *controller;
        Entity                          *entity = &entities[idx];

        if(entity_find_component(entity, (EntityComponent **)&controller, ENTITY_COMPONENT_PLAYER_CONTROLLER) < 0)
        {
            continue;    // No component found, go to next entity
        }

        if(controller->device->type == DEVICE_GAMEPAD)
        {
            gamepad_update((Gamepad *)controller->device);
            gamepad_to_inputs((Gamepad *)controller->device, &controller->inputs);
        }
    }
}

void render_process(const SceneComponent *component, Entity *entities, uint8_t nentities)
{
    scene_component_clear(component);

    for(uint8_t idx = 0; idx < nentities; idx++)
    {
        EntityTransformComponent *transform;
        EntityIconComponent      *icon;
        Entity                   *entity = &entities[idx];

        if(entity_find_component(entity, (EntityComponent **)&transform, ENTITY_COMPONENT_TRANSFORM) < 0)
        {
            continue;    // No component found, go to next entity
        }

        if(entity_find_component(entity, (EntityComponent **)&icon, ENTITY_COMPONENT_ICON) < 0)
        {
            continue;    // No component found, go to next entity
        }

        scene_component_render(component, transform->x, transform->y, icon->symbol);
    }
}
