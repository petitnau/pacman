#ifndef AI_H
#define AI_H

#include "utils.h"

#define PINKY_CHASE_OFFSET 4
#define INKY_CHASE_OFFSET 2

const Position SCATTER[4] = {{ 2, 0},  //BLINKY
                             {25, 0},  //PINKY
                             {27,35},  //INKY
                             { 0,35}}; //CLYDE

const Position HOME_TARGET = {13,14};

Position blinky_target(Entity);
Position pinky_target(Entity);
Position inky_target(Entity, Entity);
Position clyde_target(Entity, Entity);

Position scatter_target(int id);
Position eaten_target();

Direction choose_direction_random(_Bool[4]);
Direction choose_direction_target(Position, Position, _Bool[4]);

#endif