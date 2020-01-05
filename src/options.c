#include "options.h"
#include "interface.h"
#include <stdio.h>

Options pacman_options();
Options gunman_options();

Options choose_options(int mode)
{
    switch(mode)
    {
        case 0:
            return pacman_options();
            break;
        case 1: 
            return gunman_options();
            break;
    }
}

Options pacman_options()
{
    int i;
    Options options;
    options.options_shoot.enabled = false;
    options.options_shoot.armor = 0;
    options.options_spawn.enabled = false;
    options.options_spawn.time_spawn = 0;
    options.lives = 2;
    options.num_ghosts = 4;
    options.boing = false;
    options.options_fruit.fixed = true;
    options.options_fruit.pos.x = 27;
    options.options_fruit.pos.y = 17;
    options.death_timer = 0;
    
    for(i = 0; i < MAP_HEIGHT; i++)
    {
        strcpy(options.map[i], MAP_PACMAN[i]);
    }
    return options;
}

Options gunman_options()
{
    int i;
    Options options;
    options.options_shoot.enabled = true;
    options.options_shoot.armor = 5;
    options.options_spawn.enabled = true;
    options.options_spawn.time_spawn = 3;
    options.options_fruit.fixed = false;
    options.lives = 2;
    options.num_ghosts = 4;
    options.boing = true;
    options.death_timer = 3;

    for(i = 0; i < MAP_HEIGHT; i++)
    {
        strcpy(options.map[i], MAP_GUNMAN[i]);
    }   
    return options;
}

