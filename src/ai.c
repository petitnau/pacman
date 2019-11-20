#include <limits.h>
#include <curses.h>

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
    return HOME_TARGET;
}

Direction choose_direction_random (_Bool possibleDirections[4])
{
    int i;
    int randomDirection;
    int numPossible = 0;

    for (i=0; i < 4; i++)
        if (possibleDirections[i])
            numPossible++;

    randomDirection = rand_between(1, numPossible);

    // Scorre le direzioni possibili e restituisce la numPossible-esima posizione
    for (i=0; i < 4; i++)
    {
        if (possibleDirections[i])
        {
            numPossible--;
            if (randomDirection == 0)
                return i;
        }
    }
}

Direction choose_direction_target (Entity ghost, Position target)
{
    int i;
    float dir_dist[4];
    Position dir_ghost[4] = {{ghost.p.x, ghost.p.y-1},  // up
                            {ghost.p.x, ghost.p.y+1},  // down
                            {ghost.p.x+1, ghost.p.y}, // right
                            {ghost.p.x-1, ghost.p.y}};  // left

    _Bool possible_dirs[4] = {};

    for (i=0; i < 4; i++)      
    {
        if (can_move(ghost, i))
            possible_dirs[i] = true;
    }

    switch(ghost.dir)
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

    for (i=0; i < 4; i++)
    {
        if (possible_dirs[i])
            dir_dist[i] = distance(dir_ghost[i], target);
        else
            dir_dist[i] = INT_MAX;    
    }

    return min_index(4, dir_dist);
}