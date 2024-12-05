#include "game.h"
#include "networking.h"
#include "utils.h"
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define TICKS 64

typedef struct
{
    Server   *server;
    char     *addr;
    in_port_t port;
} thread_args_t;

typedef struct
{
    char     *addr;
    in_port_t port;
    int       host;
    int       connect;
} Arguments;

static int  get_arguments(Arguments *args, int argc, char *argv[], int *err);
static void validate_arguments(const Arguments *args);

static void  create_game_thread(pthread_t *thread, thread_args_t *targs);
static void  create_server_thread(pthread_t *thread, thread_args_t *targs);
static void *start_server(void *targs);
static void *start_game(void *targs);
static void  on_packet(Server *server, Client *client, const uint8_t *packet);

int main(int argc, char *argv[])
{
    int err;

    Arguments args;

    thread_args_t targs;
    pthread_t     game_thread   = 0;
    pthread_t     server_thread = 0;

    Server server;

    memset(&args, 0, sizeof(Arguments));

    err = 0;
    get_arguments(&args, argc, argv, &err);
    validate_arguments(&args);

    memset(&targs, 0, sizeof(thread_args_t));
    targs.addr   = args.addr;
    targs.port   = args.port;
    targs.server = &server;

    create_game_thread(&game_thread, &targs);
    if(args.host)
    {
        create_server_thread(&server_thread, &targs);
    }

    pthread_join(game_thread, NULL);
    if(args.host)
    {
        pthread_join(server_thread, NULL);
    }

    return EXIT_SUCCESS;
}

static int get_arguments(Arguments *args, int argc, char *argv[], int *err)
{
    int retval;
    int opt;

    *err = 0;
    while((opt = getopt(argc, argv, "a:p:hc")) != -1)
    {
        switch(opt)
        {
            case 'a':
                args->addr = optarg;
                break;
            case 'p':
                args->port = convert_port(optarg, err);
                break;
            case 'h':
                args->host = 1;
                break;
            case 'c':
                args->connect = 1;
                break;
            default:
                retval = -1;
                goto exit;
        }
    }

    retval = 0;
exit:
    return retval;
}

static void validate_arguments(const Arguments *args)
{
    if(args->addr == NULL)
    {
        fprintf(stderr, "You must provide an address with -a\n");
        exit(EXIT_FAILURE);
    }

    if(args->port == 0)
    {
        fprintf(stderr, "You must provide a port with -p\n");
        exit(EXIT_FAILURE);
    }

    if(args->host == 0 && args->connect == 0)
    {
        fprintf(stderr, "You must choose one of: -h, -c\n");
        exit(EXIT_FAILURE);
    }

    if(args->host == 1 && args->connect == 1)
    {
        fprintf(stderr, "-h and -c are mutually exclusive, pick one op the other.\n");
        exit(EXIT_FAILURE);
    }
}

static void create_game_thread(pthread_t *thread, thread_args_t *targs)
{
    pthread_create(thread, NULL, start_game, targs);
}

static void create_server_thread(pthread_t *thread, thread_args_t *targs)
{
    int err;

    memset(targs->server, 0, sizeof(Server));

    server_init(targs->server, targs->addr, targs->port, &err);
    if(err < 0)
    {
        errno = err;
        perror("server_init");
    }

    pthread_create(thread, NULL, start_server, targs);
}

static void *start_game(void *targs)
{
    int err;

    thread_args_t *args = (thread_args_t *)targs;
    Game          *game;
    Client         client;

    err = 0;
    client_init(&client, args->addr, args->port, &err);

    game = game_init(&client, TICKS, &err);
    game_start(game);
    game_destroy(game);

    return 0;
}

static void *start_server(void *targs)
{
    thread_args_t *args = (thread_args_t *)targs;

    int err;

    while(1)
    {
        server_read_packet(args->server, &on_packet, &err);
    }

    return NULL;
}

static void on_packet(Server *server, Client *client, const uint8_t *packet)
{
    int     err;
    uint8_t client_id = 0;

    PacketHeader header;
    size_t       offset;

    offset = 0;
    deserialize_header(&header, packet, &offset);

    // Add client if not exists
    if(server_find_client_by_address(server, &client->conn.addr, &client_id) == 0)
    {
        server_add_player(server, &client->conn);
        server_find_client_by_address(server, &client->conn.addr, &client_id);
    }

    // Forward to all clients except this addr
    err = 0;
    server_send_packet_all_except(server, packet, client_id, &err);
    if(err < 0)
    {
        errno = err;
        perror("send_packet_except");
    }
}
