#include <stdbool.h>
#include <unistd.h>

#include "pacman.h"
#include "entity.h"
#include "utils.h"
#include "interface.h"

void manage_p_info_in(int, CharPacman*);
void manage_p_cmd_in(int, CharPacman*, Direction*);
void switch_direction(CharPacman*);
void pac_wait(CharPacman);
_Bool accept_turn(CharPacman, Direction);
void pacman_move(Entity*);
_Bool can_move_pacman(Entity, Direction);

CharPacman init_pacman_char()
{
    CharPacman pacman;
    pacman.e.id = PACMAN_ID;
    pacman.e.dir = PACMAN_START_DIR;
    pacman.e.p.x = PACMAN_START_X;
    pacman.e.p.y = PACMAN_START_Y;
    pacman.lives = PACMAN_START_LIVES;
    pacman.next_dir = PACMAN_START_DIR;
    pacman.paused = true;
    pacman.dead = false;
    return pacman;
}

PacManInfo init_pacman_info()
{
    PacManInfo info;
    info.new = false;
    info.death = false;
    info.pause = false;
    info.resume = false;
    info.sleeptime = 0;
    return info;
}
void pacman_main(int cmd_in, int info_in, int pos_out, int log_out)
{
    CharPacman pacman = init_pacman_char();
    PacManInfo info_pkg = init_pacman_info();
    Direction cmd_pkg;
    int i;

    while(1)
    {
        //Legge solo l'ultimo inserito nella pipe e controlla se è una mossa valida
        manage_p_info_in(info_in, &pacman);
        manage_p_cmd_in(cmd_in, &pacman, &cmd_pkg);
        switch_direction(&pacman);
        if (!pacman.paused) pacman_move(&pacman.e);
        write(pos_out, &pacman, sizeof(pacman)); //invia la posizione a control
        pac_wait(pacman);
    }
}

void manage_p_info_in(int info_in, CharPacman *pacman)
{ 
    PacManInfo info_pkg;

    while(read(info_in, &info_pkg, sizeof(info_pkg)) != -1)
    {                
        if(info_pkg.death)
        {
            pacman->e.p.x = PACMAN_START_X;
            pacman->e.p.y = PACMAN_START_Y;
            pacman->e.dir = PACMAN_START_DIR;
            pacman->next_dir = PACMAN_START_DIR;
            pacman->paused = true;
            pacman->lives--;
        }
        if(info_pkg.pause)
        {
            pacman->paused = true;
        }
        if(info_pkg.resume)
        {
            pacman->paused = false;
        }
        if(info_pkg.sleeptime > 0)
        {
            usleep(info_pkg.sleeptime);
        }
    }
}

void manage_p_cmd_in(int cmd_in, CharPacman* pacman, Direction* cmd_pkg)
{
    while(read(cmd_in, cmd_pkg, sizeof(*cmd_pkg)) != -1)
    {            
        if(accept_turn(*pacman, *cmd_pkg))
        {
            pacman->next_dir = *cmd_pkg;
        }
    }
}

void switch_direction(CharPacman* pacman)
{
    if(can_move_pacman(pacman->e, pacman->next_dir))   //controlla se può fare movimento nella nuova direzione
        pacman->e.dir = pacman->next_dir;
}

void pac_wait(CharPacman pacman)
{
    int movepause = PACMAN_SPEED;

    if(pacman.e.dir == UP || pacman.e.dir == DOWN) //gestisce la velocità di pacman
        movepause*=2;

    usleep(movepause);
}

_Bool accept_turn(CharPacman pacman, Direction direction)
{
    int i;

    switch(direction)
    {
        case UP:
            if(pacman.e.dir == UP || pacman.e.dir == DOWN) 
                return true;
            else
                for(i=0; i<VERTICALCUT; i++)
                {
                    if(pacman.e.dir == RIGHT && is_empty_space(get_map_at(pacman.e.p.x+i, pacman.e.p.y-1)))
                            return true;
                    else if(pacman.e.dir == LEFT && is_empty_space(get_map_at(pacman.e.p.x-i, pacman.e.p.y-1)))
                            return true;
                }
            break;
        case LEFT:
            if(pacman.e.dir == RIGHT || pacman.e.dir == LEFT)
                return true;
            else
                for(i=0; i<HORIZONTALCUT; i++)
                {
                    if(pacman.e.dir == UP && is_empty_space(get_map_at(pacman.e.p.x-2, pacman.e.p.y-i)))
                            return true;
                    else if(pacman.e.dir == DOWN && is_empty_space(get_map_at(pacman.e.p.x-2, pacman.e.p.y+i)))
                            return true;
                }
            break;
        case DOWN:
            if(pacman.e.dir == UP || pacman.e.dir == DOWN) 
                return true;
            else
                for(i=0; i<VERTICALCUT; i++)
                {
                    if(pacman.e.dir == RIGHT && is_empty_space(get_map_at(pacman.e.p.x+i, pacman.e.p.y+1)))
                            return true;
                    else if(pacman.e.dir == LEFT && is_empty_space(get_map_at(pacman.e.p.x-i, pacman.e.p.y+1)))
                            return true;
                }
            break;
        case RIGHT:
            if(pacman.e.dir == RIGHT || pacman.e.dir == LEFT) 
                return true;
            else
                for(i=0; i<HORIZONTALCUT; i++)
                {
                    if(pacman.e.dir == UP && is_empty_space(get_map_at(pacman.e.p.x+2, pacman.e.p.y-i)))
                            return true;
                    else if(pacman.e.dir == DOWN && is_empty_space(get_map_at(pacman.e.p.x+2, pacman.e.p.y+i)))
                            return true;
                }
            break;
    }

    return false;
}

void pacman_move(Entity* entity)
{         
    if(can_move_pacman(*entity, entity->dir))
    {        
        switch(entity->dir)
        {
            case UP:
                entity->p.y--;
                break;
            case LEFT:
                entity->p.x--;
                break;
            case DOWN:
                entity->p.y++;
                break;
            case RIGHT:
                entity->p.x++;
                break;
        }
    }
    map_loop(entity);
}

_Bool can_move_pacman(Entity entity, Direction direction)
{
    int i;

    switch(direction)
    {
        case UP:
            for(i=-1; i<=1; i++)
            {
                if(!is_empty_space(get_map_at(entity.p.x+i, entity.p.y-1)))
                    return false;
            }
            break;
        case LEFT:
            if(!is_empty_space(get_map_at(entity.p.x-2, entity.p.y)))
                return false;
            break;
        case DOWN:
            for(i=-1; i<=1; i++)
            {
                if(!is_empty_space(get_map_at(entity.p.x+i, entity.p.y+1)))
                    return false;
            }
            break;
        case RIGHT:
            if(!is_empty_space(get_map_at(entity.p.x+2, entity.p.y)))
                return false;
            break;
    }

    return true;
}
