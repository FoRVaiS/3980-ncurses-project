#include "game.h"
#include <stdlib.h>
#include <unistd.h>

#define TICKS 64

int main(void)
{
    int err;

    Game *game;

    game = game_init(TICKS, &err);
    game_start(game);
    game_destroy(game);

    return EXIT_SUCCESS;
}
