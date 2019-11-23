#include <unistd.h>
#include "ghost.h"
#include "entity.h"
#include "utils.h"
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
void ghost_move(CharGhost*);
_Bool is_empty_space_ghost(char);

CharGhost init_ghost_char()
{
    CharGhost ghost;
    ghost.e.id = GHOST_ID;
    ghost.e.dir = GHOST_START_DIR;
    ghost.e.p.x = GHOST_START_X;
    ghost.e.p.y = GHOST_START_Y;
    ghost.mode = M_CHASE;
    ghost.paused = true;
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
    info.full = false;
    info.pause = false;
    info.resume = false;
    info.sleeptime = 0;
    return info;
}

void ghost_main(int info_in, int pos_out, int log_out)
{
    CharGhost ghost = init_ghost_char();
    GhostInfo info_pkg = init_ghost_info();
    GhostTimers timers = {};
    //creazione thread
    //pthread_t blinky;
    int i = 0;

    while(1)
    {       
        manage_g_info_in(info_in, &ghost, &info_pkg, &timers);
        manage_g_timers(timers, &ghost);
        ghost_choose_dir(&ghost, info_pkg); 
        if(!ghost.paused) ghost_move(&ghost);
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
            ghost->e.dir = choose_direction_random(*ghost);
            break;
        case M_DEAD:
            ghost->e.dir = choose_direction_target(*ghost, HOME_TARGET);
            break;
        default:
            ghost->e.dir = choose_direction_target(*ghost, blinky_target(info.pacman));
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
