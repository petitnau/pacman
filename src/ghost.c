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
void manage_position_events(GhostShared*, CharGhost*);
void ghost_wait(CharGhost, GhostShared*);
void ghost_move(CharGhost*, char[MAP_HEIGHT][MAP_WIDTH+1]);
_Bool is_empty_space_ghost(char);
void* ghost_thread(void*);
void set_ghost_start(GhostShared*, CharGhost*);
void init_ghost_map(GhostShared*);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

CharGhost init_ghost_char(GhostShared *ghost_shared, int id)
{
    CharGhost ghost;
    ghost.ghost_id = id;
    ghost.e.id = id;
    set_ghost_start(ghost_shared, &ghost);

    //if(ghost_shared->options.options_spawn.enabled)
    //    ghost.mode = M_INACTIVE;
    //else
        ghost.mode = M_CHASE;

    ghost.timers.fright = 0;
    ghost.timers.shoot = start_timer(1e3);
    //ghost.paused = true;
    return ghost;
}

void set_ghost_start(GhostShared *ghost_shared, CharGhost *ghost)
{
    if(ghost_shared->options.options_spawn.enabled)
    {
        ghost->e.p = ghost_shared->starting_pos[ghost->ghost_id];
    }
    else
    {
        ghost->e.p = GHOST_START_POS[ghost->ghost_id];
    }
    
    ghost->e.dir = UP;
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

void ghost_main(Options options, int info_in, int pos_out, int bullet_out, int log_out) //int num_fantasmi
{
    int i;
    pthread_t ghost;
    GhostInfo info_pkg = init_ghost_info();
    GhostShared ghost_shared = {};
    GhostParameters *ghost_parameters = malloc(sizeof(GhostParameters)*options.num_ghosts);

    ghost_shared.options = options;
    ghost_shared.num_ghosts = options.num_ghosts;
    ghost_shared.pacman = init_pacman_char(options).e;
    ghost_shared.paused = true;
    ghost_shared.mode = M_CHASE;
    ghost_shared.pos_out = pos_out;
    ghost_shared.bullet_out = bullet_out;
    ghost_shared.log_out = log_out;
    ghost_shared.ghosts = malloc(sizeof(CharGhost)*options.num_ghosts); //alloca puntatori
    ghost_shared.starting_pos = malloc(sizeof(Position)*options.num_ghosts); //alloca puntatori
    
    init_ghost_map(&ghost_shared);
    
    for(i = 0; i < options.num_ghosts; i++)
    {
        ghost_shared.ghosts[i].mode = M_INACTIVE;

        ghost_parameters[i].ghost_shared = &ghost_shared;
        ghost_parameters[i].id = i;
    }     
    if(!options.options_spawn.enabled)
        for(i = 0; i < options.num_ghosts; i++)
            pthread_create(&ghost, NULL, &ghost_thread, &ghost_parameters[i]);    

    while(1)
    {
        manage_g_info_in(info_in, &ghost_shared);
        if(options.options_spawn.enabled) check_ghost_spawn(ghost_parameters);
    }
}

void* ghost_thread(void* parameters)
{
    CharGhost* ghost;
    GhostShared* ghost_shared = ((GhostParameters*)parameters)->ghost_shared;
    int id = ((GhostParameters*)parameters)->id;

    int i = 0;  
    //Assegno ad ogni ghost un ghost_id univoco
    pthread_mutex_lock(&mutex);
    ghost_shared->ghosts[id] = init_ghost_char(ghost_shared, id);
    ghost = &ghost_shared->ghosts[id];
    pthread_mutex_unlock(&mutex);

    while(1)
    {       
        pthread_mutex_lock(&mutex);
        manage_g_timers(ghost_shared, ghost);
        ghost_choose_dir(ghost, ghost_shared); 
        if(!ghost_shared->paused) ghost_move(ghost, ghost_shared->options.map);
        manage_position_events(ghost_shared, ghost);
        write(ghost_shared->pos_out, ghost, sizeof(*ghost)); //invia la posizione a control
        pthread_mutex_unlock(&mutex);
        ghost_wait(*ghost, ghost_shared);
    }
}

void manage_g_info_in(int info_in, GhostShared* ghost_shared)
{
    GhostInfo info;
    static int i=0;
    int k;
        
    pthread_mutex_lock(&mutex);
    while(read(info_in, &info, sizeof(info)) != -1)
    {
        if(info.death != -1)
        {
            ghost_shared->ghosts[info.death].mode = M_DEAD;
        }
        if(info.fright)
        {
            for(i=0; i < ghost_shared->num_ghosts; i++)
            {
                if(ghost_shared->ghosts[i].mode != M_DEAD && ghost_shared->ghosts[i].mode != M_INACTIVE && !is_in_pen(ghost_shared->ghosts[i]))
                {
                    ghost_shared->ghosts[i].timers.fright = start_timer(6e3);
                    ghost_shared->ghosts[i].mode = M_FRIGHT;
                    reverse_direction(&(ghost_shared->ghosts[i].e.dir));
                }
            } 
        }
        if(info.restart)
        {
            for(i=0; i < ghost_shared->num_ghosts; i++)
            {
                ghost_shared->ghosts[i] = init_ghost_char(ghost_shared, ghost_shared->ghosts[i].e.id);
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
    pthread_mutex_unlock(&mutex);
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
    if(ghost->timers.shoot != 0 && ghost_shared->options.options_shoot.enabled)
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
            ghost->e.dir = choose_direction_random(*ghost, ghost_shared->options.map);
            break;
        case M_DEAD:
            ghost->e.dir = choose_direction_target(*ghost, HOME_TARGET, ghost_shared->options.map);
            break;
        case M_SCATTER:
            ghost->e.dir = choose_direction_target(*ghost, SCATTER[ghost->e.id], ghost_shared->options.map);
            break;
        case M_CHASE:
            switch(ghost->e.id)
            {
                case 0: 
                    ghost->e.dir = choose_direction_target(*ghost, blinky_target(ghost_shared->pacman), ghost_shared->options.map);
                    break;
                case 1:
                    ghost->e.dir = choose_direction_target(*ghost, pinky_target(ghost_shared->pacman), ghost_shared->options.map);
                    break;
                case 2:
                    ghost->e.dir = choose_direction_target(*ghost, inky_target(ghost_shared->pacman, ghost_shared->ghosts[0].e), ghost_shared->options.map);
                    break;
                case 3:
                    ghost->e.dir = choose_direction_target(*ghost, clyde_target(ghost_shared->pacman, ghost_shared->ghosts[3].e), ghost_shared->options.map);
                    break;
            }
            break;
    }
}

void manage_position_events(GhostShared* ghost_shared, CharGhost* ghost)
{
    int i;
    if(ghost->e.p.x == HOME_TARGET.x && ghost->e.p.y == HOME_TARGET.y && ghost->mode == M_DEAD)
    {
        ghost->e.dir = UP;
        ghost->mode = M_CHASE;
    }
    if(ghost_shared->options.boing && !is_in_pen(*ghost))
    {
        for(i = 0; i < ghost_shared->num_ghosts; i++)
        {
            if(i != ghost->ghost_id)
            {            
                if(ghost->e.dir != ghost_shared->ghosts[i].e.dir)
                {
                    if(ghost->e.p.x == ghost_shared->ghosts[i].e.p.x && ghost->e.p.y == ghost_shared->ghosts[i].e.p.y )
                    {
                        reverse_direction(&ghost->e.dir);
                        reverse_direction(&ghost_shared->ghosts[i].e.dir);
                    }
                }
            } 
        
        }
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

void ghost_move(CharGhost* ghost, char map[MAP_HEIGHT][MAP_WIDTH+1])
{         
    if(can_move_ghost(*ghost, ghost->e.dir, map))
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
    return is_empty_space(c) || c=='^' || c == 'v' || c=='<' || c=='>' || c=='[' || c==']';
}

_Bool can_move_ghost(CharGhost ghost, Direction direction, char map[MAP_HEIGHT][MAP_WIDTH+1])
{
    int i;

    switch(direction)
    {
        case UP:
            for(i=-1; i<=1; i++)
            {
                if(!is_empty_space_ghost(get_map_at(ghost.e.p.x+i, ghost.e.p.y-1, map)))
                    return false;
                if(ghost.mode != M_DEAD && get_map_at(ghost.e.p.x+i, ghost.e.p.y-1, map) == 'v')
                    return false;
            }
            if(get_map_at(ghost.e.p.x, ghost.e.p.y, map) == '#')
                return false;
            break;
        case LEFT:
            if(!is_empty_space_ghost(get_map_at(ghost.e.p.x-2, ghost.e.p.y, map)))
                return false;
            if(ghost.mode != M_DEAD && (get_map_at(ghost.e.p.x-2, ghost.e.p.y, map) == '>' || get_map_at(ghost.e.p.x-2, ghost.e.p.y, map) == ']'))
                return false;
            break;
        case DOWN:
            for(i=-1; i<=1; i++)
            {
                if(!is_empty_space_ghost(get_map_at(ghost.e.p.x+i,ghost.e.p.y+1, map)))
                    return false;
                if(ghost.mode != M_DEAD && get_map_at(ghost.e.p.x+i, ghost.e.p.y+1, map) == '^')
                    return false;
            }
            break;
        case RIGHT:
            if(!is_empty_space_ghost(get_map_at(ghost.e.p.x+2, ghost.e.p.y, map)))
                return false;
            if(ghost.mode != M_DEAD && (get_map_at(ghost.e.p.x+2, ghost.e.p.y, map) == '<' || get_map_at(ghost.e.p.x+2, ghost.e.p.y, map) == '['))
                return false;
            break;
    }

    return true;
}

_Bool is_in_pen(CharGhost ghost)
{
    return (ghost.e.p.x >= 20 && ghost.e.p.y >= 12 && ghost.e.p.x <= 34 && ghost.e.p.y <= 16);
}

void init_ghost_map(GhostShared* ghost_shared)
{
    int n_pos;
    //numeri a caso per la posizione dei pellet dove andranno i fantasmi
    int *rand_nums = malloc(sizeof(int)*(ghost_shared->options.num_ghosts));
    int i,j;
    Position pos;

    n_pos = count_mat_occ(MAP_HEIGHT, MAP_WIDTH, PELLETS, '~');
    get_rand_nums(0, n_pos, ghost_shared->options.num_ghosts, rand_nums);
    for (i = 0; i < ghost_shared->options.num_ghosts; i++)
    {
        pos = get_i_ch_pos(MAP_HEIGHT, MAP_WIDTH, PELLETS, '~', rand_nums[i]);   
        ghost_shared->starting_pos[i] = pos;
    }
}

void check_ghost_spawn(GhostParameters* ghost_parameters)
{
    GhostShared* ghost_shared = ghost_parameters[0].ghost_shared;
    int num_ghosts = ghost_shared->num_ghosts;
    int i,j;
    pthread_t ghost;

    for(i = 0; i < num_ghosts; i++)
    {

        if(ghost_shared->pacman.p.x == ghost_shared->starting_pos[i].x && ghost_shared->pacman.p.y == ghost_shared->starting_pos[i].y && ghost_shared->ghosts[i].mode == M_INACTIVE)
        {        
            pthread_create(&ghost, NULL, &ghost_thread, &ghost_parameters[i]);
            ghost_shared->ghosts[i].mode = M_CHASE;
        }   
    }
}