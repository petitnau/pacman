#ifndef BULLET_H
#define BULLET_H

#include <pthread.h>

#include "entity.h"

typedef struct 
{
    _Bool new;
    _Bool create_bullet;
    Position p;
    int dir;
    _Bool destroy_bullet;
    int destroy_id;
} BulletInfo;

typedef struct
{
    pthread_t id;
    Position p;
    int dir;
    _Bool dead;
} Bullet;

typedef struct
{
    int bullet_pos;
    Bullet bullet;
} BulletThreadPar;

#endif