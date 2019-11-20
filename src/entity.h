#ifndef ENTITY_H
#define ENTITY_H

#include <stdbool.h>
#include "utils.h"

#define VERTICALCUT 6
#define HORIZONTALCUT 4

typedef enum {M_SCATTER, M_CHASE, M_FRIGHT, M_DEAD} GhostMode;

typedef struct
{
    int id;
    Position p;
    Direction dir;
} Entity;

_Bool accept_turn(Entity, Direction);
_Bool can_move(Entity, Direction);
void map_loop(Entity*);
void e_move(Entity*);

#endif