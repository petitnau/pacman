#include <curses.h>
#include <math.h>
#include <stdlib.h>

#include "utils.h"

void print_map_at(int x, int y)
{
    char c = MAP[y][x];

    attron(COLOR_PAIR(1));

    if(c=='m' || c=='q' || c=='x' || c=='j' 
     || c=='l' || c=='k' || c=='t' || c=='u' 
     || c=='w' || c=='~' || c=='`')
        mvaddch(y, x, NCURSES_ACS(c));
    else if(c=='@')
        mvaddch(y, x, ' ');
    else
        mvaddch(y, x, c);
}

void print_map()
{
    int i,j;

    for(i=0; i<MAP_WIDTH; i++)
        for(j=0; j<MAP_HEIGHT; j++)
           print_map_at(i, j);
}

void print_pellets()
{
    int i, j;
    attron(COLOR_PAIR(2));

    for(i=0; i<MAP_WIDTH; i++)
        for(j=0; j<MAP_HEIGHT; j++)
            if(PELLETS[j][i] != ' ')
                mvaddch(j, i, NCURSES_ACS(PELLETS[j][i]));
        
    attroff(COLOR_PAIR(2));
}

void print_pacman(Entity pacman)
{
    Position pos;
    int i;
    
    attron(COLOR_PAIR(3));
    for(i=0; i<3; i++)
    {
        pos.x=pacman.p.x+(i-1);
        pos.y=pacman.p.y;
        pos = get_pac_eff_pos(pos);
        mvaddch(pos.y,pos.x, S_PAC[pacman.dir][i]);
    }
    attroff(COLOR_PAIR(3));
}

char get_map_at(int x, int y)
{
    Position orig_pos = {x,y};
    Position mod_pos = get_pac_eff_pos(orig_pos);

    return MAP[mod_pos.y][mod_pos.x];
}

Position get_pac_eff_pos(Position pos)
{
    Position mod_pos = {mod(pos.x, MAP_WIDTH), mod(pos.y, MAP_HEIGHT)};

    return mod_pos;
}

int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

int rand_between (int min, int max)
{
    return min+rand()%(max-min+1);
}

int min_index (int size, int *array)
{
    int i, x=0;

    for(i=0; i < size; i++)
        if (array[i] < array[x]) //se uguali viene seguita la priorità up left down right
            x = i;

    return i;
}

int distance (Position a, Position b)
{
    return sqrt(pow(b.x-a.x,2) + pow(b.y-a.y,2));
}

Position offset_position (Position position, Direction direction, int quantity)
{
    switch (direction)
    {
        case UP:
#ifdef LEGACY
            position.x -= quantity;
#endif
            position.y -= quantity;
            break;
        case LEFT:
            position.x -= quantity;
            break;
        case DOWN:
            position.y -= quantity;
            break;
        case RIGHT:
            position.x += quantity;
            break;
    }

    return position;
}

Direction reverse_direction (Direction direction)
{
    switch (direction)
    {
        case UP:    return DOWN;
        case LEFT:  return RIGHT;
        case DOWN:  return UP;
        case RIGHT: return LEFT;
    }
}