#ifndef SYSTEMS_H
#define SYSTEMS_H
#include "entity.h"

void movement_system_process(Entity *entities, uint8_t nentities);
void input_process(Entity *entities, uint8_t nentities);

#endif