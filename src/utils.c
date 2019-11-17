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