#ifndef BULLET_H
#define BULLET_H

#include <pthread.h>

#include "entity.h"

typedef struct 
{
    _Bool new;
    _Bool create_bullet;
    int x;
    int y;
    int dir;
    _Bool destroy_bullet;
    int destroy_id;
} BulletInfo;

typedef struct
{
    pthread_t id;
    Position p;
    int direction;
} Bullet;

typedef struct
{
    int bullet_pos;
    Bullet bullet;
} BulletThreadPar;

#endif