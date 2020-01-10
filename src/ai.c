#include <limits.h>
#include <curses.h>

#include "ghost.h" 
#include "ai.h"

Position blinky_target (Entity pacman)
{
    return pacman.p;
}

Position pinky_target (Entity pacman)
{
    return offset_position(pacman.p, pacman.dir, PINKY_CHASE_OFFSET);
}

Position inky_target (Entity pacman, Entity blinky)
{
    Position offset, target;

    offset = offset_position(pacman.p, pacman.dir, INKY_CHASE_OFFSET);
    target.x = 2*offset.x - blinky.p.x; // semplificato da offset.x - (blinky.position.x - offset.x)
    target.y = 2*offset.y - blinky.p.y;

    return target;
}

Position clyde_target (Entity pacman, Entity clyde)
{
    if (distance(pacman.p, clyde.p) >= 8)
        return pacman.p;
    else
        return SCATTER[3]; //3=clyde
}

Position scatter_target (int id)
{
    return SCATTER[id];
}

Position eaten_target ()
{
    return HOME_POSITION;
}

Position offset_position (Position position, Direction direction, int quantity)
{
    switch (direction)
    {
        case UP:
            position.y -= quantity;
            break;
        case LEFT:
            position.x -= quantity*2;
            break;
        case DOWN:
            position.y -= quantity;
            break;
        case RIGHT:
            position.x += quantity*2;
            break;
    }

    return position;
}

Direction choose_direction_random (CharGhost ghost, char map[MAP_HEIGHT][MAP_WIDTH+1])
{
    int i;
    int rand_dir;
    int num_possible = 0;
    _Bool possible_dirs[4] = {};

    ghost_possible_dirs(ghost, possible_dirs, map);

    for (i=0; i < 4; i++)
        if (possible_dirs[i])
            num_possible++;

    rand_dir=rand_between(1, num_possible);

    // Scorre le direzioni possibili e restituisce la numPossible-esima posizione
    for (i=0; i < 4; i++)
    {
        if (possible_dirs[i])
        {
            rand_dir--;
            if (rand_dir == 0)
            {
                return i;
            }
        }
    }
}

void ghost_possible_dirs(CharGhost ghost, _Bool possible_dirs[4], char map[MAP_HEIGHT][MAP_WIDTH+1])
{
    int i;

    for (i=0; i < 4; i++)      
    {
        possible_dirs[i] = can_move_ghost(ghost, i, map);        
    }

    switch(ghost.e.dir)
    {
        case UP: possible_dirs[DOWN] = false;
            break;
        case DOWN: possible_dirs[UP] = false;
            break;
        case RIGHT: possible_dirs[LEFT] = false;
            break;
        case LEFT: possible_dirs[RIGHT] = false;
            break;
    }
}

Direction choose_direction_target (CharGhost ghost, Position target, char map[MAP_HEIGHT][MAP_WIDTH+1])
{
    int i;
    float dir_dist[4];
    Position dir_ghost[4] = {{ghost.e.p.x, ghost.e.p.y-1},  // up
                            {ghost.e.p.x-1, ghost.e.p.y},  // left
                            {ghost.e.p.x, ghost.e.p.y+1},  // down
                            {ghost.e.p.x+1, ghost.e.p.y}}; // right

    _Bool possible_dirs[4] = {};

    ghost_possible_dirs(ghost, possible_dirs, map);

    for (i=0; i < 4; i++)
    {
        if (possible_dirs[i])
            dir_dist[i] = distance(dir_ghost[i], target);
        else
            dir_dist[i] = INT_MAX;    
    }

    return min_index(4, dir_dist);
}