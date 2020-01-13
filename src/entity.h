#ifndef ENTITY_H
#define ENTITY_H

typedef enum {UP, LEFT, DOWN, RIGHT} Direction;

typedef struct
{
    int x,y;
} Position;

typedef struct
{
    int id;
    int move_speed;
    Position p;
    Direction dir;
} Entity;

#endif