#include <curses.h>
#include <string.h>
#include "interface.h"
#include "control.h"
#include "pacman.h"
#include "ghost.h"

void print_gui_string(int, int, char*); 
void print_health(CharPacman);
void print_map_at(WINDOW*, int, int,char[MAP_HEIGHT][MAP_WIDTH+1], int);
void print_map(WINDOW*, ControlData*);
void print_food(WINDOW*, ControlData*);
void print_pacman(WINDOW*, CharPacman);
void print_ghost(WINDOW*, CharGhost);
void print_temp_text(WINDOW*, TempText);

void print_pacman(WINDOW* win, CharPacman pacman)
{
    Position pos;
    int i;
    char sprite[3];
    
    wattron(win, COLOR_PACMAN);
    strcpy(sprite, S_PACMAN[pacman.e.dir]);
    
    for(i=0; i<3; i++)
    {
        pos.x = pacman.e.p.x+(i-1);
        pos.y = pacman.e.p.y;
        pos = get_pac_eff_pos(pos);
        mvwaddch(win, pos.y,pos.x, sprite[i]);
    }
    
    wattroff(win, COLOR_PACMAN);
}

void print_bullet(WINDOW* win, Bullet bullet)
{
    Position pos = bullet.p;
    int i;
    char sprite[3];
    
    if(!bullet.dead)
    {
        if(bullet.enemy)
            wattron(win, COLOR_PAIR(10));
        else
            wattron(win, COLOR_PAIR(17));
        
        pos = get_pac_eff_pos(pos);
        mvwaddch(win, pos.y,pos.x, '+');

        if(bullet.enemy)
            wattroff(win, COLOR_PAIR(10));
        else
            wattroff(win, COLOR_PAIR(17));
    }        
}

void print_ghost(WINDOW *win, CharGhost ghost)
{
    Position pos;
    int i;
  /*  if(ghost.mode == M_INACTIVE)
            return; */
            
    switch(ghost.mode)
    {
        case M_DEAD:
            wattron(win, COLOR_TEXT);
            break;
        case M_FRIGHT:
            wattron(win, COLOR_FRIGHT);
            break;
        case M_IDLE:
            switch(ghost.type)
            {
                case BLINKY:
                    wattron(win, COLOR_REDTEXT);
                    break;
                case PINKY:
                    wattron(win, COLOR_PINKTEXT);
                    break;
                case INKY:
                    wattron(win, COLOR_CYANTEXT);
                    break;
                case CLYDE:
                    wattron(win, COLOR_ORANGETEXT);
                    break;
            }
            break;
        default:
            switch(ghost.type)
            {
                case BLINKY:
                    wattron(win, COLOR_BLINKY);
                    break;
                case PINKY:
                    wattron(win, COLOR_PINKY);
                    break;
                case INKY:
                    wattron(win, COLOR_INKY);
                    break;
                case CLYDE:
                    wattron(win, COLOR_CLYDE);
                    break;
            }
            break;
    }
    for(i=0; i<3; i++)
    {
        pos.x = ghost.e.p.x+(i-1);
        pos.y = ghost.e.p.y;
        pos = get_pac_eff_pos(pos);
        mvwaddch(win, pos.y,pos.x, S_GHOST[ghost.mode][i]);
    }

    switch(ghost.mode)
    {
        case M_DEAD:
            wattroff(win, COLOR_TEXT);
            break;
        case M_FRIGHT:
            wattroff(win, COLOR_FRIGHT);
            break;
        case M_IDLE:

            switch(ghost.type)
            {
                case BLINKY:
                    wattroff(win, COLOR_REDTEXT);
                    break;
                case PINKY:
                    wattroff(win, COLOR_PINKTEXT);
                    break;
                case INKY:
                    wattroff(win, COLOR_CYANTEXT);
                    break;
                case CLYDE:
                    wattroff(win, COLOR_ORANGETEXT);
                    break;
            }
            break;
        default:
            switch(ghost.type)
            {
                case BLINKY:
                    wattroff(win, COLOR_BLINKY);
                    break;
                case PINKY:
                    wattroff(win, COLOR_PINKY);
                    break;
                case INKY:
                    wattroff(win, COLOR_INKY);
                    break;
                case CLYDE:
                    wattroff(win, COLOR_CLYDE);
                    break;
            }
            break;
    }
}

