#ifndef GHOST_H
#define GHOST_H

#include <stdbool.h>
#include "utils.h"

#define GHOST_START_X 27
#define GHOST_START_Y 11
#define GHOST_START_DIR UP

#define GHOST_SPEED 75000

typedef enum {M_SCATTER, M_CHASE, M_FRIGHT, M_DEAD} GhostMode;

typedef struct
{
    Entity e;
    GhostMode mode;
    _Bool paused;
} CharGhost;

typedef struct
{
    Entity pacman;
    _Bool new;
    _Bool fright;
    _Bool death;
    _Bool full;
    _Bool pause;
    _Bool resume;
    int sleeptime;
} GhostInfo;

CharGhost init_ghost_char();
GhostInfo init_ghost_info();
void ghost_main(int, int, int);
_Bool can_move_ghost(CharGhost, Direction);

#endif