/*
#include <limits.h>

#include "ai.h"

Position blinkyTarget (Character pacman)
{
    return pacman.position;
}

Position pinkyTarget (Character pacman)
{
    return offsetPosition(pacman.position, pacman.direction, PINKY_CHASE_OFFSET);
}

Position inkyTarget (Character pacman, Character blinky)
{
    Position offset, target;

    offset = offsetPosition(pacman.position, pacman.direction, INKY_CHASE_OFFSET);
    target.x = 2*offset.x - blinky.position.x; // semplificato da offset.x - (blinky.position.x - offset.x)
    target.y = 2*offset.y - blinky.position.y;

    return target;
}

Position clydeTarget (Character pacman, Character clyde)
{
    if (distance(pacman.position, clyde.position) >= 8)
        return pacman.position;
    else
        return SCATTER[CLYDE];
}

Position scatterTarget (CId id)
{
    return SCATTER[id];
}

Position eatenTarget ()
{
    return HOME_TARGET;
}

Direction chooseDirectionRandom (_Bool possibleDirections[4])
{
    int i;
    int randomDirection;
    int numPossible = 0;

    for (i=0; i < 4; i++)
        if (possibleDirections[i])
            numPossible++;

    randomDirection = randomBetween(1, numPossible);

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

Direction chooseDirectionTarget (Position ghost, Position target, _Bool possibleDirections[4])
{
    int i;
    int dirDist[4];
    Position dirGhost[4] = {{ghost.x, ghost.y+1},  // up
                            {ghost.x-1, ghost.y},  // left
                            {ghost.x, ghost.y-1},  // down
                            {ghost.x+1, ghost.y}}; // right

    for (i=0; i < 4; i++)
    {
        if (possibleDirections[i])
            dirDist[i] = distance(dirGhost[i], target);
        else
            dirDist[i] = INT_MAX;
    }

    return minIndex(4, dirDist);
}
*/