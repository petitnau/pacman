#ifndef CONTROL_H
#define CONTROL_H

#include "pacman.h"
#include "ghost.h"

typedef struct
{
    CharPacman* pacman;
    CharGhost* ghosts;
    int num_ghosts;
} Characters;

void control_main(int, int, int, int, int, int, int);

#endif