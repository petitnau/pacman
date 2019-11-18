#ifndef PACMAN_H
#define PACMAN_H

#include "utils.h"

#define PAC_SPEED 70000

_Bool accept_turn(Entity, Direction);
_Bool can_move(Entity, Direction);

void pacman_main();

#endif