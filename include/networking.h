#ifndef NETWORKING_H
#define NETWORKING_H

#include "packet.h"
#include <arpa/inet.h>
#include <stdint.h>

#define SERVER_MAX_CLIENTS 0xFF  /* 256 */
#define CLIENT_MAX_ENTITIES 0xFF /* 256 */

typedef struct
{
    int                sockfd;
    struct sockaddr_in addr;
    socklen_t          addr_len;

    uint8_t   packet_id;
    long long last_packet_timestamp;
} Connection;

typedef struct
{
    Connection conn;

    PacketHeader history[PAYLOAD_TYPE_LEN];
    uint8_t      entity_id;
} Client;

typedef struct
{
    Connection conn;

    Client  clients[SERVER_MAX_CLIENTS];
    uint8_t nclients;
} Server;

typedef void (*ServerPacketHandlerFn)(Server *server, Client *client, const uint8_t *packet);

int  server_init(Server *server, char *addr, in_port_t port, int *err);
void server_destroy(Server *server);
int  server_read_packet(Server *server, ServerPacketHandlerFn on_packet, int *err);
void server_send_packet(Server *server, const uint8_t *packet, uint8_t client_id, int *err);
void server_send_packet_all(Server *server, const uint8_t *packet, int *err);
void server_send_packet_all_except(Server *server, const uint8_t *packet, uint8_t client_id, int *err);
int  server_find_client_by_address(Server *server, const struct sockaddr_in *addr, uint8_t *client_idx);
void server_add_player(Server *server, const Connection *conn);

int     client_init(Client *client, char *addr, in_port_t port, int *err);
void    client_destroy(Client *client);
int     client_read_packet(Client *client, uint8_t **packet, int *err);
ssize_t client_send_packet(Client *client, const uint8_t *packet, int *err);

int  connection_init(Connection *conn, char *addr, in_port_t port, int *err);
void connection_destroy(Connection *conn);
void connection_update_packet_id(Connection *conn, const PacketHeader *header);

ssize_t read_packet(int sockfd, uint8_t **packet, struct sockaddr_in *addr, socklen_t *addrlen, int *err);
ssize_t send_packet(int sockfd, const uint8_t *packet, const struct sockaddr_in *addr, const socklen_t *addrlen, int *err);

#endif
