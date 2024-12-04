#include "packet.h"
#include "entity-component.h"
#include "utils.h"
#include <arpa/inet.h>
#include <stdint.h>
#include <string.h>

void packet_get_header(PacketHeader *header, const uint8_t *bytes)
{
    size_t offset = 0;

    memcpy(&header->packet_id, &bytes[offset], sizeof(header->packet_id));
    offset += sizeof(header->packet_id);

    memcpy(&header->payload_type, &bytes[offset], sizeof(header->payload_type));
    offset += sizeof(header->payload_type);

    memcpy(&header->payload_size, &bytes[offset], sizeof(header->payload_size));
}

void packet_get_entity_component_header(EntityComponentPacketHeader *header, const uint8_t *bytes)
{
    size_t offset = 0;

    memcpy(&header->entity_id, &bytes[offset], sizeof(header->entity_id));
    offset += sizeof(header->entity_id);

    memcpy(&header->component_type, &bytes[offset], sizeof(header->component_type));
    offset += sizeof(header->component_type);

    memcpy(&header->component_size, &bytes[offset], sizeof(header->component_size));
}

void packet_create_header(PacketHeader *header, uint8_t packet_id, uint8_t payload_type, uint16_t payload_size, uint32_t packet_timestamp)
{
    header->packet_id        = packet_id;
    header->payload_type     = payload_type;
    header->payload_size     = payload_size;
    header->packet_timestamp = packet_timestamp;
}

void packet_create_entity_component_header(EntityComponentPacketHeader *header, uint8_t entity_id, uint8_t component_type, uint16_t component_size)
{
    header->entity_id      = entity_id;
    header->component_type = component_type;
    header->component_size = component_size;
}

void packet_create_connect(ConnectPacket *packet, PacketHeader *packet_header, uint8_t packet_id)
{
    packet_create_header(packet_header, packet_id, PAYLOAD_CONNECT, 0, (uint32_t)get_time_ms());
    memcpy(&packet->packet_header, packet_header, sizeof(packet->packet_header));
}

void packet_create_player_state(PlayerStatePacket *packet, PacketHeader *packet_header, uint8_t packet_id, uint8_t state)
{
    uint16_t payload_size = sizeof(PlayerStatePacket) - sizeof(PacketHeader);

    packet_create_header(packet_header, packet_id, PAYLOAD_PLAYER_STATE, payload_size, (uint32_t)get_time_ms());
    memcpy(&packet->packet_header, packet_header, sizeof(packet->packet_header));
    packet->state = state;
}

void packet_create_entity_transform_component(EntityTransformComponentPacket *packet, PacketHeader *packet_header, EntityComponentPacketHeader *component_header, uint8_t packet_id, uint8_t entity_id, const EntityTransformComponent *component)
{
    uint16_t payload_size   = sizeof(EntityTransformComponentPacket) - sizeof(PacketHeader);
    uint16_t component_size = sizeof(EntityTransformComponentPacket) - sizeof(PacketHeader) - sizeof(EntityComponentPacketHeader);

    packet_create_header(packet_header, packet_id, PAYLOAD_COMPONENT, payload_size, (uint32_t)get_time_ms());
    packet_create_entity_component_header(component_header, entity_id, ENTITY_COMPONENT_TRANSFORM, component_size);
    memcpy(&packet->packet_header, packet_header, sizeof(packet->packet_header));
    memcpy(&packet->component_header, component_header, sizeof(packet->component_header));
    memcpy(&packet->component, component, sizeof(packet->component));
}

int packet_validate_order(const PacketHeader *newheader, const PacketHeader *oldheader)
{
    if(newheader->packet_timestamp > oldheader->packet_timestamp)
    {
        return 0;
    }

    if(newheader->packet_timestamp < oldheader->packet_timestamp)
    {
        return -1;
    }

    if(newheader->packet_id > oldheader->packet_id)
    {
        return 0;
    }

    if(newheader->packet_id < oldheader->packet_id)
    {
        return -1;
    }

    return -1;
}

// ===============
//  Serialization
// ===============
void serialize_header(uint8_t *bytes, size_t *offset, const PacketHeader *header)
{
    serialize_1_byte(bytes, offset, header->packet_id);
    serialize_1_byte(bytes, offset, header->payload_type);
    serialize_2_bytes(bytes, offset, header->payload_size);
    serialize_4_bytes(bytes, offset, header->packet_timestamp);
}

void serialize_entity_component_header(uint8_t *bytes, size_t *offset, const EntityComponentPacketHeader *header)
{
    serialize_1_byte(bytes, offset, header->entity_id);
    serialize_1_byte(bytes, offset, header->component_type);
    serialize_2_bytes(bytes, offset, header->component_size);
}

int serialize_connect(uint8_t **bytes, ConnectPacket *packet)
{
    int    retval;
    size_t offset;
    size_t nbytes = sizeof(ConnectPacket);

    *bytes = (uint8_t *)calloc(nbytes, sizeof(uint8_t));
    if(*bytes == NULL)
    {
        retval = -1;
        goto exit;
    }

    offset = 0;
    serialize_header(*bytes, &offset, &packet->packet_header);

    retval = 0;
exit:
    return retval;
}

