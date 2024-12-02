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
        memcpy(&level->entities[level->nentities], entity, sizeof(Entity));
        level->nentities++;
        return 0;
    }

    return -1;
}
