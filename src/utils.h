#ifndef UTILS_H
#define UTILS_H

#include "entity.h"

#define K_UP 65
#define K_DOWN 66
#define K_RIGHT 67
#define K_LEFT 68
#define k_ESC 27

#define MAP_WIDTH 55
#define MAP_HEIGHT 31

#define NUM_GHOSTS 4
#define PACMAN_ID 0
#define PLAYER_ID 1
#define GHOST_ID 2

#define GUI_HEIGHT 6

#define P_RD 0
#define P_WR 1

#define PIPE_ERROR 16
#define FORK_ERROR 32

#define MAX_HP 3

#define COLOR_PINK 8
#define COLOR_ORANGE 9

#define COLOR_MAP COLOR_PAIR(1)
#define COLOR_TEXT COLOR_PAIR(2)
#define COLOR_PELLETS COLOR_PAIR(3)
#define COLOR_PACMAN COLOR_PAIR(4)
#define COLOR_BLINKY COLOR_PAIR(5)
#define COLOR_INKY COLOR_PAIR(6)
#define COLOR_PINKY COLOR_PAIR(7)
#define COLOR_CLYDE COLOR_PAIR(8)
#define COLOR_FRIGHT COLOR_PAIR(9)
#define COLOR_REDTEXT COLOR_PAIR(10)
#define COLOR_GREENTEXT COLOR_PAIR(11)
#define COLOR_CYANTEXT COLOR_PAIR(12)
#define COLOR_MAGENTATEXT COLOR_PAIR(13)

void print_gui_string(int, int, char*); 
void print_map_at(int, int);
void print_lives(CharPacman);
void print_map();
void print_food(char[MAP_HEIGHT][MAP_WIDTH]);
char get_map_at(int, int);
Position get_pac_eff_pos(Position);
int mod(int, int);

int rand_between(int, int);
float distance(Position, Position);
int min_index(int, float *);
void reverse_direction(Direction*);
unsigned long long start_timer(unsigned long long l);
_Bool check_timer(unsigned long long l);
_Bool is_empty_space(char);
void map_loop(Position*);

#endif