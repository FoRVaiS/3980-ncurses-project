#include "entity-component.h"
#include "entity.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int retval;

    Entity player;

    if(entity_create_player(&player, 2, 3, '!') == -1)
    {
        retval = EXIT_FAILURE;
        goto exit;
    }

    printf("Ent Type: %d\n", (int)player.type);
    for(uint8_t comp_id = 0; comp_id < player.ncomponents; comp_id++)
    {
        EntityComponent *component = player.components[comp_id];
        uint8_t          comp_type = component->type;

        switch(comp_type)
        {
            case ENTITY_COMPONENT_TRANSFORM:
            {
                const EntityTransformComponent *transform;

                transform = (EntityTransformComponent *)component;
                printf("X: %d, Y: %d\n", transform->x, transform->y);
                break;
            }
            case ENTITY_COMPONENT_ICON:
            {
                const EntityIconComponent *icon;

                icon = (EntityIconComponent *)component;
                printf("Symbol: %c\n", icon->symbol);
                break;
            }
            default:
            {
            }
        }
    }

    entity_destroy(&player);
    retval = EXIT_SUCCESS;

exit:
    return retval;
}
