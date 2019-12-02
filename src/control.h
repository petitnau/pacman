#ifndef CONTROL_H
#define CONTROL_H

#include "pacman.h"
#include "ghost.h"

#define MAP_WIDTH 55
#define MAP_HEIGHT 31

#define FRUIT_POS_Y 17
#define FRUIT_POS_X 27

typedef struct
{
    CharPacman* pacman;
    CharGhost* ghosts;
    int num_ghosts;
} Characters;

typedef struct
{
    unsigned long long fruit_timer;
    unsigned long long fright_timer;
} ControlTimers;

void control_main(int, int, int, int, int, int, int);
void create_fruit(char[MAP_HEIGHT][MAP_WIDTH], ControlTimers*);

#endif