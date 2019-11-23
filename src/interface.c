#include <curses.h>
#include <string.h>
#include "interface.h"
#include "control.h"
#include "pacman.h"
#include "ghost.h"

void print_pacman(CharPacman pacman)
{
    Position pos;
    int i;
    char sprite[3];
    
    attron(COLOR_PACMAN);
    strcpy(sprite, S_PAC[pacman.e.dir]);
    
    for(i=0; i<3; i++)
    {
        pos.x = pacman.e.p.x+(i-1);
        pos.y = pacman.e.p.y;
        pos = get_pac_eff_pos(pos);
        mvaddch(pos.y+GUI_HEIGHT,pos.x, sprite[i]);
    }
    
    attroff(COLOR_PACMAN);
}

void print_ghost(CharGhost ghost)
{
    Position pos;
    int i;
    
    switch(ghost.mode)
    {
        case M_DEAD:
            attron(COLOR_TEXT);
            break;
        case M_FRIGHT:
            attron(COLOR_FRIGHT);
            break;
        default:
            attron(COLOR_BLINKY);
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
            attroff(COLOR_TEXT);
            break;
        case M_FRIGHT:
            attroff(COLOR_FRIGHT);
            break;
        default:
            attroff(COLOR_BLINKY);
            break;
    }
}

void print_fruit()
{
    attron(COLOR_REDTEXT);
    mvaddch(23, FRUIT_POS_X-1, FRUIT[0][0]);
    mvaddch(17+GUI_HEIGHT, FRUIT_POS_X+1, FRUIT[0][2]);
    attroff(COLOR_REDTEXT);
    attron(COLOR_GREENTEXT);
    mvaddch(23, FRUIT_POS_X, FRUIT[0][1]);
    attroff(COLOR_GREENTEXT);
}

void unprint_area(int y, int x, int size, char game_food[MAP_HEIGHT][MAP_WIDTH])
{
    Position pos;
    int i;
    char c;

    for(i=0; i<size; i++)
    {
        pos.x=x+i;
        pos.y=y;
        pos = get_pac_eff_pos(pos);
        
        c = game_food[pos.y][pos.x];
        if(c == '^')
        {
            attron(COLOR_GREENTEXT);
            mvaddch(pos.y+GUI_HEIGHT, pos.x, c);
            attroff(COLOR_GREENTEXT);
        }
        else if(c == '.')
        {
            attron(COLOR_REDTEXT);
            mvaddch(pos.y+GUI_HEIGHT, pos.x, c);
            attroff(COLOR_REDTEXT);
        }
        //else if(c == '~')
        else
        {
            attron(COLOR_PELLETS);
            mvaddch(pos.y+GUI_HEIGHT, pos.x, NCURSES_ACS(c));
            attroff(COLOR_PELLETS);
        }
    }
}

void print_ui(int score, Characters characters)
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
    print_lives(characters.pacman->lives);
    print_pacman(*characters.pacman);
    print_ghost(*characters.ghost);
}

void create_temp_text(TempText* temp_text, int x, int y, char* string, int time)
{
    temp_text->timer = start_timer(time);
    strcpy(temp_text->text, string);
    temp_text->p.x = x;
    temp_text->p.y = y;
}

void print_temp_text(TempText temp_text)
{
    if(temp_text.timer != 0 && check_timer(temp_text.timer))
    {
        mvprintw(temp_text.p.y, temp_text.p.x, "%s", temp_text.text);
        refresh();
    }
}

void sunprint_area(int y, int x, int size, char game_food[MAP_HEIGHT][MAP_WIDTH], Characters characters)
{
    unprint_area(y-GUI_HEIGHT,x,size,game_food);
    print_pacman(*characters.pacman);
    print_ghost(*characters.ghost);
}