#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdio.h>
#include "ghost.h"
#include "entity.h"
#include "utils.h"
#include "interface.h"
#include "ai.h"

void manage_g_info_in(int, GhostShared*);
void manage_g_timers(GhostShared*, CharGhost*);
void ghost_choose_dir(CharGhost*, GhostShared*);
void manage_position_events(CharGhost*);
void ghost_wait(CharGhost, GhostShared* ghost_shared);
void ghost_move(CharGhost*);
_Bool is_empty_space_ghost(char);
void* ghost_thread(void*);
void manage_shared_info(GhostShared* ghost_shared, CharGhost* ghost);
void set_ghost_start(Entity*);

CharGhost init_ghost_char(int id)
{
    CharGhost ghost;
    ghost.ghost_id = id;
    ghost.e.id = id;
    set_ghost_start(&ghost.e);
    ghost.mode = M_CHASE;
    ghost.timers.fright = 0;
    ghost.timers.shoot = start_timer(1e3);
    //ghost.paused = true;
    return ghost;
}

void set_ghost_start(Entity* e)
{
    e->p = GHOST_START_POS[e->id];
    e->dir = UP;
}

GhostInfo init_ghost_info()
{
    GhostInfo info;
    info.pacman.id = PACMAN_ID;
    info.pacman.dir = PACMAN_START_DIR;
    info.pacman.p.x = PACMAN_START_X;
    info.pacman.p.y = PACMAN_START_Y;
    info.new = false;
    info.fright = false;
    info.restart = false;
    info.pause = false;
    info.resume = false;
    info.death = -1;
    info.sleeptime = 0;
    return info;
}

void ghost_main(int info_in, int pos_out, int bullet_out, int log_out) //int num_fantasmi
{
    int num_fantasmi = 4;
    GhostInfo info_pkg = init_ghost_info();
    GhostShared ghost_shared = {};
    ghost_shared.ghost_number = 0;
    ghost_shared.pacman = init_pacman_char().e;
    ghost_shared.paused = true;
    ghost_shared.mode = M_CHASE;
    ghost_shared.pos_out = pos_out;
    ghost_shared.bullet_out = bullet_out;
    ghost_shared.log_out = log_out;
    pthread_t fantasma;
    sem_init(&ghost_shared.mutex, 0, 1);
    ghost_shared.ghosts = malloc(sizeof(CharGhost*)*num_fantasmi);

    pthread_create(&fantasma, NULL, &ghost_thread, &ghost_shared);
    pthread_create(&fantasma, NULL, &ghost_thread, &ghost_shared);
    pthread_create(&fantasma, NULL, &ghost_thread, &ghost_shared);
    pthread_create(&fantasma, NULL, &ghost_thread, &ghost_shared);

    while(1)
    {
        manage_g_info_in(info_in, &ghost_shared);
    }
}

void* ghost_thread(void* parameters)
{
    CharGhost ghost;
    GhostShared* ghost_shared = (GhostShared*) parameters;

    int i = 0;  

    //Assegno ad ogni ghost un ghost_id univoco
    sem_wait(&ghost_shared->mutex);
    ghost = init_ghost_char(ghost_shared->ghost_number);
    ghost_shared->ghosts[ghost.ghost_id] = &ghost;
    ghost_shared->ghost_number++;
    sem_post(&ghost_shared->mutex);

    while(1)
    {       
        sem_wait(&ghost_shared->mutex);
        manage_shared_info(ghost_shared, &ghost);
        manage_g_timers(ghost_shared, &ghost);
        ghost_choose_dir(&ghost, ghost_shared); 
        if(!ghost_shared->paused) ghost_move(&ghost);
        manage_position_events(&ghost);
        write(ghost_shared->pos_out, &ghost, sizeof(ghost)); //invia la posizione a control
        sem_post(&ghost_shared->mutex);
        ghost_wait(ghost, ghost_shared);
    }
}

