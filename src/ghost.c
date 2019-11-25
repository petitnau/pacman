#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include "ghost.h"
#include "entity.h"
#include "utils.h"
#include "interface.h"
#include "ai.h"

void manage_g_info_in(int, GhostShared*, GhostTimers*);
void manage_g_timers(GhostTimers*, GhostShared*);
void ghost_choose_dir(CharGhost*, GhostShared*);
void manage_position_events(CharGhost*);
void ghost_wait(CharGhost, GhostShared* ghost_shared);
void ghost_move(CharGhost*);
_Bool is_empty_space_ghost(char);
void* ghost_thread(void*);
void manage_shared_info(GhostShared* ghost_shared, CharGhost* ghost);

CharGhost init_ghost_char()
{
    CharGhost ghost;
    ghost.e.id = GHOST_ID;
    ghost.e.dir = GHOST_START_DIR;
    ghost.e.p.x = GHOST_START_X;
    ghost.e.p.y = GHOST_START_Y;
    ghost.mode = M_CHASE;
    //ghost.paused = true;
    return ghost;
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
    info.death = false;
    info.restart = false;
    info.pause = false;
    info.resume = false;
    info.sleeptime = 0;
    return info;
}

void ghost_main(int info_in, int pos_out, int log_out) //int num_fantasmi
{
    int num_fantasmi = 1;
    GhostInfo info_pkg = init_ghost_info();
    GhostTimers timers = {};
    GhostShared ghost_shared = {};
    ghost_shared.ghost_number = 0; //bugfix
    ghost_shared.pacman = init_pacman_char().e;
    ghost_shared.mode = M_CHASE;
    ghost_shared.pos_out = pos_out;
    ghost_shared.log_out = log_out;
    pthread_t fantasma;
    sem_init(&ghost_shared.mutex, 0, 1);
    ghost_shared.ghosts = malloc(sizeof(CharGhost*)*num_fantasmi);

    pthread_create(&fantasma, NULL, &ghost_thread, &ghost_shared);

    while(1)
    {
        manage_g_info_in(info_in, &ghost_shared, &timers);
        manage_g_timers(&timers, &ghost_shared);
    }
}

void* ghost_thread(void* parameters)
{
    CharGhost ghost = init_ghost_char();
    GhostShared* ghost_shared = (GhostShared*) parameters;

    int i = 0;

    //Assegno ad ogni ghost un ghost_id univoco
    sem_wait(&ghost_shared->mutex);
    ghost.ghost_id = ghost_shared->ghost_number; //Era qua il bug stavamo inizializzando a 1 e non a 0 la prima cosa dell'array
    ghost_shared->ghosts[ghost.ghost_id] = &ghost;
    ghost_shared->ghost_number++;
    sem_post(&ghost_shared->mutex);


    while(1)
    {       
        write(ghost_shared->log_out, "test", 50);
        manage_shared_info(ghost_shared, &ghost);
        ghost_choose_dir(&ghost, ghost_shared); 
        if(!ghost_shared->paused) ghost_move(&ghost);
        manage_position_events(&ghost);
        write(ghost_shared->pos_out, &ghost, sizeof(ghost)); //invia la posizione a control
        
        
        write(ghost_shared->log_out, "ciao", 50);
        ghost_wait(ghost, ghost_shared);
        write(ghost_shared->log_out, "prova", 50);
    }
}

void manage_shared_info(GhostShared* ghost_shared, CharGhost* ghost)
{
    sem_wait(&ghost_shared->mutex);
    
    ghost->mode = ghost_shared->mode;

    sem_post(&ghost_shared->mutex);
}

void manage_g_info_in(int info_in, GhostShared* ghost_shared, GhostTimers* timers)
{
    GhostInfo info;
    int i;
    int k;
        
    //sem_wait(&ghost_shared->mutex);
    while(read(info_in, &info, sizeof(info)) != -1)
    {


        //f(info.death)
        //{
        //    timers->fright = 0;
        //    ghost_shared->mode = M_DEAD;
        //}
        if(info.fright)
        {
            timers->fright = start_timer(6e3);
            ghost_shared->mode = M_FRIGHT; 
            for(i=0; i < ghost_shared->ghost_number; i++)
            {
                reverse_direction(&(ghost_shared->ghosts[i]->e.dir));
            } 
        }
        if(info.restart)
        {
            for(i=0; i < ghost_shared->ghost_number; i++)
            {
                ghost_shared->ghosts[i]->e.p.x = GHOST_START_X;
                ghost_shared->ghosts[i]->e.p.y = GHOST_START_Y;
                ghost_shared->ghosts[i]->e.dir = GHOST_START_DIR;
            } 
            ghost_shared->paused = true;
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
        }
        ghost_shared->pacman = info.pacman;
    }
    //sem_post(&ghost_shared->mutex);
}

void manage_g_timers(GhostTimers* timers, GhostShared* ghost_shared)
{
    if(timers->fright != 0)
    {
        if(!check_timer(timers->fright))
        {
            ghost_shared->mode = M_CHASE; //ciclo di chase saltato?   
            timers->fright = 0; 
        }
    }
}

void ghost_choose_dir(CharGhost* ghost, GhostShared* ghost_shared)
{
    switch(ghost_shared->mode)
    {
        case M_FRIGHT:
            ghost->e.dir = choose_direction_random(*ghost);
            break;
        case M_DEAD:
            ghost->e.dir = choose_direction_target(*ghost, HOME_TARGET);
            break;
        default:
            ghost->e.dir = choose_direction_target(*ghost, blinky_target(ghost_shared->pacman));
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
        write(ghost_shared->log_out, "prova", 50);
    int movepause = GHOST_SPEED;

    if(ghost.e.dir == UP || ghost.e.dir == DOWN) //gestisce la velocità
        movepause *= 2;
    if(ghost_shared->mode == M_FRIGHT)
        movepause *= 2;
    else if(ghost_shared->mode == M_DEAD)
        movepause /= 3;

    usleep(movepause);
        write(ghost_shared->log_out, "prova2", 50);
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
    map_loop(&ghost->e);
}

_Bool is_empty_space_ghost(char c)
{
    return is_empty_space(c) || c=='^';
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
            break;
    }

    return true;
}
