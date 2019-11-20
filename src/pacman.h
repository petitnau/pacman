#ifndef PACMAN_H
#define PACMAN_H

#include "entity.h"

#define PAC_START_X 27
#define PAC_START_Y 23
#define PAC_START_DIR UP
#define PAC_SPEED 70000
#define PAC_START_LIVES 2

typedef struct
{
    Entity e;
    Direction next_dir;
    int lives;
} CharPacman;

typedef struct
{
    _Bool eaten;
} PacManInfo;

void pacman_main(int, int, int, int);

#endif