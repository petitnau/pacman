#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

#include "pacman.h"
#include "entity.h"
#include "utils.h"
#include "interface.h"
#include "options.h"

void manage_p_info_in(Options, int, int, CharPacman*, char[MAP_HEIGHT][MAP_WIDTH+1]);
void switch_direction(CharPacman*, char map[MAP_HEIGHT][MAP_WIDTH+1]);
void pac_wait(CharPacman);
_Bool accept_turn(CharPacman, Direction, char[MAP_HEIGHT][MAP_WIDTH+1]);
void pacman_move(Entity*, char map[MAP_HEIGHT][MAP_WIDTH+1]);
_Bool can_move_pacman(Entity, Direction, char[MAP_HEIGHT][MAP_WIDTH+1]);
void manage_p_timers(CharPacman*, Options);

CharPacman init_pacman_char(Options options)
{
    CharPacman pacman;
    pacman.e.id = PACMAN_ID;
    pacman.e.dir = PACMAN_START_DIR;
    pacman.e.p.x = PACMAN_START_X;
    pacman.e.p.y = PACMAN_START_Y;
    pacman.lives = options.lives;
    pacman.armor = options.options_shoot.armor;
    pacman.bullets = options.options_shoot.max_bullets;
    pacman.next_dir = PACMAN_START_DIR;
    pacman.paused = true;
    pacman.dead = false;
    return pacman;
}

PacManInfo init_pacman_info()
{
    PacManInfo info;
    info.new = false;
    info.hit = false;
    info.collide = false;
    info.pause = false;
    info.resume = false;
    info.shoot = false;
    info.reset = false;
    info.direction = -1;
    info.sleeptime = 0;
    return info;
}
void pacman_main(Options options, int info_in, int pos_out, int bullet_out, int log_out)
{
    CharPacman pacman = init_pacman_char(options);
    PacManInfo info_pkg = init_pacman_info();
    int i;

    while(1)
    {
        //Legge solo l'ultimo inserito nella pipe e controlla se è una mossa valida
        manage_p_info_in(options, info_in, bullet_out, &pacman, options.map);
        switch_direction(&pacman, options.map);
        if (!pacman.paused && !pacman.dead) pacman_move(&pacman.e, options.map);
        write(pos_out, &pacman, sizeof(pacman)); //invia la posizione a control
        pac_wait(pacman);
        manage_p_timers(&pacman, options);
    }
}

void manage_p_timers(CharPacman* pacman, Options options)
{
    if(pacman->cooldown != 0)
    {
        if(!check_timer(pacman->cooldown))
        {
            pacman->bullets++;
            if(pacman->bullets < options.options_shoot.max_bullets)
                pacman->cooldown = start_timer(options.options_shoot.shoot_cd);
            else
                pacman->cooldown = 0;
        }
    }
}

void manage_p_info_in(Options options,int info_in, int bullet_out, CharPacman *pacman, char map[MAP_HEIGHT][MAP_WIDTH+1])
{ 
    PacManInfo info_pkg;
    BulletInfo bullet_info = {};

    while(read(info_in, &info_pkg, sizeof(info_pkg)) != -1)
    {                
        //Pacman viene mangiato
        if(info_pkg.hit)
        {
            if(pacman->armor <= 0)
            {
                pacman->lives--;
                pacman->dead = true;
            }
            else
            {
                pacman->armor--;
            }
        }
        if(info_pkg.collide)
        {
            pacman->lives--;
            pacman->dead = true;
        }
        //Il gioco va in pausa
        if(info_pkg.pause)
        {
            pacman->paused = true;
        }
        //Il gioco viene riavviato
        if(info_pkg.resume)
        {
            pacman->paused = false;
        }
        //Pacman fermo
        if(info_pkg.sleeptime > 0)
        {
            usleep(info_pkg.sleeptime);
        }
        //Pacman sparocharacters.pacman
        if(info_pkg.shoot && pacman->bullets > 0)
        {
            pacman->bullets--;
            if(pacman->cooldown == 0)
                pacman->cooldown = start_timer(options.options_shoot.shoot_cd);
            bullet_info.create_bullet = true;
            bullet_info.p = pacman->e.p;
            bullet_info.dir = pacman->e.dir;   
            bullet_info.enemy = false;

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

            write(bullet_out, &bullet_info, sizeof(bullet_info));
        }
        if(info_pkg.reset)
        {
            pacman->armor = options.options_shoot.armor;
            pacman->e.p.x = PACMAN_START_X;
            pacman->e.p.y = PACMAN_START_Y;
            pacman->e.dir = PACMAN_START_DIR;
            pacman->next_dir = PACMAN_START_DIR;
            pacman->dead = false;
            pacman->paused = true;
        }
        //Pacman riceve una nuova direzione da control
        if(info_pkg.direction != -1)
        {  
            if(accept_turn(*pacman, info_pkg.direction, map))
            {
                pacman->next_dir = info_pkg.direction;
            }
        }
    }
}

