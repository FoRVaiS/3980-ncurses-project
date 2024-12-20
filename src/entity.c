#include "entity.h"
#include "entity-component.h"
#include "input.h"
#include <stdlib.h>
#include <string.h>

int entity_create_player(Entity *entity, uint8_t x, uint8_t y, uint16_t symbol, Device *device)
{
    int retval;

    EntityComponent *transform  = entity_component_transform_create(x, y);
    EntityComponent *icon       = entity_component_icon_create(symbol);
    EntityComponent *controller = entity_component_player_controller_create(device);

    memset(entity, 0, sizeof(Entity));
    entity->type = ENTITY_TYPE_PLAYER;

    if(entity_add_component(entity, transform) == -2)
    {
        retval = -1;
        goto cleanup;
    }

    if(entity_add_component(entity, icon) == -2)
    {
        retval = -1;
        goto cleanup;
    }

    if(entity_add_component(entity, controller) == -2)
    {
        retval = -1;
        goto cleanup;
    }

    retval = 0;
    goto exit;

cleanup:
    free(transform);
    free(icon);
    free(controller);
exit:
    return retval;
}

void entity_destroy(Entity *entity)
{
    for(uint8_t comp_id = 0; comp_id < entity->ncomponents; comp_id++)
    {
        free(entity->components[comp_id]);
    }
}

int entity_add_component(Entity *entity, EntityComponent *component)
{
    if(component == NULL)
    {
        return -1;
    }

    if(entity->ncomponents < ENTITY_MAX_COMPONENTS)
    {
        entity->components[entity->ncomponents] = component;
        entity->ncomponents++;
        return 0;
    }

    return -2;
}

int entity_find_component(Entity *entity, EntityComponent **component, EntityComponentType type)
{
    for(uint8_t component_idx = 0; component_idx < entity->ncomponents; component_idx++)
    {
        if(entity->components[component_idx]->type == type)
        {
            *component = entity->components[component_idx];
            return 0;
        }
    }

    return -1;
}
