#include "networking.h"
#include "packet.h"
#include "utils.h"
#include <arpa/inet.h>
#include <errno.h>
#include <memory.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int server_init(Server *server, char *addr, in_port_t port, int *err)
{
    int retval;

    *err   = 0;
    retval = connection_init(&server->conn, addr, port, err);
    if(retval < 0)
    {
        retval = -1;
        goto exit;
    }

    // Bind the socket
    errno = 0;
    if(bind(server->conn.sockfd, &server->conn.addr, server->conn.addr_len) < 0)
    {
        *err   = errno;
        retval = -2;
        goto cleanup;
    }

    retval = 0;
    goto exit;

cleanup:
    server_destroy(server);
exit:
    return retval;
}

void server_destroy(Server *server)
{
    connection_destroy(&server->conn);
    memset(server, 0, sizeof(Server));
}

void server_send_packet(Server *server, const uint8_t *packet, uint8_t client_id, int *err)
{
    Client *client = &server->clients[client_id];

    send_packet(server->conn.sockfd, packet, &client->conn.addr, &client->conn.addr_len, err);
}

void server_send_packet_all(Server *server, const uint8_t *packet, int *err)
{
    for(size_t client_id = 0; client_id < server->nclients; client_id++)
    {
        server_send_packet(server, packet, (uint8_t)client_id, err);
    }
}

void server_send_packet_all_except(Server *server, const uint8_t *packet, uint8_t client_id, int *err)
{
    for(uint8_t _client_id = 0; _client_id < server->nclients; _client_id++)
    {
        if(_client_id != client_id)
        {
            server_send_packet(server, packet, _client_id, err);
        }
    }
}

int server_read_packet(Server *server, ServerPacketHandlerFn on_packet, int *err)
{
    int retval;
    int ready;

    struct pollfd fds[1];

    *err = 0;
    if(server->conn.sockfd < 0)
    {
        *err   = EBADF;
        retval = -1;
        goto exit;
    }

    fds[0].fd     = server->conn.sockfd;
    fds[0].events = POLLIN;

    ready = poll(fds, 1, 0);

    if(fds[0].revents & POLLIN && ready >= 0)
    {
        size_t offset;

        PacketHeader header;
        uint8_t     *packet;

        uint8_t client_idx;
        Client  client;
        client.conn.addr_len = sizeof(struct sockaddr_in);

        // Get packet
        *err = 0;
        if(read_packet(server->conn.sockfd, &packet, &client.conn.addr, &client.conn.addr_len, err) < 0)
        {
            retval = -3;
            goto exit;
        }

        // Get header information to handle CONNECT packets
        offset = 0;
        deserialize_header(&header, packet, &offset);

        // Update packet history for existing clients
        client_idx = 0;
        if(server_find_client_by_address(server, &client.conn.addr, &client_idx) > 0)
        {
            PacketHeader storedHeader = client.history[header.payload_type];
            client                    = server->clients[client_idx];

            // Drop if packet is older than the last record packet
            if(packet_validate_order(&header, &storedHeader) < 0)
            {
                retval = 0;    // Not an error, just another possible state for this function
                goto exit;
            }

            // Update packet history for client
            memcpy(&storedHeader, &header, sizeof(PacketHeader));
        }

        // Process the packet
        on_packet(server, &client, packet);
    }

    retval = 0;

exit:
    return retval;
}

int server_find_client_by_address(Server *server, const struct sockaddr_in *addr, uint8_t *client_idx)
{
    const char *client_addr;
    in_port_t   client_port;

    client_addr = inet_ntoa(addr->sin_addr);
    client_port = addr->sin_port;

    for(uint8_t idx = 0; idx < server->nclients; idx++)
    {
        const Client *client = &server->clients[idx];
        if(inet_ntoa(client->conn.addr.sin_addr) == client_addr && client->conn.addr.sin_port == client_port)
        {
            *client_idx = idx;
            return 1;
        }
    }

    return 0;
}

void server_add_player(Server *server, const Connection *conn)
{
    Client client;
    memset(&client, 0, sizeof(Client));
    client.conn = *conn;

    memcpy(&server->clients[server->nclients], &client, sizeof(Client));
    server->nclients++;
}

int client_init(Client *client, char *addr, in_port_t port, int *err)
{
    memset(client, 0, sizeof(Client));

    *err = 0;
    if(connection_init(&client->conn, addr, port, err) < 0)
    {
        return -1;
    }

    return 0;
}

