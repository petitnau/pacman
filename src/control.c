#include <unistd.h>
#include <curses.h>
#include <string.h>

#include "control.h"
#include "utils.h"
#include "pacman.h"
#include "ghost.h"
#include "entity.h"
#include "drawings.h"
#include "list.h"

void collision_handler(CharPacman pacman, CharGhost ghost, GhostInfo* ghost_info);
void food_handler(int* score, Position pos, int rows, int col, char game_food[rows][col], GhostInfo *ghost_info);
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

void control_main(int pacman_in, int ghost_in, int ghost_out, int log_in)
{
    CharPacman pac_tmp, pacman = {{PACMAN_ID, {PAC_START_X, PAC_START_Y}, PAC_START_DIR}, 3};
    CharGhost ghost_tmp, ghost = {{GHOST_ID, {GHOST_START_X, GHOST_START_Y}, GHOST_START_DIR}, M_CHASE};
    GhostInfo ghost_info = {pacman.e, false, false};

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
        while(read(pacman_in, &pac_tmp, sizeof(pac_tmp)) != -1)
        {
            for(i=-1; i<=1; i++)    //cancella pacman
            {
                pos.x=pacman.e.p.x+i;
                pos.y=pacman.e.p.y;
                pos = get_pac_eff_pos(pos);
                
                mvaddch(pos.y+GUI_HEIGHT, pos.x, NCURSES_ACS(game_food[pos.y][pos.x]));
            }
            pacman = pac_tmp;

            food_handler(&score, pacman.e.p, MAP_HEIGHT, MAP_WIDTH, game_food, &ghost_info);
            
            ghost_info.pacman = pacman.e;
            
        }
        while(read(ghost_in, &ghost_tmp, sizeof(ghost_tmp)) != -1)
        {
            for(i=-1; i<=1; i++)    //cancella ghost
            {
                pos.x=ghost.e.p.x+i;
                pos.y=ghost.e.p.y;
                pos = get_pac_eff_pos(pos);
                
                mvaddch(pos.y+GUI_HEIGHT, pos.x, NCURSES_ACS(game_food[pos.y][pos.x]));
            }
            
            ghost = ghost_tmp;
        }
        collision_handler(pacman, ghost, &ghost_info);
        //info inviate a ghost
        write(ghost_out, &ghost_info, sizeof(ghost_info));
        ghost_info.fright = false;
        ghost_info.death = false;

        sprintf(scorestr, "%d", score/10);
        sprintf(nupstr, "1UP");
        print_gui_string(0,11, nupstr);
        print_gui_string(3,13, "0");
        print_gui_string(3,11, scorestr);
        print_gui_string(3,33, "0");
        print_gui_string(3,31, scorestr);
        print_gui_string(0,37, "HIGH SCORE");
        print_pacman(pacman);
        print_ghost(ghost);
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

void food_handler(int* score, Position pos, int rows, int col, char game_food[rows][col], GhostInfo *ghost_info)
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
                ghost_info->fright = true;
                break;
        }
        game_food[pe_pos.y][pe_pos.x] = ' ';
    }
}

void collision_handler(CharPacman pacman, CharGhost ghost, GhostInfo* ghost_info)
{
    if(pacman.e.p.x == ghost.e.p.x && pacman.e.p.y == ghost.e.p.y)
    {
        if(ghost.mode == M_FRIGHT)
        {
            ghost_info->death = true;
            // morto il fantasma
        }
        else
        {
            //perdi una vita
            pacman.hp--;
            //pacman viene riportato alla pos. inziiale idem
        }
    }
}