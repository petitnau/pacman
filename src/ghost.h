#ifndef GHOST_H
#define GHOST_H

#include <stdbool.h>
#include <semaphore.h>
#include "utils.h"
#include "list.h"

#define GHOST_START_X 27
#define GHOST_START_Y 11
#define GHOST_START_DIR UP

#define GHOST_SPEED 75000

typedef enum {M_SCATTER, M_CHASE, M_FRIGHT, M_DEAD} GhostMode;

typedef struct
{
    Entity pacman;
    _Bool new;
    _Bool fright;
    _Bool restart;
    _Bool pause;
    _Bool resume;
    int death;
    int sleeptime;
} GhostInfo;

typedef struct
{
    Entity e;
    int ghost_id;
    GhostMode mode;
} CharGhost;

typedef struct
{
    unsigned long long fright;
} GhostTimers;

typedef struct
{
    CharGhost** ghosts;
    int ghost_number;
    Entity pacman;
    GhostMode mode;
    sem_t mutex;
    int death;
    _Bool paused;
    int pos_out;
    int log_out;
} GhostShared;

CharGhost init_ghost_char();
GhostInfo init_ghost_info();
void ghost_main(int, int, int);
_Bool can_move_ghost(CharGhost, Direction);

#endif