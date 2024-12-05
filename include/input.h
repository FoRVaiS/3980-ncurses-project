// cppcheck-suppress-file unusedStructMember

#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL_gamecontroller.h>
#include <stdint.h>

#define SDL_NAXES 6
#define SDL_NBUTTONS 21
#define PRINTABLE_ASCII_OFFSET 32

typedef enum
{
    DEVICE_GAMEPAD = 0,
    DEVICE_KEYBOARD,
} DeviceTypes;

typedef enum
{
    KB_KEY_SPACE = 0,
    KB_KEY_EXCLAMATION,
    KB_KEY_DOUBLE_QUOTE,
    KB_KEY_POUND_SIGN,
    KB_KEY_DOLLAR,
    KB_KEY_PERCENT,
    KB_KEY_AMPERSAND,
    KB_KEY_SINGLE_QUOTE,
    KB_KEY_OPEN_PARENTHESIS,
    KB_KEY_CLOSE_PARENTHESIS,
    KB_KEY_ASTERISK,
    KB_KEY_PLUS,
    KB_KEY_COMMA,
    KB_KEY_HYPHEN,
    KB_KEY_PERIOD,
    KB_KEY_SLASH,
    KB_KEY_ZERO,
    KB_KEY_ONE,
    KB_KEY_TWO,
    KB_KEY_THREE,
    KB_KEY_FOUR,
    KB_KEY_FIVE,
    KB_KEY_SIX,
    KB_KEY_SEVEN,
    KB_KEY_EIGHT,
    KB_KEY_NINE,
    KB_KEY_COLON,
    KB_KEY_SEMICOLON,
    KB_KEY_LESS_THAN,
    KB_KEY_EQUALS,
    KB_KEY_GREATER_THAN,
    KB_KEY_QUESTION_MARK,
    KB_KEY_AT_SIGN,
    KB_KEY_UPPER_A,
    KB_KEY_UPPER_B,
    KB_KEY_UPPER_C,
    KB_KEY_UPPER_D,
    KB_KEY_UPPER_E,
    KB_KEY_UPPER_F,
    KB_KEY_UPPER_G,
    KB_KEY_UPPER_H,
    KB_KEY_UPPER_I,
    KB_KEY_UPPER_J,
    KB_KEY_UPPER_K,
    KB_KEY_UPPER_L,
    KB_KEY_UPPER_M,
    KB_KEY_UPPER_N,
    KB_KEY_UPPER_O,
    KB_KEY_UPPER_P,
    KB_KEY_UPPER_Q,
    KB_KEY_UPPER_R,
    KB_KEY_UPPER_S,
    KB_KEY_UPPER_T,
    KB_KEY_UPPER_U,
    KB_KEY_UPPER_V,
    KB_KEY_UPPER_W,
    KB_KEY_UPPER_X,
    KB_KEY_UPPER_Y,
    KB_KEY_UPPER_Z,
    KB_KEY_OPEN_BRACKET,
    KB_KEY_BACKSLASH,
    KB_KEY_CLOSE_BRACKET,
    KB_KEY_CARET,
    KB_KEY_UNDERSCORE,
    KB_KEY_GRAVE,
    KB_KEY_A,
    KB_KEY_B,
    KB_KEY_C,
    KB_KEY_D,
    KB_KEY_E,
    KB_KEY_F,
    KB_KEY_G,
    KB_KEY_H,
    KB_KEY_I,
    KB_KEY_J,
    KB_KEY_K,
    KB_KEY_L,
    KB_KEY_M,
    KB_KEY_N,
    KB_KEY_O,
    KB_KEY_P,
    KB_KEY_Q,
    KB_KEY_R,
    KB_KEY_S,
    KB_KEY_T,
    KB_KEY_U,
    KB_KEY_V,
    KB_KEY_W,
    KB_KEY_X,
    KB_KEY_Y,
    KB_KEY_Z,
    KB_KEY_OPEN_BRACE,
    KB_KEY_PIPE,
    KB_KEY_CLOSE_BRACE,
    KB_KEY_TILDE,
    KB_KEY_DELETE,
    KEYBOARD_KEYS_LEN,
} KeyboardKeys;

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

typedef struct
{
    Device  device;
    uint8_t keys[KEYBOARD_KEYS_LEN];
} Keyboard;

Device *get_input_device(void);

int  keyboard_init(Keyboard *keyboard);
void keyboard_update(Keyboard *keyboard);
void keyboard_to_inputs(Keyboard *keyboard, Inputs *inputs);

int  gamepad_init(Gamepad *gamepad);
void gamepad_destroy(Gamepad *gamepad);
void gamepad_update(Gamepad *gamepad);
void gamepad_to_inputs(Gamepad *gamepad, Inputs *inputs);

#endif
