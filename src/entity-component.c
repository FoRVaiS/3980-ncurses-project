#include "entity-component.h"
#include <stdlib.h>
#include <string.h>

EntityComponent *entity_component_transform_create(uint8_t x, uint8_t y)
{
    EntityTransformComponent *transform = (EntityTransformComponent *)malloc(sizeof(EntityTransformComponent));
    if(transform == NULL)
    {
        return NULL;
    }

    memset(transform, 0, sizeof(EntityTransformComponent));

    transform->component.type = ENTITY_COMPONENT_TRANSFORM;
    transform->x              = x;
    transform->y              = y;

    return (EntityComponent *)transform;
}

EntityComponent *entity_component_icon_create(uint16_t symbol)
{
    EntityIconComponent *icon = (EntityIconComponent *)malloc(sizeof(EntityIconComponent));
    if(icon == NULL)
    {
        return NULL;
    }

    memset(icon, 0, sizeof(EntityIconComponent));

    icon->component.type = ENTITY_COMPONENT_ICON;
    icon->symbol         = symbol;

    return (EntityComponent *)icon;
}
