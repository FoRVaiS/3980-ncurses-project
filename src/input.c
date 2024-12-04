#include "input.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_gamecontroller.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define DEADZONE_THRESHOLD ((short)22938) /* 70% of (2^16 / 2) */

Device *get_input_device(void)
{
    Gamepad *gamepad = (Gamepad *)calloc(1, sizeof(Gamepad));
    if(gamepad == NULL)
    {
        return NULL;
    }

    if(gamepad_init(gamepad) >= 0)
    {
        return (Device *)gamepad;
    }

    free(gamepad);
    return NULL;
}

int gamepad_init(Gamepad *gamepad)
{
    struct sigaction action;

    int retval = 0;

    memset(gamepad, 0, sizeof(Gamepad));
    sigaction(SIGINT, NULL, &action);
    if(SDL_Init(SDL_INIT_GAMECONTROLLER) != 0)
    {
        retval = -1;
        goto exit;
    }
    sigaction(SIGINT, &action, NULL);

    if(SDL_NumJoysticks() <= 0)
    {
        retval = -2;
        goto sdl_cleanup;
    }

    gamepad->device.type    = DEVICE_GAMEPAD;
    gamepad->sdl_controller = SDL_GameControllerOpen(0);
    if(!gamepad->sdl_controller)
    {
        retval = -3;
        goto sdl_cleanup;
    }

    retval = 0;
    goto exit;

sdl_cleanup:
    SDL_Quit();
    memset(gamepad, 0, sizeof(Gamepad));

exit:
    return retval;
}

void gamepad_destroy(Gamepad *gamepad)
{
    SDL_GameControllerClose(gamepad->sdl_controller);
    SDL_Quit();
}

void gamepad_update(Gamepad *gamepad)
{
    SDL_Event event;

    if(SDL_PollEvent(&event))
    {
        if(event.type == SDL_CONTROLLERBUTTONUP)
        {
            gamepad->buttons[event.cbutton.button] = 0;
        }

        if(event.type == SDL_CONTROLLERBUTTONDOWN)
        {
            gamepad->buttons[event.cbutton.button] = 1;
        }

        if(event.type == SDL_CONTROLLERAXISMOTION)
        {
            gamepad->axes[event.caxis.axis] = event.caxis.value;
        }
    }
}

void gamepad_to_inputs(Gamepad *gamepad, Inputs *inputs)
{
    int JOY_LEFT  = (gamepad->axes[SDL_CONTROLLER_AXIS_LEFTX] < -DEADZONE_THRESHOLD) || (gamepad->axes[SDL_CONTROLLER_AXIS_RIGHTX] < -DEADZONE_THRESHOLD);
    int JOY_RIGHT = (gamepad->axes[SDL_CONTROLLER_AXIS_LEFTX] > DEADZONE_THRESHOLD) || (gamepad->axes[SDL_CONTROLLER_AXIS_RIGHTX] > DEADZONE_THRESHOLD);
    int JOY_UP    = (gamepad->axes[SDL_CONTROLLER_AXIS_LEFTY] < -DEADZONE_THRESHOLD) || (gamepad->axes[SDL_CONTROLLER_AXIS_RIGHTY] < -DEADZONE_THRESHOLD);
    int JOY_DOWN  = (gamepad->axes[SDL_CONTROLLER_AXIS_LEFTY] > DEADZONE_THRESHOLD) || (gamepad->axes[SDL_CONTROLLER_AXIS_RIGHTY] > DEADZONE_THRESHOLD);

    inputs->LEFT  = JOY_LEFT ? 1 : 0;
    inputs->RIGHT = JOY_RIGHT ? 1 : 0;
    inputs->UP    = JOY_UP ? 1 : 0;
    inputs->DOWN  = JOY_DOWN ? 1 : 0;
}
