#include <curses.h>

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
    int i, j;

    for(i=0; i<55; i++)
        for(j=0; j<31; j++)
           print_map_at(i, j);
}

void print_pellets()
{
    int i, j;
    attron(COLOR_PAIR(1));

    for(i=0; i<55; i++)
        for(j=0; j<31; j++)
            if(PELLETS[j][i] != ' ')
                mvaddch(j, i, NCURSES_ACS(PELLETS[j][i]));
        
    attroff(COLOR_PAIR(1));
}

void print_pacman(Entity pacman)
{
    int x,y, i;
    
    attron(COLOR_PAIR(2));
    for(i=0; i<3; i++)
    {
        x=pacman.x+(i-1);
        y=pacman.y;
        get_pac_eff_pos(&y, &x);
        mvaddch(y,x, S_PAC[pacman.dir][i]);
    }
    attroff(COLOR_PAIR(2));
}

char get_map_at(int y, int x)
{
    get_pac_eff_pos(&y, &x);
    return MAP[y][x];
}

void get_pac_eff_pos(int* y, int* x)
{
    *x = mod(*x, MAPXMAX);
    *y = mod(*y, MAPYMAX);
}

int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}