void switch_direction(CharPacman* pacman, char map[MAP_HEIGHT][MAP_WIDTH+1])
{
    if(can_move_pacman(pacman->e, pacman->next_dir, map))   //controlla se può fare movimento nella nuova direzione
        pacman->e.dir = pacman->next_dir;
}

void pac_wait(CharPacman pacman)
{
    int movepause = PACMAN_SPEED;

    if(pacman.e.dir == UP || pacman.e.dir == DOWN) //gestisce la velocità di pacman
        movepause*=2;

    usleep(movepause);
}

_Bool accept_turn(CharPacman pacman, Direction direction, char map[MAP_HEIGHT][MAP_WIDTH+1])
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
                    if(pacman.e.dir == RIGHT && is_empty_space(get_map_at(pacman.e.p.x+i, pacman.e.p.y-1, map)))
                            return true;
                    else if(pacman.e.dir == LEFT && is_empty_space(get_map_at(pacman.e.p.x-i, pacman.e.p.y-1, map)))
                            return true;
                }
            break;
        case LEFT:
            if(pacman.e.dir == RIGHT || pacman.e.dir == LEFT)
                return true;
            else
                for(i=0; i<HORIZONTALCUT; i++)
                {
                    if(pacman.e.dir == UP && is_empty_space(get_map_at(pacman.e.p.x-2, pacman.e.p.y-i, map)))
                            return true;
                    else if(pacman.e.dir == DOWN && is_empty_space(get_map_at(pacman.e.p.x-2, pacman.e.p.y+i, map)))
                            return true;
                }
            break;
        case DOWN:
            if(pacman.e.dir == UP || pacman.e.dir == DOWN) 
                return true;
            else
                for(i=0; i<VERTICALCUT; i++)
                {
                    if(pacman.e.dir == RIGHT && is_empty_space(get_map_at(pacman.e.p.x+i, pacman.e.p.y+1, map)))
                            return true;
                    else if(pacman.e.dir == LEFT && is_empty_space(get_map_at(pacman.e.p.x-i, pacman.e.p.y+1, map)))
                            return true;
                }
            break;
        case RIGHT:
            if(pacman.e.dir == RIGHT || pacman.e.dir == LEFT) 
                return true;
            else
                for(i=0; i<HORIZONTALCUT; i++)
                {
                    if(pacman.e.dir == UP && is_empty_space(get_map_at(pacman.e.p.x+2, pacman.e.p.y-i, map)))
                            return true;
                    else if(pacman.e.dir == DOWN && is_empty_space(get_map_at(pacman.e.p.x+2, pacman.e.p.y+i, map)))
                            return true;
                }
            break;
    }

    return false;
}

void pacman_move(Entity* pacman, char map[MAP_HEIGHT][MAP_WIDTH+1])
{         
    if(can_move_pacman(*pacman, pacman->dir, map))
    {        
        switch(pacman->dir)
        {
            case UP:
                pacman->p.y--;
                break;
            case LEFT:
                pacman->p.x--;
                break;
            case DOWN:
                pacman->p.y++;
                break;
            case RIGHT:
                pacman->p.x++;
                break;
        }
    }
    map_loop(&pacman->p);
}

_Bool can_move_pacman(Entity entity, Direction direction, char map[MAP_HEIGHT][MAP_WIDTH+1])
{
    int i;

    switch(direction)
    {
        case UP:
            for(i=-1; i<=1; i++)
            {
                if(!is_empty_space(get_map_at(entity.p.x+i, entity.p.y-1, map)))
                    return false;
            }
            break;
        case LEFT:
            if(!is_empty_space(get_map_at(entity.p.x-2, entity.p.y, map)))
                return false;
            break;
        case DOWN:
            for(i=-1; i<=1; i++)
            {
                if(!is_empty_space(get_map_at(entity.p.x+i, entity.p.y+1, map)))
                    return false;
            }
            break;
        case RIGHT:
            if(!is_empty_space(get_map_at(entity.p.x+2, entity.p.y, map)))
                return false;
            break;
    }

    return true;
}
