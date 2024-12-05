// cppcheck-suppress-file unusedStructMember

#ifndef ENTITY_COMPONENT_H
#define ENTITY_COMPONENT_H

#include "input.h"
#include <stdint.h>

typedef enum
{
    ENTITY_COMPONENT_TRANSFORM = 0,
    ENTITY_COMPONENT_ICON,
    ENTITY_COMPONENT_PLAYER_CONTROLLER,
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

typedef struct
{
    EntityComponent component;
    Inputs          inputs;
    Device         *device;
} EntityPlayerControllerComponent;

EntityComponent *entity_component_transform_create(uint8_t x, uint8_t y);
EntityComponent *entity_component_icon_create(uint16_t symbol);
EntityComponent *entity_component_player_controller_create(Device *device);

#endif
