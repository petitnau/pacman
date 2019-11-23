#include <curses.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "utils.h"
#include "interface.h"

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

void print_lives(int n)
{
    int i;

    attron(COLOR_PACMAN);

    for(i = 0; i < n; i++)
        mvprintw(37,4+(i*4), S_PACMAN[LEFT]);

    attroff(COLOR_PACMAN);

    for(i = n; i < MAX_HP; i++)
        mvprintw(37,4+(i*4), "   ");
}

void print_map_at(int x, int y)
{
    char c = MAP[y][x];

    attron(COLOR_MAP);

    if(c=='m' || c=='q' || c=='x' || c=='j' 
     || c=='l' || c=='k' || c=='t' || c=='u' 
     || c=='w' || c=='~' || c=='`')
        mvaddch(y+GUI_HEIGHT, x, NCURSES_ACS(c));
    else if(c=='@')
        mvaddch(y+GUI_HEIGHT, x, ' ');
    else
        mvaddch(y+GUI_HEIGHT, x, c);
        
    attroff(COLOR_MAP);
}

void print_map()
{
    int i,j;

    for(i=0; i<MAP_WIDTH; i++)
        for(j=0; j<MAP_HEIGHT; j++)
           print_map_at(i, j);
}

void print_pellets_at(int x, int y)
{
    if(PELLETS[y][x] != ' ')
        mvaddch(y+GUI_HEIGHT, x, NCURSES_ACS(PELLETS[y][x]));
}

void print_pellets()
{
    int i, j;
    attron(COLOR_PELLETS);

    for(i=0; i<MAP_WIDTH; i++)
        for(j=0; j<MAP_HEIGHT; j++)
            print_pellets_at(i,j);
        
    attroff(COLOR_PELLETS);
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

int min_index (int size, float *array)
{
    int i, x=0;

    for(i=0; i < size; i++)
        if (array[i] < array[x]) //se uguali viene seguita la priorità up left down right
            x = i;

    return x;
}

float distance (Position a, Position b)
{
    return sqrt(pow(b.x/2.0-a.x/2.0,2) + pow(b.y-a.y,2));
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

unsigned long long start_timer(unsigned long long duration)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec * 1000 + now.tv_usec / 1000 + duration;
}

_Bool check_timer(unsigned long long timer)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return timer > (now.tv_sec * 1000 + now.tv_usec / 1000);
}