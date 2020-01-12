#ifndef CONTROL_H
#define CONTROL_H

#include "pacman.h"
#include "ghost.h"
#include "utils.h"
#include "options.h"

#define FRUIT_POS_Y 17
#define FRUIT_POS_X 27

typedef struct
{
    CharPacman pacman;
    CharGhost* ghosts;
    BulletList bullets;
} Characters;

typedef struct
{
    unsigned long long fruit_timer;
    unsigned long long fright_timer;
} ControlTimers;

typedef struct
{
    int pacman_in;
    int pacman_out;
    int ghost_in;
    int ghost_out;
    int cmd_in;
    int log_in;
    int bullet_in;
    int bullet_out;
} ControlPipes;

typedef struct
{
    GhostInfo ghost_info;
    PacManInfo pacman_info;
    Characters characters;
    Options options;

    char game_food[MAP_HEIGHT][MAP_WIDTH+1];
    Position fruit;
    int score;
    int eaten_dots;
    int ghost_streak;

    ControlTimers timers;
    TempText temp_text;

    ControlPipes* pipes;
} ControlData;

void control_main(ControlPipes, Options);

#endif