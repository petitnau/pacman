#ifndef PACMAN_H
#define PACMAN_H

#include "utils.h"

_Bool accept_turn(Entity, Direction);
_Bool can_move(Entity, Direction);

void pacman_main();

#endif