void print_ui(WINDOW* win_map, ControlData* cd)
{    
    int i;
    Position p;

    char scorestr[10];
    char nupstr[10];    

    print_map(win_map, cd);
    print_food(win_map, cd);
    sprintf(scorestr, "%d", cd->score/10);
    sprintf(nupstr, "1UP");
    print_gui_string(0,11, nupstr);
    print_gui_string(3,13, "0");
    print_gui_string(3,11, scorestr);
    print_gui_string(3,33, "0");
    print_gui_string(3,31, scorestr);
    print_gui_string(0,37, "HIGH SCORE");
    print_health(cd->characters.pacman);
    
    BulletNode* aux = cd->characters.bullets.head;
    while(aux != NULL)
    {            
        if(!cd->options.spooky || distance(cd->characters.pacman.e.p, aux->bullet.p) < 5 || !aux->bullet.enemy)
            print_bullet(win_map, aux->bullet);
        aux = aux->next;
    }
    
    print_pacman(win_map, cd->characters.pacman);

    for(i = 0; i < cd->options.num_ghosts; i++)
    {                 
        if(!cd->options.spooky || distance(cd->characters.pacman.e.p, cd->characters.ghosts[i].e.p) < 5 || cd->characters.ghosts[i].mode == M_FRIGHT || cd->characters.ghosts[i].mode == M_DEAD)
            if(cd->characters.ghosts[i].mode != M_INACTIVE)
                print_ghost(win_map, cd->characters.ghosts[i]);
    }

    print_temp_text(win_map, cd->temp_text);
    wrefresh(win_map);
    refresh();
}

void print_temp_text(WINDOW *win, TempText temp_text)
{
    wattron(win, COLOR_PAIR(temp_text.color));
    if(temp_text.timer != 0 && check_timer(temp_text.timer))
    {
        mvwprintw(win, temp_text.p.y, temp_text.p.x, "%s", temp_text.text);
        refresh();
    }
    wattroff(win, COLOR_PAIR(temp_text.color));
}

void print_gui_string(int y, int x, char* str)
{
    int i,j,k;
    int len = strlen(str);
    char c;

    attron(COLOR_TEXT);

    for(i=0; i<len; i++)
    {
        for(j=0; j<6; j++)
        {
            if(str[i] >= '0' && str[i] <= '9')
                c = F_NUMBERS[str[i]-'0'][j];
            else if(str[i] >= 'a' && str[i] <= 'z')
                c = F_LETTERS[str[i]-'a'][j];
            else if(str[i] >= 'A' && str[i] <= 'Z')
                c = F_LETTERS[str[i]-'A'][j];
            else 
                c = ' ';

            if(c=='m' || c=='q' || c=='x' || c=='j' 
            || c=='l' || c=='k' || c=='t' || c=='u' 
            || c=='w' || c=='v')
                mvaddch(y+(j/2),x+(j%2)-(len-i)*2, NCURSES_ACS(c));
            else
                mvaddch(y+(j/2),x+(j%2)-(len-i)*2, c);
        }
    }

    attroff(COLOR_TEXT);
}

void print_health(CharPacman pacman)
{
    int i;

    mvprintw(37,0, "%*s", MAP_WIDTH, " ");
    for(i=0; i<40; i++)
    {
        mvaddch(i,57, ' ');
        mvaddch(i,58, ' ');
    }

    for(i=0; i<pacman.bullets; i++)
    {
        attron(COLOR_SHIELD);
        mvprintw(35-i*3, 57, "/\\");
        attroff(COLOR_SHIELD);
        attron(COLOR_PACMAN);
        mvprintw(36-i*3, 57, "  ");
        attroff(COLOR_PACMAN);
    }

    if(pacman.lives <= 5)
    {
        attron(COLOR_PACMAN);
        for(i = 0; i < pacman.lives; i++)
            mvprintw(37,4+(i*4), S_PACMAN[LEFT]);
        attroff(COLOR_PACMAN);
    }
    else
    {
        attron(COLOR_PACMAN);
        mvprintw(37,4, S_PACMAN[LEFT]);
        attroff(COLOR_PACMAN);

        mvprintw(37,8, "x%d", pacman.lives);
    }
    
    if(pacman.armor <= 5)
    {
        attron(COLOR_PAIR(18));
        for(i = 0; i < pacman.armor; i++)
        {
            mvaddch(37,MAP_WIDTH-7-(i*4), '[');
            mvaddch(37,MAP_WIDTH-6-(i*4), NCURSES_ACS('~'));
            mvaddch(37,MAP_WIDTH-5-(i*4), ']');
        }
        attroff(COLOR_PAIR(18));
    }
    else
    {
        attron(COLOR_PAIR(18));
        mvaddch(37,MAP_WIDTH-7, '[');
        mvaddch(37,MAP_WIDTH-6, NCURSES_ACS('~'));
        mvaddch(37,MAP_WIDTH-5, ']');
        attroff(COLOR_PAIR(18));

        mvprintw(37,MAP_WIDTH-12, "%3dx", pacman.armor);
    }
    
}

