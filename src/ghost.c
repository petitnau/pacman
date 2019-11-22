#include <unistd.h>
#include "ghost.h"
#include "utils.h"
#include "entity.h"
#include "interface.h"
#include "ai.h"

typedef struct
{
    unsigned long long fright;
} GhostTimers;

void manage_g_info_in(int, CharGhost*, GhostInfo*, GhostTimers*);
void manage_g_timers(GhostTimers, CharGhost*);
void ghost_choose_dir(CharGhost*, GhostInfo);
void manage_position_events(CharGhost*);
void ghost_wait(CharGhost);

void ghost_main(int info_in, int pos_out, int log_out)
{
    CharGhost ghost = {{GHOST_ID, {GHOST_START_X, GHOST_START_Y}, GHOST_START_DIR}, M_CHASE, true};
    GhostInfo info_pkg = {{PACMAN_ID, {PAC_START_X, PAC_START_Y}, PAC_START_DIR}, false, false, false, false, 0};
    GhostTimers timers = {};
    //creazione thread
    //pthread_t blinky;
    int i = 0;

    while(1)
    {       
        manage_g_info_in(info_in, &ghost, &info_pkg, &timers);
        manage_g_timers(timers, &ghost);
        ghost_choose_dir(&ghost, info_pkg); 
        if(!ghost.paused) e_move(&ghost.e);
        manage_position_events(&ghost);
        write(pos_out, &ghost, sizeof(ghost)); //invia la posizione a control
        ghost_wait(ghost);
    }
}

void manage_g_info_in(int info_in, CharGhost* ghost, GhostInfo* info_pkg, GhostTimers* timers)
{
    while(read(info_in, info_pkg, sizeof(*info_pkg)) != -1)
    {                
        if(info_pkg->fright && ghost->mode != M_DEAD)
        {
            timers->fright = start_timer(6e3);
            ghost->mode = M_FRIGHT;  
            ghost->e.dir = reverse_direction(ghost->e.dir);
        }
        if(info_pkg->death)
        {
            timers->fright = 0;
            ghost->mode = M_DEAD;
        }
        if(info_pkg->full)
        {
            ghost->e.p.x = GHOST_START_X;
            ghost->e.p.y = GHOST_START_Y;
            ghost->e.dir = GHOST_START_DIR;
            ghost->paused = true;
        }
        if(info_pkg->pause)
        {
            ghost->paused = true;
        }
        if(info_pkg->resume)
        {
            ghost->paused = false;
        }
        if(info_pkg->sleeptime > 0)
        {
            usleep(info_pkg->sleeptime);
        }
    }
}

void manage_g_timers(GhostTimers timers, CharGhost* ghost)
{
    if(ghost->mode == M_FRIGHT)
    {
        if(check_timer(timers.fright))
            ghost->mode = M_FRIGHT;
        else
            ghost->mode = M_CHASE; //ciclo di chase saltato?    
    }
}

void ghost_choose_dir(CharGhost* ghost, GhostInfo info)
{
    switch(ghost->mode)
    {
        case M_FRIGHT:
            ghost->e.dir = choose_direction_random(ghost->e);
            break;
        case M_DEAD:
            ghost->e.dir = choose_direction_target(ghost->e, HOME_TARGET);
            break;
        default:
            ghost->e.dir = choose_direction_target(ghost->e, blinky_target(info.pacman));
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

void ghost_wait(CharGhost ghost)
{
    int movepause = GHOST_SPEED;

    if(ghost.e.dir == UP || ghost.e.dir == DOWN) //gestisce la velocità
        movepause *= 2;
    if(ghost.mode == M_FRIGHT)
        movepause *= 2;
    else if(ghost.mode == M_DEAD)
        movepause /= 3;

    usleep(movepause);
}