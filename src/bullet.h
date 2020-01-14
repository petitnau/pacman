#ifndef BULLET_H
#define BULLET_H

#include <pthread.h>

#include "entity.h"

typedef struct 
{
    _Bool new;
    _Bool create_bullet;
    _Bool enemy;
    Position p;
    int dir;
    _Bool destroy_bullet;
    unsigned long destroy_id;
} BulletInfo;

typedef struct
{
    pthread_t id;
    Position p;
    int dir;
    _Bool dead;
    _Bool enemy;
} Bullet;

int bullet_main(int, int);

#endif