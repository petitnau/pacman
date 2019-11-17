#ifndef AI_H
#define AI_H

#define PINKY_CHASE_OFFSET 4
#define INKY_CHASE_OFFSET 2

const Position SCATTER[4] = {{ 2, 0},  //BLINKY
                             {25, 0},  //PINKY
                             {27,35},  //INKY
                             { 0,35}}; //CLYDE

const Position HOME_TARGET = {13,14};

Position blinkyTarget(Entity);
Position pinkyTarget(Entity);
Position inkyTarget(Entity, Entity);
Position clydeTarget(Entity, Entity);

Position scatterTarget(int id);
Position eatenTarget();

Direction chooseDirectionRandom(_Bool[4]);
Direction chooseDirectionTarget(Position, Position, _Bool[4]);

#endif