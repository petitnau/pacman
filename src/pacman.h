#ifndef PACMAN_H
#define PACMAN_H

#include "entity.h"

#define PAC_SPEED 70000

typedef struct
{
    Entity e;
} CharPacman;

void pacman_main(int, int, int);

#endif