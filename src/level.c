#include "level.h"
#include "entity.h"
#include <string.h>

void level_init(Level *level)
{
    memset(level, 0, sizeof(Level));
}

int level_add_entity(Level *level, const Entity *entity)
{
    if(level->nentities < (LEVEL_MAX_ENTITIES - 1))
    {
        uint8_t ent_id = level->nentities;

        memcpy(&level->entities[ent_id], entity, sizeof(Entity));
        level->nentities++;

        return ent_id;
    }

    return -1;
}
