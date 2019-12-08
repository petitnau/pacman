#include <pthread.h>
#include <unistd.h>

#include "entity.h"
#include "bullet.h"

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



//int bullet_main(BulletPipes pipes)
int bullet_main(int bullet_info, int bullet_pos)
{
    while(1)
    {
        manage_b_info_in(bullet_info, bullet_pos);
    }
}

void* bullet_thread(void* parameters)
{
    BulletThreadPar* b_par = (BulletThreadPar*) parameters;

    while(1)
    {
        switch(b_par->bullet.direction)
        {
            case UP:
                b_par->bullet.p.y--;
                break;
            case DOWN:
                b_par->bullet.p.y++;
                break;
            case RIGHT:
                b_par->bullet.p.x++;
                break;
            case LEFT:
                b_par->bullet.p.y--;
                break;
        }
        
        write(b_par->bullet_pos, &b_par->bullet, sizeof(b_par->bullet));
        usleep(1e5);
    }
}

void manage_b_info_in(int bullet_info, int bullet_pos)
{
    BulletInfo info;

    while(read(bullet_info, &info, sizeof(info)) != -1)
    {
        if(info.create_bullet)
        {
            BulletThreadPar* bullet_par = malloc(sizeof(BulletThreadPar));
            pthread_create(NULL, NULL, &bullet_thread, bullet_par);
        }
        if(info.destroy_bullet)
        {
            pthread_join(info.destroy_id, NULL);
        }
    }
}
