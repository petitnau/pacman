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
    strcpy(sprite, S_PACMAN[pacman.e.dir]);
    
    for(i=0; i<3; i++)
    {
        pos.x = pacman.e.p.x+(i-1);
        pos.y = pacman.e.p.y;
        pos = get_pac_eff_pos(pos);
        mvaddch(pos.y+GUI_HEIGHT,pos.x, sprite[i]);
    }
    
    attroff(COLOR_PACMAN);
}

void print_bullet(Bullet bullet)
{
    Position pos = bullet.p;
    int i;
    char sprite[3];
    
    attron(COLOR_PACMAN);
    
    pos = get_pac_eff_pos(pos);
    mvaddch(pos.y+GUI_HEIGHT,pos.x, '*');
    
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
            switch(ghost.e.id)
            {
                case 0:
                    attron(COLOR_BLINKY);
                    break;
                case 1:
                    attron(COLOR_PINKY);
                    break;
                case 2:
                    attron(COLOR_INKY);
                    break;
                case 3:
                    attron(COLOR_CLYDE);
                    break;
            }
            break;
    }
    for(i=0; i<3; i++)
    {
        pos.x = ghost.e.p.x+(i-1);
        pos.y = ghost.e.p.y;
        pos = get_pac_eff_pos(pos);
        mvaddch(pos.y+GUI_HEIGHT,pos.x, S_GHOST[ghost.mode][i]);
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
            switch(ghost.e.id)
            {
                case 0:
                    attroff(COLOR_BLINKY);
                    break;
                case 1:
                    attroff(COLOR_PINKY);
                    break;
                case 2:
                    attroff(COLOR_INKY);
                    break;
                case 3:
                    attroff(COLOR_CLYDE);
                    break;
            }
            break;
    }
}

void unprint_area(int y, int x, int size, char game_food[MAP_HEIGHT][MAP_WIDTH])
{
    /*
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
        else if(c == '~' || c == '`')
        {
            attron(COLOR_PELLETS);
            mvaddch(pos.y+GUI_HEIGHT, pos.x, NCURSES_ACS(c));
            attroff(COLOR_PELLETS);
        }
        else if(c == ' ')
        {
            print_map_at(pos.x, pos.y);
        }
    }
    */
}

void print_ui(ControlData* cd)
{    
    //wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '); // Erase frame around the window

    int i;

    char scorestr[10];
    char nupstr[10];    

    //erase(); //Cancella tutto
    print_map();
    print_food(cd->game_food);
    sprintf(scorestr, "%d", cd->score/10);
    sprintf(nupstr, "1UP");
    print_gui_string(0,11, nupstr);
    print_gui_string(3,13, "0");
    print_gui_string(3,11, scorestr);
    print_gui_string(3,33, "0");
    print_gui_string(3,31, scorestr);
    print_gui_string(0,37, "HIGH SCORE");
    print_lives(cd->characters.pacman.lives);
    print_pacman(cd->characters.pacman);
    
    for(i = 0; i < cd->characters.num_ghosts; i++)
    {                 
        print_ghost(cd->characters.ghosts[i]);
    }
    BulletNode* aux = cd->characters.bullets.head;
    while(aux != NULL)
    {                 
        print_bullet(aux->bullet);
        aux = aux->next;
    }

    print_temp_text(cd->temp_text);
}

void create_temp_text(TempText* temp_text, int x, int y, char* string, int time, int color)
{
    temp_text->timer = start_timer(time);
    strcpy(temp_text->text, string);
    temp_text->p.x = x;
    temp_text->p.y = y;
    temp_text->color = color;
}

void print_temp_text(TempText temp_text)
{
    attron(COLOR_PAIR(temp_text.color));
    if(temp_text.timer != 0 && check_timer(temp_text.timer))
    {
        mvprintw(temp_text.p.y, temp_text.p.x, "%s", temp_text.text);
        refresh();
    }
    attroff(COLOR_PAIR(temp_text.color));
}

void sunprint_area(int y, int x, int size, ControlData* cd)
{
    /*
    int i;

    unprint_area(y-GUI_HEIGHT,x,size, cd->game_food);
    print_pacman(cd->characters.pacman);
    for(i = 0; i < cd->characters.num_ghosts; i++)
    {
        print_ghost(cd->characters.ghosts[i]);
    }
    */
}