// cppcheck-suppress-file unusedStructMember

#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL_gamecontroller.h>
#include <stdint.h>

#define SDL_NAXES 6
#define SDL_NBUTTONS 21

typedef enum
{
    DEVICE_GAMEPAD = 0,
    DEVICE_KEYBOARD,
} DeviceTypes;

typedef struct
{
    DeviceTypes type;
    long long   _alignment_;    // Alignment fix
} Device;

typedef struct
{
    uint8_t UP;
    uint8_t LEFT;
    uint8_t DOWN;
    uint8_t RIGHT;
} Inputs;

typedef struct
{
    Device              device;
    SDL_GameController *sdl_controller;
    short               axes[SDL_NAXES];
    uint8_t             buttons[SDL_NBUTTONS];
} Gamepad;

Device *get_input_device(void);

int  gamepad_init(Gamepad *gamepad);
void gamepad_destroy(Gamepad *gamepad);
void gamepad_update(Gamepad *gamepad);
void gamepad_to_inputs(Gamepad *gamepad, Inputs *inputs);

#endif
