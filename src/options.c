#include "options.h"
#include "interface.h"

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
    options.options_shoot.shoot = false;
    options.options_shoot.armor = 0;
    options.options_spawn.spawn = false;
    options.num_ghosts = 4;
    options.options_spawn.time_spawn = 0;
    options.boing = false;
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
    options.options_shoot.shoot = true;
    options.options_shoot.armor = 5;
    options.options_spawn.spawn = true;
    options.num_ghosts = 4;
    options.options_spawn.time_spawn = 3;
    options.boing = true;
    options.death_timer = 3;

    for(i = 0; i < MAP_HEIGHT; i++)
    {
        strcpy(options.map[i], MAP_GUNMAN[i]);
    }

    return options;
}

