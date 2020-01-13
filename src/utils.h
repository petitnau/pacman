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
#define COLOR_BROWN 10
#define COLOR_BLUE_1 11
#define COLOR_BLUE_2 12
#define COLOR_BLUE_3 13

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
#define COLOR_SHIELD COLOR_PAIR(18)
#define COLOR_PINKTEXT COLOR_PAIR(19)
#define COLOR_ORANGETEXT COLOR_PAIR(20)
#define COLOR_MAP_1 COLOR_PAIR(21)
#define COLOR_MAP_2 COLOR_PAIR(22)
#define COLOR_MAP_3 COLOR_PAIR(23)

typedef struct
{
    Position p;
    char text[10];
    unsigned long long timer;
    int color;
} TempText;

char get_map_at(int, int, char[MAP_HEIGHT][MAP_WIDTH+1]);
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
void swap(int*, int*);
Position get_i_ch_pos(int r, int c, char[r][c+1], char, int);
int count_mat_occ(int r, int c, char[r][c+1], char);
void get_rand_nums(int, int, int, int[]);
void diff_matrix(int r, int c, char[r][c+1], char[r][c+1], char[r][c+1]);
void create_temp_text(TempText*, int, int, char*, int, int);
_Bool blink(unsigned long long);

#endif