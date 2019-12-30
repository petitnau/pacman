#ifndef PACMAN_H
#define PACMAN_H

#include "utils.h"

#define PACMAN_START_X 27
#define PACMAN_START_Y 23
#define PACMAN_START_DIR UP
#define PACMAN_SPEED 70000
#define PACMAN_START_LIVES 2

#define VERTICALCUT 6
#define HORIZONTALCUT 4

typedef struct
{
    Entity e;
    Direction next_dir;
    int lives;
    _Bool paused;
    _Bool dead;
} CharPacman;

typedef struct
{
    _Bool new;
    _Bool death;
    _Bool pause;
    _Bool resume;
    _Bool shoot;
    int sleeptime;
    int direction;
} PacManInfo;

CharPacman init_pacman_char();
PacManInfo init_pacman_info();
void pacman_main(int, int, int, int);

#endif