#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>
#include "utils.h"

typedef struct 
{
    _Bool enabled;
    int armor;
    int max_bullets;
    int shoot_cd;
}OptionsShoot;

typedef struct 
{
    _Bool random;
}OptionsSpawn;

typedef struct 
{
    _Bool fixed;
    Position pos;
}OptionsFruit;


typedef struct{
    OptionsShoot options_shoot;
    OptionsSpawn options_spawn;
    OptionsFruit options_fruit;
    int time_spawn;
    int lives;
    char map[MAP_HEIGHT][MAP_WIDTH+1];
    _Bool boing;
    int num_ghosts;
}Options;

Options choose_options(int mode);
Options gunman_options();


/*
Options mode_options[] = {{{false, 0}, {false, 0, 0}, MAP_PACMAN, false, 0}, //pacman
                          {{true, 5}, {true, 3, 4}, MAP_GUNMAN, true, 2}};    //gunman
*/


#endif