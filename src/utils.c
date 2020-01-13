#include <curses.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "utils.h"
#include "interface.h"

char get_map_at(int x, int y, char map[MAP_HEIGHT][MAP_WIDTH+1])
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

//Distanza tra due punti
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

//Effetto pacman nella mappa carattere per carattere
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

void diff_matrix(int r, int c, char a_mat[r][c+1], char b_mat[r][c+1], char x_mat[r][c+1])
{
    int i, j;
    for(i=0; i<r; i++)
    {
        for(j=0; j<c; j++)
        {
            x_mat[i][j] = (a_mat[i][j] != b_mat[i][j]) ? 'X' : ' ';
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
    
    //mischia i numeri possibili e prende i primi n;
    for(i=0; i<size-1 && i<n; i++)
    {
        swap(&possible[i], &possible[rand_between(i+1, size-1)]);
        rands[i] = possible[i];
    }

    free(possible);
}

int count_mat_occ(int r, int c, char mat[r][c+1], char ch)
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

//Ottiene posizione i-esimo carattere
Position get_i_ch_pos(int r, int c, char mat[r][c+1], char ch, int i)
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

void create_temp_text(TempText* temp_text, int x, int y, char* string, int time, int color)
{
    temp_text->timer = start_timer(time);
    strcpy(temp_text->text, string);
    temp_text->p.x = x;
    temp_text->p.y = y;
    temp_text->color = color;
}

_Bool blink(unsigned long long freq)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    unsigned long long millis = now.tv_sec * 1000 + now.tv_usec / 1000;

    return (millis/freq)%2 == 0;
}