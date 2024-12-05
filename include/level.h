// cppcheck-suppress-file unusedStructMember

#ifndef LEVEL_H
#define LEVEL_H

#include "entity.h"

#define LEVEL_MAX_ENTITIES 256

typedef struct
{
    Entity  entities[LEVEL_MAX_ENTITIES];
    uint8_t nentities;
} Level;

void level_init(Level *level);
int  level_add_entity(Level *level, const Entity *entity);

#endif
