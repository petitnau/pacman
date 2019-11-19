#ifndef GHOST_H
#define GHOST_H

#include <stdbool.h>
#include "entity.h"

#define GHOST_START_X 27
#define GHOST_START_Y 11
#define GHOST_START_DIR UP

#define GHOST_SPEED 75000

typedef struct
{
    Entity pacman;
    _Bool fright;
    _Bool death;
    _Bool full;
} GhostInfo;

typedef struct
{
    Entity e;
    GhostMode mode;
} CharGhost;

void ghost_main(int, int, int);

#endif