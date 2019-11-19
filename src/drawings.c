#include <curses.h>
#include <string.h>
#include "drawings.h"
#include "pacman.h"
#include "ghost.h"

void print_pacman(CharPacman pacman)
{
    Position pos;
    int i;
    char sprite[3];
    
    attron(COLOR_PAIR(4));
    strcpy(sprite, S_PAC[pacman.e.dir]);
    
    for(i=0; i<3; i++)
    {
        pos.x = pacman.e.p.x+(i-1);
        pos.y = pacman.e.p.y;
        pos = get_pac_eff_pos(pos);
        mvaddch(pos.y+GUI_HEIGHT,pos.x, sprite[i]);
    }
    
    attroff(COLOR_PAIR(4));
}

void print_ghost(CharGhost ghost)
{
    Position pos;
    int i;
    
    switch(ghost.mode)
    {
        case M_DEAD:
        case M_FRIGHT:
            attron(COLOR_PAIR(9));
            break;
        default:
            attron(COLOR_PAIR(5));
            break;
    }
    for(i=0; i<3; i++)
    {
        pos.x = ghost.e.p.x+(i-1);
        pos.y = ghost.e.p.y;
        pos = get_pac_eff_pos(pos);
        mvaddch(pos.y+GUI_HEIGHT,pos.x, S_GHST[ghost.mode][i]);
    }

    switch(ghost.mode)
    {
        case M_DEAD:
        case M_FRIGHT:
            attroff(COLOR_PAIR(9));
            break;
        default:
            attroff(COLOR_PAIR(5));
            break;
    }
}