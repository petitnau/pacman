#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#include "entity.h"
#include "bullet.h"

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
    int movepause;

    BulletThreadPar* b_par = (BulletThreadPar*) parameters;

    while(1)
    {
        switch(b_par->bullet.dir)
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
                b_par->bullet.p.x--;
                break;
        }
        
        write(b_par->bullet_pos, &b_par->bullet, sizeof(b_par->bullet));
        
        movepause = 3e4;
        if(b_par->bullet.dir == UP || b_par->bullet.dir == DOWN) //gestisce la velocità di pacman
            movepause*=2;  

        usleep(movepause);  
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
            bullet_par->bullet_pos = bullet_pos;
            bullet_par->bullet.p = info.p;
            bullet_par->bullet.dir = info.dir;
            pthread_create(&bullet_par->bullet.id, NULL, &bullet_thread, bullet_par);
        }
        if(info.destroy_bullet)
        {
            //pthread_join(info.destroy_id, NULL);
        }
    }
}
