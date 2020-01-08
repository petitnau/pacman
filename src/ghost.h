#ifndef GHOST_H
#define GHOST_H

#include <stdbool.h>
#include <semaphore.h>
#include "utils.h"
#include "list.h"
#include "options.h"

static const Position GHOST_START_POS[4] = {{27,11},{27,14},{24,14},{30,14}};

#define GHOST_SPEED 75000

typedef enum {M_SCATTER, M_CHASE, M_FRIGHT, M_DEAD, M_INACTIVE, M_IDLE} GhostMode;

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
    unsigned long long fright;
    unsigned long long shoot;
    unsigned long long load;
} GhostTimers;

typedef struct
{
    Entity e;
    int ghost_id;
    GhostMode mode;
    GhostTimers timers;
} CharGhost;

typedef struct
{
    CharGhost* ghosts;
    Options options;
    int num_ghosts;
    Entity pacman;
    GhostMode mode;
    Position* starting_pos;
    _Bool fright;
    _Bool paused;
    int pos_out;
    int bullet_out;
    int log_out;
} GhostShared;

typedef struct
{
    GhostShared* ghost_shared;
    int id;
} GhostParameters;

CharGhost init_ghost_char(GhostShared*, int);
GhostInfo init_ghost_info();
void ghost_main(Options ,int, int, int, int);
_Bool can_move_ghost(CharGhost, Direction, char map[MAP_HEIGHT][MAP_WIDTH+1]);
_Bool is_in_pen(CharGhost);

#endif