void client_destroy(Client *client)
{
    connection_destroy(&client->conn);
}

int client_read_packet(Client *client, uint8_t **packet, int *err)
{
    int retval;
    int ready;

    struct pollfd fds[1];

    *err = 0;
    if(client->conn.sockfd < 0)
    {
        *err   = EBADF;
        retval = -1;
        goto exit;
    }

    fds[0].fd     = client->conn.sockfd;
    fds[0].events = POLLIN;

    ready = poll(fds, 1, 0);

    if(fds[0].revents & POLLIN && ready >= 0)
    {
        size_t offset;

        PacketHeader header;
        PacketHeader storedHeader;

        // Get packet
        *err = 0;
        if(read_packet(client->conn.sockfd, packet, &client->conn.addr, &client->conn.addr_len, err) < 0)
        {
            retval = -2;
            goto exit;
        }

        // Get header information to handle CONNECT packets
        offset = 0;
        deserialize_header(&header, *packet, &offset);

        // Drop if packet is older than the last record packet
        storedHeader = client->history[header.payload_type];
        if(packet_validate_order(&header, &storedHeader) < 0)
        {
            retval = 0;
            goto exit;
        }

        // Update packet history for client
        memcpy(&storedHeader, &header, sizeof(PacketHeader));

        retval = 1;
        goto exit;
    }

    retval = 0;

exit:
    return retval;
}

ssize_t client_send_packet(Client *client, const uint8_t *packet, int *err)
{
    return send_packet(client->conn.sockfd, packet, &client->conn.addr, &client->conn.addr_len, err);
}

int connection_init(Connection *conn, char *addr, in_port_t port, int *err)
{
    int retval;

    errno        = 0;
    conn->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(errno < 0)
    {
        *err   = errno;
        retval = -1;
        goto exit;
    }

    errno = 0;
    if(inet_pton(AF_INET, addr, &(conn->addr.sin_addr)) <= 0)
    {
        *err   = errno;
        retval = -2;
        goto cleanup;
    }

    conn->addr.sin_family = AF_INET;
    conn->addr.sin_port   = htons(port);
    conn->addr_len        = sizeof(struct sockaddr_in);

    conn->packet_id             = 0;
    conn->last_packet_timestamp = get_time_ms();

    retval = 0;
    goto exit;

cleanup:
    connection_destroy(conn);
exit:
    return retval;
}

void connection_destroy(Connection *conn)
{
    close(conn->sockfd);
    conn->sockfd = -1;
}

void connection_update_packet_id(Connection *conn, const PacketHeader *header)
{
    if(header->packet_timestamp > conn->last_packet_timestamp)
    {
        // Time has passed since the last packet
        conn->last_packet_timestamp = header->packet_timestamp;
        conn->packet_id             = 0;
    }

    conn->packet_id++;
}

ssize_t read_packet(int sockfd, uint8_t **packet, struct sockaddr_in *addr, socklen_t *addrlen, int *err)
{
    ssize_t retval;
    ssize_t readval;

    errno   = 0;
    *packet = (uint8_t *)calloc(MTU, sizeof(uint8_t));
    if(*packet == NULL)
    {
        *err   = errno;
        retval = -1;
        goto exit;
    }

    errno   = 0;
    readval = recvfrom(sockfd, *packet, MTU, MSG_DONTWAIT, addr, addrlen);
    if(readval < 0)
    {
        *err   = errno;
        retval = -2;
        goto cleanup;
    }

    retval = readval;
    goto exit;

cleanup:
    free(*packet);
exit:
    return retval;
}

ssize_t send_packet(int sockfd, const uint8_t *packet, const struct sockaddr_in *addr, const socklen_t *addrlen, int *err)
{
    ssize_t retval;
    ssize_t nwritten;

    PacketHeader header;

    size_t packet_len;

    memset(&header, 0, sizeof(PacketHeader));
    packet_get_header(&header, packet);

    packet_len = sizeof(PacketHeader) + header.payload_size;

    errno    = 0;
    nwritten = sendto(sockfd, packet, packet_len, MSG_CONFIRM, addr, *addrlen);
    if(nwritten < 0)
    {
        *err   = errno;
        retval = -1;
        goto exit;
    }

    retval = 0;

exit:
    return retval;
}