void set_map_brightness(WINDOW* win, int brightness)
{
    switch(brightness)
    {
        case 0:
            wattron(win, COLOR_MAP);
            break;
        case 1:
            wattron(win, COLOR_MAP_1);
            break;
        case 2:
            wattron(win, COLOR_MAP_2);
            break;
        case 3:
            wattron(win, COLOR_MAP_3);
            break;
    }
}

void unset_map_brightness(WINDOW* win, int brightness)
{
    switch(brightness)
    {
        case 0:
            wattroff(win, COLOR_MAP);
            break;
        case 1:
            wattroff(win, COLOR_MAP_1);
            break;
        case 2:
            wattroff(win, COLOR_MAP_2);
            break;
        case 3:
            wattroff(win, COLOR_MAP_3);
            break;
    }
}

void print_map_at(WINDOW* win, int x, int y, char map[MAP_HEIGHT][MAP_WIDTH+1], int brightness)
{
    char c = map[y][x];

    if(c=='m' || c=='q' || c=='x' || c=='j' 
     || c=='l' || c=='k' || c=='t' || c=='u' 
     || c=='w' || c=='~' || c=='`')
    {
        set_map_brightness(win, brightness);
        mvwaddch(win, y, x, NCURSES_ACS(c));
        unset_map_brightness(win, brightness);

    }
    else if(is_empty_space(c) || c == '@' || c == '[' || c== ']')
    {
        set_map_brightness(win, brightness);
        mvwaddch(win, y, x, ' ');
        unset_map_brightness(win, brightness);
    }
    else if(c == '^' || c =='v')
    {
        wattron(win, COLOR_MAGENTATEXT);
        mvwaddch(win, y, x, ACS_HLINE);
        wattroff(win, COLOR_MAGENTATEXT);
    }
    else if(c == '<' || c =='>')
    {
        wattron(win, COLOR_MAGENTATEXT);
        mvwaddch(win, y, x, ACS_VLINE);
        wattroff(win, COLOR_MAGENTATEXT);
    }
    else
    {
        mvwaddch(win, y, x, c);
    }
        
}

void print_map(WINDOW* win, ControlData* cd)
{
    int i,j;
    Position p;

    for(i=0; i<MAP_WIDTH; i++)
        for(j=0; j<MAP_HEIGHT; j++)
        {
            p.y = j;
            p.x = i;
            if(!cd->options.spooky || distance(cd->characters.pacman.e.p, p) < 5)
                print_map_at(win, i, j, cd->options.map, 0);        
            else
                print_map_at(win, i, j, cd->options.map, 3);           
        }
}

void print_food_at(WINDOW *win, int x, int y, char game_food[MAP_HEIGHT][MAP_WIDTH+1])
{
    if(game_food[y][x] != ' ')
    {
        if(game_food[y][x] == '~')
        {
            wattron(win, COLOR_PELLETS);
            mvwaddch(win, y, x, NCURSES_ACS(game_food[y][x]));
            wattroff(win, COLOR_PELLETS);
        }
        else if(game_food[y][x] == '`')
        {
            if(blink(2e2))
            {
                wattron(win, COLOR_PELLETS);
                mvwaddch(win, y, x, NCURSES_ACS(game_food[y][x]));
                wattroff(win, COLOR_PELLETS);
            }
        }
        else if(game_food[y][x] == '^')
        {
            wattron(win, COLOR_GREENTEXT);
            mvwaddch(win, y, x, game_food[y][x]);
            wattroff(win, COLOR_GREENTEXT);
        }
        else if(game_food[y][x] == '.')
        {

            wattron(win, COLOR_REDTEXT);
            mvwaddch(win, y, x, game_food[y][x]);
            wattroff(win, COLOR_REDTEXT);        
        }
    }
}

void print_food(WINDOW *win, ControlData* cd)
{
    int i, j;
    Position p;

    for(i=0; i<MAP_WIDTH; i++)
        for(j=0; j<MAP_HEIGHT; j++)
        {
            p.y = j;
            p.x = i;
            if(!cd->options.spooky || distance(cd->characters.pacman.e.p, p) < 5)
                print_food_at(win, i,j, cd->game_food);   
        }
}
