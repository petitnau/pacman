#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>
#include "utils.h"

typedef struct 
{
    _Bool enabled;
    int armor;
}OptionsShoot;

typedef struct 
{
    _Bool enabled;
    int time_spawn;
}OptionsSpawn;

typedef struct{
    OptionsShoot options_shoot;
    OptionsSpawn options_spawn;
    char map[MAP_HEIGHT][MAP_WIDTH+1];
    _Bool boing;
    int death_timer;
    int num_ghosts;
}Options;

Options choose_options(int mode);

/*
Options mode_options[] = {{{false, 0}, {false, 0, 0}, MAP_PACMAN, false, 0}, //pacman
                          {{true, 5}, {true, 3, 4}, MAP_GUNMAN, true, 2}};    //gunman
*/


#endif