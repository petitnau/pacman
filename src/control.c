#include <unistd.h>
#include <curses.h>
#include <string.h>

#include "control.h"
#include "utils.h"
#include "drawings.h"

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
    char scorestr[10];
    char nupstr[10];

    food_setup(MAP_HEIGHT, MAP_WIDTH, game_food);

    while(1)
    {
        read(pos_in, &tmp_pkg, sizeof(tmp_pkg));    //leggo posizione di pacman
        
        for(i=-1; i<=1/* code */; i++)    //cancella pacman
        {
            pos.x=pacman.p.x+i;
            pos.y=pacman.p.y;
            pos = get_pac_eff_pos(pos);
            
            mvaddch(pos.y+GUI_HEIGHT, pos.x, NCURSES_ACS(game_food[pos.y][pos.x]));
        }
        
        pacman = tmp_pkg;
        
        food_handler(&score, pacman.p, MAP_HEIGHT, MAP_WIDTH, game_food);

        mvprintw(5, 40, "x:%2d,y:%2d,d:%d", pacman.p.x, pacman.p.y, pacman.dir);

        sprintf(scorestr, "%d", score/10);
        sprintf(nupstr, "1UP");
        print_gui_string(0,11, nupstr);
        print_gui_string(3,13, "0");
        print_gui_string(3,11, scorestr);
        print_gui_string(3,33, "0");
        print_gui_string(3,31, scorestr);
        print_gui_string(0,37, "HIGH SCORE");
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
    } 
}

void food_handler(int* score, Position pos, int rows, int col, char game_food[rows][col])
{  
    int i;
    Position pe_pos;

    for(i=-1; i<=1; i++)
    {
        pe_pos.x = pos.x+i;
        pe_pos.y = pos.y;
        pe_pos = get_pac_eff_pos(pe_pos);

        switch(game_food[pe_pos.y][pe_pos.x])
        {
            case '~':
                *score += 10;
                beep();
                break;
            case '`': 
                //Funzione "spaventa fantasmi"
                *score += 50;
                break;
        }
        game_food[pe_pos.y][pe_pos.x] = ' ';
    }
}
