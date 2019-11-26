#ifndef AI_H
#define AI_H

#include "utils.h"

#define PINKY_CHASE_OFFSET 4
#define INKY_CHASE_OFFSET 2

static const Position SCATTER[4] = {{ 52, -3},  //BLINKY
                                    {2, -3},  //PINKY
                                    {54,31},  //INKY
                                    { 0,31}}; //CLYDE

static const Position HOME_TARGET = {27,14};

Position blinky_target(Entity);
Position pinky_target(Entity);
Position inky_target(Entity, Entity);
Position clyde_target(Entity, Entity);

Position scatter_target(int id);
Position eaten_target();
Position offset_position(Position, Direction, int);

Direction choose_direction_random(CharGhost);
Direction choose_direction_target(CharGhost, Position); //Si può usare entity
void ghost_possible_dirs(CharGhost, _Bool [4]);


#endif