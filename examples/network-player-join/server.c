#include "networking.h"
#include "packet.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

#define PORT 3000

static void on_packet(Server *server, Client *client, const uint8_t *packet);

int main(void)
{
    int retval;
    int err;

    Server server;

    // Server Details
    char      addr[] = "0.0.0.0";
    in_port_t port   = PORT;

    printf("Server started on %s:%d\n", addr, port);
    memset(&server, 0, sizeof(Server));

    err = 0;
    server_init(&server, addr, port, &err);
    if(err < 0)
    {
        retval = EXIT_FAILURE;
        goto exit;
    }

    while(1)
    {
        server_read_packet(&server, on_packet, &err);
    }

    retval = EXIT_SUCCESS;

exit:
    return retval;
}

static void on_packet(Server *server, Client *client, const uint8_t *packet)
{
    int err;

    PacketHeader header;
    size_t       offset;

    uint8_t player_id;

    offset = 0;
    deserialize_header(&header, packet, &offset);

    player_id = 0;
    if(server_find_client_by_address(server, &client->conn.addr, &player_id) == 1)
    {
        if(header.payload_type == PAYLOAD_PLAYER_STATE)
        {
            PlayerStatePacket player_state_packet;
            deserialize_player_state(&player_state_packet, packet);

            printf("PACKET[%zu bytes/%ld]: Player (%d) is %s\n", header.payload_size + sizeof(PacketHeader), (long)header.packet_timestamp, player_state_packet.player_id, player_state_packet.state == 0 ? "leaving..." : "joining!");
        }

        if(header.payload_type == PAYLOAD_COMPONENT)
        {
            EntityTransformComponentPacket ent_transform_packet;
            deserialize_entity_transform_component(&ent_transform_packet, packet);

            printf("PACKET[%zu bytes/%ld]: Player (%d) is at (%d,%d)\n",
                   header.payload_size + sizeof(PacketHeader),
                   (long)header.packet_timestamp,
                   ent_transform_packet.component_header.player_id,
                   ent_transform_packet.component.x,
                   ent_transform_packet.component.y);

            server_send_packet_all_except(server, packet, player_id, &err);
        }
    }
    else
    {
        if(header.payload_type == PAYLOAD_CONNECT)
        {
            uint8_t          *bytes_player_state;    // player_state
            PacketHeader      packet_header;
            PlayerStatePacket player_state_packet;

            printf("PACKET[%zu bytes/%ld]: Connection request by %s:%d\n", header.payload_size + sizeof(PacketHeader), (long)header.packet_timestamp, inet_ntoa(client->conn.addr.sin_addr), client->conn.addr.sin_port);

            // Add client to server's playerlist
            server_add_player(server, &client->conn);

            // Send a player_join packet to all clients
            packet_create_player_state(&player_state_packet, &packet_header, client->conn.packet_id, ((uint8_t)(server->nclients - 1)), 1);
            serialize_player_state(&bytes_player_state, &player_state_packet);
            connection_update_packet_id(&client->conn, &packet_header);
            server_send_packet_all(server, bytes_player_state, &err);
            free(bytes_player_state);
        }
    }
}
