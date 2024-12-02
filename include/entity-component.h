#ifndef ENTITY_COMPONENT_H
#define ENTITY_COMPONENT_H

#include <stdint.h>

typedef enum
{
    ENTITY_COMPONENT_TRANSFORM = 0,
    ENTITY_COMPONENT_ICON,
} EntityComponentType;

typedef struct
{
    uint8_t  type;
    uint16_t _alignment_;    // Alignment fix for EntityIconComponent (16-bits)
} EntityComponent;

typedef struct
{
    EntityComponent component;
    uint8_t         x;
    uint8_t         y;
} EntityTransformComponent;

typedef struct
{
    EntityComponent component;
    uint16_t        symbol;
} EntityIconComponent;

EntityComponent *entity_component_transform_create(uint8_t x, uint8_t y);
EntityComponent *entity_component_icon_create(uint16_t symbol);

#endif
