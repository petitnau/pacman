#ifndef UTILS_H
#define UTILS_H

#define K_UP 65
#define K_DOWN 66
#define K_RIGHT 67
#define K_LEFT 68

#define NUM_GHOSTS 4
#define PACMAN_ID 0
#define PLAYER_ID 1
#define GHOST_ID 2

#define PAC_START_X 27
#define PAC_START_Y 23
#define PAC_START_DIR UP
#define GUI_HEIGHT 6

#define P_RD 0
#define P_WR 1

#define PIPE_ERROR 16
#define FORK_ERROR 32

typedef enum {UP, DOWN, RIGHT, LEFT} Direction;

typedef struct
{
    int x,y;
} Position;

void print_gui_string(int, int, char*); 
void print_map_at(int, int);
void print_map();
void print_pellets();
char get_map_at(int, int);
Position get_pac_eff_pos(Position);
int mod(int, int);

int rand_between(int, int);
float distance(Position, Position);
int min_index(int, float *);
Position offset_position(Position, Direction, int);
Direction reverse_direction(Direction);
long start_timer(long);
_Bool check_timer(long);

#endif