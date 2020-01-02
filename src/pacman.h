#ifndef PACMAN_H
#define PACMAN_H

#include "utils.h"
#include "options.h"

#define PACMAN_START_X 27
#define PACMAN_START_Y 23
#define PACMAN_START_DIR UP
#define PACMAN_SPEED 70000
#define PACMAN_START_LIVES 2
#define PACMAN_START_ARMOR 5

#define VERTICALCUT 6
#define HORIZONTALCUT 4

typedef struct
{
    Entity e;
    Direction next_dir;
    int lives;
    int armor;
    _Bool paused;
    _Bool dead;
} CharPacman;

typedef struct
{
    _Bool new;
    _Bool hit;
    _Bool collide;
    _Bool pause;
    _Bool resume;
    _Bool shoot;
    _Bool reset;
    int sleeptime;
    int direction;
} PacManInfo;

CharPacman init_pacman_char();
PacManInfo init_pacman_info();
void pacman_main(Options options, int, int, int, int);

#endif