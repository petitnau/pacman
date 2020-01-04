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

void print_lives(CharPacman pacman)
{
    int i;

    attron(COLOR_PACMAN);

    for(i = 0; i < pacman.lives; i++)
        mvprintw(37,4+(i*4), S_PACMAN[LEFT]);

    attroff(COLOR_PACMAN);

    for(i = pacman.lives; i < MAX_HP; i++)
        mvprintw(37,4+(i*4), "   ");
    
    attron(COLOR_PAIR(18));

    for(i = 0; i < pacman.armor; i++)
    {
        mvaddch(37,15+(i*4), '[');
        mvaddch(37,16+(i*4), NCURSES_ACS('~'));
        mvaddch(37,17+(i*4), ']');
    }
    
    attroff(COLOR_PAIR(18));

    for(i = pacman.armor; i < PACMAN_START_ARMOR; i++)
        mvprintw(37,15+(i*4), "   ");
}

void print_map_at(int x, int y, char map[MAP_HEIGHT][MAP_WIDTH])
{
    char c = map[y][x];

    attron(COLOR_MAP);

    if(c=='m' || c=='q' || c=='x' || c=='j' 
     || c=='l' || c=='k' || c=='t' || c=='u' 
     || c=='w' || c=='~' || c=='`')
        mvaddch(y+GUI_HEIGHT, x, NCURSES_ACS(c));
    else if(is_empty_space(c) || c == '@' || c == '<' || c == '>')
        mvaddch(y+GUI_HEIGHT, x, ' ');
    else if(c == '^')
        mvaddch(y+GUI_HEIGHT, x, '-');
    else
        mvaddch(y+GUI_HEIGHT, x, c);
        
    attroff(COLOR_MAP);
}

void print_map(char map[MAP_HEIGHT][MAP_WIDTH])
{
    int i,j;

    for(i=0; i<MAP_WIDTH; i++)
        for(j=0; j<MAP_HEIGHT; j++)
           print_map_at(i, j, map);
}

void print_food_at(int x, int y, char game_food[MAP_HEIGHT][MAP_WIDTH])
{
    if(game_food[y][x] != ' ')
    {
        if(game_food[y][x] == '~' || game_food[y][x] == '`')
            mvaddch(y+GUI_HEIGHT, x, NCURSES_ACS(game_food[y][x]));
        else if(game_food[y][x] == '^')
        {
            attron(COLOR_GREENTEXT);
            mvaddch(y+GUI_HEIGHT, x, game_food[y][x]);
            attroff(COLOR_GREENTEXT);
        }
        else if(game_food[y][x] == '.')
        {

            attron(COLOR_REDTEXT);
            mvaddch(y+GUI_HEIGHT, x, game_food[y][x]);
            attroff(COLOR_REDTEXT);        
        }
    }
}

void print_food(char game_food[MAP_HEIGHT][MAP_WIDTH])
{
    int i, j;
    attron(COLOR_PELLETS);

    for(i=0; i<MAP_WIDTH; i++)
        for(j=0; j<MAP_HEIGHT; j++)
            print_food_at(i,j, game_food);
        
    attroff(COLOR_PELLETS);
}


char get_map_at(int x, int y, char map[MAP_HEIGHT][MAP_WIDTH])
{
    Position orig_pos = {x,y};
    Position mod_pos = get_pac_eff_pos(orig_pos);

    return map[mod_pos.y][mod_pos.x];
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

void reverse_direction (Direction* direction)
{
    switch (*direction)
    {
        case UP:    *direction = DOWN; break;
        case LEFT:  *direction = RIGHT; break;
        case DOWN:  *direction = UP; break;
        case RIGHT: *direction = LEFT; break;
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

_Bool is_empty_space(char c)
{
    return c == ' ' || c == '#';
}

void map_loop(Position* position)
{
    if(position->x == -1)
        position->x = MAP_WIDTH-1;
    else if(position->x == MAP_WIDTH)
        position->x = 0;

    if(position->y == -1)
        position->y = MAP_HEIGHT-1;
    else if(position->y == MAP_HEIGHT)
        position->y= 0;
}

void swap(int *a, int *b)
{
    int c;

    c = *a;
    *a = *b;
    *b = c;
}

void diff_matrix(int r, int c, char a_mat[r][c], char b_mat[r][c], char x_mat[r][c])
{
    int i, j;
    for(i=0; i<r; i++)
    {
        for(j=0; j<c; j++)
        {
            x_mat[r][c] = (a_mat[r][c] != b_mat[r][c]) ? 'X' : ' ';
        }
    }
}

void get_rand_nums(int min, int max, int n, int rands[n])
{
    int size = max-min;
    int *possible = malloc(sizeof(int)*size);
    int i;

    for(i=0; i<size; i++)
    {
        possible[i] = i+min;
    }

    for(i=0; i<size-1 && i<n; i++)
    {
        swap(possible[i], possible[rand_between(i+1, size-1)]);

        rands[i] = possible[i];
    }
}

int count_mat_occ(int r, int c, char mat[r][c], char ch)
{
    int count = 0;
    int i, j;
    for(i=0; i<r; i++)
    {
        for(j=0; j<c; j++)
        {
            if(mat[i][j] == ch)
                count++;
        }
    }

    return count;
}

Position get_i_ch_pos(int r, int c, char mat[r][c], char ch, int i)
{
    Position pos;
    int j, k;
    for(j=0; j<r; j++)
    {
        for(k=0; k<c; k++)
        {
            if(mat[j][k] == ch)
            {
                i--;
                if(i<0)
                {
                    pos.y=j;
                    pos.x=k;
                    return pos;
                }
            }
        }
    }

}