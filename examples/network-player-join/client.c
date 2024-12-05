#include "networking.h"
#include "packet.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#define PORT 3000

static void on_packet(Client *client, const uint8_t *packet);

int main(void)
{
    int retval;
    int err;

    char      addr[] = "0.0.0.0";
    in_port_t port   = PORT;

    Client client;

    uint8_t      *bytes_connect;    // connect
    PacketHeader  packet_header;
    ConnectPacket connect_packet;

    err = 0;
    if(client_init(&client, addr, port, &err) < 0)
    {
        errno = err;
        perror("client_init");
        retval = EXIT_FAILURE;
        goto exit;
    }

    // Create connect packet
    packet_create_connect(&connect_packet, &packet_header, client.conn.packet_id);
    serialize_connect(&bytes_connect, &connect_packet);
    connection_update_packet_id(&client.conn, &packet_header);

    err = 0;
    if(client_send_packet(&client, bytes_connect, &err) < 0)
    {
        errno = err;
        perror("client_send_packet");
        retval = EXIT_FAILURE;
        goto cleanup;
    }

    // Read responses from server
    while(1)
    {
        err = 0;
        if(client_read_packet(&client, on_packet, &err) < 0)
        {
            errno = err;
            perror("client_read_packet");
            retval = EXIT_FAILURE;
            goto cleanup;
        }

        if(client.player_id != SERVER_MAX_CLIENTS)
        {
            uint8_t                       *bytes_position;
            EntityComponentPacketHeader    component_header;
            EntityTransformComponentPacket etc_packet;

            EntityTransformComponent transform_component = {1, 2, 3};

            packet_create_entity_transform_component(&etc_packet, &packet_header, &component_header, client.conn.packet_id, client.player_id, &transform_component);
            serialize_entity_transform_component(&bytes_position, &etc_packet);
            connection_update_packet_id(&client.conn, &packet_header);

            err = 0;
            if(client_send_packet(&client, bytes_position, &err) < 0)
            {
                perror("client_send_packet");
            };

            free(bytes_position);

            sleep(1);
        }
    }

    retval = EXIT_SUCCESS;

cleanup:
    free(bytes_connect);
exit:
    return retval;
}

static void on_packet(Client *client, const uint8_t *packet)
{
    PacketHeader header;
    size_t       offset;

    offset = 0;
    deserialize_header(&header, packet, &offset);

    // printf("Packet Id: %d | Payload Type: %d\n", header.packet_id, header.payload_type);

    if(header.payload_type == PAYLOAD_PLAYER_STATE)
    {
        PlayerStatePacket player_state_packet;
        deserialize_player_state(&player_state_packet, packet);

        printf("PACKET[%zu bytes/%ld]: Player (%d) is %s\n", header.payload_size + sizeof(PacketHeader), (long)header.packet_timestamp, player_state_packet.player_id, player_state_packet.state == 0 ? "leaving..." : "joining!");

        if(client->player_id == SERVER_MAX_CLIENTS)
        {
            client->player_id = player_state_packet.player_id;
        }
    }
}
