// cppcheck-suppress-file unusedStructMember

#ifndef PACKET_H
#define PACKET_H

#include "entity-component.h"
#include <stdint.h>
#include <stdlib.h>

#define MTU 1500

/**
 *  Only components that are required to be shared
 *  with other clients should be included.
 */
typedef enum
{
    PAYLOAD_CONNECT = 0,
    PAYLOAD_PLAYER_STATE,
    PAYLOAD_COMPONENT,
    PAYLOAD_TYPE_LEN,
} PayloadType;

// HEADERS
typedef struct
{
    uint8_t  packet_id;
    uint8_t  payload_type;
    uint16_t payload_size;
    uint32_t packet_timestamp;
} PacketHeader;

typedef struct
{
    uint8_t  component_type;
    uint16_t component_size;
} EntityComponentPacketHeader;

// Packets
typedef struct
{
    PacketHeader packet_header;
} ConnectPacket;

typedef struct
{
    PacketHeader packet_header;
    uint8_t      state;
} PlayerStatePacket;

typedef struct
{
    PacketHeader                packet_header;
    EntityComponentPacketHeader component_header;
    EntityTransformComponent    component;
} EntityTransformComponentPacket;

void packet_get_header(PacketHeader *header, const uint8_t *bytes);
void packet_get_entity_component_header(EntityComponentPacketHeader *header, const uint8_t *bytes);

void packet_create_header(PacketHeader *header, uint8_t packet_id, uint8_t payload_type, uint16_t payload_size, uint32_t packet_timestamp);
void packet_create_entity_component_header(EntityComponentPacketHeader *header, uint8_t component_type, uint16_t component_size);

void packet_create_connect(ConnectPacket *packet, PacketHeader *packet_header, uint8_t packet_id);
void packet_create_player_state(PlayerStatePacket *packet, PacketHeader *packet_header, uint8_t packet_id, uint8_t state);
void packet_create_entity_transform_component(EntityTransformComponentPacket *packet, PacketHeader *packet_header, EntityComponentPacketHeader *component_header, uint8_t packet_id, const EntityTransformComponent *component);

int packet_validate_order(const PacketHeader *newheader, const PacketHeader *oldheader);

// Serialize
void serialize_header(uint8_t *bytes, size_t *offset, const PacketHeader *packet_header);
void serialize_entity_component_header(uint8_t *bytes, size_t *offset, const EntityComponentPacketHeader *component_header);

int serialize_connect(uint8_t **bytes, ConnectPacket *packet);
int serialize_player_state(uint8_t **bytes, PlayerStatePacket *packet);
int serialize_entity_transform_component(uint8_t **bytes, EntityTransformComponentPacket *packet);

// Deserialize
void deserialize_header(PacketHeader *header, const uint8_t *bytes, size_t *offset);
void deserialize_entity_component_header(EntityComponentPacketHeader *header, const uint8_t *bytes, size_t *offset);

void deserialize_connect(ConnectPacket *packet, const uint8_t *bytes);
void deserialize_player_state(PlayerStatePacket *packet, const uint8_t *bytes);
void deserialize_entity_transform_component(EntityTransformComponentPacket *packet, const uint8_t *bytes);

// Utils
void serialize_1_byte(uint8_t *bytes, size_t *offset, uint8_t data);
void serialize_2_bytes(uint8_t *bytes, size_t *offset, uint16_t data);
void serialize_4_bytes(uint8_t *bytes, size_t *offset, uint32_t data);

uint8_t  deserialize_1_byte(const uint8_t *bytes, size_t *offset);
uint16_t deserialize_2_bytes(const uint8_t *bytes, size_t *offset);
uint32_t deserialize_4_bytes(const uint8_t *bytes, size_t *offset);

#endif
