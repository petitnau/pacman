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

    for(i=0; i<55; i++)
        for(j=0; j<31; j++)
           print_map_at(i, j);
}

void print_pacman(Entity pacman)
{
    Position pos;
    int i;
    
    attron(COLOR_PAIR(2));
    for(i=0; i<3; i++)
    {
        pos.x=pacman.p.x+(i-1);
        pos.y=pacman.p.y;
        pos = get_pac_eff_pos(pos);
        mvaddch(pos.y,pos.x, S_PAC[pacman.dir][i]);
    }
    attroff(COLOR_PAIR(2));
}

char get_map_at(int x, int y)
{
    Position orig_pos = {x,y};
    Position mod_pos = get_pac_eff_pos(orig_pos);

    return MAP[mod_pos.y][mod_pos.x];
}

Position get_pac_eff_pos(Position pos)
{
    Position mod_pos = {mod(pos.x, MAPXMAX), mod(pos.y, MAPYMAX)};

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

int distance (Position a, Position b)
{
    return sqrt(pow(b.x-a.x,2) + pow(b.y-a.y,2));
}
