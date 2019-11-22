#include <curses.h>
#include <string.h>
#include "interface.h"
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
            attron(COLOR_PAIR(10));
            break;
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
            attroff(COLOR_PAIR(10));
            break;
        case M_FRIGHT:
            attroff(COLOR_PAIR(9));
            break;
        default:
            attroff(COLOR_PAIR(5));
            break;
    }
}

void print_fruit()
{
    attron(COLOR_PAIR(11));
    mvaddch(23, PAC_START_X-1, FRUIT[0][0]);
    mvaddch(17+GUI_HEIGHT, PAC_START_X+1, FRUIT[0][2]);
    attroff(COLOR_PAIR(11));
    attron(COLOR_PAIR(12));
    mvaddch(23, PAC_START_X, FRUIT[0][1]);
    attroff(COLOR_PAIR(12));
}

void unprint_entity(Entity entity, char game_food[MAP_HEIGHT][MAP_WIDTH])
{
    Position pos;
    int i;
    char c;

    for(i=-1; i<=1; i++)
    {
        pos.x=entity.p.x+i;
        pos.y=entity.p.y;
        pos = get_pac_eff_pos(pos);
        
        c = game_food[pos.y][pos.x];
        if(c == '^')
        {
            attron(COLOR_PAIR(12));
            mvaddch(pos.y+GUI_HEIGHT, pos.x, c);
            attroff(COLOR_PAIR(12));
        }
        else if(c == '.')
        {
            attron(COLOR_PAIR(11));
            mvaddch(pos.y+GUI_HEIGHT, pos.x, c);
            attroff(COLOR_PAIR(11));
        }
        //else if(c == '~')
        else
        {
            attron(COLOR_PAIR(3));
            mvaddch(pos.y+GUI_HEIGHT, pos.x, NCURSES_ACS(c));
            attroff(COLOR_PAIR(3));
        }
    }
}

void print_ui(int score, CharPacman pacman, CharGhost ghost)
{
    char scorestr[10];
    char nupstr[10];

    sprintf(scorestr, "%d", score/10);
    sprintf(nupstr, "1UP");
    print_gui_string(0,11, nupstr);
    print_gui_string(3,13, "0");
    print_gui_string(3,11, scorestr);
    print_gui_string(3,33, "0");
    print_gui_string(3,31, scorestr);
    print_gui_string(0,37, "HIGH SCORE");
    print_lives(pacman.lives);
    print_pacman(pacman);
    print_ghost(ghost);
}