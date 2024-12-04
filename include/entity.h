#ifndef ENTITY_H
#define ENTITY_H

#include "entity-component.h"

#define ENTITY_MAX_COMPONENTS 16

typedef enum
{
    ENTITY_TYPE_PLAYER = 0,
} EntityType;

typedef struct
{
    EntityType type;

    EntityComponent *components[ENTITY_MAX_COMPONENTS];
    uint8_t          ncomponents;
} Entity;

int  entity_create_player(Entity *entity, uint8_t x, uint8_t y, uint16_t symbol);
void entity_destroy(Entity *entity);

int entity_add_component(Entity *entity, EntityComponent *component);
int entity_find_component(Entity *entity, EntityComponent **component, EntityComponentType type);

#endif