int serialize_player_state(uint8_t **bytes, PlayerStatePacket *packet)
{
    int    retval;
    size_t offset;
    size_t nbytes = sizeof(PlayerStatePacket);

    *bytes = (uint8_t *)calloc(nbytes, sizeof(uint8_t));
    if(*bytes == NULL)
    {
        retval = -1;
        goto exit;
    }

    offset = 0;
    serialize_header(*bytes, &offset, &packet->packet_header);
    serialize_1_byte(*bytes, &offset, packet->state);

    retval = 0;
exit:
    return retval;
}

int serialize_entity_transform_component(uint8_t **bytes, EntityTransformComponentPacket *packet)
{
    int    retval;
    size_t offset;
    size_t nbytes = sizeof(EntityTransformComponentPacket);

    *bytes = (uint8_t *)calloc(nbytes, sizeof(uint8_t));
    if(*bytes == NULL)
    {
        retval = -1;
        goto exit;
    }

    offset = 0;
    serialize_header(*bytes, &offset, &packet->packet_header);
    serialize_entity_component_header(*bytes, &offset, &packet->component_header);
    serialize_1_byte(*bytes, &offset, packet->component.x);
    serialize_1_byte(*bytes, &offset, packet->component.y);

    retval = 0;
exit:
    return retval;
}

// =================
//  Deserialization
// =================
void deserialize_header(PacketHeader *header, const uint8_t *bytes, size_t *offset)
{
    uint8_t  packet_id;
    uint8_t  payload_type;
    uint16_t payload_size;
    uint32_t packet_timestamp;

    packet_id        = deserialize_1_byte(bytes, offset);
    payload_type     = deserialize_1_byte(bytes, offset);
    payload_size     = deserialize_2_bytes(bytes, offset);
    packet_timestamp = deserialize_4_bytes(bytes, offset);

    packet_create_header(header, packet_id, payload_type, payload_size, packet_timestamp);
}

void deserialize_entity_component_header(EntityComponentPacketHeader *header, const uint8_t *bytes, size_t *offset)
{
    uint8_t  entity_id;
    uint8_t  component_type;
    uint16_t component_size;

    entity_id      = deserialize_1_byte(bytes, offset);
    component_type = deserialize_1_byte(bytes, offset);
    component_size = deserialize_2_bytes(bytes, offset);

    packet_create_entity_component_header(header, entity_id, component_type, component_size);
}

void deserialize_connect(ConnectPacket *packet, const uint8_t *bytes)
{
    PacketHeader packet_header;

    size_t offset = 0;

    // Packet Header
    memset(&packet_header, 0, sizeof(PacketHeader));
    deserialize_header(&packet_header, bytes, &offset);
    memcpy(&packet->packet_header, &packet_header, sizeof(PacketHeader));
}

void deserialize_player_state(PlayerStatePacket *packet, const uint8_t *bytes)
{
    PacketHeader packet_header;

    size_t offset = 0;

    // Packet Header
    memset(&packet_header, 0, sizeof(PacketHeader));
    deserialize_header(&packet_header, bytes, &offset);
    memcpy(&packet->packet_header, &packet_header, sizeof(PacketHeader));

    // Player State
    packet->state = deserialize_1_byte(bytes, &offset);
}

void deserialize_entity_transform_component(EntityTransformComponentPacket *packet, const uint8_t *bytes)
{
    PacketHeader                packet_header;
    EntityComponentPacketHeader component_header;

    size_t offset = 0;

    // Packet Header
    memset(&packet_header, 0, sizeof(PacketHeader));
    deserialize_header(&packet_header, bytes, &offset);
    memcpy(&packet->packet_header, &packet_header, sizeof(PacketHeader));

    // Component Header
    memset(&component_header, 0, sizeof(EntityComponentPacketHeader));
    deserialize_entity_component_header(&component_header, bytes, &offset);
    memcpy(&packet->component_header, &component_header, sizeof(EntityComponentPacketHeader));

    // Transform Component
    packet->component.x = deserialize_1_byte(bytes, &offset);
    packet->component.y = deserialize_1_byte(bytes, &offset);
}

// =========================
//  Utilities/Serialization
// =========================
void serialize_1_byte(uint8_t *bytes, size_t *offset, uint8_t data)
{
    bytes[*offset] = data;
    *offset += sizeof(data);
}

void serialize_2_bytes(uint8_t *bytes, size_t *offset, uint16_t data)
{
    uint16_t network_order;

    network_order = htons(data);
    memcpy(&bytes[*offset], &network_order, sizeof(data));
    *offset += sizeof(data);
}

void serialize_4_bytes(uint8_t *bytes, size_t *offset, uint32_t data)
{
    uint32_t network_order;

    network_order = htonl(data);
    memcpy(&bytes[*offset], &network_order, sizeof(data));
    *offset += sizeof(data);
}

// ===========================
//  Utilities/Deserialization
// ===========================
uint8_t deserialize_1_byte(const uint8_t *bytes, size_t *offset)
{
    uint8_t ddata;    // deserialized data

    ddata = bytes[*offset];
    *offset += 1;

    return ddata;
}

uint16_t deserialize_2_bytes(const uint8_t *bytes, size_t *offset)
{
    uint16_t ddata;

    memcpy(&ddata, &bytes[*offset], sizeof(uint16_t));
    ddata = ntohs(ddata);
    *offset += sizeof(uint16_t);

    return ddata;
}

uint32_t deserialize_4_bytes(const uint8_t *bytes, size_t *offset)
{
    uint32_t ddata;

    memcpy(&ddata, &bytes[*offset], sizeof(uint32_t));
    ddata = ntohl(ddata);
    *offset += sizeof(uint32_t);

    return ddata;
}
