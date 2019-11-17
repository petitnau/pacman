#include <unistd.h>
#include <curses.h>
#include <string.h>

#include "control.h"
#include "utils.h"


void food_handler(int * score, Position pos, int rows, int col, char game_food[rows][col]);
void food_setup();

void control_main(int pos_in, int ghost_out)
{
    Entity pacman = {PACMAN_ID, PAC_START_X, PAC_START_Y, PAC_START_DIR};
    Entity tmp_pkg;

    Position pos;
    int i,j;

    int score = 0;
    char game_food[MAP_HEIGHT][MAP_WIDTH];
    int eated_dots; //Dopo 70 e 170 spawna frutto

    food_setup(MAP_HEIGHT, MAP_WIDTH, game_food);

    while(1)
    {
        mvprintw(2, 60, "Score:   %d", score);
        read(pos_in, &tmp_pkg, sizeof(tmp_pkg));    //leggo posizione di pacman
        
        food_handler(&score, pacman.p, MAP_HEIGHT, MAP_WIDTH, game_food);

        for(i=-1; i<=1/* code */; i++)    //cancella pacman
        {
            pos.x=pacman.p.x+i;
            pos.y=pacman.p.y;
            pos = get_pac_eff_pos(pos);
            
            mvaddch(pos.y, pos.x, MAP[pos.y][pos.x]);
        }
        
        pacman = tmp_pkg;
        
        mvprintw(0,60, "x:%d,y:%d,d:%d", pacman.p.x, pacman.p.y, pacman.dir);
        print_pacman(pacman);
        refresh();
    }
}

void food_setup(int row, int col, char game_food[row][col])
{
    int i;
    for (i = 0; i < MAP_HEIGHT; i++)
    {
        strcpy(game_food[i], PELLETS[i]);
        mvprintw(9, 60, "Debug:   %d", i);
    } 
}

void food_handler(int * score, Position pos,int rows, int col, char game_food[rows][col])
{   //Diventerà uno switch
    char next_ch;
    /*switch (tmp_pkg.dir)
        {
            case UP:
                next_ch = mvinch(tmp_pkg.p.y, tmp_pkg.p.x); //prende carattere a schermo nella pos y,x
                if(next_ch == '~')
                    *score += 100;
                break;
            case DOWN:
                next_ch = mvinch(tmp_pkg.p.y, tmp_pkg.p.x);
                if(next_ch == '~')
                    *score += 100;
            break;
            case RIGHT:
                next_ch = mvinch(tmp_pkg.p.y, tmp_pkg.p.x+1);
                if(next_ch == '~')
                    *score += 100;
            break;
            case LEFT:
                next_ch = mvinch(tmp_pkg.p.y, tmp_pkg.p.x-1);
                if(next_ch == '~')
                    *score += 100;
            break;
        }*/
    if(game_food[pos.y][pos.x] == '~')
    {
        *score += 10;
        game_food[pos.y][pos.x] = MAP[pos.y][pos.x];
    }
    else if(game_food[pos.y][pos.x] == '`') //energizers
    {
        *score += 50;
        game_food[pos.y][pos.x] = MAP[pos.y][pos.x];
        //Funzione "spaventa fantasmi"
    }
}