void manage_shared_info(GhostShared* ghost_shared, CharGhost* ghost)
{
    //sem_wait(&ghost_shared->mutex);
    /*
    if(ghost->mode != M_DEAD)
    {
        if(ghost_shared->fright && ghost->frighted)
        {
            ghost->mode = M_FRIGHT;
            
            ghost->frighted = false;
        }
        else if(!ghost_shared->fright)
            ghost->mode = ghost_shared->mode;
    }*/

    //sem_post(&ghost_shared->mutex);
}

void manage_g_info_in(int info_in, GhostShared* ghost_shared)
{
    GhostInfo info;
    static int i=0;
    int k;
        
    sem_wait(&ghost_shared->mutex);
    while(read(info_in, &info, sizeof(info)) != -1)
    {
        if(info.death != -1)
        {
            ghost_shared->ghosts[info.death]->mode = M_DEAD;
        }
        if(info.fright)
        {
            for(i=0; i < ghost_shared->ghost_number; i++)
            {
                if(ghost_shared->ghosts[i]->mode != M_DEAD && !is_in_pen(*ghost_shared->ghosts[i]))
                {
                    ghost_shared->ghosts[i]->timers.fright = start_timer(6e3);
                    ghost_shared->ghosts[i]->mode = M_FRIGHT;
                    reverse_direction(&(ghost_shared->ghosts[i]->e.dir));
                }
            } 
        }
        if(info.restart)
        {
            for(i=0; i < ghost_shared->ghost_number; i++)
            {
                *ghost_shared->ghosts[i] = init_ghost_char(ghost_shared->ghosts[i]->e.id);
            } 
            ghost_shared->paused = true;
            ghost_shared->fright = false;
            ghost_shared->mode = M_CHASE;
        }
        if(info.pause)
        {
            ghost_shared->paused = true;
        }
        if(info.resume)
        {
            ghost_shared->paused = false;
        }
        if(info.sleeptime > 0)
        {
            ghost_shared->paused = true;
            usleep(info.sleeptime);
            ghost_shared->paused = false;
            info.sleeptime = 0;
        }
        ghost_shared->pacman = info.pacman;
    }
    sem_post(&ghost_shared->mutex);
}

void manage_g_timers(GhostShared* ghost_shared, CharGhost* ghost)
{
    BulletInfo bullet_info = {};
    int i;

    if(ghost_shared->paused)
        return;

    if(ghost->timers.fright != 0)
    {
        if(!check_timer(ghost->timers.fright))
        {
            if(ghost->mode != M_DEAD)
                ghost->mode = ghost_shared->mode;
            ghost->timers.fright = 0; 
        }
    }
    if(ghost->timers.shoot != 0)
    {
        if(!is_in_pen(*ghost) && ghost->e.p.x % 2 == 0 && ghost->mode == M_CHASE && !check_timer(ghost->timers.shoot))
        {
            for(i = 0; i < 4; i++)
            {
                bullet_info.create_bullet = true;
                bullet_info.p = ghost->e.p;
                bullet_info.dir = i;
                bullet_info.enemy = true;

                switch(bullet_info.dir)
                {
                    case UP:
                        bullet_info.p.y--;
                        break;
                    case LEFT:
                        bullet_info.p.x-=2;
                        break;
                    case DOWN:
                        bullet_info.p.y++;
                        break;
                    case RIGHT:
                        bullet_info.p.x+=2;
                        break;
                }       
                
                write(ghost_shared->bullet_out, &bullet_info, sizeof(bullet_info));
            }
            ghost->timers.shoot = start_timer(1e3); 
        }
    }
}

