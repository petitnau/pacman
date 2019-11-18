#include <limits.h>

#include "ai.h"
#include "entity.h"
#include "utils.h"

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

Direction choose_direction_target (Position ghost, Position target, _Bool possible_dirs[4])
{
    int i;
    int dir_dist[4];
    Position dir_ghost[4] = {{ghost.x, ghost.y+1},  // up
                            {ghost.x-1, ghost.y},  // left
                            {ghost.x, ghost.y-1},  // down
                            {ghost.x+1, ghost.y}}; // right

    for (i=0; i < 4; i++)
    {
        if (possible_dirs[i])
            dir_dist[i] = distance(dir_ghost[i], target);
        else
            dir_dist[i] = INT_MAX;
    }

    return min_index(4, dir_dist);
}