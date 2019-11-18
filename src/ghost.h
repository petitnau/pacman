#ifndef GHOST_H
#define GHOST_H

#include <stdbool.h>
#include "entity.h"

#define GHOST_START_X 27
#define GHOST_START_Y 14
#define GHOST_START_DIR UP

#define GHOST_SPEED 70000

typedef struct
{
    Entity pacman;
    _Bool fear;
} Ghost_Info;

void ghost_main(int, int, int);

#endif