void ghost_choose_dir(CharGhost* ghost, GhostShared* ghost_shared)
{
    switch(ghost->mode)
    {
        case M_FRIGHT:
            ghost->e.dir = choose_direction_random(*ghost);
            break;
        case M_DEAD:
            ghost->e.dir = choose_direction_target(*ghost, HOME_TARGET);
            break;
        case M_SCATTER:
            ghost->e.dir = choose_direction_target(*ghost, SCATTER[ghost->e.id]);
            break;
        case M_CHASE:
            switch(ghost->e.id)
            {
                case 0: 
                    ghost->e.dir = choose_direction_target(*ghost, blinky_target(ghost_shared->pacman));
                    break;
                case 1:
                    ghost->e.dir = choose_direction_target(*ghost, pinky_target(ghost_shared->pacman));
                    break;
                case 2:
                    ghost->e.dir = choose_direction_target(*ghost, inky_target(ghost_shared->pacman, ghost_shared->ghosts[0]->e));
                    break;
                case 3:
                    ghost->e.dir = choose_direction_target(*ghost, clyde_target(ghost_shared->pacman, ghost_shared->ghosts[3]->e));
                    break;
            }
            break;
    }
}

void manage_position_events(CharGhost* ghost)
{
    if(ghost->e.p.x == HOME_TARGET.x && ghost->e.p.y == HOME_TARGET.y && ghost->mode == M_DEAD)
    {
        ghost->e.dir = UP;
        ghost->mode = M_CHASE;
    }
}

void ghost_wait(CharGhost ghost, GhostShared* ghost_shared)
{
    int movepause = GHOST_SPEED;

    if(ghost.e.dir == UP || ghost.e.dir == DOWN) //gestisce la velocità
        movepause *= 2;
    if(ghost.mode == M_DEAD)
        movepause /= 3;
    else if(ghost.mode == M_FRIGHT)
        movepause *= 2;

    usleep(movepause);
}

void ghost_move(CharGhost* ghost)
{         
    if(can_move_ghost(*ghost, ghost->e.dir))
    {        
        switch(ghost->e.dir)
        {
            case UP:
                ghost->e.p.y--;
                break;
            case LEFT:
                ghost->e.p.x--;
                break;
            case DOWN:
                ghost->e.p.y++;
                break;
            case RIGHT:
                ghost->e.p.x++;
                break;
        }
    }
    map_loop(&ghost->e.p);
}

_Bool is_empty_space_ghost(char c)
{
    return is_empty_space(c) || c=='^' || c=='<' || c=='>';
}

_Bool can_move_ghost(CharGhost ghost, Direction direction)
{
    int i;

    switch(direction)
    {
        case UP:
            for(i=-1; i<=1; i++)
            {
                if(!is_empty_space_ghost(get_map_at(ghost.e.p.x+i, ghost.e.p.y-1)))
                    return false;
            }
            if(get_map_at(ghost.e.p.x, ghost.e.p.y) == '#')
                return false;
            break;
        case LEFT:
            if(!is_empty_space_ghost(get_map_at(ghost.e.p.x-2, ghost.e.p.y)))
                return false;
            if(ghost.mode != M_DEAD && get_map_at(ghost.e.p.x-2, ghost.e.p.y) == '>')
                return false;
            break;
        case DOWN:
            for(i=-1; i<=1; i++)
            {
                if(!is_empty_space_ghost(get_map_at(ghost.e.p.x+i,ghost.e.p.y+1)))
                    return false;
                if(ghost.mode != M_DEAD && get_map_at(ghost.e.p.x+i, ghost.e.p.y+1) == '^')
                    return false;
            }
            break;
        case RIGHT:
            if(!is_empty_space_ghost(get_map_at(ghost.e.p.x+2, ghost.e.p.y)))
                return false;
            if(ghost.mode != M_DEAD && get_map_at(ghost.e.p.x+2, ghost.e.p.y) == '<')
                return false;
            break;
    }

    return true;
}

_Bool is_in_pen(CharGhost ghost)
{
    return (ghost.e.p.x >= 20 && ghost.e.p.y >= 12 && ghost.e.p.x <= 34 && ghost.e.p.y <= 16);
}
