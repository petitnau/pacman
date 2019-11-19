#include <unistd.h>
#include <curses.h>
#include <string.h>

#include "control.h"
#include "utils.h"
#include "ghost.h"
#include "entity.h"
#include "drawings.h"
#include "list.h"

void food_handler(int* score, Position pos, int rows, int col, char game_food[rows][col], Ghost_Info *ghost_info);
void food_setup();

void manage_logs(int log_in, MessageList* log_list)
{
    static int p=0;
    int i;
    char msg_pkg[50];

    while(read(log_in, &msg_pkg, sizeof(char)*50) != -1)
    {
        list_push(log_list, msg_pkg);
    }    

    while(log_list->count > MAP_HEIGHT-2)
        list_pop(log_list);

    i = 0;
    MessageNode* aux = log_list->tail;
    while(aux != NULL)
    {
        mvprintw(MAP_HEIGHT+GUI_HEIGHT-(2+i), 60, "%*s",50,"");
        mvprintw(MAP_HEIGHT+GUI_HEIGHT-(2+i), 60, "%s", aux->msg);
        aux = aux->prev;
        i++;
    }
    refresh();
}

void control_main(int pos_in, int ghost_out, int log_in)
{
    Entity pacman = {PACMAN_ID, {PAC_START_X, PAC_START_Y}, PAC_START_DIR};
    Entity ghost = {GHOST_ID, {GHOST_START_X, GHOST_START_Y}, GHOST_START_DIR};
    Entity tmp_pkg;
    Ghost_Info ghost_info = {pacman, 0};

    Position pos;
    int i,j;

    int score = 0;
    char game_food[MAP_HEIGHT][MAP_WIDTH];
    int eated_dots; //Dopo 70 e 170 spawna frutto
    char scorestr[10];
    char nupstr[10];

    food_setup(MAP_HEIGHT, MAP_WIDTH, game_food);

    MessageList log_list;
    list_init(&log_list);

    while(1)
    {  
        read(pos_in, &tmp_pkg, sizeof(tmp_pkg));    //leggo posizione di pacman
        
        if(tmp_pkg.id == PACMAN_ID)
        {
            for(i=-1; i<=1; i++)    //cancella pacman
            {
                pos.x=pacman.p.x+i;
                pos.y=pacman.p.y;
                pos = get_pac_eff_pos(pos);
                
                mvaddch(pos.y+GUI_HEIGHT, pos.x, NCURSES_ACS(game_food[pos.y][pos.x]));
            }
            
            pacman = tmp_pkg;
            food_handler(&score, pacman.p, MAP_HEIGHT, MAP_WIDTH, game_food, &ghost_info);

            ghost_info.pacman = pacman;
            
            write(ghost_out, &ghost_info, sizeof(ghost_info));
        }
        else if(tmp_pkg.id == GHOST_ID)
        {
            for(i=-1; i<=1; i++)    //cancella ghost
            {
                pos.x=ghost.p.x+i;
                pos.y=ghost.p.y;
                pos = get_pac_eff_pos(pos);
                
                mvaddch(pos.y+GUI_HEIGHT, pos.x, NCURSES_ACS(game_food[pos.y][pos.x]));
            }
                
            ghost = tmp_pkg;
        }
    
        sprintf(scorestr, "%d", score/10);
        sprintf(nupstr, "1UP");
        print_gui_string(0,11, nupstr);
        print_gui_string(3,13, "0");
        print_gui_string(3,11, scorestr);
        print_gui_string(3,33, "0");
        print_gui_string(3,31, scorestr);
        print_gui_string(0,37, "HIGH SCORE");
        print_entity(pacman);
        print_entity(ghost);
        manage_logs(log_in, &log_list);
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

void food_handler(int* score, Position pos, int rows, int col, char game_food[rows][col], Ghost_Info *ghost_info)
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
                *score += 50;
                ghost_info->fright = start_timer(6);
                break;
        }
        game_food[pe_pos.y][pe_pos.x] = ' ';
    }
}